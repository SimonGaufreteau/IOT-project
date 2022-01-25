#include "matrix_led_display.hpp"
#include <CircularBuffer.h>
#include "sound_sensor.hpp"

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
const int matrix_size = 8;
CircularBuffer<int, matrix_size> current_sound_buffer;
const int max_buffer = 3000;

int current_sound = 0;

// Presets
int current_preset_ml = 0;

// Constant delays
const int MATRIX_SOUND_DELAY = 100;

// ################################ ML methods ################################
void changeCurrentSound(int n)
{
    current_sound = n;
}

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

void updateSoundBuffer(int soundValue)
{
    if (current_sound_buffer.isFull())
    {
        current_sound_buffer.shift();
    }
    current_sound_buffer.push(soundValue);
}

void displayCurrentSoundBuffer()
{
    SERIAL.println("Current sound buffer : ");
    for (int i = 0; i < matrix_size; i++)
    {
        SERIAL.printf(" i : %d \n", current_sound_buffer[i]);
    }
}

void displayCurrentSound()
{
    //displayCurrentSoundBuffer();

    // Setup matrix
    uint8_t *res = new uint8_t[matrix_size * matrix_size];
    for (int i = 0; i < matrix_size * matrix_size; i++)
    {
        res[i] = 0xff;
    }

    // Print peaks
    for (int i = 0; i < matrix_size; i++)
    {
        int buffer_val = current_sound_buffer[i] * 2;
        //SERIAL.printf("Buffer : %d\n", buffer_val);
        int min_val = min(buffer_val, max_buffer);
        //SERIAL.printf("Min : %d\n", min_val);
        int multiplied = min_val * matrix_size;
        //SERIAL.printf("Multiplied : %d\n", multiplied);
        double peak = (double)multiplied / (double)max_buffer;
        //SERIAL.printf("Peak : %f\n", peak);

        for (int j = 0; j < peak; j++)
        {
            res[j + matrix_size * i] = display_color;
        }
    }
    /*for (int i = 0; i < matrix_size; i++)
    {
        for (int j = 0; j < matrix_size; j++)
        {
            SERIAL.printf("%d ", res[i + j * matrix_size]);
        }
        SERIAL.print("\n");
    }*/
    matrix.displayFrames(res, MATRIX_SOUND_DELAY, false, 1);
    vTaskDelay(MATRIX_SOUND_DELAY);
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
        int cval = getSoundValue();
        //SERIAL.printf("Current sound value is : %d\n", cval);
        updateSoundBuffer(cval);
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
                displayCurrentSound();
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
                vTaskDelay(pdMS_TO_TICKS(1000));

                break;
            }
            } // switch end
              //changed_ml = false;
        }
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
