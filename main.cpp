#include <bitset>
#include <cstdint>
#include <climits>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <queue>
#include <vector>
#include "node.h"
using namespace std;

vector<uint64_t> createFreqTable(string filename);
shared_ptr<Node> createHuffmanTree(vector<uint64_t> freq);
vector<vector<uint8_t>> createDictionary(shared_ptr<Node> root);
void createDictionary(shared_ptr<Node> root, vector<uint8_t> code, vector<vector<uint8_t>> &dictionary);
void applyCompression(string inFilename, vector<uint64_t> &freq, vector<vector<uint8_t>> &dictionary);
void decompress(string inFilename);

int main(int argc, char* argv[]) {
	bool verbose = false;
	bool shouldDecompress = false;
	string input;
	string filePath;
	
	/* Parse CLI arugments */
	if (argc == 1) {
		cout << "huf requires a file path\n";
		return 0;
	} else {
		input = argv[1];
		if (input == "-v" || input == "-d") {
			if (argc == 2) {
				cout << "huf requires a file path\n";
				return 0;
			} else if (argc > 2) {
				filePath = argv[2];
				if (input == "-v") verbose = true;
				if (input == "-d") shouldDecompress = true;
			}
		} else {
			filePath = input;
		}
	}
	if (!shouldDecompress) {
		/* Run compression algorithm */
		vector<uint64_t> freq = createFreqTable(filePath);
		shared_ptr<Node> root = createHuffmanTree(freq);
		vector<vector<uint8_t>> dictionary = createDictionary(root);
		applyCompression(filePath, freq, dictionary); 
		
		/* Optional verbose output */
		if (verbose) {
			cout << "\nFrequency Table\n";
			for (int i = 0x00; i <= 0xFF; i++) {
				if (freq[i]) {
					cout << "0x" << hex << setw(2) << setfill('0') << uppercase << i
						 << dec << ": " << freq[i] << endl;;
				}
			}
			cout << endl;

			cout << "Dictionary\n";
			for (int i = 0x00; i <= 0xFF; i++) {
				if (dictionary[i].size()) {
					cout << "0x" << hex << setw(2) << setfill('0') << uppercase << i
						 << ": ";
					for (uint8_t c : dictionary[i]) {
						int out = c;
						cout << out;
					}
					cout << endl;
				}
			}
		}
	} else {
		/* Run decompression algorithm */
		decompress(filePath);
	}	
}

vector<uint64_t> createFreqTable(string filename) {
	vector<uint64_t> freq(0x100, 0);
	ifstream file(filename, ios::in|ios::binary|ios::ate);
	if (!file) {
		cout << "Invalid file path\n";
		exit(0);
	}
	if (file.is_open()) {
		file.seekg(0, ios::end);
		size_t size = file.tellg();
		file.seekg(0, ios::beg);
		
		/* Copy entire file into RAM */
		vector<char> buf(size);
		file.read(reinterpret_cast<char*>(buf.data()), size);
		cout << "Read in " << file.gcount() << " bytes\n";
		file.close();
		
		/* Loop through evey byte in the copied file */
		for (uint64_t i = 0; i < size; i++) {
			unsigned char c = buf[i];
			freq[c]++; // Increment freqency of current byte
		}
	}

	return freq;
}

shared_ptr<Node> createHuffmanTree(vector<uint64_t> freq) {
	priority_queue<Node> q;
	shared_ptr<Node> root = nullptr;
	
	/* Create priority queue from frequency table */
	for (int i = 0x00; i <= 0xFF; i++) {
		if (freq[i]) {
			Node node(i, freq[i]);
			q.push(node);
		}
	}
	
	/* Create Huffman Tree from priority queue */
	if (!q.size()) {
		cout << "File is empty\n";
		exit(0);
	}
	if (q.size() == 1) {
		root = shared_ptr<Node>(new Node(0, q.top().freq));
		root->left = shared_ptr<Node>(new Node(q.top().val, q.top().freq));
	}
	while (q.size() > 1) {
		shared_ptr<Node> n1;
		shared_ptr<Node> n2;
		
		n1 = shared_ptr<Node>(new Node(q.top().val, q.top().freq, q.top().left, q.top().right));
		q.pop();
		n2 = shared_ptr<Node>(new Node(q.top().val, q.top().freq, q.top().left, q.top().right));
		q.pop();
		Node newNode(0, n1->freq + n2->freq, n1, n2);
		q.push(newNode);
		if (q.size() == 1) {
			root = shared_ptr<Node>(new Node(newNode.val, newNode.freq, newNode.left, newNode.right));
		}
	}

	return root;
}

/* Helper function */
vector<vector<uint8_t>> createDictionary(shared_ptr<Node> root) {
	vector<vector<uint8_t>> dictionary(0x100);
	vector<uint8_t> code;
	createDictionary(root, code, dictionary);
	return dictionary;
}

/* Perform preorder traversal of the Huffman tree */
void createDictionary(shared_ptr<Node> root, vector<uint8_t> code, vector<vector<uint8_t>> &dictionary) {
	if (!root->left && !root->right) { // Add code to dictionary
		int val = root->val-36;
		dictionary[root->val] = code;
	}
	if (root->left) { // Append 0 to code
		code.push_back(0);
		createDictionary(root->left, code, dictionary);
		code.pop_back();
	}
	if (root->right) { // Append 1 to code
		code.push_back(1);
		createDictionary(root->right, code, dictionary);
		code.pop_back();
	}
}

/* Read in a byte from input file, lookup code in dictionary, write code to output file */
void applyCompression(string inFilename, vector<uint64_t> &freq, vector<vector<uint8_t>> &dictionary) {
	uint8_t currBit = 0x00;
	uint8_t bitBuffer = 0x00;
	vector<uint8_t> inFile;

	string outFilename = inFilename;
	outFilename += ".huf";
	ofstream ofs(outFilename, ios::binary);
	ifstream ifs(inFilename, ios::binary);
	
	/* Load entire input file into RAM */
	if (ifs.is_open()) {
		ifs.seekg(0, ios::end);
		size_t size = ifs.tellg();
		ifs.seekg(0, ios::beg);
		inFile.resize(size);
		ifs.read(reinterpret_cast<char*>(inFile.data()), size);
		ifs.close();
	}
	
	/* Write codes to file bit by bit */
	if (ofs.is_open()) {
		/* Embed frequency table at start of file */
		ofs.write((char*)&freq[0], sizeof(freq[0])*freq.size());

		uint8_t writeBit = 0b00000001;
		char bitBuf = 0b00000000;
		
		for (uint8_t readByte : inFile) {
			char c = readByte;
			vector<uint8_t> code = dictionary[readByte];
			
			for (uint8_t bit : code) {
				if (bit) bitBuf |= writeBit;
				int prevWriteBit = writeBit;
				writeBit *= 2; // Circular left shift
				
				if (writeBit < prevWriteBit) { // bitBuf filled on overflow
					ofs.write(&bitBuf, 1);
					writeBit = 0b00000001;
					bitBuf = 0b00000000;
					int i = bitBuf;
				}
			}
		}
		
		if (writeBit > 0b00000001) { 
			ofs.write(&bitBuf, 1); // Flush buffer
			
			/* Embed number of bits that should be written from last byte to EOF */
			char endWriteLen = 1;
			for (int i = 2; i < writeBit; i *= 2) endWriteLen++;
			ofs.write(&endWriteLen, 1);
		} else {
			char fullByte = 8;
			ofs.write(&fullByte, 1); 
		}
		ofs.close();
	}
}

void decompress(string inFilename) {
	ifstream ifs(inFilename, ios::binary);
	vector<uint64_t> freq(0x100);
	vector<uint8_t> inFile;
	char ewl = 0; // Number of bits that should be written from the last byte

	/* Extract extension and create output filename */
	size_t pos = inFilename.find(".huf");
	if (pos == string::npos) {
		cout << "\'huf -d\' only works on files with \'.huf\' extension\n";
		exit(0);
	}
	string outFilename = inFilename;
	outFilename.erase(outFilename.begin()+pos, outFilename.end());

	/* load ewl, frequency table and rest of file seperately */
	if (ifs.is_open()) {
		ifs.seekg(0, ios::end);
		size_t size = ifs.tellg();
		ifs.seekg(0, ios::beg);
		
		ifs.read((char*)&freq[0], sizeof(freq[0])*freq.size());
		size -= (sizeof(freq[0])*freq.size());

		inFile.resize(size-1);
		ifs.read(reinterpret_cast<char*>(inFile.data()), size-1);
		
		ifs.read(&ewl, 1);
		ifs.close();
	}

	/* Build Huffman tree from frequency table */
	ofstream ofs(outFilename, ios::binary);
	shared_ptr<Node> root = createHuffmanTree(freq);
	shared_ptr<Node> currNode = root;
	
	/* Traverse tree using codes to find original bytes */
	for (uint64_t i = 0; i < inFile.size(); i++) {
		uint8_t byte = inFile[i];
		char c = byte;
		uint8_t mask = 0b00000001;
		int writeLength = 8;

		if (i == inFile.size()-1) {
			writeLength = ewl;
		} 
		
		for (int i = 0; i < writeLength; i++) {
			if (byte & mask && currNode->right) {
				currNode = currNode->right;
			} else if (currNode->left){
				currNode = currNode->left;
			}
			if (!currNode->right && !currNode->left) {
				char c = currNode->val;
				ofs.write(&c, 1);
				currNode = root;
			}
			mask *= 2; // Left shift
		}
	}
}
