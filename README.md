## SYNOPSIS
A C compiler for x86-64 written in C++11

## ABSTRACT
CXCC is a C compiler for x86-64. It supports a strict subset of the C language, and compiles code that uses 64-bit QWORDs for each variable (pointers and integers). It supports many features, such as pointers, functions, conditional statements, loops, and the System V AMD64 ABI function calling-convention. It's output is extremely unoptimized AT&T assembly, which can be assembled using any assembler (GAS and NASM are popular choices). Obviously, it does not conform to any C standards.

## USAGE
```
Usage: ./cxcc <in> [out [-o]]
    Compiles C source <in> to x86-64 assembly and stores the
    output in [out]. If no value for [out] is provided, the
    output is stored in the file <in>.s. If the option -o is
    provided, gcc is used to assemble [out] and store the image
    in [out].
```
To compile `test/hello.c`, you might use these commands:
```
cobalt$ ./cxcc test/hello.c hello -o
cobalt$ ./hello
Hello, world!
cobalt$ rm -f hello
```

## RESOURCES
I used the lexer/parser architecture in Joel Yliluoma's "How to Create a Compiler" series (mainly part 1) [1] as a reference for my lexer/parser architecture. I used Mihai Bazon's "How to implement a programming language in JavaScript" tutorial [2] as a starting point while implementing my lexer/parser. I used Nora Sandler's "Writing a C Compiler" series [3] to figure out the basic design of compilers, and how C code can translate into assembly. Finally, I used Matt Godbolt's "Compiler Explorer" [4] to quickly study (and replicate) the output of professional compilers like GCC and Clang.

## REFERENCES
[1] https://www.youtube.com/watch?v=eF9qWbuQLuw \
[2] http://lisperator.net/pltut/ \
[3] https://norasandler.com/2017/11/29/Write-a-Compiler.html \
[4] https://godbolt.org/

## CREDITS
Thanks to Joel Yliluoma, Mihai Bazon, Nora Sandler, Matt Godbolt, Ruslan Spivak and chekwob.

## LICENSE 
This repository and it's contents are licensed under the MIT License.
