#include "chainable_led_display.hpp"

// CL variables
ChainLedsDisplayType current_cldt;
int **current_RGB;
int current_A;
TaskHandle_t task_ChLeds = NULL;
ChainableLED leds(D4, D5, NUM_LEDS);

//################################ CL methods ################################
void generateRandomRGB(int i)
{
    int r = random(3);
    current_RGB[i][r] = 0;
    current_RGB[i][abs(r - 1)] = 255;
    current_RGB[i][abs(r - 2)] = 25 + random(175);
}

void generateRandomRGB()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        generateRandomRGB(i);
    }
}

void setCurrentRGB(int i, int R, int G, int B, int A)
{
    current_RGB[i][0] = R % 256;
    current_RGB[i][1] = G % 256;
    current_RGB[i][2] = B % 256;
    current_A = A % 256;
}

void setCurrentRGB(int i, int R, int G, int B)
{
    setCurrentRGB(i, R, G, B, 255);
}

void setCurrentRGB(int i, String rgb_string)
{
    int r = extractHexa(rgb_string.substring(1, 3).c_str());
    int g = extractHexa(rgb_string.substring(3, 5).c_str());
    int b = extractHexa(rgb_string.substring(5, 7).c_str());
    SERIAL.printf("Changing : %s to --> R  :  %d / G : %d / B : %d\n", rgb_string, r, g, b);
    setCurrentRGB(i, r, g, b);
}

void displayCurrentColorCH()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds.setColorRGB(i, current_RGB[i][0], current_RGB[i][1], current_RGB[i][2]);
        //SERIAL.printf("Updation duration (Chain leds) = %ld\n", stop);
    }
    vTaskDelay(pdMS_TO_TICKS(TIMER_LED));
}

void displayRandomColorCH()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        generateRandomRGB(i);
        leds.setColorRGB(i, current_RGB[i][0], current_RGB[i][1], current_RGB[i][2]);
    }
    vTaskDelay(pdMS_TO_TICKS(TIMER_LED));
}

void changeDisplayTypeCL(ChainLedsDisplayType cldt)
{
    current_cldt = cldt;
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
        /*int r = random(3);
        changeDisplayTypeCL(r);*/
        switch (current_cldt)
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
        default:
        {
            SERIAL.printf("Default CL");
            vTaskDelay(pdMS_TO_TICKS(TIMER_LED));
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

    ChainLedsDisplayType cldt = ChainLedsDisplayType::Random;
    changeDisplayTypeCL(cldt);

    if (current_RGB == nullptr)
    {
        current_RGB = new int *[NUM_LEDS];
        for (int i = 0; i < NUM_LEDS; i++)
        {
            current_RGB[i] = new int[3];
            generateRandomRGB(i);
        }
    }
    // Chainable LEDs init
    leds.init();

    // TODO : get cldt from backend
    current_A = 100;
}