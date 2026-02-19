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

//void SPI_Encoder::setHomePos(uint16_t pos){
//	home_pos = pos;
//}
// ENCODER 1: FOR MOTOR 1 (RIGHT SIDE)
// ENCODER 2: FOR MOTOR 2 (LEFT SIDE)
SPI_Encoder SPI_Encoder_Mgr::encoders[2] = {SPI_Encoder(ENCODER_CS_1), SPI_Encoder(ENCODER_CS_2)};
bool SPI_Encoder_Mgr::homed = false;

uint16_t SPI_Encoder_Mgr::thetas[2] = {0,0};
int32_t SPI_Encoder_Mgr::X_Y_Pos[2] = {HOME_X, HOME_Y}; // [X, Y]

void SPI_Encoder_Mgr::init(){
	encoders[0].init();
	encoders[1].init();
}

void SPI_Encoder_Mgr::reportPosition(){

	if(!homed) return;

	uint16_t theta_1_new = encoders[0].getAngle();
	uint16_t theta_2_new = encoders[1].getAngle();

	// GET DELTAS OF EACH ANGLE (as magnitude reading in [0, 16384)])
	int32_t theta_1_delta = wrap_angle((int32_t)theta_1_new - (int32_t)thetas[0]);
	int32_t theta_2_delta = wrap_angle((int32_t)theta_2_new - (int32_t)thetas[1]);

	// STORES NEW ANGLE VALUES AS CURRENT VALUES
	thetas[0] = theta_1_new;
	thetas[1] = theta_2_new;

	// CONVERT DELTAS OF EACH ANGLE TO CHANGE IN COORDINATES
	// DELTA_B: change in belt length of Motor 1 (RIGHT MOTOR)
	// 		below is EQUIVALENT TO DELTA_B = (theta_2_delta/16384.0f) * 60.0f;
	int32_t DELTA_B = angle_to_mm(theta_2_delta);
	// DELTA_A: change in belt length of Motor 2 (LEFT MOTOR)
	//		below is EQUIVALENT TO DELTA_A = (theta_1_delta/16384.0f) * 60.0f;
	int32_t DELTA_A = angle_to_mm(theta_1_delta);

	// 		below is EQUIVALENT TO DELTA_X = (DELTA_A + DELTA_B)/2
	int32_t DELTA_X = (DELTA_A + DELTA_B) >> 1;
	// 		below is EQUIVALENT TO DELTA_Y = (DELTA_A - DELTA_B)/2;
	int32_t DELTA_Y = (DELTA_A - DELTA_B) >> 1;
	SERIAL_ECHOLN(F("\tDELTA_X "), (DELTA_X/(float)BIT_SHIFTED_ONE), " Y ", (DELTA_Y/(float)BIT_SHIFTED_ONE));

	// update XY position
	X_Y_Pos[0] += DELTA_X;
	X_Y_Pos[1] += DELTA_Y;

	// 200 is inverse of 5 microseconds (how often we update encoder readings)
	//float VEL = SQRT((DELTA_X * DELTA_X + DELTA_Y * DELTA_Y)/(float)(BIT_SHIFTED_ONE * BIT_SHIFTED_ONE)) * 200.0f;

	SERIAL_ECHOLN(F("X "), (X_Y_Pos[0]/(float)BIT_SHIFTED_ONE), " Y ", (X_Y_Pos[1]/(float)BIT_SHIFTED_ONE)/*, " V ", VEL*/);
}

void SPI_Encoder_Mgr::setHome(){
	homed = true;

	X_Y_Pos[0] = HOME_X;
	X_Y_Pos[1] = HOME_Y;

	thetas[0] = encoders[0].getAngle();
	thetas[1] = encoders[1].getAngle();
}

#endif // SPI_POSITION_ENCODERS

