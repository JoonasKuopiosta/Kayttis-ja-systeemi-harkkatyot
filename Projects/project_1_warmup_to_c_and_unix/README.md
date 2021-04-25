# Project 1: Warmup to C and Unix programming / reverse

A program to read lines of data from given input and write it in reverse order to given output

## Features

* Line sizes are dynamically allocated to fit the size, to allow very long lines and memory efficiency
* Lines are stored in doubly linked list, which allows for very long files to be processed
* All errors are handled and sent to standard error
* No memory leaks or warnings tested with valgrind

## Building
gcc -o reverse reverse.c

## Usage
`./reverse`

Reads from stdin and writes to stdout
tip: use ctrl+D to end input stream

`./reverse <input>`

Reads from given file and writes to stdout

`./reverse <input> <output>`

Reads from given file and writes to given file


## Made and tested with
Ubuntu 20.04.2 LTS 64-bit

gcc version 9.3.0

valgrind-3.15.0
