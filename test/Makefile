default: main

CXXFLAGS=-g -fsanitize=address -fsanitize=undefined -std=c++14
LDFLAGS=-lasan -lubsan

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

test.o: test.cpp ../include/parser.hpp
	$(CXX) $(CXXFLAGS) -I../include -c test.cpp

math_expression.o: math_expression.cpp ../include/parser.hpp ../include/math_expression.hpp
	$(CXX) $(CXXFLAGS) -I../include -c math_expression.cpp

main: main.o test.o math_expression.o
	$(CXX) -o main main.o test.o math_expression.o $(LDFLAGS)

clean:
	rm -rf main *.o
