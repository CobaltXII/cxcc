# Debug cxcc
clang++ cxcc.cpp -o cxcc -std=c++11 && ./cxcc "$@" > tmp.s && ./dasm.sh tmp