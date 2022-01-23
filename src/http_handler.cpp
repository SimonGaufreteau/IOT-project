#include <WiFi.h>
#include <HTTPClient.h>
#include "utils.hpp"

// Tasks
TaskHandle_t task_HTTPPing = NULL;

const char *ssid = "exi_kubuntu";
const char *password = "J41m3L3sPommes";

//Your Domain name with URL path or IP address with path
String serverName = "http://10.42.0.1:8000";

unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

void setupHTPPHandler()
{
    WiFi.begin(ssid, password);
    SERIAL.println("Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        SERIAL.print(".");
    }
    SERIAL.println("");
    SERIAL.print("Connected to WiFi network with IP Address: ");
    SERIAL.println(WiFi.localIP());

    SERIAL.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}
void taskPing(void *pvParameters)
{
    for (;;)
    {
        //Send a ping every 5 seconds
        if ((millis() - lastTime) > timerDelay)
        {
            //Check WiFi connection status
            if (WiFi.status() == WL_CONNECTED)
            {
                HTTPClient http;

                String serverPath = serverName;
                SERIAL.println("Starting a ping request");

                // Your Domain name with URL path or IP address with path
                http.begin(serverPath.c_str());

                // Send HTTP GET request
                int httpResponseCode = http.GET();

                if (httpResponseCode > 0)
                {
                    SERIAL.print("HTTP Response code: ");
                    SERIAL.println(httpResponseCode);
                    String payload = http.getString();
                    SERIAL.println(payload);
                }
                else
                {
                    SERIAL.print("Error code: ");
                    SERIAL.println(httpResponseCode);
                }
                // Free resources
                http.end();
            }
            else
            {
                Serial.println("WiFi Disconnected");
            }
            lastTime = millis();
        }
    }
}

void setupTasksHTTP()
{
    setupHTPPHandler();
    xTaskCreate(taskPing, "taskPing", 10000, NULL, 1, &task_HTTPPing);
}