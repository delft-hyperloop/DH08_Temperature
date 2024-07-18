// Temp Outside 1 //

// #include <Arduino.h>
// #include <FlexCAN_T4.h>
// #include <iostream>
// #include <string>
// #include <bitset>
// #include <cmath>
// #include <SD.h>
// #include <SPI.h>

// // Define the chip select pin for the SD card slot
// const int chipSelect = BUILTIN_SDCARD;

// // CAN
// FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can1;
// CAN_message_t msg;
// int ID = 0;

// // hemsA(1): 20a
// // hemsC(3): 20c
// // emsA(1): 222
// // emsC(3): 225
// // frontMotor(1): 20e
// // ambient pressure: 220

// const int hemsA_tempID[1] = { 0x20a };
// const int hemsC_tempID[1] = { 0x20c };
// const int emsA_tempID[1] = { 0x222 };
// const int emsC_tempID[1] = { 0x225 };
// const int frontMotor_tempID[1] = { 0x20e };
// const int ambpressureID[1] = { 0x220 };

// //PINS

// const int HEMS_A = 14; 
// const int EMS_A = 15; 
// const int EMS_C = 16; 
// const int FRONT_MOTOR = 18; 
// const int HEMS_C = 20; 
// const int PressureAnalog = 41;


// float HEMS_A_reading = 0; 
// float EMS_A_reading = 0; 
// float EMS_C_reading = 0; 
// float FRONT_MOTOR_reading = 0; 
// float HEMS_C_reading = 0; 
// float PressureAnalog_reading = 0;

// // Calculating the Temp constants
// const int Beta = 3950; // Renamed from B to Beta
// const float T0 = 298.15; // T0 in Kelvin (25 degrees Celsius)
// const int maxAnalogValue = 1023; // Maximum analog value for 10-bit ADC

// int TempAnalog[5]; 
// float Temperature_reading[5]; 

// float calculateTemperature(int analogValue) {
//     float a = (float)analogValue;
//     float x = log((maxAnalogValue * 3.9) / (10 * a) - 3.9 / 10);
//     float T = 1 / (x / Beta + 1 / T0) - 273; // Calculate the temperature in Kelvin
//     return T;
// }

//  void CANtransmit(const int* tempIDs, float average_temp, int size) {
//      for (int i = 0; i < size; ++i) {
//          msg.id = tempIDs[i]; // Assign a unique ID for each temperature
//          msg.len = 2;
//          int tempInt = static_cast<int>(average_temp * 100); // Convert float to int (with scaling)
//          msg.buf[1] = tempInt & 0xff00;
//          msg.buf[0] = tempInt & 0xff;
//          can1.write(msg);
//      }
//  }

// void CANtransmitIndividual(const int* tempIDs, float* Temperature_reading, int size) {
//     for (int i = 0; i < size; ++i) {
//         msg.id = tempIDs[i]; // Assign a unique ID for each temperature
//         msg.len = 2;
//         int tempInt = static_cast<int>(Temperature_reading[i] * 100); // Convert float to int (with scaling)
//         msg.buf[0] = (tempInt >> 8) & 0xff;
//         msg.buf[1] = tempInt & 0xff;
//         can1.write(msg);
//     }
// }

// void setup() {
//     Serial.begin(9600);

//     pinMode(LED_BUILTIN, OUTPUT);
//     digitalWrite(LED_BUILTIN, HIGH);

//     pinMode(HEMS_A, INPUT);
//     //pinMode(Analog2, INPUT);
//     pinMode(HEMS_C, INPUT);
//     //pinMode(Analog4, INPUT);
//     pinMode(EMS_A, INPUT);
//     //pinMode(Analog6, INPUT);
//     pinMode(EMS_C, INPUT);
//    // pinMode(Analog8, INPUT);
//     pinMode(FRONT_MOTOR, INPUT);
//     //pinMode(Analog10, INPUT);
//     pinMode(PressureAnalog, INPUT);

//     // Initialize CAN bus
//     can1.begin();
//     can1.setBaudRate(1000000); // Set CAN baud rate
// }

// void loop() {
//     // Read the analog values

//     HEMS_A_reading = calculateTemperature(analogRead(HEMS_A)); 
//     EMS_A_reading = calculateTemperature(analogRead(EMS_A)); 
//     EMS_C_reading = calculateTemperature(analogRead(EMS_C));
//     FRONT_MOTOR_reading = calculateTemperature(analogRead(FRONT_MOTOR));
//     HEMS_C_reading = calculateTemperature(analogRead(HEMS_C));
//     PressureAnalog_reading = (((analogRead(PressureAnalog)*3.3/1023)-0.14)/0.0064)/101.5;
//     // Calculate the temperature for each analog value

//     //Print the temperatures
//     Serial.print("T_HEMS A (C): ");
//     Serial.print(HEMS_A_reading);
//     Serial.print(" T_HEMS C (C): ");
//     Serial.print(HEMS_C_reading);
//     Serial.print(" T_EMS A (C): ");
//     Serial.print(EMS_A_reading);
//     Serial.print(" T_EMS C (C): ");
//     Serial.print(HEMS_C_reading);
//     Serial.print(" T_FRONT MOTOR (C): ");
//     Serial.print(FRONT_MOTOR_reading);
//     Serial.print("  AMBIENT PRESSURE: ");
//     Serial.println(PressureAnalog_reading);

//     // Transmit data
//     CANtransmit(hemsA_tempID, HEMS_A_reading, 1);
//     CANtransmit(hemsC_tempID, HEMS_C_reading, 1);
//     CANtransmit(emsA_tempID, EMS_A_reading , 1);
//     CANtransmit(emsC_tempID, EMS_C_reading , 1);
//     CANtransmit(frontMotor_tempID, FRONT_MOTOR_reading,1);
//     CANtransmit(ambpressureID,  PressureAnalog_reading*100,1);

//     delay(100); // Wait before the next loop
// }





/// Temp Outside 2 ///


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


const int AMB_TEMP = 16; 
const int EMS_D = 15; 
const int HEMS_B = 17; 
const int HEMS_D = 18; 
const int EMS_B = 20;
const int BACK_MOTOR = 22; 

float HEMS_B_reading = 0; 
float EMS_B_reading = 0; 
float EMS_D_reading = 0; 
float BACK_MOTOR_reading = 0; 
float HEMS_D_reading = 0; 
float AMB_TEMP_reading = 0;

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
         msg.buf[1] = tempInt & 0xff00;
         msg.buf[0] = tempInt & 0xff;
         can1.write(msg);
     }
 }

void setup() {
    Serial.begin(9600);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    pinMode(AMB_TEMP, INPUT);
    pinMode(EMS_D, INPUT);
    pinMode(HEMS_B, INPUT);
    //pinMode(Analog4, INPUT);
    pinMode(HEMS_D, INPUT);
    //pinMode(Analog6, INPUT);
    pinMode(EMS_B, INPUT);
   // pinMode(Analog8, INPUT);
    pinMode(BACK_MOTOR, INPUT);
    //pinMode(Analog10, INPUT);

    // Initialize CAN bus
    can1.begin();
    can1.setBaudRate(1000000); // Set CAN baud rate
}

void loop() {
  
    // Read the analog values

    HEMS_B_reading = calculateTemperature(analogRead(HEMS_B)); 
    EMS_B_reading = calculateTemperature(analogRead(EMS_B)); 
    EMS_D_reading = calculateTemperature(analogRead(EMS_D));
    BACK_MOTOR_reading = calculateTemperature(analogRead(BACK_MOTOR));
    HEMS_D_reading = calculateTemperature(analogRead(HEMS_D));
    AMB_TEMP_reading = calculateTemperature(analogRead(AMB_TEMP));
    // Calculate the temperature for each analog value

    //Print the temperatures
    Serial.print("T_HEMS B (C): ");
    Serial.print(HEMS_B_reading);
    Serial.print(" T_HEMS D (C): ");
    Serial.print(EMS_D_reading);
    Serial.print(" T_EMS B (C): ");
    Serial.print(EMS_B_reading);
    Serial.print(" T_EMS D (C): ");
    Serial.print(EMS_D_reading);
    Serial.print(" T_BACK MOTOR (C): ");
    Serial.print(BACK_MOTOR_reading);
    Serial.print(" T_AMB (C): ");
    Serial.println(AMB_TEMP_reading);

    // Transmit data
    CANtransmit(hemsB_tempID, HEMS_B_reading, 1);
    CANtransmit(hemsD_tempID, HEMS_D_reading, 1);
    CANtransmit(emsB_tempID, EMS_B_reading, 1);
    CANtransmit(emsD_tempID, EMS_D_reading, 1);
    CANtransmit(backMotor_tempID, BACK_MOTOR_reading,1);
    CANtransmit(amb_tempID, AMB_TEMP_reading,1);

    delay(100); // Wait before the next loop
} 


/// Temp VB ///

/*#include <Arduino.h>
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
         msg.buf[1] = tempInt & 0xff00;
         msg.buf[0] = tempInt & 0xff;
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
}*/
