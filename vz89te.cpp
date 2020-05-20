#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <string.h>

#include "vz89te.hpp"

#define SYSERR(expr) (([&](){ const auto r = ((expr)); if( (long)r == -1L ) { throw #expr; } else return r; })())
#define EXPECT(expr, value) (([&](){ const auto r = ((expr)); if( r != value ) { throw #expr; } else return r; })())

namespace vz89te
{
	bool DEBUG = false;

	static uint8_t CalculateCRC(const uint8_t* const buffer, const uint8_t sz)
	{
		uint16_t sum = 0;

		for(uint8_t i = 0; i < sz; i++)
			sum += buffer[i];

		return 0xff - (sum + (sum / 0x100));
	}

	static bool IsAllZero(const uint8_t* const buffer, const uint8_t sz)
	{
		for(uint8_t i = 0; i < sz; i++)
			if(buffer[i])
				return false;
		return true;
	}

	void Hexdump(const char* const title, const void* const buffer, const unsigned sz)
	{
		fprintf(stderr, "DEBUG: %s: ", title);
		for(unsigned i = 0; i < sz; i++)
			fprintf(stderr, "%02hhx ", ((const unsigned char*)buffer)[i]);
		fputc('\n', stderr);
	}

	static const uint8_t CMD_GET_STATUS = 0b000001100;

	void TVZ89TE::SendCommand(const uint8_t cmd, const uint8_t data[4])
	{
		if(DEBUG) fprintf(stderr, "DEBUG: sending command 0x%02x\n", cmd);
		uint8_t buffer[6];
		buffer[0] = cmd;
		memcpy(buffer + 1, data, 4);
		buffer[5] = CalculateCRC(buffer, 5);
		if(DEBUG) Hexdump("tx-buffer", buffer, sizeof(buffer));
		SYSERR(write(this->fd, buffer, 6));
	}

	void TVZ89TE::Refresh()
	{
		SendCommand(CMD_GET_STATUS, (const uint8_t*)"\x00\x00\x00\x00");
		usleep(100000);	// 100ms delay, required by sensor
		uint8_t buffer[7];
		EXPECT(read(this->fd, buffer, sizeof(buffer)), sizeof(buffer));

		if(DEBUG) Hexdump("rx-buffer", buffer, sizeof(buffer));

		if(IsAllZero(buffer, sizeof(buffer)))
		{
			if(DEBUG) fprintf(stderr, "DEBUG: did not receive any data (refreshed too quickly?)\n");
			throw "no data received";
		}

		const uint8_t calc_crc = CalculateCRC(buffer, sizeof(buffer) - 1);
		if(buffer[6] != calc_crc)
		{
			if(DEBUG) fprintf(stderr, "DEBUG: CRC mismatch; my calculated CRC = %02x vs. transmitted CRC = %02x\n", calc_crc, buffer[6]);
			throw "CRC mismatch";
		}

		if(buffer[5] != 0)
		{
			if(DEBUG) fprintf(stderr, "DEBUG: chip returned error code = %02x\n", buffer[5]);
			throw "chip returned error status";
		}

		if(buffer[0] < 13 || buffer[1] < 13)
		{
			if(DEBUG) fprintf(stderr, "DEBUG: chip returned bogus values (D1 < 13 or D2 < 13); D1 = %hhu, D2 = %hhu", buffer[0], buffer[1]);
			throw "bogus data received";
		}

		this->ppb_voc = (double)(buffer[0] - 13) * (1000.0 / 229.0);
		this->ppm_co2 = (double)(buffer[1] - 13) * (1600.0 / 229.0) + 400.0;

		if(DEBUG) fprintf(stderr, "DEBUG: successfully refreshed sensor values: voc = %hhu, co2 = %hhu\n", buffer[0], buffer[1]);
	}

	TVZ89TE::TVZ89TE(const char* const i2c_bus_device, const uint8_t addr) : fd(SYSERR(open(i2c_bus_device, O_RDWR | O_CLOEXEC | O_SYNC))), addr(addr)
	{
		if(DEBUG) fprintf(stderr, "DEBUG: i2c_bus_device = \"%s\", fd = %d, addr = 0x%hhx\n", i2c_bus_device, fd, addr);
		SYSERR(ioctl(this->fd, I2C_SLAVE, this->addr));
	}

	TVZ89TE::TVZ89TE(const int fd_i2c_bus, const uint8_t addr) : fd(fd_i2c_bus), addr(addr)
	{
		if(DEBUG) fprintf(stderr, "DEBUG: fd = %d, addr = 0x%hhx\n", fd, addr);
		SYSERR(ioctl(this->fd, I2C_SLAVE, this->addr));
	}

	TVZ89TE::~TVZ89TE()
	{
		close(this->fd);
	}
}
