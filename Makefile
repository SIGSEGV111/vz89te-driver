.PHONY: clean all test

all: example

example: example.cpp vz89te.cpp vz89te.hpp
	g++ -flto -O1 -g example.cpp vz89te.cpp -o example

clean:
	rm -vf example

test: example
	time ./example
