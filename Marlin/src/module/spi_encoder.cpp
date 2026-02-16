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

uint16_t SPI_Encoder::getAngle(){
	return chip->readAngleRaw(true);
}

uint16_t SPI_Encoder::getMagnitude(){
	return chip->readMagnitude();
}

void SPI_Encoder::setHomePos(uint16_t pos){
	home_pos = pos;
}
// ENCODER 1: FOR MOTOR 1 (RIGHT SIDE)
// ENCODER 2: FOR MOTOR 2 (LEFT SIDE)
SPI_Encoder SPI_Encoder_Mgr::encoders[2] = {SPI_Encoder(ENCODER_CS_1), SPI_Encoder(ENCODER_CS_2)};
bool SPI_Encoder_Mgr::homed = false;

uint16_t SPI_Encoder_Mgr::thetas[2] = {0,0};
float SPI_Encoder_Mgr::X_Y_Pos[2] = {HOME_X, HOME_Y}; // [X, Y]

void SPI_Encoder_Mgr::init(){
	encoders[0].init();
	encoders[1].init();
}

void SPI_Encoder_Mgr::reportPosition(){
	uint16_t theta_1_new = encoders[0].getAngle();
	uint16_t theta_2_new = encoders[1].getAngle();
	if(homed){
		// GET DELTAS OF EACH ANGLE
		int32_t theta_1_delta = (int32_t)theta_1_new - (int32_t)thetas[0];
		int32_t theta_2_delta = (int32_t)theta_2_new - (int32_t)thetas[1];

		// DEALS WITH WRAPAROUND ISSUES
		if (theta_1_delta > 8192) theta_1_delta -= 16384;
		else if(theta_1_delta < -8192) theta_1_delta += 16384;

		if (theta_2_delta > 8192) theta_2_delta -= 16384;
		else if(theta_2_delta < -8192) theta_2_delta += 16384;

		// STORES NEW ANGLE VALUES AS CURRENT VALUES
		thetas[0] = theta_1_new;
		thetas[1] = theta_2_new;

		// CONVERT DELTAS OF EACH ANGLE TO CHANGE IN COORDINATES
		// --- THIS CODE DEFINITELY WORKS ---
		// DELTA_B: change in belt length of Motor 1 (RIGHT MOTOR)
		float DELTA_B = (theta_2_delta/16384.0f) * 60.0f;
		// DELTA_A: change in belt length of Motor 2 (LEFT MOTOR)
		float DELTA_A = (theta_1_delta/16384.0f) * 60.0f;

		// DELTA_X = 0.5f * (DELTA_A + DELTA_B)
		X_Y_Pos[0] += (0.5f * (DELTA_A + DELTA_B));
		// DELTA_Y = 0.5f * (DELTA_A - DELTA_B);
		X_Y_Pos[1] += (0.5f * (DELTA_A - DELTA_B));

		float VEL = HYPOT((0.5f * (DELTA_A + DELTA_B)), (0.5f * (DELTA_A - DELTA_B))) * 200;

		// --- THIS CODE IS MAYBE MORE EFFICIENT ---
		//uint16_t DELTA_X = (theta_1_delta >> 13) + (theta_2_delta >> 13);
		//uint16_t DELTA_Y = (theta_1_delta >> 13) - (theta_2_delta >> 13);
		//SERIAL_ECHOLN(F("DELTA_X "), DELTA_X, " DELTA_Y ", DELTA_Y);
		//
		//X_Y_Pos[0] = X_Y_Pos[0] + (15.0f * (DELTA_X));
		//X_Y_Pos[1] = X_Y_Pos[1] + (15.0f * (DELTA_Y));

		SERIAL_ECHOLN(F("X "), X_Y_Pos[0], " Y ", X_Y_Pos[1], " V ", VEL);
	}
}

void SPI_Encoder_Mgr::setHome(){
	homed = true;

	X_Y_Pos[0] = HOME_X;
	X_Y_Pos[1] = HOME_Y;

	thetas[0] = encoders[0].getAngle();
	thetas[1] = encoders[1].getAngle();

	uint16_t theta_1_i = encoders[0].getAngle();
	encoders[0].setHomePos(theta_1_i);

	uint16_t theta_2_i = encoders[1].getAngle();
	encoders[1].setHomePos(theta_2_i);
}

#endif // SPI_POSITION_ENCODERS

