#include <SD.h>
#include <SPI.h>
#include <LittleFS.h>

LittleFS_SPIFlash myfs;
const int chipSelectSD = BUILTIN_SDCARD;
const int chipSelect = 10;

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect.
  }

  // Initializing SD Card
  Serial.print("Initializing SD card and Flash Chip...");
  if (!SD.begin(chipSelectSD)) {
    Serial.println("Card failed, or not present");
    while (1) {
      // No SD card, so don't do anything more - stay stuck here
    }
  }
  Serial.println("SD initialized.");

  // Initializing Flash Chip
  if (!myfs.begin(chipSelect)) {
    Serial.printf("Error starting %s\n", "SPI FLASH");
    while (1) {
      // Error, so don't do anything more - stay stuck here
    }
  }
  Serial.println("Flash Chip initialized.");
  
  // open the read and write files
  File dataFileSD = SD.open("datalogNew.txt", FILE_WRITE);
  File dataFile = myfs.open("datalog.txt");

  // check if both reading and writing files are available
  Serial.println("Starting writing...");
  if (dataFile) {
    while (dataFile.available()) {
      //delay(100);
      dataFileSD.write(dataFile.read()); // maybe .write() instead
      //Serial.write(dataFile.read());
    }
    dataFile.close();
    dataFileSD.close();
    Serial.println("Finished writing");
  } else {
    Serial.println("error opening datalog.txt");
  } 
}

void loop()
{
}
