

void setup() {
  // put your setup code here, to run once:
  portMode(PIN_PB0, OUTPUT);
}

void loop() {
  // Use PIN_PB0 macro to refer to pin PB0 (Arduino pin 0 with the standard and sanguino pinout)
  digitalWrite(PIN_PB0, HIGH);
}
