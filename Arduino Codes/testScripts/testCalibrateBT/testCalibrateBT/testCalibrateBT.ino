#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Adafruit_MPL3115A2.h>
#include <Servo.h>
#include <SD.h>
#include <SPI.h>

const int chipSelect = BUILTIN_SDCARD;
int a = 0;
SoftwareSerial mySerial(0,1);
#define BNO055_SAMPLERATE_DELAY_MS (100)
Adafruit_BNO055 myIMU = Adafruit_BNO055();
//Servo myservo;
Adafruit_MPL3115A2 baro;
int ledpin=13;

void setup() {
  // put your setup code here, to run once:
  mySerial.begin(9600);
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  Serial.println("hey3");
  myIMU.begin();
  //myservo.attach(9);
  Serial.println("hey1");
  delay(1000);
  Serial.println("hey2");
  //myIMU.setExtCrystalUse(true);
  //baro.setSeaPressure(1013.26);
    Serial.println("hey5");

}

void loop() {
  Serial.println("hey7");
  // ***** READING IN DATA FROM SENSORS *****
  int i;
  uint8_t system, gyro, accel, mg = 0;
  myIMU.getCalibration(&system, &gyro, &accel, &mg);
  imu::Vector<3> acc =myIMU.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  float pressure = baro.getPressure();
  float altitude = baro.getAltitude();
  float temperature = baro.getTemperature();
  // *****
  

  // ***** BLUETOOTH TESTS *****
  if (mySerial.available()){
    i=mySerial.read();
    
    if(i=='1'){ // TEST IMU
      mySerial.write("Accel: ");
      mySerial.print(accel);
      mySerial.write(", Gyro: ");
      mySerial.print(gyro);
      mySerial.write(", Mag: ");
      mySerial.print(mg);
      mySerial.write(", Sys: ");
      mySerial.print(system);
    } else if(i=='2'){ // TEST ALTIMETER
      mySerial.write("Pressure (hPa): ");
      mySerial.print(pressure);
      mySerial.write(", Altitude (m): ");
      mySerial.print(altitude);
      mySerial.write(", Temperature (C): ");
      mySerial.print(temperature);
      mySerial.write(", Sys: ");
      mySerial.print(system);
    } else if(i=='3'){ // TEST SERVO
      //myservo.write(270);
      //delay(5000);
      //myservo.write(0);
      //delay(5000);
    }
  }
    // *****
  
  delay(250); // delay to account for sample rate
}
