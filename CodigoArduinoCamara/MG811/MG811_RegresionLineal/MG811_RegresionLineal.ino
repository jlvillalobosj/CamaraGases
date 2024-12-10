const int sensorPin = A1; 

// Calibration variables
float baseVoltage;
float calibrationFactor;
float a = 0;
float b = 0;

const float CO2_REF[] = {400.0, 1000.0, 8000.0}; // Valores de referencia de CO2
float VOL_REF[] = {0.323, 0.304, 0.268};  // Valores de referencia de voltaje

void setup() {
  Serial.begin(9600);
  Serial.println("Calibrando MG811..."); 
  
  // Calibrate the sensor by reading the base voltage in clean air
  calibrateMG811_LinearRegression();
  Serial.println("MG811 calibrado. Comenzando mediciones...");
  delay(2000); // time to read data
}

void loop() {
  // Read sensor voltage
  float sensorVoltage = analogRead(sensorPin) * (5.0 / 1023.0); // ADC to voltage conversion
  float co2LOG = ((sensorVoltage - b)/a);
  float co2PPM = (pow(10, co2LOG));
 
  Serial.print("Voltaje sensor: ");
  Serial.print(sensorVoltage, 3);
  Serial.print(" V, CO2: ");
  Serial.print(co2PPM);
  Serial.println(" ppm");
  
  delay(2000); // Esperar 2 segundos entre mediciones
}

// Average voltage
void calibrateMG811_LinearRegression() {
  
  //baseVoltage = readAverageVoltage();  
  //VOL_REF[0] = baseVoltage;
  int N = sizeof(CO2_REF) / sizeof(CO2_REF[0]);
  
  // Variables for regression
  float sum_x = 0, sum_y = 0, sum_xy = 0, sum_x2 = 0;
  
  // Calculations for regression

  for (int i = 0; i < N; i++) {
    sum_x += log10(CO2_REF[i]);
    sum_y += VOL_REF[i];
    sum_xy += log10(CO2_REF[i]) * VOL_REF[i];
    sum_x2 += log10(CO2_REF[i]) * log10(CO2_REF[i]);
  }
  
  // Calcular a y b
  a = (N * sum_xy - sum_x * sum_y) / (N * sum_x2 - sum_x * sum_x);
  b = (sum_y - a * sum_x) / N;
  Serial.print(" a--------------|");
  Serial.println(a);
  Serial.print(" b--------------|");
  Serial.println(b);
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