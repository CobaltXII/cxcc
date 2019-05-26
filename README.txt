SYNOPSIS
A C compiler for x86-64 written in C++11

ABSTRACT
CXCC is a C compiler for x86-64. It supports a strict subset of the C language, and compiles code that uses 64-bit QWORDs for each variable (pointers and integers). It supports many features, such as pointers, functions, conditional statements, loops, and the System V AMD64 ABI function calling-convention. It's output is extremely unoptimized AT&T assembly, which can be assembled using any assembler (GAS and NASM are popular choices). Obviously, it does not conform to any C standards.

FEATURES
Programs:
	- explicit function declarations
	- implicit function definitions
Statements:
	- compound statements
	- conditional statements
	- while loop statements
	- return statements
	- variable declaration statements
	- expression statements
Expressions:
	- integer literals
	- string literals
	- character literals
	- identifiers
	- indexing expressions
	- function call expressions
	- binary expressions
	- unary expressions
Binary operators:
	- addition
	- subtraction
	- multiplication
	- division
	- modulo
	- assignment
	- logical AND
	- logical OR
	- equivalence
	- non-equivalence
	- relational greater-than
	- relational lesser-than
Unary operators:
	- address-of
	- value-of
	- arithmetic negative
	- arithmetic positive
	- logical NOT
Miscellaneous:
	- colored error messages
	- caret diagnostic

RESOURCES
I used the lexer/parser architecture in Joel Yliluoma's "How to Create a Compiler" series (mainly part 1) [1] as a reference for my lexer/parser architecture. I used Mihai Bazon's "How to implement a programming language in JavaScript" tutorial [2] as a starting point for implementing my lexer/parser. I used Nora Sandler's "Writing a C Compiler" series [3] to figure out the basic design of compilers, and how C code can translate into assembly. Finally, I used Matt Godbolt's "Compiler Explorer" [4] to quickly study (and replicate) the output of professional compilers like GCC and Clang.

REFERENCES
[1] https://www.youtube.com/watch?v=eF9qWbuQLuw
[2] http://lisperator.net/pltut/
[3] https://norasandler.com/2017/11/29/Write-a-Compiler.html
[4] https://godbolt.org/

CREDITS
Thanks to Joel Yliluoma, Mihai Bazon, Nora Sandler, Matt Godbolt, Ruslan Spivak and chekwob.

LICENSE 
This repository and it's contents are licensed under the MIT License.