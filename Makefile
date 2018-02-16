default: main

main: main.cpp
	$(CC) -O2 -std=c++17 -o main main.cpp
