#include "SPI.h"
#include "AS5047P.h"
#include "encoder_reader.h"

// pins are defined here: https://teamgloomy.github.io/btt_octopus_1.1_f429_pins_3_5.html

// HAS_SPI_TFT uses similar SPI pin, check xpt2046.cpp and xpt2046.h for using SPI


SPI_Encoder::SPI_Encoder(){
	//SPIClass(ENCODER_MOSI, ENCODER_MISO, ENCODER_SCK)
	
	chip = AS5047P(ENCODER_CS);
	chip.initSPI();
}

float SPI_Encoder::getAngle(){
	return chip.getAngleDegree(true);
}

uint16_t SPI_Encoder::getMagnitude(){
	return chip.readMagnitude();
}

// USING EXISTING QUICKSTART FILE
//void setup(){
//	Serial.begin(115200);
//	// Initialize SPI via library and verify connectivity
//	if (!as5047p.initSPI()) {
//		Serial.println("AS5047P init failed. Check wiring and power.");
//		while (true) { delay(1000); }
//	}
//}
//
//void loop(){
//	// Read 14-bit angle (degrees) with DAE compensation
//	float deg = as5047p.readAngleDegree(true);
//	Serial.print("Angle (deg): ");
//	Serial.println(deg, 3);
//
//	// Read magnitude
//	uint16_t mag = as5047p.readMagnitude();
//	Serial.print("Magnitude: ");
//	Serial.println(mag);
//
//	delay(500);
//}