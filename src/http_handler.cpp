#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include "ArduinoJson.h"
#include "chainable_led_display.hpp"
#include "matrix_led_display.hpp"

// Tasks
TaskHandle_t task_HTTPPing = NULL;
TaskHandle_t task_HTTPServer = NULL;

const char *ssid = "Freebox-A1D499";
const char *password = "BrSR1wzKaCW(1pL5";

// Board name
String boardName = "BoardIOT1";

//Your Domain name with URL path or IP address with path
String serverName = "http://78.198.193.139:8080";

unsigned long lastTime = 0;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

// Set web server port number to 80
WebServer server(8081);

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

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
                String serverPath = serverName + "/ping/1/" + boardName + "/78.198.193.139:8081";
                //SERIAL.println("Starting a ping request");

                // Your Domain name with URL path or IP address with path
                http.begin(serverPath.c_str());

                // Send HTTP GET request
                int httpResponseCode = http.GET();

                if (httpResponseCode > 0)
                {
                    //SERIAL.print("HTTP Response code: ");
                    //SERIAL.println(httpResponseCode);
                    String payload = http.getString();
                    //SERIAL.println(payload);
                }
                else
                {
                    SERIAL.print("Error code on ping: ");
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

void handle_NotFound()
{
    server.send(404, "text/plain", "Not found");
}

void handleMessage()
{
    // If a new client connects,
    SERIAL.println("Got a message request.");
    String postBody = server.arg("plain");
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, postBody);
    SERIAL.print("DeSerialized the json : ");
    SERIAL.println(error.c_str());
    JsonObject postObj = doc.as<JsonObject>();
    if (postObj.containsKey("message"))
    {
        String message = postObj.getMember("message");
        SERIAL.println("Message received correctly : " + message + " / Sending the response to the server.");
        server.send(200, "text/html", "Message received : " + message);
        setCurrentMessage(message.c_str());
        MatrixDisplayType mdt = MatrixDisplayType::Message;
        changeDisplayTypeML(mdt);
    }
    else
    {
        SERIAL.println("Could not parse the message correctly.");
        server.send(404, "text/html", "Error, message not found.");
    }
}

// Example

void handleChangeColor()
{
    // If a new client connects,
    SERIAL.println("Got a color change request.");
    String postBody = server.arg("plain");
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, postBody);
    SERIAL.print("DeSerialized the json : ");
    SERIAL.println(error.c_str());
    JsonObject postObj = doc.as<JsonObject>();
    if (postObj.containsKey("color"))
    {
        String color = postObj.getMember("color");
        SERIAL.println("Color received correctly : " + color + " / Sending the response to the server.");
        server.send(200, "text/html", "Color received : " + color);
        //setCurrentRGB()
    }
    else
    {
        SERIAL.println("Could not parse the message correctly.");
        server.send(404, "text/html", "Error, message not found.");
    }
}

void handleChangeColorPreset()
{
    // If a new client connects,
    SERIAL.println("Got a changeColorPreset request.");
    String postBody = server.arg("plain");
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, postBody);
    SERIAL.print("DeSerialized the json : ");
    SERIAL.println(error.c_str());
    JsonObject postObj = doc.as<JsonObject>();
    if (postObj.containsKey("led1"))
    {
        String led1 = postObj.getMember("led1");
        String led2 = postObj.getMember("led2");
        String led3 = postObj.getMember("led3");
        String res = "Message received correctly : 1 : " + led1 + " / 2 : " + led2 + " / 3 : " + led3;
        SERIAL.println(res + " / Sending the response to the server.");
        server.send(200, "text/html", res);
        SERIAL.println("Setting led colors...");
        setCurrentRGB(0, led1);
        setCurrentRGB(1, led2);
        setCurrentRGB(2, led3);
        ChainLedsDisplayType cldt = ChainLedsDisplayType::Color;
        changeDisplayTypeCL(cldt);
    }
    else
    {
        SERIAL.println("Could not parse the message correctly.");
        server.send(404, "text/html", "Error, led1 not found.");
    }
}

void handleSoundSensor()
{
    SERIAL.println("Got a sound sensor request.");
    String postBody = server.arg("plain");
    DynamicJsonDocument doc(512);
    server.send(200, "text/html", "Sound sensor received correctly.");
    SERIAL.println("Setting matrix sound sensor...");
    MatrixDisplayType mdt = MatrixDisplayType::Beat;
    changeDisplayTypeML(mdt);
}

void handleRandomColors()
{
    SERIAL.println("Got a random colors request.");
    String postBody = server.arg("plain");
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, postBody);
    SERIAL.print("DeSerialized the json : ");
    SERIAL.println(error.c_str());
    JsonObject postObj = doc.as<JsonObject>();
    server.send(200, "text/html", "Random colors received correctly.");
    SERIAL.println("Setting led random colors...");
    generateRandomRGB();
    ChainLedsDisplayType cldt = ChainLedsDisplayType::Random;
    changeDisplayTypeCL(cldt);
}

void taskServer(void *pvParameters)
{
    SERIAL.println("Starting the server.");
    server.begin();
    for (;;)
    {
        //SERIAL.println("Waiting for a client...");
        server.handleClient();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
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

    SERIAL.println("Setting up routes for server");
    server.on("/displayMessage", HTTP_POST, handleMessage);
    server.on("/changeColor", HTTP_POST, handleChangeColor);
    server.on("/changeColorPreset", HTTP_POST, handleChangeColorPreset);
    server.on("/randomColors", HTTP_POST, handleRandomColors);
    server.on("/soundSensor", HTTP_POST, handleSoundSensor);
    server.onNotFound(handle_NotFound);
}

void setupTasksHTTP()
{
    setupHTPPHandler();
    //xTaskCreate(taskPing, "taskPing", 10000, NULL, 1, &task_HTTPPing);
    xTaskCreate(taskServer, "taskServer", 10000, NULL, 1, &task_HTTPServer);
}