#
# this makefile is meant to be simple and easy to understand
# this means everything is getting typed out
#

#
# final executable
#
main: obj/preprocess.o obj/tokenize.o obj/parser.o obj/assemblererror.o main.cpp
	g++ -o main main.cpp -std=c++11 -march=native -O3 obj/preprocess.o obj/tokenize.o obj/parser.o obj/assemblererror.o

test: obj/preprocess.o obj/tokenize.o obj/parser.o obj/assemblererror.o test.cpp
	g++ -o test test.cpp -std=c++11 -march=native -O3 obj/preprocess.o obj/tokenize.o obj/parser.o obj/assemblererror.o

#
# remove existing object files
#
clean:
	rm obj/*.o

# =========================================================
# build various object files needed for final executable:

obj/preprocess.o: src/preprocess.cpp inc/preprocess.h
	g++ -c -o obj/preprocess.o src/preprocess.cpp -I. -std=c++11 -march=native -O3

obj/tokenize.o: src/tokenize.cpp inc/tokenize.h
	g++ -c -o obj/tokenize.o src/tokenize.cpp -I. -std=c++11 -march=native -O3

obj/parser.o: src/parser.cpp inc/parser.h
	g++ -c -o obj/parser.o src/parser.cpp -I. -std=c++11 -march=native -O3

obj/assemblererror.o: src/assemblererror.cpp inc/assemblererror.h
	g++ -c -o obj/assemblererror.o src/assemblererror.cpp -I. -std=c++11 -march=native -O3

# =========================================================
