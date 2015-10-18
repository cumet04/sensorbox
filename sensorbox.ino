#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <DHT.h>
#include <Ticker.h>
#include "FS.h"

#include "config.h"


ESP8266WebServer *server = NULL;

void setup() {
    // initialize
    Serial.begin(115200);
    Serial.println();
    if (!SPIFFS.begin()) {
        Serial.println("Failed to mount file system");
        goto restart;
    }
    wifi_config config;
    if (loadConfig(config) != 0) {
        Serial.println("Failed to load config file");
        goto restart;
    }
    initializeSensors();

    // connect to wifi
    WiFi.config(*config.ip, *config.gateway, *config.subnet);
    WiFi.begin(config.ssid, config.pass);
    Serial.print("connecting.");
    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nwifi connected.");

    // server setup
    server = new ESP8266WebServer(8080);
    server->on("/sensor/", handleHelp);
    server->on("/sensor/help", handleHelp);
    server->on("/sensor/button", handleGet_button);
    server->on("/sensor/brightness", handleGet_brightness);
    server->on("/sensor/pressure", handleGet_pressure);
    server->on("/sensor/mpltemp", handleGet_MPLTemp);
    server->on("/sensor/temprature", handleGet_temprature);
    server->on("/sensor/humidity", handleGet_humidity);
    server->on("/sensor/currentpir", handleGet_currentPIR);
    server->on("/sensor/pircount", handleGet_PIRCount);
    server->onNotFound(handleNotFound);
    server->begin();
    Serial.println("HTTP server started !!");
    return;

restart:
    Serial.println("setup failed. It's to restart after 5 secs.");
    delay(5 * 1000);
    ESP.restart();
}

void loop() {
    if (server != NULL) server->handleClient();
}

void handleGet_button() {
    String result;

    StaticJsonBuffer<100> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["name"] = "button";
    root["value"] = getButton();
    root.printTo(result);

    Serial.println(result);

    server->send(200, "application/json", result);
}

void handleGet_PIRCount() {
    String result;

    StaticJsonBuffer<100> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["name"] = "pircount";
    root["value"] = getPIRCount();
    root["max"] = 30;
    root.printTo(result);

    Serial.println(result);

    server->send(200, "application/json", result);
}

void handleGet_currentPIR() {
    String result;

    StaticJsonBuffer<100> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["name"] = "currentpir";
    root["value"] = getCurrentPIR();
    root.printTo(result);

    Serial.println(result);

    server->send(200, "application/json", result);
}

void handleGet_brightness() {
    String result;

    StaticJsonBuffer<100> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["name"] = "brightness";
    root["value"] = getBrightness();
    root.printTo(result);

    Serial.println(result);

    server->send(200, "application/json", result);
}

void handleGet_pressure() {
    String result;

    StaticJsonBuffer<100> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["name"] = "pressure";
    root["value"] = getPressure();
    root.printTo(result);

    Serial.println(result);

    server->send(200, "application/json", result);
}

void handleGet_MPLTemp() {
    String result;

    StaticJsonBuffer<100> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["name"] = "mpltemp";
    root["value"] = getMPLTemp();
    root.printTo(result);

    Serial.println(result);

    server->send(200, "application/json", result);
}

void handleGet_temprature() {
    String result;

    StaticJsonBuffer<100> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["name"] = "temprature";
    root["value"] = getTemprature();
    root.printTo(result);

    Serial.println(result);

    server->send(200, "application/json", result);
}

void handleGet_humidity() {
    String result;

    StaticJsonBuffer<100> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["name"] = "humidity";
    root["value"] = getHumidity();
    root.printTo(result);

    Serial.println(result);

    server->send(200, "application/json", result);
}

void handleHelp() {
    // help
    server->send(200, "text/plain", "help");
}

void handleNotFound() {
    // not found
    server->send(404, "text/plain", "not found");
}