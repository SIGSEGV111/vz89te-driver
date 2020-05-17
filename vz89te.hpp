#include <stdint.h>

namespace vz89te
{
	extern bool DEBUG;

	class TVZ89TE
	{
		protected:
			const int fd;
			const uint8_t addr;

			void SendCommand(const uint8_t cmd, const uint8_t data[4]);

		public:
			double ppb_voc;
			double ppb_co2;

			void Refresh();

			TVZ89TE(const char* const i2c_bus_device, const uint8_t addr = 0x70);
			TVZ89TE(const int fd_i2c_bus, const uint8_t addr = 0x70);
			~TVZ89TE();
	};
}
