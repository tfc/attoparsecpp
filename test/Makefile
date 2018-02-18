default: main

catch.hpp:
	wget https://raw.githubusercontent.com/CatchOrg/Catch2/master/single_include/catch.hpp

CXXFLAGS=-O2 -std=c++1z

main.o: main.cpp catch.hpp
	$(CC) $(CXXFLAGS) -c main.cpp

test.o: test.cpp ../include/parser.hpp catch.hpp
	$(CC) $(CXXFLAGS) -I../include -c test.cpp

main: main.o test.o
	$(CC) -o main main.o test.o

clean:
	rm -r main main.o test.o
