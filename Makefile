.PHONY: clean all

all: simple csv influx

simple: simple.cpp vz89te.cpp vz89te.hpp Makefile
	g++ -Wall -Wextra -Werror -flto -O1 -g simple.cpp vz89te.cpp -o simple

csv: csv.cpp vz89te.cpp vz89te.hpp Makefile
	g++ -Wall -Wextra -Werror -flto -O1 -g csv.cpp vz89te.cpp -o csv

influx: influx.cpp vz89te.cpp vz89te.hpp Makefile
	g++ -Wall -Wextra -Werror -flto -O1 -g influx.cpp vz89te.cpp -o influx

clean:
	rm -vf simple csv influx
