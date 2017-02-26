//#include <util.h>
#include <ctype.h>

/*

 PINBELEGEUNG
 
 RS232
 
 PIN  5    TX
 PIN  6    RX
 
 ESP8266
 
 
 
 
 */

#define bit9600Delay 100
#define halfBit9600Delay 50
#define bit4800Delay 188
#define halfBit4800Delay 94

byte rx = 6;
byte tx = 5;
byte SWval;

void setup() {
  pinMode(tx, OUTPUT); //TX
  pinMode(rx, INPUT); // RX
  digitalWrite (tx, HIGH);
  delay(2);
  digitalWrite(13, HIGH); //debugging LED
  SWprint('h');
  SWprint('i');
  SWprint(10); //carriage return
}

void SWprint(int data)
{
  byte mask;
  //startbit
  digitalWrite(tx,LOW);
  delayMicroseconds(bit9600Delay);
  for (mask = 0x01; mask>0; mask <<= 1) {
    if (data & mask){ // choose bit
      digitalWrite(tx,HIGH); // send 1
    }
    else{
      digitalWrite(tx,LOW); // send 0
    }
    delayMicroseconds(bit9600Delay);
  }
  //stop bit
  digitalWrite(tx, HIGH);
  delayMicroseconds(bit9600Delay);
}

int SWread()
{
  byte val = 0;
  while (digitalRead(rx));
  //wait for start bit
  if (digitalRead(rx) == LOW) {
    delayMicroseconds(halfBit9600Delay);
    for (int offset = 0; offset < 8; offset++) {
      delayMicroseconds(bit9600Delay);
      val |= digitalRead(rx) << offset;
    }
    //wait for stop bit + extra
    delayMicroseconds(bit9600Delay);
    delayMicroseconds(bit9600Delay);
    return val;
  }
}

void loop()
{
  SWval = SWread();
  SWprint(toupper(SWval));
}

