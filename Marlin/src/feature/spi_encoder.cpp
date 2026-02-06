#include "../inc/MarlinConfig.h"

#if ENABLED(SPI_POSITION_ENCODERS)

#include <SPI.h>
#include "spi_encoder.h"

// pins are defined here: https://teamgloomy.github.io/btt_octopus_1.1_f429_pins_3_5.html

//SPI_Encoder_Mgr SPI_Encoder_Manager;

// ======================================================================
// Constructors
// ======================================================================

SPI_Encoder::SPI_Encoder(uint8_t chipSelectPinNo){
    chip = new AS5047P(chipSelectPinNo, 100000);
}

// ======================================================================
// Init
// ======================================================================

void SPI_Encoder::init(){
	chip->initSPI(ENCODER_MISO, ENCODER_MOSI, ENCODER_SCK);
}

float SPI_Encoder::getAngle(){
	return chip->readAngleDegree(true);
}

uint16_t SPI_Encoder::getMagnitude(){
	return chip->readMagnitude();
}

#endif // SPI_POSITION_ENCODERS