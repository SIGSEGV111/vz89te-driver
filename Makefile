.PHONY: clean all

all: vz89te-csv vz89te-influx

vz89te-csv: vz89te-csv.cpp vz89te.cpp vz89te.hpp Makefile
	g++ -Wall -Wextra -flto -O3 vz89te-csv.cpp vz89te.cpp -o vz89te-csv

vz89te-influx: vz89te-influx.cpp vz89te.cpp vz89te.hpp Makefile
	g++ -Wall -Wextra -flto -O3 vz89te-influx.cpp vz89te.cpp -o vz89te-influx

clean:
	rm -vf simple vz89te-csv vz89te-influx
