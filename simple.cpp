#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "vz89te.hpp"

const static double BILLION = 1000000000;
const static double MILLION = 1000000;

int main()
{
	try
	{
		using namespace vz89te;
		::vz89te::DEBUG = true;

		TVZ89TE vz89te("/dev/i2c-1");

		vz89te.Refresh();
		printf("VOC: %lf [ppb] / %lf %%\nCO²: %lf [ppm] / %lf %%\n\n", vz89te.ppb_voc, vz89te.ppb_voc / (BILLION / 100.0), vz89te.ppm_co2, vz89te.ppm_co2 / (MILLION / 100.0));

		return 0;
	}
	catch(const char* err)
	{
		perror(err);
		return 1;
	}
	return 2;
}
