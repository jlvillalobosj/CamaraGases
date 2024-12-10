
const int sensorPin = A1; 

// Calibration variables
float baseVoltage;
const float CO2_REF = 400.0; // Base voltage in clean air (~400 ppm)

const float VOLTAGE_REF1 = 0.323; // Base voltage in clean air (~400 ppm)
const float VOLTAGE_REF2 = 400.0; // Base voltage in clean air (~400 ppm)

void setup() {
  Serial.begin(9600);
  Serial.println("Calibrando MG811...");
  
  // Calibrate the sensor by reading the base voltage in clean air
  baseVoltage = readAverageVoltage();
  Serial.print("Voltaje base (aire limpio): ");
  Serial.println(baseVoltage, 3);
  
  Serial.println("MG811 calibrado. Comenzando mediciones...");
  delay(2000); // time to read data
}

void loop() {
  // Read sensor voltage
  float sensorVoltage = analogRead(sensorPin) * (5.0 / 1023.0); // ADC to voltage conversion
  float co2PPM = voltageToPPM(sensorVoltage, baseVoltage);
  
 
  Serial.print("Voltaje sensor: ");
  Serial.print(sensorVoltage, 3);
  Serial.print(" V, CO2: ");
  Serial.print(co2PPM);
  Serial.println(" ppm");
  
  delay(2000); // Esperar 2 segundos entre mediciones
}

// Convert voltage to ppm
float voltageToPPM(float voltage, float baseVoltage) {
  float calibrationFactor = 2000.0 / (baseVoltage - 2.0); 
  
  Serial.println(calibrationFactor);
  return (voltage - baseVoltage) * calibrationFactor + CO2_REF;
}

// Average voltage
float readAverageVoltage() {
  const int numReadings = 50; 
  float totalVoltage = 0.0;

  for (int i = 0; i < numReadings; i++) {
    int rawValue = analogRead(sensorPin);
    totalVoltage += rawValue * (5.0 / 1023.0); // Convert ADC to voltage
    delay(10); 
  }
  
  return totalVoltage / numReadings;
}