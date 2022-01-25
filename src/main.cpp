#include "matrix_led_display.hpp"
#include "chainable_led_display.hpp"
#include "http_handler.hpp"
#include "utils.hpp"
#include "sound_sensor.hpp"

void createDisplayTasks()
{
    //xTaskCreate( vTaskPeriodic, "vTaskPeriodic", 10000, NULL, 1, NULL );
    createMLTasks();
    createCHTasks();
    createSoundTasks();
}

void createHTTPTasks()
{
    setupTasksHTTP();
}

void setup()
{
    SERIAL.begin(115200);
    setupLEDMatrix();
    setupLEDChain();
    createDisplayTasks();
    createHTTPTasks();
    //createPingTasks();
    for (;;)
        ;
}

void loop()
{
}