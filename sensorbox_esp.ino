#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "FS.h"

#include "config.h"


ESP8266WebServer *server = NULL;

void setup() {
    delay(3 * 1000);

    // initialize
    Serial.begin(115200);
    if (!SPIFFS.begin()) {
        Serial.println("Failed to mount file system");
        return;
    }
    wifi_config config;
    if (loadConfig(config) != 0) {
        Serial.println("Failed to load config file");
        return;
    }

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
    server->onNotFound(handleNotFound);
    server->begin();
    Serial.println("HTTP server started !!");
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

void handleHelp() {
    // help
    server->send(200, "text/plain", "help");
}

void handleNotFound() {
    // not found
    server->send(404, "text/plain", "not found");
}