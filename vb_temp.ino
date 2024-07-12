#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <iostream>
#include <string>
#include <bitset>
#include <cmath>
#include <SD.h>
#include <SPI.h>

// Define the chip select pin for the SD card slot
const int chipSelect = BUILTIN_SDCARD;

// CAN
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can2;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can1;
CAN_message_t msg;
int ID = 0;

// pressure vb: 210
// temp vb top: 211
// temp vb bot: 200
const int bot_tempID[1] = { 0x200 };
const int top_tempID[1] = { 0x211 };
const int pressID[1] = { 0x210 };

const int Analog1 = 23;
const int Analog2 = 22;
const int Analog3 = 21;
const int Analog5 = 19;
const int Analog7 = 17;
const int Analog9 = 15;
const int PressureAnalog = 41;

// Calculating the Temp constants
const int Beta = 3950; // Renamed from B to Beta
const float T0 = 298.15; // T0 in Kelvin (25 degrees Celsius)
const int maxAnalogValue = 1023; // Maximum analog value for 10-bit ADC

float av_temp = 0;
int TempAnalog_bot[3]; // Array to store the temperature values
int TempAnalog_top[3]; // Array to store the temperature values
float Temperature_reading_bot[3];
float Temperature_reading_top[3];

float calculateTemperature(int analogValue) {
    float a = (float)analogValue;
    float x = log((maxAnalogValue * 3.9) / (10 * a) - 3.9 / 10);
    float T = 1 / (x / Beta + 1 / T0) - 273; // Calculate the temperature in Kelvin
    return T;
}

 void CANtransmit(const int* tempIDs, float average_temp, int size) {
     for (int i = 0; i < size; ++i) {
         msg.id = tempIDs[i]; // Assign a unique ID for each temperature
         msg.len = 2;
         int tempInt = static_cast<int>(average_temp * 100); // Convert float to int (with scaling)
         msg.buf[0] = (tempInt >> 8) & 0xff;
         msg.buf[1] = tempInt & 0xff;
         can1.write(msg);
     }
 }

void CANtransmitIndividual(const int* tempIDs, float* Temperature_reading, int size) {
    for (int i = 0; i < size; ++i) {
        msg.id = tempIDs[i]; // Assign a unique ID for each temperature
        msg.len = 2;
        int tempInt = static_cast<int>(Temperature_reading[i] * 100); // Convert float to int (with scaling)
        msg.buf[0] = (tempInt >> 8) & 0xff;
        msg.buf[1] = tempInt & 0xff;
        can1.write(msg);
    }
}

float averagetemp(int TempAnalog[], int size) {
    float sum = 0;
    for (int i = 0; i < size; i++) {
        sum += TempAnalog[i];
    }
    return sum / size;
}

void setup() {
    Serial.begin(9600);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    pinMode(Analog1, INPUT);
    pinMode(Analog2, INPUT);
    pinMode(Analog3, INPUT);
    //pinMode(Analog4, INPUT);
    pinMode(Analog5, INPUT);
    //pinMode(Analog6, INPUT);
    pinMode(Analog7, INPUT);
   // pinMode(Analog8, INPUT);
    pinMode(Analog9, INPUT);
    //pinMode(Analog10, INPUT);
    pinMode(PressureAnalog, INPUT);

    // Initialize CAN bus
    can1.begin();
    can1.setBaudRate(1000000); // Set CAN baud rate
}

void loop() {
    av_temp = 0;
    Serial.println(analogRead(PressureAnalog));
    float pressure = (((analogRead(PressureAnalog)*3.3/1023)-0.14)/0.0064)/101.5;
    Serial.println(pressure);
    // Read the analog values
    TempAnalog_bot[0] = analogRead(Analog1);
    TempAnalog_bot[1] = analogRead(Analog2);
    TempAnalog_bot[2] = analogRead(Analog3);
    TempAnalog_top[0] = analogRead(Analog5);
    TempAnalog_top[1] = analogRead(Analog7);
    TempAnalog_top[2] = analogRead(Analog9);

    // Calculate the temperature for each analog value
    for (int i = 0; i < 3; ++i) {
      Temperature_reading_bot[i] = calculateTemperature(TempAnalog_bot[i]);
    }

    // Calculate the temperature for each analog value
    for (int i = 0; i < 3; ++i) {
      Temperature_reading_top[i] = calculateTemperature(TempAnalog_top[i]);
    }

    //Print the temperatures
    Serial.print("Tbot 1 (C): ");
    Serial.print(Temperature_reading_bot[0]);
    Serial.print(" Tbot 2 (C): ");
    Serial.print(Temperature_reading_bot[1]);
    Serial.print(" Tbot 3 (C): ");
    Serial.print(Temperature_reading_bot[2]);
    Serial.print(" Ttop 1 (C): ");
    Serial.print(Temperature_reading_top[0]);
    Serial.print(" Ttop 2 (C): ");
    Serial.print(Temperature_reading_top[1]);
    Serial.print(" Ttop 3 (C): ");
    Serial.print(Temperature_reading_top[2]);

    float average_temp_bot = averagetemp(TempAnalog_bot, 3);
    float average_temp_top = averagetemp(TempAnalog_top, 3);

    // Transmit the average temperatures
    CANtransmit(bot_tempID, average_temp_bot, 1);
    CANtransmit(top_tempID, average_temp_top, 1);
    CANtransmit(pressID, pressure*100,1);
  
    // Transmit the individual temperatures
    // CANtransmitIndividual(tempIDs3, Temperature_reading, 10);

    delay(100); // Wait before the next loop
}