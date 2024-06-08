
#include <LittleFS.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Adafruit_MPL3115A2.h>
#include <Servo.h>
#include <SD.h>
#include <SPI.h>

LittleFS_SPIFlash myfs;  // Used to create FS on SPI NOR flash chips such as the W25Q16JV*IQ/W25Q16FV,
                         // for the full list of supported NOR flash see 
                         // https://github.com/PaulStoffregen/LittleFS#nor-flash

const int chipSelect = 10; 
File dataFile;  // Specifes that dataFile is of File type
SoftwareSerial mySerial(0,1);
#define BNO055_SAMPLERATE_DELAY_MS (100)
Adafruit_BNO055 myIMU = Adafruit_BNO055();
//Servo myservo;
Adafruit_MPL3115A2 baro;
int ledpin=13;
int writing=1;

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  mySerial.begin(9600);
  Serial.println("here1");
  while (!Serial) {
    // wait for serial port to connect.
  }
  Serial.println("here2");
  myIMU.begin();
  delay(1000);
  myIMU.setExtCrystalUse(true);
  baro.setSeaPressure(1013.26);
  Serial.println("here3");

  Serial.print("Initializing LittleFS ...");

  // see if the Flash is present and can be initialized:
  if (!myfs.begin(chipSelect)) {
    Serial.printf("Error starting %s\n", "SPI FLASH");
    while (1) {
      // Error, so don't do anything more - stay stuck here
    }
  }
  Serial.println("LittleFS initialized.");
}

void loop()
{
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
      
    } else if(i=='4'){ // START RECORDING DATA
      Serial.println("Starting Data Log");
      writing=2;
      
    } else if(i=='5'){ // STOP RECORDING DATA
      Serial.println("Stopping Data Log");
      writing=1;
      dataFile.close();
    }
  }
    // *****
  if(writing==2) {
      Serial.println("Logging Data");
      dataFile = myfs.open("datalog.txt", FILE_WRITE);
      String dataString = "";
      dataString += String(pressure,3) + String(altitude,3) + String(temperature,3);
      if (dataFile) {
        dataFile.println(dataString);
      } else {
        Serial.println("error opening datalog.txt");
      }
  }
}




void listFiles()
{
  Serial.print("\n Space Used = ");
  Serial.println(myfs.usedSize());
  Serial.print("Filesystem Size = ");
  Serial.println(myfs.totalSize());

  printDirectory(myfs);
}

void eraseFiles()
{
  myfs.quickFormat();  // performs a quick format of the created di
  Serial.println("\nFiles erased !");
}

void printDirectory(FS &fs) {
  Serial.println("Directory\n---------");
  printDirectory(fs.open("/"), 0);
  Serial.println();
}

void printDirectory(File dir, int numSpaces) {
   while(true) {
     File entry = dir.openNextFile();
     if (! entry) {
       //Serial.println("** no more files **");
       break;
     }
     printSpaces(numSpaces);
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numSpaces+2);
     } else {
       // files have sizes, directories do not
       printSpaces(36 - numSpaces - strlen(entry.name()));
       Serial.print("  ");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}

void printSpaces(int num) {
  for (int i=0; i < num; i++) {
    Serial.print(" ");
  }
}
