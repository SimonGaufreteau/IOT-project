#ifndef INCLUDE_ML_DISPLAY_HPP
#define INCLUDE_ML_DISPLAY_HPP

#include "grove_two_rgb_led_matrix.h"
#include "utils.hpp"
//#include <chrono>

enum class MatrixDisplayType
{
    Message,
    Beat,
    Preset
};

// Setup
void setupLEDMatrix();

// Tasks
void manageMatrixLed(void *pvParameters);
void createMLTasks();

// DisplayTypes
void changeDisplayTypeML(MatrixDisplayType mdt);

// Variable changes
void changeCurrentPresetML(int preset);
void setCurrentMessage(std::string message);
void setCurrentMessage(const char *message);
void updateSoundBuffer(int soundValue);


#endif