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
        return;
    }
    wifi_config config;
    if (loadConfig(config) != 0) {
        Serial.println("Failed to load config file");
        return;
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

    server->on("/sensor/all", respondAllValues);

    // 各センサ値用のハンドラを追加する．
    // ラムダ式内からコード内の通常関数は呼び出せず(not in scope)，
    // 更にaddHandlerに関しても 'class ESP8266WebServer' has no member
    // だったため，このような実装にするしかなかった
    std::function<void(std::function<void(JsonObject&)>)> handler = [](std::function<void(JsonObject&)> getter){
        String result;

        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        getter(root);
        root.printTo(result);

        Serial.println(result);
        server->send(200, "application/json", result);
    };
    server->on("/sensor/button",     [handler]{ handler(getButton); });
    server->on("/sensor/brightness", [handler]{ handler(getBrightness); });
    server->on("/sensor/pressure",   [handler]{ handler(getPressure); });
    server->on("/sensor/mpltemp",    [handler]{ handler(getMPLTemp); });
    server->on("/sensor/temprature", [handler]{ handler(getTemprature); });
    server->on("/sensor/humidity",   [handler]{ handler(getHumidity); });
    server->on("/sensor/currentpir", [handler]{ handler(getCurrentPIR); });
    server->on("/sensor/pircount",   [handler]{ handler(getPIRCount); });

    server->begin();
    Serial.println("HTTP server started !!");
}

void loop() {
    if (server != NULL) server->handleClient();
}

void respondAllValues() {
    String result;

    StaticJsonBuffer<1000> jsonBuffer;
    JsonArray& root = jsonBuffer.createArray();

    getButton    (root.createNestedObject());
    getBrightness(root.createNestedObject());
    getPressure  (root.createNestedObject());
    getMPLTemp   (root.createNestedObject());
    getTemprature(root.createNestedObject());
    getHumidity  (root.createNestedObject());
    getCurrentPIR(root.createNestedObject());
    getPIRCount  (root.createNestedObject());

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
