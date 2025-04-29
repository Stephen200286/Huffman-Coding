#include "node.h"
using namespace std;

Node::Node(uint8_t v, uint64_t f) {
	val = v;
	freq = f;
}
Node::Node(uint8_t v, uint64_t f, shared_ptr<Node> l, shared_ptr<Node> r) {
	val = v;
	freq = f;
	left = l;
	right = r;
}

bool operator<(Node const& lhs, Node const& rhs) {
	// This is backwards because that's easier than getting std::priority_queue to 
	// act as a min priority queue.
	return lhs.freq > rhs.freq; 
}
