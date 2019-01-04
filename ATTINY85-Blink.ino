/*
 * This blink program is for ATTINY85
 * if the LED blinks every 8 seconds you need to 
 * burn the bootloader to use 8MHz clock.
 */

const int LED=4;

void setup() {
  //initialize pin 4
  pinMode(LED, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
