#!/bin/sh

cd ./build/depqbf/examples
gcc -o depqbf depqbf.c -L.. -lqdpll
./depqbf
.
