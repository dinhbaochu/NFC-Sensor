#include "gpt.h" 
Ntag ntag;

void setup(){

ntag.begin();
int number=0;
number = analogRead(A0);

byte eepromdata[4];

eepromdata[0] = 0x30+number / 1000;          // Thousands place
eepromdata[1] = 0x30+(number / 100) % 10;    // Hundreds place
eepromdata[2] = 0x30+(number / 10) % 10;     // Tens place
eepromdata[3] = 0x30+number % 10;            // Units place


ntag.writeEeprom(16*1, eepromdata, 4);}

void loop() {}
