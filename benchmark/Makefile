default: main

CXXFLAGS=-O2 -std=c++14

LDFLAGS=-lbenchmark -lpthread
 
main: main.cpp ../include/parser.hpp ../include/math_expression.hpp
	$(CXX) $(CXXFLAGS) -I../include main.cpp -o main $(LDFLAGS)

clean:
	rm -rf main *.o 
