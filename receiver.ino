#define PINA0 A0
#include <LiquidCrystal.h>
#include <Arduino.h>

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
unsigned int cursorPos = 2;
int debug = 1;

int threshold = 110;

unsigned int previousBitReading = 0;


void setup() {
  pinMode(PINA0, INPUT);
  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.print(" LIFI RX DATA : ");
  lcd.setCursor(14,1);
  lcd.print("--");
  lcd.setCursor(0,1);
  lcd.print("--");

  lcd.cursor();
  //lcd.autoscroll();
}

void printToLCD(char val) {
  if(cursorPos > 12) {
    cursorPos = 3;
  }
  lcd.setCursor(cursorPos, 1);
  cursorPos++;
  //lcd.print(millis());
  lcd.print(val);
}



void printIntReading(String str, unsigned int val, unsigned int newLine) {
  Serial.print(str);
  Serial.print(val);
  if(newLine) Serial.println();
}

void printStrReading(String str, String val, unsigned int newLine) {
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

bool receiveBit(int PIN) {
  int reading = analogRead(PIN);
  if(debug) { 
    Serial.print(" The reading is ");
    Serial.println(reading);
  }
  if(reading > threshold) return 1; 
  else   return 0;
}

void receiveChar(unsigned int PIN, uint32_t timedelayMicroseconds) {
  unsigned int bitreading = receiveBit(PIN);
  
  if(debug) {
    Serial.print(" The start bit reading is ");
    Serial.println(bitreading);
  }
  if(previousBitReading == 1 && bitreading == 0) {
    if(debug) Serial.println(" START DETECTED ");
    delay(timedelayMicroseconds/1000);

    char ch = 0;
    for(unsigned int i =0; i < 8; i++) {
      unsigned int bit = receiveBit(PIN);
      if(debug) {
        Serial.print(" The bit reading is ");
        Serial.println(bit);
      }
      delay(timedelayMicroseconds/1000);
      ch = ch | (bit << i);
    }

    bitreading = receiveBit(PIN);
    if(bitreading == 1) {
      if(debug) {
        Serial.print(" END DETECTED, Displaying '");
        Serial.println(ch);
        //Serial.print("' ");
        //printCharInBinary(ch);
      }
      printToLCD(ch);
    } else {
      // Dispose the data stream
      if(debug) Serial.println(" END DETECTION FAILED, DATA CORRUPTED");
    }
    //delay(timedelayMicroseconds/1000);
  } else {
    //delay(timedelayMicroseconds/1000);
  }
  previousBitReading = bitreading;
}


void loop() {

  uint32_t timedelayMicroseconds = 150000;

  receiveChar(PINA0, timedelayMicroseconds);
}
