#include "../inc/MarlinConfig.h"

#if ENABLED(SPI_POSITION_ENCODERS)

#include <SPI.h>
#include "spi_encoder.h"

// pins are defined here: https://teamgloomy.github.io/btt_octopus_1.1_f429_pins_3_5.html

SPI_Encoder_Mgr SPI_Encoder_Manager;

SPI_Encoder::SPI_Encoder(){
	chip = AS5047P(ENCODER_CS);
}

void SPI_Encoder::init(){
	chip.initSPI(ENCODER_MISO, ENCODER_MOSI, ENCODER_SCK);
}

float SPI_Encoder::getAngle(){
	return chip.getAngleDegree(true);
}

uint16_t SPI_Encoder::getMagnitude(){
	return chip.readMagnitude();
}

void SPI_Encoder_Mgr::init(){
	SPI_Encoder_Manager.encoders[0].init();
	SPI_Encoder_Manager.encoders[1].init();
}
#endif // SPI_POSITION_ENCODERS