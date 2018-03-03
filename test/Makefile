default: .depend main

CXXFLAGS=-g -fsanitize=address -fsanitize=undefined -std=c++14 -I../include
LDFLAGS=-lasan -lubsan

SRC = main.cpp math_expression.cpp test.cpp

.depend: $(SRC)
	$(CXX) $(CXXFLAGS) -MM $(SRC) > .depend

-include .depend

main: main.o test.o math_expression.o
	$(CXX) $(CXXFLAGS) -o main main.o test.o math_expression.o $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf main *.o .depend

check: main
	./main
