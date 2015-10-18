#include <ESP8266WiFi.h>
#include <functional>
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
    server->onNotFound(handleNotFound);

    server->on("/sensor/button",     []{ respondValue(getButton); });
    server->on("/sensor/brightness", []{ respondValue(getBrightness); });
    server->on("/sensor/pressure",   []{ respondValue(getPressure); });
    server->on("/sensor/mpltemp",    []{ respondValue(getMPLTemp); });
    server->on("/sensor/temprature", []{ respondValue(getTemprature); });
    server->on("/sensor/humidity",   []{ respondValue(getHumidity); });
    server->on("/sensor/currentpir", []{ respondValue(getCurrentPIR); });
    server->on("/sensor/pircount",   []{ respondValue(getPIRCount); });

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

void respondValue(std:function<void(JsonObject&)> getter) {
    String result;

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    getter(root);
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
