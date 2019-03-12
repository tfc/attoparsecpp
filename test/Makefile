default: .depend main

CXXFLAGS=-g -std=c++14 -I../include -Wfatal-errors
LDFLAGS=

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
