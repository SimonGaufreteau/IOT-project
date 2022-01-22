#ifndef INCLUDE_DISPLAY_HPP
#define INCLUDE_DISPLAY_HPP
#include <ChainableLED.h>
#include <utils_perso.h>
#include "grove_two_rgb_led_matrix.h"
//#include <chrono>

#ifdef ARDUINO_SAMD_VARIffT_COMPLIffCE
#define SERIAL SerialUSB
#else
#define SERIAL Serial
#endif

#define NUM_LEDS 3
#define TIMER_LED 1000

enum class MatrixDisplayType
{
    Message,
    Beat,
    Preset
};

enum class ChainLedsDisplayType
{
    Color,
    Preset
};

// Setup
void setupLEDMatrix();
void setupLEDChain();
void createDisplayTasks();

// Tasks
void manageChLeds(void *pvParameters);
void manageMatrixLed(void *pvParameters);

// DisplayTypes
void changeDisplayTypeML(MatrixDisplayType mdt);
void changeDisplayTypeCL(ChainLedsDisplayType cldt);

#endif