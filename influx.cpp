#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include "vz89te.hpp"

#define SYSERR(expr) (([&](){ const auto r = ((expr)); if( (long)r == -1L ) { throw #expr; } else return r; })())

const static double BILLION = 1000000000;
const static double MILLION = 1000000;

int main()
{
	try
	{
		using namespace vz89te;
		::vz89te::DEBUG = false;

		timeval tv_start;
		SYSERR(gettimeofday(&tv_start, NULL));

		// write in influx compatible format to stdout
		const unsigned long long ns = tv_start.tv_sec * 1000000000ULL + tv_start.tv_usec * 1000ULL;

		TVZ89TE vz89te("/dev/i2c-1");
		vz89te.Refresh();

		printf("voc_ppb=%f,co2_ppm=%f %llu\n", vz89te.ppb_voc, vz89te.ppm_co2, ns);

		return 0;
	}
	catch(const char* err)
	{
		perror(err);
		return 1;
	}
	return 2;
}
