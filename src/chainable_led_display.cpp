#include "chainable_led_display.hpp"
// CL variables
ChainLedsDisplayType *current_cldt;
int *current_RGB;
int current_A;
TaskHandle_t task_ChLeds = NULL;
ChainableLED leds(D4, D5, NUM_LEDS);

//################################ CL methods ################################
void generateRandomRGB()
{
    int r = random(3);
    current_RGB[r] = 0;
    current_RGB[abs(r - 1)] = 255;
    current_RGB[abs(r - 2)] = 25 + random(175);
}

void setCurrentRGB(int R, int G, int B, int A)
{
    current_RGB[0] = R % 256;
    current_RGB[1] = G % 256;
    current_RGB[2] = B % 256;
    current_A = A % 256;
}

void setCurrentRGB(int R, int G, int B)
{
    setCurrentRGB(R, G, B, 255);
}

void displayCurrentColorCH()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds.setColorRGB(i, current_RGB[0], current_RGB[1], current_RGB[2]);
        //SERIAL.printf("Updation duration (Chain leds) = %ld\n", stop);
    }
    vTaskDelay(pdMS_TO_TICKS(TIMER_LED));
}

void displayRandomColorCH()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        generateRandomRGB();
        leds.setColorRGB(i, current_RGB[0], current_RGB[1], current_RGB[2]);
    }
    vTaskDelay(pdMS_TO_TICKS(TIMER_LED));
}

void changeDisplayTypeCL(ChainLedsDisplayType cldt)
{
    current_cldt = &cldt;
    SERIAL.printf("Changing current CL cldt to : %d\n", cldt);
}

void changeDisplayTypeCL(int mdt_int)
{
    ChainLedsDisplayType type = ChainLedsDisplayType::Preset;
    if (mdt_int % 3 == 0)
    {
        type = ChainLedsDisplayType::Color;
    }
    else if (mdt_int % 3 == 1)
    {
        type = ChainLedsDisplayType::Random;
    }
    if (mdt_int % 3 == 2)
    {
        type = ChainLedsDisplayType::Preset;
    }
    changeDisplayTypeCL(type);
}

void manageChLeds(void *pvParameters)
{
    SERIAL.println("Starting chainable leds task");

    ChainLedsDisplayType cldt = ChainLedsDisplayType::Random;
    changeDisplayTypeCL(cldt);
    for (;;)
    {
        int r = random(3);
        changeDisplayTypeCL(r);
        switch (*current_cldt)
        {
        case ChainLedsDisplayType::Color:
        {
            //SERIAL.println("CL COLOR");
            // Nothing to do on color, we just display as usual
            displayCurrentColorCH();
            break;
        }
        case ChainLedsDisplayType::Preset:
        {
            //SERIAL.println("CL PRESET");
            //TODO : manage presets
            break;
        }
        case ChainLedsDisplayType::Random:
        {
            //SERIAL.println("CL RANDOM");
            displayRandomColorCH();
            break;
        }
        } // switch end
    }
}

void createCHTasks()
{
    xTaskCreate(manageChLeds, "manageChLeds", 10000, NULL, 1, &task_ChLeds);
}

void setupLEDChain()
{
    if (current_cldt == nullptr)
    {
        ChainLedsDisplayType cldt = ChainLedsDisplayType::Random;
        changeDisplayTypeCL(cldt);
    }
    if (current_RGB == nullptr)
    {
        current_RGB = new int[3];
        generateRandomRGB();
    }
    // Chainable LEDs init
    leds.init();

    // TODO : get cldt from backend
    current_A = 100;
}