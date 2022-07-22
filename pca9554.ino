/* 
  PCA9536 LIBRARY - BASIC DEVICE TESTING EXAMPLE
  ----------------------------------------------
  
  INTRODUCTION
  ------------
  This sketch offers a quick & simple code for testing that the PCA9554 is hooked-up and operating correctly.
  
  INPORTANT: The current library depends on the Arduino IDE's native 'Wire' library for I2C communication between the Arduino (Master) and the PCA9554 (Slave).
  WIRING DIAGRAM
  --------------
               PCA9554
               -------
        A0  --| •     |-- VCC
        A1  --|       |-- SDA
        A2  --|       |-- SCL
        IO1 --|       |-- !INT
        IO2 --|       |-- IO7
        IO3 --|       |-- IO6
        IO4 --|       |-- IO5
        GND --|       |-- IO4
               -------

  PIN 1  (A0)   - HW address pin 0
  PIN 2  (A1)   - HW address pin 1
  PIN 3  (A2)   - HW address pin 2
  PIN 4  (IO0)  - General I/O
  PIN 5  (IO1)  - General I/O
  PIN 6  (IO2)  - General I/O
  PIN 7  (IO3)  - General I/O  
  PIN 8  (GND)  - GND

  PIN 9  (IO4)  - General I/O
  PIN 10 (IO5)  - General I/O
  PIN 11 (IO6)  - General I/O
  PIN 12 (IO7)  - General I/O  
  PIN 13 (!INT) - Pin change interrupt output, connect to interrupt pin on MCU to receive pin change notifications
  PIN 14 (SCL)  - Connect to Arduino SCL with a 2K2 (400MHz) or 10K (100MHz) pull-up resistor
  PIN 15 (SDA)  - Connect to Arduino SDA with a 2K2 (400MHz) or 10K (100MHz) pull-up resistor
  PIN 16 (VCC)  - 3v3 / 5v
  
  DECOUPING   - Connect a 0.1uF Ceramic Capacitor between the PCA9554's VCC & GND pins
  
  LICENSE
  -------
  The MIT License (MIT)
  Copyright (c) 2016 Nadav Matalon
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
  documentation files (the "Software"), to deal in the Software without restriction, including without
  limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
  the Software, and to permit persons to whom the Software is furnished to do so, subject to the following
  conditions:
  
  The above copyright notice and this permission notice shall be included in all copies or substantial
  portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
  LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "PCA9554.h"

PCA9554 pca9554_0(0);
PCA9554 pca9554_1(1);

void setup() {
    Serial.begin(9600);
    Wire.begin();
    while(!Serial);
    printDivider();
    Serial.print(F("\nPCA9536 DEVICE TESTING\n"));
    printDivider();
    Serial.print(F("\nINITIATING SERIAL COMMUNICATION\n"));  
    Serial.print(F("\nSerial Port is "));
    Serial.print(Serial ? "Open\n" : "Could not be opened\n"); 
    printDivider();
    pca9554_1.reset();
    pca9554_0.reset();
    delay(50);
    pca9554_1.setMode((pin_t) 7, IO_OUTPUT);
    delay(50);
    Serial.print(F("\nSEARCHING FOR DEVICE\n\nDevice "));
    pca9554_1.ping() ? Serial.print(F("Not Found\n")) : Serial.print(F("Found!\n"));
    pca9554_0.ping() ? Serial.print(F("Not Found\n")) : Serial.print(F("Found!\n"));
    printDivider();
    delay(50);
    Serial.print(F("\nBLINKING LED ON PIN IO0\n"));
    printDivider();
}

void loop() {
    pca9554_1.toggleState(IO7);
    delay(500);
}

void printDivider() {
    Serial.print(F("\n-----------------------------------------\n"));
}
