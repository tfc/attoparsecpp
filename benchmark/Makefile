default: main

benchpress/benchpress.hpp:
	(mkdir -p benchpress && \
	cd benchpress && \
	wget https://raw.githubusercontent.com/cjgdev/benchpress/master/src/benchpress/benchpress.hpp)

benchpress/cxxopts.hpp:
	(mkdir -p benchpress && \
	cd benchpress && \
	wget https://raw.githubusercontent.com/cjgdev/benchpress/master/src/benchpress/cxxopts.hpp)

CXXFLAGS=-O2 -std=c++1z

main: main.cpp benchpress/benchpress.hpp benchpress/cxxopts.hpp ../include/parser.hpp ../include/math_expression.hpp
	$(CC) $(CXXFLAGS) -I../include main.cpp -o main

clean:
	rm -rf main *.o benchpress
