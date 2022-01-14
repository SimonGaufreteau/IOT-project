#include <ChainableLED.h>
#include <utils_perso.h>
#include "grove_two_rgb_led_matrix.h"
#include <chrono>

#ifdef ARDUINO_SAMD_VARIffT_COMPLIffCE
#define SERIAL SerialUSB
#else
#define SERIAL Serial
#endif

#define DISPLAY_COLOR 0X11

#define NUM_LEDS 3

GroveTwoRGBLedMatrixClass matrix;
ChainableLED leds(D4, D5, NUM_LEDS);

TaskHandle_t task_ChLeds = NULL;
TaskHandle_t task_MatrixLed = NULL;

float hue = 0.0;
boolean up = true;

uint64_t example[] = {

    0x4d4d4d4d4d4d4d4d,
    0x4d4d4d4d4d4d4d4d,
    0x4d4d4d4d4d4d4d4d,
    0x4d4d4d0d4d4d4d4d,
    0xffffffffffffffff,
    0x4d4d4d4d4d4d4d4d,
    0x404d4d4d414d4d4d,
    0x4d4d4d4d4d4d4d4d,

    0xffffffffffffffff,
    0xff90fffff5ffffff,
    0xffffffffffff4dff,
    0xffffff4dffffffff,
    0xffffffffffffffff,
    0xffffffffffffffff,
    0xffff25ffffffffff,
    0xffffff25ffffffff,

    0x5fff5fffffffff5f,
    0xfff5ffffffffffff,
    0xfffffffffff5ffff,
    0xff5ffff5ffffffff,
    0xfffffffffffff5ff,
    0xfffff5ffffffffff,
    0xfffffffffff5f5ff,
    0xffffffff5fffffff};

void manageChLeds(void *pvParameters)
{
    int TIMER_LED = 1000;

    for (;;)
    {
        int *rgb_colors = new int[3];

        for (int i = 0; i < NUM_LEDS; i++)
        {

            int r = random(3);
            rgb_colors[r] = 0;
            rgb_colors[abs(r - 1)] = 255;
            rgb_colors[abs(r - 2)] = 25 + random(175);
            long start = xTaskGetTickCount();
            leds.setColorRGB(i, rgb_colors[0], rgb_colors[1], rgb_colors[2]);
            long stop = xTaskGetTickCount() - start;
            SERIAL.printf("Updation duration (Chain leds) = %ld\n", stop);
        }

        vTaskDelay(pdMS_TO_TICKS(TIMER_LED));
    }
}

void manageMatrixLed(void *pvParameters)
{
    int TIMER_LED = 1000;

    for (;;)
    {
        for (int i = 0; i < 3; i++)
        {
            long start = xTaskGetTickCount();
            matrix.displayFrames(example + i * 8, TIMER_LED, false, 1);
            long stop = xTaskGetTickCount() - start;
            SERIAL.printf("Updation duration (Led Matrix) = %ld\n", stop);
            vTaskDelay(pdMS_TO_TICKS(TIMER_LED));
        }
    }
}

void setup()
{
    // LED matrix init
    Wire.begin();
    SERIAL.begin(115200);
    delay(1000);
    uint32_t version = 0;
    version = matrix.getTestVersion();
    SERIAL.print("version = ");
    SERIAL.println(version, HEX);
    SERIAL.println("Matrix init success!!!");

    // Chainable LEDs init
    leds.init();

    //xTaskCreate( vTaskPeriodic, "vTaskPeriodic", 10000, NULL, 1, NULL );
    xTaskCreate(manageChLeds, "manageChLeds", 10000, NULL, 1, &task_ChLeds);
    xTaskCreate(manageMatrixLed, "manageMatrixLed", 10000, NULL, 1, &task_MatrixLed);

    for (;;)
        ;
}

void loop()
{
}