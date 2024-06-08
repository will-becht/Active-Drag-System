#include <Adafruit_MPL3115A2.h>
Adafruit_MPL3115A2 baro;
void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Adafruit_MPL3115A2 test!");

  if (!baro.begin()) {
    Serial.println("Could not find sensor. Check wiring.");
    while(1);
  }
  baro.setSeaPressure(1013.26);

}

void loop() {
  // put your main code here, to run repeatedly:
  float pressure = baro.getPressure();
  float altitude = baro.getAltitude();
  float temperature = baro.getTemperature();

  Serial.println("-----------------");
  Serial.print("pressure = "); Serial.print(pressure); Serial.println(" hPa");
  Serial.print("altitude = "); Serial.print(altitude); Serial.println(" m");
  Serial.print("temperature = "); Serial.print(temperature); Serial.println(" C");

  delay(250);
}
