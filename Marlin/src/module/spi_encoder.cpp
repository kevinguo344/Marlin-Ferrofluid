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

// ENCODER 1: FOR MOTOR 1 (RIGHT SIDE)
// ENCODER 2: FOR MOTOR 2 (LEFT SIDE)
SPI_Encoder SPI_Encoder_Mgr::encoders[2] = {SPI_Encoder(ENCODER_CS_1), SPI_Encoder(ENCODER_CS_2)};
bool SPI_Encoder_Mgr::homed = false;

uint16_t SPI_Encoder_Mgr::thetas[2] = {0,0};
int32_t SPI_Encoder_Mgr::X_Y_Pos[2] = {HOME_X, HOME_Y}; // [X, Y]0
millis_t SPI_Encoder_Mgr::last_update = millis();

#ifdef VEL_SMOOTHING_ENABLED
	float SPI_Encoder_Mgr::vel_acc_time = 0.0f;
	float SPI_Encoder_Mgr::vel_acc_dist = 0.0f;
#endif

static const uint32_t hypot_lut[256] PROGMEM = {
	65536, 65537, 65538, 65541, 65544, 65549, 65554, 65561,
	65568, 65577, 65586, 65597, 65609, 65621, 65635, 65649,
	65665, 65681, 65699, 65718, 65737, 65758, 65779, 65802,
	65826, 65850, 65876, 65902, 65930, 65958, 65988, 66018,
	66050, 66083, 66116, 66150, 66186, 66222, 66260, 66298,
	66337, 66378, 66419, 66461, 66504, 66549, 66594, 66640,
	66687, 66735, 66784, 66834, 66885, 66937, 66989, 67043,
	67098, 67153, 67210, 67267, 67326, 67385, 67445, 67506,
	67569, 67632, 67696, 67760, 67826, 67893, 67960, 68029,
	68098, 68169, 68240, 68312, 68385, 68459, 68533, 68609,
	68685, 68763, 68841, 68920, 69000, 69081, 69163, 69245,
	69329, 69413, 69498, 69584, 69671, 69758, 69847, 69936,
	70026, 70117, 70209, 70302, 70395, 70489, 70584, 70680,
	70777, 70874, 70973, 71072, 71172, 71272, 71374, 71476,
	71579, 71682, 71787, 71892, 71998, 72105, 72213, 72321,
	72430, 72540, 72650, 72762, 72874, 72986, 73100, 73214,
	73329, 73445, 73561, 73678, 73796, 73914, 74034, 74154,
	74274, 74395, 74517, 74640, 74763, 74887, 75012, 75138,
	75264, 75390, 75518, 75646, 75774, 75904, 76034, 76164,
	76296, 76427, 76560, 76693, 76827, 76961, 77096, 77232,
	77368, 77505, 77643, 77781, 77920, 78059, 78199, 78339,
	78481, 78622, 78764, 78907, 79051, 79195, 79339, 79484,
	79630, 79776, 79923, 80071, 80219, 80367, 80516, 80666,
	80816, 80966, 81118, 81269, 81422, 81574, 81728, 81881,
	82036, 82191, 82346, 82502, 82658, 82815, 82972, 83130,
	83289, 83448, 83607, 83767, 83927, 84088, 84249, 84411,
	84573, 84736, 84899, 85062, 85227, 85391, 85556, 85721,
	85887, 86054, 86221, 86388, 86555, 86723, 86892, 87061,
	87230, 87400, 87570, 87741, 87912, 88084, 88256, 88428,
	88601, 88774, 88947, 89121, 89296, 89471, 89646, 89821,
	89997, 90174, 90350, 90527, 90705, 90883, 91061, 91240,
	91419, 91598, 91778, 91958, 92138, 92319, 92500, 92682
};

static inline int32_t hypot_lut_fp(int32_t x, int32_t y) {
  x = abs(x);
  y = abs(y);

  if (x == 0) return y;
  if (y == 0) return x;

  int32_t a = max(x, y);
  int32_t b = min(x, y);

  uint8_t idx = (uint32_t(b) << 8) / a;

  const uint32_t k = pgm_read_dword(&hypot_lut[idx]);

  return (int64_t(a) * k) >> BIT_SHIFT;
}

float SPI_Encoder_Mgr::VEL = 0.0f;

void SPI_Encoder_Mgr::init(){
	SERIAL_ECHOLN(F("ENCODER 1: "));
	encoders[0].init();
	SERIAL_ECHOLN(F("ENCODER 2: "));
	encoders[1].init();
	SERIAL_ECHOLN(F("\n"));
}

void SPI_Encoder_Mgr::reportPosition__debug(){
	uint16_t theta_1 = encoders[0].getAngle();
	uint16_t theta_2 = encoders[1].getAngle();
	SERIAL_ECHOLN(F("ENCODER 1: "), theta_1, " ENCODER 2: ", theta_2);
}

void SPI_Encoder_Mgr::reportPosition(millis_t call_time){
	if(!homed) return;

	float time_elapsed = (call_time - last_update) * 0.001f;
	last_update = call_time;

	//---- NOTE: CALCULATIONS ARE DONE MAINLY IN Q16.16 FORMAT (16 bits for integer values, 16 bits for decimal values)
	uint16_t theta_1_new = encoders[0].getAngle();
	uint16_t theta_2_new = encoders[1].getAngle();

	// GET DELTAS OF EACH ANGLE (as magnitude reading in [0, 16384)])
	int32_t theta_1_delta = wrap_angle((int32_t)theta_1_new - (int32_t)thetas[0]);
	int32_t theta_2_delta = wrap_angle((int32_t)theta_2_new - (int32_t)thetas[1]);

	// STORES NEW ANGLE VALUES AS CURRENT VALUES
	thetas[0] = theta_1_new;
	thetas[1] = theta_2_new;

	// CONVERT DELTAS OF EACH ANGLE TO CHANGE IN COORDINATES
	// DELTA_B: change in belt length of Motor 1 (RIGHT MOTOR) in Q16.16 format
	// 		below is EQUIVALENT TO DELTA_B = (theta_2_delta/16384.0f) * 60.0f;
	int32_t DELTA_B = angle_to_mm(theta_2_delta);
	// DELTA_A: change in belt length of Motor 2 (LEFT MOTOR) in Q16.16 format
	//		below is EQUIVALENT TO DELTA_A = (theta_1_delta/16384.0f) * 60.0f;
	int32_t DELTA_A = angle_to_mm(theta_1_delta);

	// 		below is EQUIVALENT TO DELTA_X = (DELTA_A + DELTA_B)/2 in Q16.16 format
	int32_t DELTA_X = (DELTA_A + DELTA_B) >> 1;
	// 		below is EQUIVALENT TO DELTA_Y = (DELTA_A - DELTA_B)/2 in Q16.16 format
	int32_t DELTA_Y = (DELTA_A - DELTA_B) >> 1;

	// update XY position (stored in X_Y_Pos in Q16.16 format)
	X_Y_Pos[0] += DELTA_X;
	X_Y_Pos[1] += DELTA_Y;

	// converts DELTAs from Q16.16 format to regular decimal numbers
	//float DELTA_X_REAL = DELTA_X/(float)BIT_SHIFTED_ONE;
	//float DELTA_Y_REAL = DELTA_Y/(float)BIT_SHIFTED_ONE;

	#ifdef VEL_SMOOTHING_ENABLED
		// do Exponential Moving Average for velocity
		vel_acc_time += time_elapsed;
		vel_acc_dist += HYPOT(DELTA_X_REAL, DELTA_Y_REAL);

		if (vel_acc_time >= VEL_TIME_STEP){
			VEL += VEL_ALPHA * ((vel_acc_dist/vel_acc_time) - VEL);
			vel_acc_time = 0.0f;
			vel_acc_dist = 0.0f;
			if (VEL < VEL_EPS) VEL = 0.0;
		}
	#else
		//VEL = HYPOT(DELTA_X_REAL, DELTA_Y_REAL)/time_elapsed;
		VEL = (hypot_lut_fp(DELTA_X, DELTA_Y)/(float)BIT_SHIFTED_ONE)/time_elapsed;
		if (VEL <= 5.0f) VEL = 0.0f;
	#endif
	SERIAL_ECHOLN(F("X "), (X_Y_Pos[0]/(float)BIT_SHIFTED_ONE), " Y ", (X_Y_Pos[1]/(float)BIT_SHIFTED_ONE), " V ", VEL);
}

void SPI_Encoder_Mgr::setHome(){
	homed = true;

	X_Y_Pos[0] = HOME_X;
	X_Y_Pos[1] = HOME_Y;

	thetas[0] = encoders[0].getAngle();
	thetas[1] = encoders[1].getAngle();
}

#endif // SPI_POSITION_ENCODERS

