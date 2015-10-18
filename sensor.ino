#include <DHT.h>
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

bool getCurrentPIR() {
	return digitalRead(PIN_PIR) == HIGH;
}

int getPIRCount() {
	return pir_count;
}

bool getButton() {
	return digitalRead(PIN_BUTTON) == HIGH;
}

float getBrightness(){
	int reading = analogRead(PIN_BRIGHTNESS);
	float voltage = ((long)reading * 3300) / 4095; //real voltage
	float microamp = (voltage * 1000) / 330; //micro amp
	float lx = microamp / (290 / 100);
	return lx;
}

float getMPLTemp(){
	return pressureSensor.getTemperature();
}

float getPressure(){
	return pressureSensor.getPressure() * 10;
}

float getHumidity(){
  return dht.readHumidity();
}

float getTemprature(){
  return dht.readTemperature();
}

