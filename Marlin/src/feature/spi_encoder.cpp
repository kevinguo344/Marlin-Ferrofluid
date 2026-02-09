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

bool SPI_Encoder::init(){
	SPIClass spi3(ENCODER_MOSI, ENCODER_MISO, ENCODER_SCK);
	return chip->initSPI(&spi3);
}

float SPI_Encoder::getAngle(){
	return chip->readAngleDegree(true);
}

uint16_t SPI_Encoder::getMagnitude(){
	return chip->readMagnitude();
}

SPI_Encoder SPI_Encoder_Mgr::encoders[1];
bool SPI_Encoder_Mgr::initiated = false;

void SPI_Encoder_Mgr::init(){
	encoders[0].setChipSelect(ENCODER_CS);
	if (encoders[0].init()){
		initiated = true;
		SERIAL_ECHOLN(F("ENCODERS INITIALIZED CORRECTLY"));
	} else {
		initiated = false;
		SERIAL_ECHOLN(F("ENCODERS FAILED INIT"));
	}
}

void SPI_Encoder_Mgr::reportPosition(){
	if(initiated){
		SERIAL_ECHO(F("Current Encoder Magnitude: "), encoders[0].getAngle(), F("\n"));
	} else {
		SERIAL_ECHOLN(F("ENCODERS FAILED INIT"));
		encoders[0].init();
		if(initiated){
			SERIAL_ECHOLN(F("ENCODERS INITIALIZED CORRECTLY"));
			SERIAL_ECHO(F("Current Encoder Magnitude: "), encoders[0].getAngle(), F("\n"));
		} else {
			SERIAL_ECHOLN(F("ENCODERS FAILED INIT"));
		}
	}
	
}

#endif // SPI_POSITION_ENCODERS

