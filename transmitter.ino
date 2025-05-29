#define PIN2 2 
#define D4 4
#define D15 15
#include "strings.h"

char m[3] = {'C', 'F', 'K'};
unsigned int currm = 0;

int debug = 1;

void setup() {
  pinMode(PIN2, OUTPUT);
  Serial.begin(9600);
  digitalWrite(PIN2, HIGH);   // default high for UART port

  pinMode(D4, INPUT);
  pinMode(D15, INPUT);
  analogReadResolution(12);
}



void printIntReading(String str, unsigned int val, unsigned int newLine) {
  Serial.print(str);
  Serial.print(val);
  if(newLine) Serial.println();
}

void printStrReading(String str, String val, unsigned  int newLine) {
  Serial.print(str);
  Serial.print(val);
  if(newLine) Serial.println();
}

void printCharInBinary(char c) {
  for (int i = 7; i >= 0; i--) {  
    Serial.print((c >> i) & 1);  
  }
  Serial.println();  
}


void sendBit(unsigned int PIN, unsigned int val, uint32_t timedelay) {
  if(val == 0) {
    digitalWrite(PIN, LOW);
    Serial.println(" Sent 0");
  } else {
    digitalWrite(PIN, HIGH);
    Serial.println(" Sent 1");
  }
  delay(timedelay);
}

void sendCharacter(unsigned int PIN, char c, uint32_t timedelay) {
  
  if(debug) Serial.println(" Transmitting START ");
  sendBit(PIN, 0, timedelay);  // start for UART 1->0

  for(int i = 0 ; i < 8; i++) {
    int x = (c >> i ) & 0x01;
    sendBit(PIN, x, timedelay);
  }

  sendBit(PIN, 1, timedelay);   // end for UART x->1
  if(debug) Serial.println(" Transmitting STOP ");
}


void loop() {
  unsigned int syncCycles = 2;

  uint32_t  timedelay = 150;

  int analogReading = digitalRead(D4);
  int threshold2 = 0;
  if(analogReading > threshold2) {
    currm = (currm+1)%3;
  } else {
  }

  int pReading = analogRead(D15);
  float celsius = (pReading * 100.0) / 4096;
  float temp = 0.0;
  if(currm == 0) {
    temp = celsius;
  } else if(currm == 1) {
    temp = (celsius * 9 / 5) + 32;  
  } else {
    temp = celsius + 273.15;
  }

  String s;
  s += " ";
  s += temp;
  s += " ";
  s += m[currm];
  s += " ";

  int length = s.length();
  if(length < 10) {
    for(int i = 0; i < 10-length ; i++ ) {
      s += " ";
    }
  }

  if(debug) {
    Serial.print(pReading);
    Serial.print(" ");
    Serial.print(temp);
    Serial.print(" ");
    Serial.print(m[currm]);
    Serial.print(" ");
    Serial.println(s);
  }

  // Transmit 10 1s to allow for rx to become ready
  for(unsigned int i = 0; i < 10; i++) {
    sendBit(PIN2,1,timedelay);
  }

  int t1 = millis();
  for(unsigned int i = 0 ; i < s.length() ; i++) {
      // Transmit 2 1s to allow for rx to become ready
      for(unsigned int j = 0; j < syncCycles; j++) {
        sendBit(PIN2,1,timedelay);
      }
      sendCharacter(PIN2, s[i], timedelay);
  }
  int t2 = millis();
  float bps = 1000*((s.length())*(8))/(float)(t2-t1);

  for(int i = 0; i < 10; i++) {
    Serial.print(" The bps measured is ");
    Serial.println(bps);
  }
}
