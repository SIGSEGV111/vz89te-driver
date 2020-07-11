#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/file.h>
#include "vz89te.hpp"

static volatile bool do_run = true;

static void OnSignal(int)
{
	do_run = false;
}

#define SYSERR(expr) (([&](){ const auto r = ((expr)); if( (long)r == -1L ) { throw #expr; } else return r; })())

// const static double BILLION = 1000000000;
// const static double MILLION = 1000000;

int main(int argc, char* argv[])
{
	signal(SIGINT,  &OnSignal);
	signal(SIGTERM, &OnSignal);
	signal(SIGHUP,  &OnSignal);
	signal(SIGQUIT, &OnSignal);

	try
	{
		if(argc != 3)
			throw "need exactly two arguments: <i2c bus device> <location>";

		using namespace vz89te;
		::vz89te::DEBUG = false;

		TVZ89TE vz89te(argv[1]);

		while(do_run)
		{
			timeval tv_start;
			timeval tv_end;

			SYSERR(gettimeofday(&tv_start, NULL));
			vz89te.Refresh();
			SYSERR(gettimeofday(&tv_end, NULL));

			SYSERR(flock(STDOUT_FILENO, LOCK_EX));
			printf("%ld.%06ld;%ld.%06ld;\"%s\";\"voc\";%f\n", tv_start.tv_sec, tv_start.tv_usec, tv_end.tv_sec, tv_end.tv_usec, argv[2], vz89te.ppb_voc);
			printf("%ld.%06ld;%ld.%06ld;\"%s\";\"co2\";%f\n", tv_start.tv_sec, tv_start.tv_usec, tv_end.tv_sec, tv_end.tv_usec, argv[2], vz89te.ppm_co2);
			fflush(stdout);
			SYSERR(flock(STDOUT_FILENO, LOCK_UN));

			usleep(10 * 1000 * 1000);
		}

		return 0;
	}
	catch(const char* const err)
	{
		fprintf(stderr, "[ERROR] %s\n", err);
		return 1;
	}
	return 2;
}
