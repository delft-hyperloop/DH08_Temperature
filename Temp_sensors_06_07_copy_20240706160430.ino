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

// Unique IDs for the temperature measurements
const int tempIDs1[2] = { 0x210, 0x211 };

// pressure vb: 210
// temp vb top: 211

const int tempIDs2[1] = { 0x200 };

// temp vb bottom: 200

const int tempIDs3[10] = { 0x222, 0x224, 0x223, 0x200, 0x20D, 0x20E, 0x22C, 0x20B, 0x20F, 0x20A};

// HEMS left back (HEMS 1): T10 : 20A
// HEMS left front (HEMS 2):T8: 20B
// HEMS right back (HEMS 3):T7: 22C
// HEMS right front (HEMS 4):T5: 20D
// Motor front (motor 2): T9: 20F
// Motor back (motor 1): T6: 20E
// EMS 1: T1
// ambient temp: T2
// ambient pressure: P4
// EMS 2: T3

const int Analog1 = 23;
const int Analog2 = 22;
const int Analog3 = 21;
const int Analog4 = 20;
const int Analog5 = 19;
const int Analog6 = 18;
const int Analog7 = 17;
const int Analog8 = 16;
const int Analog9 = 15;
const int Analog10 = 14;

// Calculating the Temp constants
const int Beta = 3950; // Renamed from B to Beta
const float T0 = 298.15; // T0 in Kelvin (25 degrees Celsius)
const int maxAnalogValue = 1023; // Maximum analog value for 10-bit ADC

float av_temp = 0;
int TempAnalog[10]; // Array to store the temperature values
float Temperature_reading[10];

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
    pinMode(Analog4, INPUT);
    pinMode(Analog5, INPUT);
    pinMode(Analog6, INPUT);
    pinMode(Analog7, INPUT);
    pinMode(Analog8, INPUT);
    pinMode(Analog9, INPUT);
    pinMode(Analog10, INPUT);

    // Initialize CAN bus
    can1.begin();
    can1.setBaudRate(1000000); // Set CAN baud rate
}

void loop() {
    av_temp = 0;
    if (can1.read(msg))
      Serial.print(msg.id);

    // Read the analog values
    TempAnalog[0] = analogRead(Analog1);
    TempAnalog[1] = analogRead(Analog2);
    TempAnalog[2] = analogRead(Analog3);
    TempAnalog[3] = analogRead(Analog4);
    TempAnalog[4] = analogRead(Analog5);
    TempAnalog[5] = analogRead(Analog6);
    TempAnalog[6] = analogRead(Analog7);
    TempAnalog[7] = analogRead(Analog8);
    TempAnalog[8] = analogRead(Analog9);
    TempAnalog[9] = analogRead(Analog10);

    // Calculate the temperature for each analog value
    for (int i = 0; i < 10; ++i) {
      Temperature_reading[i] = calculateTemperature(TempAnalog[i]);
    }

    //Print the temperatures
    Serial.print("T1 (C): ");
    Serial.print(Temperature_reading[0]);
    Serial.print(" T2 (C): ");
    Serial.print(Temperature_reading[1]);
    Serial.print(" T3 (C): ");
    Serial.print(Temperature_reading[2]);
    Serial.print(" T4 (C): ");
    Serial.print(Temperature_reading[3]);
    Serial.print(" T5 (C): ");
    Serial.print(Temperature_reading[4]);
    Serial.print(" T6 (C): ");
    Serial.print(Temperature_reading[5]);
    Serial.print(" T7 (C): ");
    Serial.print(Temperature_reading[6]);
    Serial.print(" T8 (C): ");
    Serial.print(Temperature_reading[7]);
    Serial.print(" T9 (C): ");
    Serial.print(Temperature_reading[8]);
    Serial.print(" T10 (C): ");
    Serial.println(Temperature_reading[9]);

    float average_temp = averagetemp(TempAnalog, 10);

    // Transmit the average temperatures
    // CANtransmit(tempIDs1, average_temp, 2);
    CANtransmit(tempIDs2, average_temp, 1);
    

    // Transmit the individual temperatures
    // CANtransmitIndividual(tempIDs3, Temperature_reading, 10);

    delay(100); // Wait before the next loop
}
