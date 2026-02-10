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

/**
 * Read raw 14-bit angle from AS5047P (ANGLECOM)
 * @return 0–16383
 */
uint16_t encoder_read_raw();

/**
 * Read and clear ERRFL register
 * @return bitmask of error flags (bits 2:0)
 */
uint16_t encoder_read_errfl();

/**
 * Periodic encoder update
 * Safe to call from loop() or idle()
 */
float encoder_update();