
// -----------------------------
// Configuration
// -----------------------------

#include "SPI.h"
#include "../MarlinCore.h"
#include "encoder.h"

SPI_Encoder_Mgr SPI_Encoder_Manager;

#define AS5047P_CS_PIN PA15

#define AS5047P_SPI_SPEED 1000000	// 1 MHz
#define AS5047P_ANGLECOM	0x3FFF
#define AS5047P_ERRFL		 0x0001

// defines ALPHA (smoothing factor) of Exponential Moving average
// ALPHA = 1/(2^EMA_SHIFT)
#define EMA_SHIFT 4

static int32_t reading_smoothed = 0;
static bool ema_initialized = false;

SPI_Encoder::SPI_Encoder(uint8_t chipSelectPinNo = PA15){
	CS_PIN = chipSelectPinNo;
	//empty constructor, must call setChipSelect afterwards
}

uint16_t SPI_Encoder::init(){
	return SPI_Encoder::read_errfl();
}

// -----------------------------
// Parity + command helpers
// -----------------------------

uint8_t SPI_Encoder::even_parity(uint16_t v) {
	v ^= v >> 8;
	v ^= v >> 4;
	v ^= v >> 2;
	v ^= v >> 1;
	return v & 1;
}

uint16_t SPI_Encoder::read_cmd(uint16_t addr) {
	uint16_t cmd = 0x8000 | (addr << 1); // Read command
	if (even_parity(cmd)) cmd |= 0x0001;
	return cmd;
}

// -----------------------------
// SPI transfer (shared SPI1 safe)
// -----------------------------

uint16_t SPI_Encoder::transfer(uint16_t tx) {
	uint16_t rx;

	SPI.begin();
	SPI.beginTransaction(SPISettings(
		AS5047P_SPI_SPEED,
		MSBFIRST,
		SPI_MODE1
	));

	pinMode(CS_PIN, OUTPUT);
	digitalWrite(CS_PIN, LOW);
	rx = SPI.transfer16(tx);
	digitalWrite(CS_PIN, HIGH);

	SPI.endTransaction();
	SPI.end();

	return rx;
}

// -----------------------------
// Public API
// -----------------------------

uint16_t SPI_Encoder::read_raw() {
	// Dummy frame
	transfer(read_cmd(AS5047P_ANGLECOM));

	// Actual data frame
	uint16_t rx = transfer(0x0000);
	return rx & 0x3FFF; // 14-bit angle
}

uint16_t SPI_Encoder::read_errfl() {
	transfer(read_cmd(AS5047P_ERRFL));
	return transfer(0x0000) & 0x7;
}

uint16_t SPI_Encoder::read_smoothed(){
	uint16_t raw = read_raw();

	if(ema_initialized){
		// y[n]: 	updated smoothed reading
		// y[n-1]: 	previous smoothed reading
		// x[n]:	raw sensor reading
		// equivalent to y[n] = y[n-1] + ALPHA * (x[n] - y[n-1])
		reading_smoothed += (raw - (reading_smoothed >> EMA_SHIFT));
		
		// reading_smoothed >> EMA_SHIFT equivalent to reading_smoothed/(2^EMA_SHIFT) equivalent to reading_smoothed * ALPHA
		// & 0x3FFF clamps bitshifted reading between 0 and 16383
		return ((reading_smoothed >> EMA_SHIFT) & 0x3FFF);
	} else {
		reading_smoothed = raw << EMA_SHIFT;
		ema_initialized = true;
		return raw;
	}
}

// -----------------------------
// Periodic update
// -----------------------------

float SPI_Encoder::encoder_update() {
	uint16_t raw = read_smoothed();
	float angle = (raw/(float)16384) * 360;
	return angle;
}

SPI_Encoder SPI_Encoder_Mgr::encoders[1] = {SPI_Encoder()};

uint16_t SPI_Encoder_Mgr::init(){
	return encoders[0].init();
}