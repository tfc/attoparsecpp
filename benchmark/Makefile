default: main

CXXFLAGS=-O2 -std=c++14

main: main.cpp ../include/parser.hpp ../include/math_expression.hpp
	$(CXX) $(CXXFLAGS) -I../include main.cpp -o main

clean:
	rm -rf main *.o benchpress
