#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "vz89te.hpp"

int main()
{
	try
	{
		using namespace vz89te;

		TVZ89TE vz89te("/dev/i2c-1");
		vz89te::DEBUG = false;

		while(true)
		{
			try
			{
				vz89te.Refresh();
				printf("VOC: %f [ppb]\nCO²: %f [ppb]\n\n", vz89te.ppb_voc, vz89te.ppb_co2);
			}
			catch(const char* err)
			{
				fprintf(stderr, "WARN: %s\n", err);
			}
// 			usleep(00000);
		}

		return 0;
	}
	catch(const char* err)
	{
		perror(err);
		return 1;
	}
	return 2;
}
