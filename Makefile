default: main

main: main.cpp parser.hpp
	$(CC) -O2 -std=c++17 -o main main.cpp
