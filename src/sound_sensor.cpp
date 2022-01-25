#include "sound_sensor.hpp"
#include "Arduino.h"
#include "utils_perso.h"
#include "utils.hpp"
#include "matrix_led_display.hpp"

int SOUND_DELAY = 100;
bool isRunning = true;

// Tasks
TaskHandle_t task_sound_captor = NULL;

int getSoundValue()
{
    return analogRead(A2);
}

void setRunning(bool running)
{
    isRunning = running;
}

void taskSoundSensor(void *pvParameters)
{
    SERIAL.println("Setting up sound sensor task");
    for (;;)
    {
        //SERIAL.printf("Running ? %d", isRunning);
        if (isRunning)
        {
            int cval = getSoundValue();
            //SERIAL.printf("Current sound value is : %d\n", cval);
            updateSoundBuffer(cval);
        }
        vTaskDelay(SOUND_DELAY);
    }
}

void createSoundTasks()
{
    //xTaskCreate(taskSoundSensor, "manageChLeds", 10000, NULL, 1, &task_sound_captor);
}