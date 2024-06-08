
#include <LittleFS.h>


LittleFS_SPIFlash myfs;  // Used to create FS on SPI NOR flash chips such as the W25Q16JV*IQ/W25Q16FV,
                         // for the full list of supported NOR flash see 
                         // https://github.com/PaulStoffregen/LittleFS#nor-flash

const int chipSelect = 10; 
File dataFile;  // Specifes that dataFile is of File type
int record_count = 0;
bool write_data = false;

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    // wait for serial port to connect.
  }
  Serial.println("\n" __FILE__ " " __DATE__ " " __TIME__);

  Serial.print("Initializing LittleFS ...");

  // see if the Flash is present and can be initialized:
  if (!myfs.begin(chipSelect)) {
    Serial.printf("Error starting %s\n", "SPI FLASH");
    while (1) {
      // Error, so don't do anything more - stay stuck here
    }
  }
  Serial.println("LittleFS initialized.");

  Serial.println("\nLogging Data!!!");
  // opens a file or creates a file if not present,  FILE_WRITE will append data to
  // to the file created.
  dataFile = myfs.open("datalog.txt", FILE_WRITE);
  String dataString = "kbdKSABDC"; //"hey there 423";
  
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
  } else {
    Serial.println("error opening datalog.txt");
  }
  
  delay(100); // run at a reasonable not-too-fast speed for testing

  Serial.println("\nStopped Logging Data!!!");
  dataFile.close();

  // Reading file
  Serial.println("\nReading Data!!!");
  dataFile = myfs.open("datalog.txt");
  // if the file is available, write to it:
  if (dataFile) {
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  } 

  Serial.println("\nListing Files!!!");
  listFiles();

  Serial.println("\nErasing Files!!!");
  eraseFiles();

  //Serial.println("\nRelisting Files!!!");
  //listFiles();
}

void loop()
{ 

  

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
