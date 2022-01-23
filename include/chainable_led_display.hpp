#ifndef INCLUDE_CH_DISPLAY_HPP
#define INCLUDE_CH_DISPLAY_HPP
#include <ChainableLED.h>
#include <utils_perso.h>
#include "utils.hpp"
#define NUM_LEDS 3
#define TIMER_LED 1000

enum class ChainLedsDisplayType
{
    Color,
    Random,
    Preset
};

// Setup
void setupLEDChain();

// Tasks
void manageChLeds(void *pvParameters);
void createCHTasks();

// DisplayTypes
void changeDisplayTypeCL(ChainLedsDisplayType cldt);

// Variable

#endif