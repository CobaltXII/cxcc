# Debug assembly
clang $1.s -o $1.o && ./$1.o
echo $?