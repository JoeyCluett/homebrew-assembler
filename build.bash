#!/bin/bash

g++ -c -o obj/preprocess.o src/preprocess.cpp -I. -std=c++11 -march=native -O3
g++ -c -o obj/tokenize.o src/tokenize.cpp -I. -std=c++11 -march=native -O3
g++ -c -o obj/tokenerror.o src/tokenerror.cpp -I. -std=c++11 -march=native -O3

g++ -o main main.cpp obj/tokenize.o obj/preprocess.o obj/tokenerror.o -std=c++11 -march=native -O3
