#include "matrix_led_display.hpp"
#include "chainable_led_display.hpp"
#include "utils.hpp"

void createDisplayTasks()
{
    //xTaskCreate( vTaskPeriodic, "vTaskPeriodic", 10000, NULL, 1, NULL );
    createMLTasks();
    createCHTasks();
}

void setup()
{
    SERIAL.begin(115200);
    setupLEDMatrix();
    setupLEDChain();
    createDisplayTasks();
    //createPingTasks();
    for (;;)
        ;
}

void loop()
{
}