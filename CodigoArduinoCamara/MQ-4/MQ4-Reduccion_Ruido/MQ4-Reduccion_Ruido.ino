/*
  MQUnifiedsensor Library - reading an MQ4

  Demonstrates the use a MQ4 sensor.
  Library originally added 01 may 2019
  by Miguel A Califa, Yersson Carrillo, Ghiordy Contreras, Mario Rodriguez
 
  Added example
  modified 23 May 2019
  by Miguel Califa 

  Updated library usage
  modified 26 March 2020
  by Miguel Califa 

  Wiring:
  https://github.com/miguel5612/MQSensorsLib_Docs/blob/master/static/img/MQ_Arduino.PNG
  Please make sure arduino A0 pin represents the analog input configured on #define pin

 This example code is in the public domain.

*/

//Include the library
#include <MQUnifiedsensor.h>
/************************Hardware Related Macros************************************/
#define         Board                   ("Arduino UNO")
#define         Pin                     (A4)  //Analog input 4 of your arduino
/***********************Software Related Macros************************************/
#define         Type                    ("MQ-4") //MQ4
#define         Voltage_Resolution      (5)
#define         ADC_Bit_Resolution      (10) // For arduino UNO/MEGA/NANO
#define         RatioMQ4CleanAir        (4.4) //RS / R0 = 60 ppm 
/***********************Decrease oscillation************************************/
#define         NUM_READINGS            (10) // Number of readings to calculate the average
#define         CHANGE_THRESHOLD        (2) //  Change of threshold (ppm)
/*****************************Globals***********************************************/
//Declare Sensor
MQUnifiedsensor MQ4(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type);

float readings[NUM_READINGS]; // Array to keep readings
int index = 0;                // Current index in array
float average = 0;            // Current average
float stableValue = 0;        // Stable valuue to display
float mae = 0;                // Mean Absolute Error

void setup() {
  //Init the serial port communication - to debug the library
  Serial.begin(9600); //Init serial port

  // Init the array with zeros
  for (int i = 0; i < NUM_READINGS; i++) {
    readings[i] = 0;
  }

  //Set math model to calculate the PPM concentration and the value of constants
  MQ4.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ4.setA(1012.7); MQ4.setB(-2.786); // Configure the equation to to calculate CH4 concentration
  /*
    Exponential regression:
  Gas    | a      | b
  LPG    | 3811.9 | -3.113
  CH4    | 1012.7 | -2.786
  CO     | 200000000000000 | -19.05
  Alcohol| 60000000000 | -14.01
  smoke  | 30000000 | -8.308
  */

  
  /*****************************  MQ Init ********************************************/ 
  //Remarks: Configure the pin of arduino as input.
  /************************************************************************************/ 
  MQ4.init(); 
  /* 
    //If the RL value is different from 10K please assign your RL value with the following method:
    MQ4.setRL(10);
  */
  /*****************************  MQ CAlibration ********************************************/ 
  // Explanation: 
   // In this routine the sensor will measure the resistance of the sensor supposedly before being pre-heated
  // and on clean air (Calibration conditions), setting up R0 value.
  // We recomend executing this routine only on setup in laboratory conditions.
  // This routine does not need to be executed on each restart, you can load your R0 value from eeprom.
  // Acknowledgements: https://jayconsystems.com/blog/understanding-a-gas-sensor
  
  Serial.println("");
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=100; i ++)
  {
    MQ4.update(); // Update data, the arduino will read the voltage from the analog pin
    calcR0 += MQ4.calibrate(RatioMQ4CleanAir);
    Serial.print(".");
  }
  MQ4.setR0(calcR0/100);
  Serial.println("  done!.");
  
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply"); while(1);}
  /*****************************  MQ CAlibration ********************************************/ 
  MQ4.serialDebug(true);
}

void loop() {
  MQ4.update(); // Update data, the arduino will read the voltage from the analog pin
  float ppm = MQ4.readSensor(); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup
  //MQ4.serialDebug(); // Will print the table on the serial port

  stableValue = averageCalculate(ppm, stableValue);
  mae =  meanAbsoluteError(stableValue);

  Serial.print("CH4_ppm:");
  Serial.println(ppm);

  Serial.print("CH4_ppm_average:");
  Serial.println(stableValue);

  Serial.print("Error_Porcent:");
  Serial.println(mae);
  delay(500); //Sampling frequency
}

float averageCalculate(float ppm, float stableValueCurrent) {
  // UUpdate the array with the last readings
  readings[index] = ppm;
  index = (index + 1) % NUM_READINGS;

  // Calculate the average of the lasts readings
  float sum = 0;
  for (int i = 0; i < NUM_READINGS; i++) {
    sum += readings[i];
  }
  average = sum / NUM_READINGS;

  // Set a stable value if the average changes significantly
  if (abs(average - stableValueCurrent) > CHANGE_THRESHOLD) {
    return  average;
  } 
  else return stableValueCurrent;
}

float meanAbsoluteError(float stableValueError) { 
  // Calculate MAE 
  float errorSum = 0;
  for (int i = 0; i < NUM_READINGS; i++) {
    errorSum += abs(readings[i] - average);
  }
  float error = errorSum / NUM_READINGS;

  return (error/stableValueError)*100;
}