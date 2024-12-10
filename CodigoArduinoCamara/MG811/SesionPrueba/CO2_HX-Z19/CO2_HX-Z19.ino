#include <MHZ19.h>

MHZ19 mhz(&Serial1);

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting...");

  Serial1.begin(9600);
  Serial.print("Digital"); Serial.print(","); Serial.println("Analog");
}

void loop()
{
  MHZ19_RESULT response = mhz.retrieveData();
  if (response == MHZ19_RESULT_OK)
  { int Digital = mhz.getCO2() ;
    int Analog = analogRead(A0) ;
    Serial.print(Digital); Serial.print(","); Serial.println(Analog);
    //Serial.print("Analog out");
    //Serial.println(analogRead(A0));
    //Serial.print(F("Temperature: "));
    // Serial.println( mhz.getTemperature()) ;
    //float F = mhz.getTemperature() ;
    //float C = (F -32) * 5 / 9.0 ;
    //Serial.println(C) ;
    
    //Serial.println(T);
    //Serial.print(F("Accuracy: "));
    //Serial.println(mhz.getAccuracy());
  }
  else
  {
    Serial.print(F("Error, code: "));
    Serial.println(response);
  }
  
  delay(1000);
}
