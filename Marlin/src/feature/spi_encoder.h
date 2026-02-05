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
		//AS5047P *chip;
		AS5047P chip;

	public:
		SPI_Encoder(uint8_t chipSelectPinNo = ENCODER_CS);
		void init();
		float getAngle();
		uint16_t getMagnitude();
};

//class SPI_Encoder_Mgr {
//	public:
//		static void init();
//		static SPI_Encoder encoders[2];
//};

//extern SPI_Encoder_Mgr SPI_Encoder_Manager;