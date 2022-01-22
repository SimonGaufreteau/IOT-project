#include "display.hpp"

// Display devices
GroveTwoRGBLedMatrixClass matrix;
ChainableLED leds(D4, D5, NUM_LEDS);

// Tasks
TaskHandle_t task_ChLeds = NULL;
TaskHandle_t task_MatrixLed = NULL;

// Global variables
float hue = 0.0;
boolean up = true;

// ML variables
MatrixDisplayType *current_mdt = nullptr;
std::string current_message = "Nothing";
int display_color = 0X11;
boolean changed_ml = false;

// CL variables
ChainLedsDisplayType cldt;
int *current_RGB;
int current_A;

// Presets
int current_preset_ml = 0;

// ML methods

void changeCurrentPresetML(int preset)
{
    current_preset_ml = preset % 6;
}

void changeDisplayTypeML(MatrixDisplayType mdt)
{
    changed_ml = true;
    current_mdt = &mdt;
}

void displayExampleML()
{
    matrix.displayColorAnimation(current_preset_ml, 5000, true);
    vTaskDelay(pdMS_TO_TICKS(5000));
}

void setCurrentMessage(std::string message)
{
    current_message = message;
}
void setCurrentMessage(const char *message)
{
    changed_ml = true;
    current_message = message;
}

void displayCurrentMessageML(boolean changed)
{
    int wait_time = current_message.length() * 1000;
    if (changed)
    {
        //SERIAL.println("Displaying current message : ");
        //SERIAL.println(current_message.c_str());
        matrix.displayString(current_message.c_str(), wait_time, true, display_color);
    }
    vTaskDelay(pdMS_TO_TICKS(wait_time));
}

void manageMatrixLed(void *pvParameters)
{

    for (;;)
    {
        SERIAL.printf("Current MDT = %d\n", *current_mdt);

        if (changed_ml)
        {
            switch (*current_mdt)
            {
            case MatrixDisplayType::Message:
            {
                displayCurrentMessageML(true);
                break;
            }
            case MatrixDisplayType::Beat:
            {
                //TODO
                break;
            }
            case MatrixDisplayType::Preset:
            {
                displayExampleML();
                break;
            }
            } // switch end
            changed_ml = false;
        }
    }
}

void setupLEDMatrix()
{
    // LED matrix init
    Wire.begin();
    delay(1000);
    SERIAL.println("Matrix init success!!!");

    // TODO : get mdt from backend
    if (current_mdt == nullptr)
    {
        changeDisplayTypeML(MatrixDisplayType::Preset);
    }
}

// CL methods
void generateRandomRGB()
{
    int r = random(3);
    current_RGB[r] = 0;
    current_RGB[abs(r - 1)] = 255;
    current_RGB[abs(r - 2)] = 25 + random(175);
}

void manageChLeds(void *pvParameters)
{
    for (;;)
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            generateRandomRGB();
            long start = xTaskGetTickCount();
            leds.setColorRGB(i, current_RGB[0], current_RGB[1], current_RGB[2]);
            long stop = xTaskGetTickCount() - start;
            //SERIAL.printf("Updation duration (Chain leds) = %ld\n", stop);
        }

        vTaskDelay(pdMS_TO_TICKS(TIMER_LED));
    }
}

void setupLEDChain()
{
    // Chainable LEDs init
    leds.init();

    // TODO : get cldt from backend
    current_A = 100;
    current_RGB = new int[3];
}

void createDisplayTasks()
{
    //xTaskCreate( vTaskPeriodic, "vTaskPeriodic", 10000, NULL, 1, NULL );
    xTaskCreate(manageChLeds, "manageChLeds", 10000, NULL, 1, &task_ChLeds);
    xTaskCreate(manageMatrixLed, "manageMatrixLed", 10000, NULL, 1, &task_MatrixLed);
}