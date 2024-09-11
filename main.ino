#include "tagwrite.h" 
Ntag ntag;

void setup(){

ntag.begin();
int number=0;
number = analogRead(A0);
number=number*330/1023;

byte eepromdata[2];

eepromdata[0] = 0x30 + (number / 10) % 10;   // Tens place
eepromdata[1] = 0x30 + number % 10;          // Units place


ntag.writeEeprom(16*1, eepromdata, 2);}

void loop() {}
