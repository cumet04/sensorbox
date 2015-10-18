#include <DHT.h>
#include <ArduinoJson.h>
#include "Adafruit_MPL115A2.h"

#define DHTTYPE DHT22	// DHT 22 (AM2302)
#define PIN_BUTTON 0
#define PIN_BRIGHTNESS A0
#define PIN_SDA 12
#define PIN_SCL 13
#define PIN_DHT 4
#define PIN_PIR 14

Adafruit_MPL115A2 pressureSensor;
DHT dht(PIN_DHT, DHTTYPE);
Ticker timer;
int pir_count = 0;


void initializeSensors() {
	pinMode(PIN_BUTTON, INPUT);
	pinMode(PIN_PIR, INPUT);
	pressureSensor.begin(PIN_SDA, PIN_SCL);
	dht.begin();
	timer.attach(1.0, countPIR);
	return;
}

void countPIR() {
	static int time = 0;
	static int count = 0;
	if (time++ > 30) {
		pir_count = count;
		count = 0;
		time = 0;
	}

	if (digitalRead(PIN_PIR) == HIGH) count++;
}

// getValue functions ----------------------------------------------------------

void getCurrentPIR(JsonObject& root) {
    root["name"] = "currentpir";
    root["value"] = digitalRead(PIN_PIR) == HIGH;
}

void getPIRCount(JsonObject& root) {
    root["name"] = "pircount";
    root["value"] = pir_count;
}

void getButton(JsonObject& root) {
    root["name"] = "button";
    root["value"] = digitalRead(PIN_BUTTON) == HIGH;
}

void getBrightness(JsonObject& root){
	int reading = analogRead(PIN_BRIGHTNESS);
	float voltage = ((long)reading * 3300) / 4095; //real voltage
	float microamp = (voltage * 1000) / 330; //micro amp
	float lx = microamp / (290 / 100);

    root["name"] = "brightness";
    root["value"] = lx;
    root["unit"] = "unknown";
}

void getMPLTemp(JsonObject& root){
    root["name"] = "mpltemp";
    root["value"] = pressureSensor.getTemperature();
    root["unit"] = "C";
}

void getPressure(JsonObject& root){
    root["name"] = "pressure";
    root["value"] = pressureSensor.getPressure() * 10;
    root["unit"] = "hPa";
}

void getHumidity(JsonObject& root){
    root["name"] = "humidity";
    root["value"] = dht.readHumidity();
    root["unit"] = "unknown";
}

void getTemprature(JsonObject& root){
    root["name"] = "temprature";
    root["value"] = dht.readTemperature();
    root["unit"] = "C";
}

