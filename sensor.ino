#define PIN_BUTTON 0


void initializePinMode() {
	pinMode(PIN_BUTTON, INPUT);
	return;
}

bool getButton() {
	return digitalRead(PIN_BUTTON) == HIGH;
}