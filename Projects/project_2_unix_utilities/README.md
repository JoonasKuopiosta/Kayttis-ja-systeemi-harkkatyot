# Project 2: Unix Utilities

A set of programs for simple UNIX things

## Features

Every program is compiled with -Wall and -Werror, and are runtime tested with valgrind for memory leaks.

### my-cat
* Takes in file names that it iterates through printing their content one at a time

### my-grep
* Takes in a search term and file names, and prints every line that has the search term for one file at a time
* Dynamic line size with getline()

### my-zip & my-unzip
* Uses run-length encoding (RLE) to compresses lengthy files into binary form
* Takes in multiple files and compresses to stdout in 4 int byte + 1 char byte pairs
* Dynamic line sizes and file sizes with getline() and linked list implementation

## Building
gcc -o my-cat my-cat.c
gcc -o my-grep my-grep.c
gcc -o my-zip my-zip.c
gcc -o my-unzip my-unzip.c

## Usage
`./my-cat <file1> [<file2> ...]`
Reads from given files and writes to stdout

`./my-grep <search term> <file1> [<file2> ...]`
Searches for search term in given files and prints out corresponding lines to stdout

`./my-zip <file1> [<file2> ...]`
Compressess given files to stdout

`./my-unzip <file1> [<file2> ...]`
Uncompressess given files to stdout


## Made and tested with
Ubuntu 20.04.2 LTS 64-bit

gcc version 9.3.0

valgrind-3.15.0

test data is in the same directory
