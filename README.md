# Huffman Coding
A C++ implementation of David A. Huffman's encoding algorithm with file
compression.

## Compiling and running on Linux
The program compiles to an executable called 'huf' with the provided makefile.
It takes in a filename as an argument. It can also take in optional arguments,
'-v' (verbose) and '-d' (decompress). By default, 'huf' will compress the file
and generate a compressed file named '\*.huf'.

```bash
$ ./huf -v myFile
$ ./huf -d myFile.huf
```

## Tests
Test cases are included in the tests folder. Tests can be run by running the
program on the file with the verbose parameter and comparing the outout to the
output file.

```bash
$ ./huf -v tests/fib
$ cat tests/fib.out
```

Alternatively, you can copy a file, compress and decompress it, then verify its
itegrity against the original file with a checksum.

```bash
$ ./huf myFile.pdf
$ mv myFile.pdf.huf myFileCopy.pdf.huf
$ ./huf -d myFileCopy.pdf.huf
$ md5sum myFileCopy.pdf myFile.pdf
```

The xxd utility can also be used to verify the integrity of small files after
compression and decompression.

The seventeen included test cases are as follows.
1. allBytes: Every possible by with a frequency relative to itself. One zero, two ones, three twos, etc.
2. allBytesUniform: Every possible byte once.
3. ascii: Similar to all bytes but only inlcudes lower case alphabetic characters.
4. bestCase: 10 Mib of zeros.
5. fib: Probably the most interesting test case. Byte frequencies are the Fibonacci sequence. This produces the worst case where the Huffman tree is very unbalanced and the longest code length is equal to the number of symbols.
6. g4g: Taken from GeeksForGeeks.com [1]. Verifies that the tree and symbols are generated correctly. A reference image 'g4g.png' is included to help verify.
7. qwerty: A panagram.
8. test\*: Test for edge cases when generating the Huffman tree with a small number of distinct bytes. 1-5 have uniform frequencies while 6-9 do not.
9. wikipedia: Same as six but from Wikipedia.com [2].

## Have Fun!

## References
1. https://www.geeksforgeeks.org/huffman-coding-greedy-algo-3/
2. https://en.wikipedia.org/wiki/Huffman_coding
