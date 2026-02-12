#pragma once
#include "../inc/MarlinConfig.h"
#include "./spi_encoder/AS5047P.h"

// PINS TO USE
#define ENCODER_CS_1 PA_15
#define ENCODER_CS_2 PB_3

class SPI_Encoder {
	private:
		AS5047P *chip;
		float home_pos;
	public:
		SPI_Encoder();
		SPI_Encoder(uint8_t chipSelectPinNo);
		//void setChipSelect(uint8_t chipSelectPinNo);
		void init();
		float getAngle();
		uint16_t getMagnitude();
};

class SPI_Encoder_Mgr {
	public:
		static SPI_Encoder encoders[1];
		static void init();
		static void reportPosition();
};

extern SPI_Encoder_Mgr SPI_Encoder_Manager;