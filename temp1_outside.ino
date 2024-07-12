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

// hemsA(1): 20a
// hemsC(3): 20c
// emsA(1): 222
// emsC(3): 225
// frontMotor(1): 20e
// ambient pressure: 220

const int hemsA_tempID[1] = { 0x20a };
const int hemsC_tempID[1] = { 0x20c };
const int emsA_tempID[1] = { 0x222 };
const int emsC_tempID[1] = { 0x225 };
const int frontMotor_tempID[1] = { 0x20e };
const int ambpressureID[1] = { 0x220 };

const int Analog1 = 23; // HEMS A
const int Analog3 = 21; // HEMS C
const int Analog5 = 19; // EMS A
const int Analog7 = 17; // EMS C
const int Analog9 = 15; // Front motor
const int PressureAnalog = 41;

// Calculating the Temp constants
const int Beta = 3950; // Renamed from B to Beta
const float T0 = 298.15; // T0 in Kelvin (25 degrees Celsius)
const int maxAnalogValue = 1023; // Maximum analog value for 10-bit ADC

int TempAnalog[5]; 
float Temperature_reading[5]; 

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

void setup() {
    Serial.begin(9600);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    pinMode(Analog1, INPUT);
    //pinMode(Analog2, INPUT);
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
    Serial.print("Ambient pressure: ");
    float pressure = (((analogRead(PressureAnalog)*3.3/1023)-0.14)/0.0064)/101.5;
    Serial.println(pressure);
    // Read the analog values
    TempAnalog[0] = analogRead(Analog1);
    TempAnalog[1] = analogRead(Analog3);
    TempAnalog[2] = analogRead(Analog5);
    TempAnalog[3] = analogRead(Analog7);
    TempAnalog[4] = analogRead(Analog9);

    // Calculate the temperature for each analog value
    for (int i = 0; i < 5; ++i) {
      Temperature_reading[i] = calculateTemperature(TempAnalog[i]);
    }

    //Print the temperatures
    Serial.print("T_HEMS A (C): ");
    Serial.print(Temperature_reading[0]);
    Serial.print(" T_HEMS C (C): ");
    Serial.print(Temperature_reading[1]);
    Serial.print(" T_EMS A (C): ");
    Serial.print(Temperature_reading[2]);
    Serial.print(" T_EMS C (C): ");
    Serial.print(Temperature_reading[3]);
    Serial.print(" T_FRONT MOTOR (C): ");
    Serial.print(Temperature_reading[4]);

    // Transmit data
    CANtransmit(hemsA_tempID, Temperature_reading[0], 1);
    CANtransmit(hemsC_tempID, Temperature_reading[1], 1);
    CANtransmit(emsA_tempID, Temperature_reading[2], 1);
    CANtransmit(emsC_tempID, Temperature_reading[3], 1);
    CANtransmit(frontMotor_tempID, Temperature_reading[4],1);
    CANtransmit(ambpressureID, pressure*100,1);

    delay(100); // Wait before the next loop
}