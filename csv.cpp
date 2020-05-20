#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include "vz89te.hpp"

#define SYSERR(expr) (([&](){ const auto r = ((expr)); if( (long)r == -1L ) { throw #expr; } else return r; })())

const static double BILLION = 1000000000;
const static double MILLION = 1000000;

int main(int argc, char* argv[])
{
	try
	{
		using namespace vz89te;
		::vz89te::DEBUG = false;

		timeval tv_start;
		SYSERR(gettimeofday(&tv_start, NULL));

		TVZ89TE vz89te("/dev/i2c-1");
		vz89te.Refresh();

		if(!(argc >= 2 && strcmp(argv[1], "no-header") == 0))
			printf("unixtime;voc [ppb];co2 [ppm]\n");
		printf("%ld.%06ld;%lf;%lf\n", tv_start.tv_sec, tv_start.tv_usec, vz89te.ppb_voc, vz89te.ppm_co2);

		return 0;
	}
	catch(const char* err)
	{
		perror(err);
		return 1;
	}
	return 2;
}
