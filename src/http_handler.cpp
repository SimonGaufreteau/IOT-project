#include <WiFi.h>
#include <HTTPClient.h>
#include "utils.hpp"
#include <WebServer.h>
#include "ArduinoJson.h"

// Tasks
TaskHandle_t task_HTTPPing = NULL;
TaskHandle_t task_HTTPServer = NULL;

const char *ssid = "";
const char *password = "";

//Your Domain name with URL path or IP address with path
String serverName = "";

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
                String serverPath = serverName + "/ping/1/78.198.193.139:8081";
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
    currentTime = millis();
    previousTime = currentTime;
    SERIAL.println("Got a message request.");
    String postBody = server.arg("plain");
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, postBody);
    SERIAL.print("Serialized the json : ");
    SERIAL.println(error.c_str());
    JsonObject postObj = doc.as<JsonObject>();
    if (postObj.containsKey("message"))
    {
        String message = postObj.getMember("message");
        SERIAL.println("Message received correctly : " + message + " / Sending the response to the server.");
        server.send(200, "text/html", "Message received : " + message);
    }
    else
    {
        SERIAL.println("Could not parse the message correctly.");
        server.send(404, "text/html", "Error, message not found.");
    }
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
    server.onNotFound(handle_NotFound);
}

void setupTasksHTTP()
{
    setupHTPPHandler();
    xTaskCreate(taskPing, "taskPing", 10000, NULL, 1, &task_HTTPPing);
    xTaskCreate(taskServer, "taskServer", 10000, NULL, 1, &task_HTTPServer);
}