#include "AS5047P.h"
// PINS TO USE
#define ENCODER_MOSI PB5
#define ENCODER_MISO PB4
#define ENCODER_SCK PB3
#define ENCODER_CS PA15

class SPI_Encoder {
	private:
		AS5047P chip;

	public:
		SPI_Encoder();
		float getAngle();
		uint16_t getMagnitude();
};