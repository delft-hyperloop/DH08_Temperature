#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <iostream>
#include <string>
#include <bitset>
#include <cmath>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2;
CAN_message_t msg;
int ID = 0;

// Unique IDs for the temperature measurements

const int tempIDs1[10] = {
    0x210, 0x211, 0x212, 0x213, 0x214,
    0x215, 0x216, 0x217, 0x218, 0x219
};

const int tempIDs2[10] = {
    0x200, 0x201, 0x202, 0x203, 0x204,
    0x205, 0x206, 0x207, 0x208, 0x209
};

const int tempIDs3[10] = {
    0x20A, 0x20B, 0x20C, 0x20D, 0x20E,
    0x20F, 0x220, 0x224, 0x222, 0x223
};

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

int TempAnalog[10]; // Array to store the temperature values
float Temperature_reading[10];

float calculateTemperature(int analogValue) {
  float a = (float)analogValue;
  float x = log((maxAnalogValue * 3.9) / (10 * a) - 3.9 / 10);
  float T = 1 / (x / Beta + 1 / T0) - 273; // Calculate the temperature in Kelvin
  return T;
}

void CANtransmit(const int* tempIDs, float* Temperature_reading, int size) {
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
  pinMode(Analog4, INPUT);
  pinMode(Analog5, INPUT);
  pinMode(Analog6, INPUT);
  pinMode(Analog7, INPUT);
  pinMode(Analog8, INPUT);
  pinMode(Analog9, INPUT);
  pinMode(Analog10, INPUT);

  // Initialize CAN bus
  can1.begin();
  can1.setBaudRate(500000); // Set CAN baud rate
}

void loop() {
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

  // Print the temperatures
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

  // Transmit the array of temperatures
  CANtransmit(tempIDs1, Temperature_reading, 10);

  delay(100); // Wait before the next loop
}
