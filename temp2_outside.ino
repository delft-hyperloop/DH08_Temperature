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

// hemsB(2): 20b
// hemsD(4): 20d
// emsB(2): 223
// emsD(4): 226
// backMotor(2): 20f
// ambient_temp: 224

const int hemsB_tempID[1] = { 0x20b };
const int hemsD_tempID[1] = { 0x20d };
const int emsB_tempID[1] = { 0x223 };
const int emsD_tempID[1] = { 0x226 };
const int backMotor_tempID[1] = { 0x20f };
const int amb_tempID[1] = { 0x224 };

const int Analog1 = 23; // HEMS B
const int Analog2 = 22; // Amb temp
const int Analog3 = 21; // HEMS D
const int Analog5 = 19; // EMS B
const int Analog7 = 17; // EMS D
const int Analog9 = 15; // Back motor


// Calculating the Temp constants
const int Beta = 3950; // Renamed from B to Beta
const float T0 = 298.15; // T0 in Kelvin (25 degrees Celsius)
const int maxAnalogValue = 1023; // Maximum analog value for 10-bit ADC

int TempAnalog[6]; 
float Temperature_reading[6]; 

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
    pinMode(Analog2, INPUT);
    pinMode(Analog3, INPUT);
    //pinMode(Analog4, INPUT);
    pinMode(Analog5, INPUT);
    //pinMode(Analog6, INPUT);
    pinMode(Analog7, INPUT);
   // pinMode(Analog8, INPUT);
    pinMode(Analog9, INPUT);
    //pinMode(Analog10, INPUT);

    // Initialize CAN bus
    can1.begin();
    can1.setBaudRate(1000000); // Set CAN baud rate
}

void loop() {
    // Read the analog values
    TempAnalog[0] = analogRead(Analog1);
    TempAnalog[1] = analogRead(Analog3);
    TempAnalog[2] = analogRead(Analog5);
    TempAnalog[3] = analogRead(Analog7);
    TempAnalog[4] = analogRead(Analog9);
    TempAnalog[5] = analogRead(Analog2);

    // Calculate the temperature for each analog value
    for (int i = 0; i < 6; ++i) {
      Temperature_reading[i] = calculateTemperature(TempAnalog[i]);
    }

    //Print the temperatures
    Serial.print("T_HEMS B (C): ");
    Serial.print(Temperature_reading[0]);
    Serial.print(" T_HEMS D (C): ");
    Serial.print(Temperature_reading[1]);
    Serial.print(" T_EMS B (C): ");
    Serial.print(Temperature_reading[2]);
    Serial.print(" T_EMS D (C): ");
    Serial.print(Temperature_reading[3]);
    Serial.print(" T_BACK MOTOR (C): ");
    Serial.print(Temperature_reading[4]);
    Serial.print(" T_AMB (C): ");
    Serial.print(Temperature_reading[5]);

    // Transmit data
    CANtransmit(hemsB_tempID, Temperature_reading[0], 1);
    CANtransmit(hemsD_tempID, Temperature_reading[1], 1);
    CANtransmit(emsB_tempID, Temperature_reading[2], 1);
    CANtransmit(emsD_tempID, Temperature_reading[3], 1);
    CANtransmit(backMotor_tempID, Temperature_reading[4],1);
    CANtransmit(amb_tempID, Temperature_reading[5],1);

    delay(100); // Wait before the next loop
}