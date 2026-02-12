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

// ======================================================================
// Init
// ======================================================================

void SPI_Encoder::init(){
	chip->initSPI();
}

float SPI_Encoder::getAngle(){
	return chip->readAngleDegree(true);
}

uint16_t SPI_Encoder::getMagnitude(){
	return chip->readMagnitude();
}

void SPI_Encoder::setHomePos(float pos){
	home_pos = pos;
}

SPI_Encoder SPI_Encoder_Mgr::encoders[2] = {SPI_Encoder(ENCODER_CS_1), SPI_Encoder(ENCODER_CS_2)};
//SPI_Encoder SPI_Encoder_Mgr::encoders[1] = {SPI_Encoder(ENCODER_CS_1)};

void SPI_Encoder_Mgr::init(){
	encoders[0].init();
	encoders[1].init();
}

void SPI_Encoder_Mgr::reportPosition(){
	SERIAL_ECHOLN(F("A"), encoders[0].getAngle(), F("B"), encoders[1].getAngle());
	//SERIAL_ECHOLN(F("A"), encoders[0].getAngle());
}

void SPI_Encoder_Mgr::setHome(){
	float theta_1_i = encoders[0].getAngle();
	encoders[0].setHomePos(theta_1_i);
	SERIAL_ECHOLN(F("HOME ANGLE 1 IS "), encoders[0].getAngle());

	float theta_2_i = encoders[1].getAngle();
	encoders[1].setHomePos(theta_2_i);
	SERIAL_ECHOLN(F("HOME ANGLE 2 IS "), encoders[1].getAngle());
}

#endif // SPI_POSITION_ENCODERS

