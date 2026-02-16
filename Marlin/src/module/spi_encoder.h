#pragma once
#include "../inc/MarlinConfig.h"
#include "./spi_encoder/AS5047P.h"

// PINS TO USE
#define ENCODER_CS_1 PA_15
#define ENCODER_CS_2 PB_3

#define HOME_X 260.0f
#define HOME_Y 0.0f
#define DEG_PER_MM 95.994f

class SPI_Encoder {
	private:
		AS5047P *chip;
		uint16_t home_pos;
	public:
		SPI_Encoder();
		SPI_Encoder(uint8_t chipSelectPinNo);
		void init();
		uint16_t getAngle();
		uint16_t getMagnitude();
		void setHomePos(uint16_t theta);
};

class SPI_Encoder_Mgr {
	private:
		static bool homed;
		static uint16_t thetas[2];
		static float X_Y_Pos[2];
	public:
		static SPI_Encoder encoders[2];
		static void init();
		static void reportPosition();
		static void setHome();
};

extern SPI_Encoder_Mgr SPI_Encoder_Manager;