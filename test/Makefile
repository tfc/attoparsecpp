default: .depend main

CXXFLAGS=-g -fsanitize=address -fsanitize=undefined -std=c++14 -I../include -Wfatal-errors
LDFLAGS=-lasan -lubsan

SRC = main.cpp math_expression.cpp test.cpp gdb.cpp
OBJ := $(SRC:%.cpp=%.o)

.depend: $(SRC)
	$(CXX) $(CXXFLAGS) -MM $(SRC) > .depend

-include .depend

main: $(OBJ)
	$(CXX) $(CXXFLAGS) -o main $(OBJ) $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf main *.o .depend

check: main
	./main
