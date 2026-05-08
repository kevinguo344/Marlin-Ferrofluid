#pragma once
#include "../inc/MarlinConfig.h"
#include "./spi_encoder/AS5047P.h"

// PINS TO USE
#define ENCODER_CS_1 E0_CS_PIN
#define ENCODER_CS_2 E1_CS_PIN

// USED FOR BITSHIFTING
#define BIT_SHIFT 16 						// How many bits shifting for calculation (using int32_t so 16 bits for integer, 16 bits for decimal)
#define BIT_SHIFTED_ONE (1 << BIT_SHIFT) 	// For converting bitshifted numbers back to decimal numbers

// Home position at the bottom left corner
#define HOME_X (260 << BIT_SHIFT)			// Maximum of X axis in Q16.16 format
#define HOME_Y 0							// Minimum of Y axis
#define ROTATION_IN_MM 60 					// 30 teeth GT2 Pulley has a pitch circumference of 60mm (30 teeth * 2 mm/tooth)

// For Exponential Moving Average (EMA) Smoothing in velocity readings
//#define VEL_SMOOTHING_ENABLED
#ifdef VEL_SMOOTHING_ENABLED
	#define VEL_TIME_STEP 0.01f
	#define VEL_EPS 5.0f
	#define VEL_ALPHA 0.2f
#endif

class SPI_Encoder {
	private:
		AS5047P *chip;
	public:
		SPI_Encoder();
		SPI_Encoder(uint8_t chipSelectPinNo);
		void init();
		uint16_t getAngle();
};

class SPI_Encoder_Mgr {
	private:
		// will be TRUE after both X and Y axis are homed, initializes position to be HOME_X and HOME_Y
		static bool homed;

		// stores each encoders readings as a magnitude in range [0, 16384)
		static uint16_t thetas[2];

		// XY in in Q16.16 format, equivalent to X or Y position * 2^16 (using fixed point math to make calculations faster)
		static int32_t X_Y_Pos[2];

		static millis_t last_update;

		#ifdef VEL_SMOOTHING_ENABLED
			static float vel_acc_time;
			static float vel_acc_dist;
		#endif
		static float VEL;

		// helper functions to deal with repeated math
		/**
		 * @brief Converts angle magnitude readings to mm
		 * @param theta angle magnitude reading in range [0, 16384)
		 * @return how much the pulley moved in mm in Q16.16 format
		 */
		static inline int32_t angle_to_mm(int32_t theta){
			// equivalent to (theta * 60.0)/16384.0
			return (theta * (ROTATION_IN_MM << BIT_SHIFT)) >> 14;
		}
		/**
		 * @brief Makes sure that the angle readings remain in [0, 16384)
		 * @param reading angle magnitude reading in range [0, 16384)
		 * @return angle magnitude reading in range [0, 16384)
		 */
		static inline int32_t wrap_angle(int32_t reading){
			if(reading > 8192) reading -= 16384;
			else if(reading < -8192) reading += 16384;
			return reading;
		}
	public:
		static SPI_Encoder encoders[2];
		static void init();
		static void reportPosition(millis_t call_time);
		static void setHome();
};

extern SPI_Encoder_Mgr SPI_Encoder_Manager;