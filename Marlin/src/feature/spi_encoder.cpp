#include "../inc/MarlinConfig.h"

#if ENABLED(SPI_POSITION_ENCODERS)

#include <SPI.h>
#include "spi_encoder.h"

// pins are defined here: https://teamgloomy.github.io/btt_octopus_1.1_f429_pins_3_5.html

SPI_Encoder_Mgr SPI_Encoder_Manager;

// ======================================================================
// Constructors
// ======================================================================

SPI_Encoder::SPI_Encoder(){
	//empty constructor, must call setChipSelect afterwards
}

SPI_Encoder::SPI_Encoder(uint8_t chipSelectPinNo){
    chip = new AS5047P(chipSelectPinNo, 100000UL);
}

void SPI_Encoder::setChipSelect(uint8_t chipSelectPinNo){
	chip = new AS5047P(chipSelectPinNo, 100000UL);
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

SPI_Encoder SPI_Encoder_Mgr::encoders[1];

void SPI_Encoder_Mgr::init(){
	encoders[0].setChipSelect(ENCODER_CS);
	encoders[0].init();
}

void SPI_Encoder_Mgr::reportPosition(){
	SERIAL_ECHO(F("Current Encoder Magnitude: "), encoders[0].getMagnitude(), F("\n"));
}

#endif // SPI_POSITION_ENCODERS

