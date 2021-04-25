# Project 5: Parallel zip

RLE zipping tool that uses threads

## Features

Every program is compiled with -Wall and -Werror, and are runtime tested with valgrind for memory leaks.


### pzip & punzip
* Uses run-length encoding (RLE) to compresses lengthy files into binary form
* Takes in multiple files and compresses to stdout in 4 int byte + 1 char byte pairs
* Dynamic line sizes and file sizes with getline() and linked list implementation
* Has parallel capabilities for multiple file processing by assigning files to individual threads
* Smart thread management by not launching more threads than the system can handle (get_nprocs())

## Building

gcc -o pzip pzip.c -pthread

gcc -o punzip punzip.c -pthread

## Usage
`./pzip <file1> [<file2> ...]`

Compressess given files to stdout


`./punzip <file1> [<file2> ...]`

Uncompressess given files to stdout


## Made and tested with
Ubuntu 20.04.2 LTS 64-bit

gcc version 9.3.0

valgrind-3.15.0

test data is in the same directory
