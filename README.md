# WLP4-LLVM - LLVM frontend for WLP4

[![Build Status](https://travis-ci.org/mnafees/wlp4-llvm.svg?branch=main)](https://travis-ci.org/mnafees/wlp4-llvm)

WLP4 (pronounced "Wool P Four") stands for Waterloo Language Plus Pointers Plus Procedures. It is a subset of C++ that is taught in the ["Foundations of Sequential Programs"](https://www.student.cs.uwaterloo.ca/~cs241) course at the University of Waterloo.

This project is based on the language specification available at https://www.student.cs.uwaterloo.ca/~cs241/wlp4/WLP4.html

A tutorial is also available at https://www.student.cs.uwaterloo.ca/~cs241/wlp4/WLP4tutorial.html

## Requirements

- CMake >= 3.12
- A C++ compiler that supports C++17
- LLVM >= 8

## Build

```
$ mkdir build && cd build
$ cmake ..
$ make
```
This will produce the binary `wlp4c`

## Usage

```
$ wlp4c <input file> --compiler=/path/to/C_compiler -o <output file>
```

## License

```
MIT License

Copyright (c) 2020 Mohammed Nafees

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
