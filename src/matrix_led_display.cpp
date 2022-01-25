#include "matrix_led_display.hpp"

// Display devices
GroveTwoRGBLedMatrixClass matrix;

// Tasks
TaskHandle_t task_MatrixLed = NULL;

// Global variables
float hue = 0.0;
boolean up = true;

// ML variables
MatrixDisplayType current_mdt;
std::string current_message = "Nothing";
int display_color = 0X11;
boolean changed_ml;

// Presets
int current_preset_ml = 0;

// ################################ ML methods ################################

void changeCurrentPresetML(int preset)
{
    current_preset_ml = preset % 6;
}

void changeDisplayTypeML(MatrixDisplayType mdt)
{
    SERIAL.printf("Changing current ML mdt to : %d\n", mdt);
    changed_ml = true;
    current_mdt = mdt;
}

void changeDisplayTypeML(int mdt_int)
{
    MatrixDisplayType type = MatrixDisplayType::Preset;
    if (mdt_int % 3 == 0)
    {
        type = MatrixDisplayType::Message;
    }
    else if (mdt_int % 3 == 1)
    {
        type = MatrixDisplayType::Beat;
    }
    if (mdt_int % 3 == 2)
    {
        type = MatrixDisplayType::Preset;
    }
    changeDisplayTypeML(type);
}

void displayExampleML()
{
    SERIAL.println("Displaying current preset");
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
        /*SERIAL.println("Displaying current message : ");
        SERIAL.println(current_message.c_str());*/
        matrix.displayString(current_message.c_str(), wait_time, true, display_color);
    }
    vTaskDelay(pdMS_TO_TICKS(wait_time));
}

void manageMatrixLed(void *pvParameters)
{
    SERIAL.println("Starting matrix led task");
    MatrixDisplayType type = MatrixDisplayType::Message;
    changeDisplayTypeML(type);
    for (;;)
    {
        //SERIAL.printf("Current MDT = %d / changed ? %s\n", *current_mdt, changed_ml ? "true" : "false");
        //SERIAL.printf("Current MDT = %d / changed ? %s\n", *current_mdt, changed_ml ? "true" : "false");
        /*int r = random(3);
        changeDisplayTypeML(r);*/
        //SERIAL.printf("Random : %d\n", r);

        if (changed_ml)
        {
            switch (current_mdt)
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
            default:
            {
                SERIAL.printf("Default ML");
                break;
            }
            } // switch end
              //changed_ml = false;
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
void createMLTasks()
{
    xTaskCreate(manageMatrixLed, "manageMatrixLed", 10000, NULL, 1, &task_MatrixLed);
}
void setupLEDMatrix()
{
    // LED matrix init
    Wire.begin();
    delay(1000);
    SERIAL.println("Matrix init success!!!");

    // TODO : get mdt from backend
    changeDisplayTypeML(MatrixDisplayType::Message);
    changed_ml = true;
}
