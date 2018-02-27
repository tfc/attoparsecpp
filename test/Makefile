default: main

CXXFLAGS=-O2 -std=c++14

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

test.o: test.cpp ../include/parser.hpp
	$(CXX) $(CXXFLAGS) -I../include -c test.cpp

math_expression.o: math_expression.cpp ../include/parser.hpp
	$(CXX) $(CXXFLAGS) -I../include -c math_expression.cpp

main: main.o test.o math_expression.o
	$(CXX) -o main main.o test.o math_expression.o

clean:
	rm -rf main *.o 
