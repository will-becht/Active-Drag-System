// will create a file on the sd card called datalog.txt with
// hey there on three consecutive new lines
// also prints to serial monitor

#include <SD.h>
#include <SPI.h>
const int chipSelect = BUILTIN_SDCARD;
int a = 0;

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect.
  }


  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1) {
      // No SD card, so don't do anything more - stay stuck here
    }
  }
  Serial.println("card initialized.");
}

void loop()
{
  if (a!=3) {
    // make a string for assembling the data to log:
    String dataString = "hey there";
  
    // open the file.
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
  
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
      // print to the serial port too:
      Serial.println(dataString);
    } else {
      // if the file isn't open, pop up an error:
      Serial.println("error opening datalog.txt");
    }
    delay(100); // run at a reasonable not-too-fast speed
    a = a+1;
  }
  
}
