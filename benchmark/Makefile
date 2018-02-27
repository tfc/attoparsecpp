default: main

CXXFLAGS=-O2 -std=c++14

LDFLAGS=-lbenchmark

main: main.cpp ../include/parser.hpp ../include/math_expression.hpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -I../include main.cpp -o main

clean:
	rm -rf main *.o benchpress
