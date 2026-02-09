#pragma once
#include "../inc/MarlinConfig.h"
#include "./spi_encoder/AS5047P.h"

// PINS TO USE
#define ENCODER_MOSI PB5
#define ENCODER_MISO PB4
#define ENCODER_SCK PB3
#define ENCODER_CS PA15

class SPI_Encoder {
	private:
		AS5047P *chip;

	public:
		SPI_Encoder();
		SPI_Encoder(uint8_t chipSelectPinNo);
		void setChipSelect(uint8_t chipSelectPinNo);
		bool init();
		float getAngle();
		uint16_t getMagnitude();
};

class SPI_Encoder_Mgr {
	private:
		static bool initiated;
	public:
		static SPI_Encoder encoders[1];
		static void init();
		static void reportPosition();
};

extern SPI_Encoder_Mgr SPI_Encoder_Manager;