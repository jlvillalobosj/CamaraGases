#include <LiquidCrystal.h>
#include "DHT.h"

#define DHTPIN 8     // Digital pin connected to the DHT sensor
LiquidCrystal lcd(7,6,5,4,3,2);

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

float lastTemp = 0.0;
float lastHum = 0.0;
//-----------------

//Include the library
#include <MQUnifiedsensor.h>
/************************Hardware Related Macros************************************/
#define         Board                   ("Arduino MEGA")
#define         PinMQ4                     (A0)  //Analog input 4 of your arduino

/***********************Software Related Macros MQ4-CH4************************************/
#define         Type                    ("MQ-4") //MQ4
#define         Voltage_Resolution      (5)
#define         ADC_Bit_Resolution      (10) // For arduino UNO/MEGA/NANO
#define         RatioMQ4CleanAir        (4.4) //RS / R0 = 60 ppm 
/***********************Decrease oscillation************************************/
#define         NUM_READINGS            (10) // Number of readings to calculate the average
#define         CHANGE_THRESHOLD        (2) //  Change of threshold (ppm)
/*****************************Globals***********************************************/
//Declare Sensor MQ4
MQUnifiedsensor MQ4(Board, Voltage_Resolution, ADC_Bit_Resolution, PinMQ4, Type);

float readings[NUM_READINGS]; // Array to keep readings
int index = 0;                // Current index in array
float average = 0;            // Current average
float stableValue_CH4 = 0;        // Stable valuue to display
float mae = 0;                // Mean Absolute Error

//-----------------------

/************************Hardware Related Macros MG811 - CO2************************************/
#define         sensorPinMG811                    (A1)  //define which analog input channel you are going to use


// Calibration variables
float baseVoltage_MG811;
float calibrationFactor_MG811;

const float CO2_REF_400 = 400.0; // Base voltage in clean air (~400 ppm)
const float CO2_REF_8000 = 8000.0;

const float VOLTAGE_REF_400 = 0.323; // Base voltage in clean air (~400 ppm)
const float VOLTAGE_REF_8000 = 0.268;


//two points are taken from the curve.
//with these two points, a line is formed which is
//"approximately equivalent" to the original curve.
//data format:{ x, y, slope}; point1: (lg400, 0.324), point2: (lg4000, 0.280)
//slope = ( reaction voltage ) / (log400 –log1000)


/************************INDICATOR LEDS - HMI************************************/

#define LED_RED_NOT_WORKING       13
#define LED_GREEN_WORK_MQ4        12
#define LED_GREEN_WORK_MG811      11

void setup() {


  // Set INDICATORS as output
  pinMode(LED_RED_NOT_WORKING, OUTPUT);
  pinMode(LED_GREEN_WORK_MQ4, OUTPUT);
  pinMode(LED_GREEN_WORK_MG811, OUTPUT);

  //Init the serial port communication - to debug the library
  Serial.begin(9600); //Init serial port
  
  //-------------------------------

  digitalWrite(LED_GREEN_WORK_MQ4, LOW);
  digitalWrite(LED_GREEN_WORK_MG811, LOW);
  digitalWrite(LED_RED_NOT_WORKING, HIGH);

  lcd.begin(16,2); //Definir las dimensiones del LCD (16x2)
  dht.begin();

  //--------------------------------------------
  //Init and calibrate MQ4 sensor
  calibrateMQ4();

  //--------------------------------------------
  //Init and calibrate MG811 sensor
  calibrateMG811();
  
  digitalWrite(LED_RED_NOT_WORKING, LOW);
  delay(2000); // time to read data
}

void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Verificar si las lecturas son válidas (no NaN)
  if (!isnan(h)) {
    lastHum = h; // Actualizar la última lectura válida de humedad
  }
  if (!isnan(t)) {
    lastTemp = t; // Actualizar la última lectura válida de temperatura
  }
  Serial.print(F("Humidity: "));
  Serial.print(lastHum);
  Serial.print(F("%  Temperature: "));
  Serial.print(lastTemp);
  Serial.println(F("°C "));

  //--------------------------------------- CALCULATE MG811 - CO2

  // Read sensor voltage
  float sensorVoltage = analogRead(sensorPinMG811) * (5.0 / 1023.0); // ADC to voltage conversion
  float co2LOG = ((sensorVoltage - baseVoltage_MG811)/calibrationFactor_MG811) + log10(CO2_REF_400);
  float co2PPM = (pow(10, co2LOG));
 
  Serial.print("Voltaje sensor: ");
  Serial.print(sensorVoltage, 3);
  Serial.print(" V, CO2: ");
  Serial.print(co2PPM, 3);
  Serial.println(" ppm");
  

  //----------------------------------- CALCULATE MQ4 - CH4

  MQ4.update(); // Update data, the arduino will read the voltage from the analog pin
  float ppm = MQ4.readSensor(); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup
  //MQ4.serialDebug(); // Will print the table on the serial port

  stableValue_CH4 = averageCalculateMQ4(ppm, stableValue_CH4);
  mae =  meanAbsoluteErrorMQ4(stableValue_CH4);

  Serial.print("CH4_ppm:");
  Serial.print(ppm);

  Serial.print(", CH4_ppm_average:");
  Serial.print(stableValue_CH4);

  Serial.print(", Error_Porcent:");
  Serial.println(mae);

  displayInformationLCD(lastTemp, lastHum,co2PPM, stableValue_CH4);

  delay(2000);

  
}

void calibrateMQ4(){
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
  Serial.print("Calibrating MQ4-Sensor please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=100; i ++)
  {
    MQ4.update(); // Update data, the arduino will read the voltage from the analog pin
    calcR0 += MQ4.calibrate(RatioMQ4CleanAir);
    Serial.print(".");
  }
  MQ4.setR0(calcR0/100);
  Serial.println("  done.");
  digitalWrite(LED_GREEN_WORK_MQ4, HIGH);
  
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply"); while(1);}
  /*****************************  MQ CAlibration ********************************************/ 
  MQ4.serialDebug(true);
}

void calibrateMG811(){
  Serial.println("Calibrando MG811..."); 
  
  // Calibrate the sensor by reading the base voltage in clean air
  baseVoltage_MG811 = readAverageVoltageMG811();
  calibrationFactor_MG811 = (baseVoltage_MG811 - VOLTAGE_REF_8000) / (log10(CO2_REF_400) - log10(CO2_REF_8000));
  Serial.println("MG811 calibrado. Comenzando mediciones...");
  digitalWrite(LED_GREEN_WORK_MG811, HIGH);
}

float averageCalculateMQ4(float ppm, float stableValueCurrent) {
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

float meanAbsoluteErrorMQ4(float stableValueError) { 
  // Calculate MAE 
  float errorSum = 0;
  for (int i = 0; i < NUM_READINGS; i++) {
    errorSum += abs(readings[i] - average);
  }
  float error = errorSum / NUM_READINGS;

  return (error/stableValueError)*100;
}

// Average voltage in MG811 Sensor
float readAverageVoltageMG811() {
  const int numReadings = 50; 
  float totalVoltage = 0.0;

  for (int i = 0; i < numReadings; i++) {
    int rawValue = analogRead(sensorPinMG811);
    totalVoltage += rawValue * (5.0 / 1023.0); // Convert ADC to voltage
    delay(10); 
  }
  
  return totalVoltage / numReadings;
}

void displayInformationLCD(float lastTemp, float lastHum, float CO2, float CH4) {

 //Seleccionamos en que columna y en que linea empieza a mostrar el texto
 lcd.setCursor(0,0);
 lcd.print("T:");
 lcd.print(lastTemp,1);
 lcd.print("C  H:");
 lcd.print(lastHum,1); 
 lcd.print("%");

 lcd.setCursor(0,1);
 lcd.print("CH4:");
 lcd.print(CH4,1);
 lcd.print("  CO2:");
 lcd.print(CO2,1);

}



