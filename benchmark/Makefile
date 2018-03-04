default: main

CXXFLAGS=-O2 -std=c++14 -I../include -flto -fdevirtualize
LDFLAGS=-lbenchmark -lpthread

main: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o main $(LDFLAGS)

clean:
	rm -rf main *.o

check: main
	./main
