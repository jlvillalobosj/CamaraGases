
const int sensorPin = A1; 

// Calibration variables
float baseVoltage;
float calibrationFactor;

const float CO2_REF_400 = 400.0; // Base voltage in clean air (~400 ppm)
const float CO2_REF_8000 = 8000.0;

const float VOL_REF_400 = 0.323; // Base voltage in clean air (~400 ppm)
const float VOL_REF_8000 = 0.268;

void setup() {
  Serial.begin(9600);
  Serial.println("Calibrando MG811..."); 
  
  // Calibrate the sensor by reading the base voltage in clean air
  baseVoltage = readAverageVoltage();
  calibrationFactor = (baseVoltage - VOL_REF_8000) / (log10(CO2_REF_400) - log10(CO2_REF_8000));
  Serial.println("MG811 calibrado. Comenzando mediciones...");
  delay(2000); // time to read data
}

void loop() {
  // Read sensor voltage
  float sensorVoltage = analogRead(sensorPin) * (5.0 / 1023.0); // ADC to voltage conversion
  float co2LOG = ((sensorVoltage - baseVoltage)/calibrationFactor) + log10(CO2_REF_400);
  float co2PPM = (pow(10, co2LOG));
 
  Serial.print("Voltaje sensor: ");
  Serial.print(sensorVoltage, 3);
  Serial.print(" V, CO2: ");
  Serial.print(co2PPM);
  Serial.println(" ppm");
  
  delay(2000); // Esperar 2 segundos entre mediciones
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
