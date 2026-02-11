#pragma once

/**
 * encoder.h
 *
 * AS5047P encoder feedback (read-only) interface for Marlin
 * Shared SPI1 safe (compatible with TMC5160)
 */

#include "Arduino.h"

// -----------------------------
// Public API
// -----------------------------

class SPI_Encoder {
	private:
		int32_t reading_smoothed;
		bool ema_initialized;
		uint16_t starting_pos;
		uint16_t latest_pos;

		uint8_t even_parity(uint16_t v);
		uint16_t read_cmd(uint16_t addr);
		uint16_t transfer(uint16_t tx);
		uint8_t CS_PIN;

		/**
		 * Read and clear ERRFL register
		 * @return bitmask of error flags (bits 2:0)
		 */
		uint16_t read_errfl();
		
		/**
		 * Read raw 14-bit angle from AS5047P (ANGLECOM)
		 * @return 0–16383
		 */
		uint16_t read_raw();
		uint16_t read_smoothed();
	public:
		SPI_Encoder(uint8_t chipSelectPinNo);
		uint16_t init();
		uint16_t encoder_update_raw();
		float encoder_update();
		float position_update();
};

class SPI_Encoder_Mgr {
	public:
		static SPI_Encoder encoders[1];
		//static uint16_t init();
		static float getAngleReading();
		static uint16_t getRawReading();
};

extern SPI_Encoder_Mgr SPI_Encoder_Manager;

///**
// * Read raw 14-bit angle from AS5047P (ANGLECOM)
// * @return 0–16383
// */
//uint16_t encoder_read_raw();
//
///**
// * Read raw 14-bit angle from AS5047P (ANGLECOM)
// * @return 0–16383
// */
//uint16_t encoder_read_smoothed();
//
///**
// * Read and clear ERRFL register
// * @return bitmask of error flags (bits 2:0)
// */
//uint16_t encoder_read_errfl();
//
///**
// * Periodic encoder update
// * Safe to call from loop() or idle()
// */
//float encoder_update();