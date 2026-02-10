
// -----------------------------
// Configuration
// -----------------------------

#include "SPI.h"

#define AS5047P_CS_PIN PA15

#define AS5047P_SPI_SPEED 1000000	// 1 MHz
#define AS5047P_ANGLECOM	0x3FFF
#define AS5047P_ERRFL		 0x0001

// -----------------------------
// Parity + command helpers
// -----------------------------

static uint8_t even_parity(uint16_t v) {
	v ^= v >> 8;
	v ^= v >> 4;
	v ^= v >> 2;
	v ^= v >> 1;
	return v & 1;
}

static uint16_t as5047p_read_cmd(uint16_t addr) {
	uint16_t cmd = 0x8000 | (addr << 1); // Read command
	if (even_parity(cmd)) cmd |= 0x0001;
	return cmd;
}

// -----------------------------
// SPI transfer (shared SPI1 safe)
// -----------------------------

static uint16_t as5047p_transfer(uint16_t tx) {
	uint16_t rx;

	SPI.begin();
	SPI.beginTransaction(SPISettings(
		AS5047P_SPI_SPEED,
		MSBFIRST,
		SPI_MODE1
	));

	pinMode(AS5047P_CS_PIN, OUTPUT);
	digitalWrite(AS5047P_CS_PIN, LOW);
	rx = SPI.transfer16(tx);
	digitalWrite(AS5047P_CS_PIN, HIGH);

	SPI.endTransaction();
	SPI.end();

	return rx;
}

// -----------------------------
// Public API
// -----------------------------

uint16_t encoder_read_raw() {
	// Dummy frame
	as5047p_transfer(as5047p_read_cmd(AS5047P_ANGLECOM));

	// Actual data frame
	uint16_t rx = as5047p_transfer(0x0000);
	return rx & 0x3FFF; // 14-bit angle
}

uint16_t encoder_read_errfl() {
	as5047p_transfer(as5047p_read_cmd(AS5047P_ERRFL));
	return as5047p_transfer(0x0000) & 0x7;
}

// -----------------------------
// Periodic update
// -----------------------------

float encoder_update() {
	uint16_t raw = encoder_read_raw();
	float angle = (raw/(float)16384) * 360;
	return angle;
	//static millis_t last = 0;
	//if (!ELAPSED(millis(), last + 5)) return; // 200 Hz
	//last = millis();

	//const uint16_t raw = encoder_read_raw();

	//// Example: X axis feedback only
	//const float encoder_steps = raw * (float(STEPS_PER_UNIT_X) / 16384.0f);
	//const float stepper_steps = stepper.position(X_AXIS);

	//const float error = encoder_steps - stepper_steps;

	//// Optional: report large error
	//if (ABS(error) > 20) {
	//	SERIAL_ECHOLNPAIR("Encoder error X: ", error);
	//}
}