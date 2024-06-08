// ***** LIBRARIES ***** //
#include <Adafruit_BNO055.h>
#include <Adafruit_MPL3115A2.h>
#include <Adafruit_Sensor.h>
#include <LittleFS.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <utility/imumaths.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
// ********** //


// ***** INITIALIZING VARIABLES ***** //
// Delays for sample rates
#define BNO055_SAMPLERATE_DELAY_MS (100)
#define MPL3115A2_SAMPLERATE_DELAY_MS (250)

// IMU
Adafruit_BNO055 myIMU = Adafruit_BNO055();
uint8_t systemCal, gyroCal, accCal, magCal = 0;
imu::Vector<3> acc, gyro, mag, euler, linearAccel, gravity;
imu::Quaternion quat;
int8_t imuTemp;

// ALTIMETER
Adafruit_MPL3115A2 baro;
float pressure;
float altitude;
float temperature;
float sensorSampleDelay;

// Flash Chip
LittleFS_SPIFlash myfs;
const int chipSelect = 10;
File dataFile;
char dataFn[16];
char SDdataFn[16];
bool writing = false;
bool writeHeader = true;

// SD CARD
const int chipSelectSD = BUILTIN_SDCARD;

// Servo
Servo myServo;
int maxServoMicro = 1200;
int minServoMicro = 1000;

// State changing conditions
const float g = 9.8;
const float liftOffGThreshold = -3;
const float liftOffHThreshold = 10;
const float burnOutThreshold = 0;
const float burnOutTime = 5000; // in milli seconds
const float apogeeTime = 15000; // in milli seconds
const float apogeeTolerance = 5; // in meters
int missionPhase = 1; // STANDBY, PAD, BOOST, COAST, APOGEE, DONE
float startTime = 0;
float burnTime = 0;
float coastTime = 0;
float apogeeAlti = 0;
float onPadAlti = 1000000;

// Bluetooth
SoftwareSerial mySerial(0, 1);

// Buzzer
int buzzer = 6;

// Testing
bool testMode = true; 
// ********** //


// ***** CONNECTING SERIAL PORTS AND INITIALIZING SENSORS ***** //
void setup() {
  pinMode(buzzer, OUTPUT);
  myServo.attach(9);
  mySerial.begin(9600);
  while (!mySerial) {
    ; // wait for serial port to connect.
  }

  if (!myfs.begin(chipSelect)) {
    while (1) {
      // Error, so don't do anything more - stay stuck here
    }
  }
  //dataFn = String(random(100000000),DEC) + ".txt";
  int num = random(10000000);
  itoa(num, dataFn, 10);
  dataFile = myfs.open(dataFn, FILE_WRITE);
  

  if (!baro.begin()) {
    while (1);
  }
  //baro.setSeaPressure(1013.26);
  baro.setSeaPressure(1030);

  if (!myIMU.begin()) {
    while (1);
  }
  myIMU.setExtCrystalUse(true);

  playBuzzer(1500);
}

void loop() {     
  // READ IN DATA FROM IMU
  myIMU.getCalibration(&systemCal, &gyroCal, &accCal, &magCal);
  acc = myIMU.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  gyro = myIMU.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  mag = myIMU.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
  euler = myIMU.getVector(Adafruit_BNO055::VECTOR_EULER);
  linearAccel = myIMU.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  gravity = myIMU.getVector(Adafruit_BNO055::VECTOR_GRAVITY);
  quat = myIMU.getQuat();
  imuTemp = myIMU.getTemp();

  // READ IN DATA FROM BARO
  pressure = baro.getPressure();
  altitude = baro.getAltitude() - onPadAlti;
  temperature = baro.getTemperature();
  sensorSampleDelay = millis();

  // SAVE DATA TO FLASH CHIP
  if (writing) {
    if (dataFile) {
      saveData();
    } else {
      mySerial.print("Error Saving Data");
    }
  }

  int i;
  if (mySerial.available()) {
    i = mySerial.read();
    bluetoothCommand(i);
  }
  
  // ACT ACCORDING TO MISSION PHASE
  switch (missionPhase) {
                                                        // *** PHASE 1: STANDBY *** // (NO DATA SAVED)
    case 1:
      if (writing) {
        writing = false;
        mySerial.write("Stopped Logging Data");
      }
      break;

                                                        // *** PHASE 2: ON PAD *** // (RECORDING DATA, WILL SHIFT PHASES DEPENDING ON ALT AND ACC)
    case 2:
      if (!writing) {
        writing = true;
        mySerial.write("Started Logging Data");
      }
      if (acc[2] <= g*liftOffGThreshold && altitude >= liftOffHThreshold) {
        missionPhase = 3;
        burnTime = millis();
      }
      // test mode
      if (testMode && millis() - startTime >= 10*1000) {
        missionPhase = 3;
        burnTime = millis();
      }
      break;

                                                        // *** PHASE 3: MOTOR BURN *** // (RECORDING DATA, WILL SHIFT PHASES DEPENDING ON ACC AND burnOutTime)
    case 3:
      if (acc[2] >= g*burnOutThreshold || millis() - burnTime >= burnOutTime) {
        missionPhase = 4;
        coastTime = millis();
      }
      break;

                                                        // *** PHASE 4: COAST *** // (DEPLOYING AIRBRAKES, WILL SHIFT PHASES DEPENDING ON ALT and apogeeTime)
    case 4:
      if (altitude > apogeeAlti) {
          apogeeAlti = altitude;
      }
      if (apogeeAlti - altitude >= apogeeTolerance || millis() - coastTime >= apogeeTime) {
          missionPhase = 5;
      }
      // deploy airbrakes until apogee
      myServo.writeMicroseconds(maxServoMicro);
      break;

                                                        // *** PHASE 5: APOGEE *** // (RETRACT TABS)
    case 5:
      myServo.writeMicroseconds(minServoMicro);
      break;
    
                                                        // *** PHASE 6: DONE *** // (LAUNCH VEHICLE ON GROUND)
    case 6:
      if (writing) {
        mySerial.write("Stopped Logging Data");
        writing = false;
      }
      break;
    default:
      break;
  }

  // Delaying based on sensor sample rate
  if (sensorSampleDelay > millis()) {
    delay(sensorSampleDelay - millis());
  }
}

// BLUETOOTH COMMANDS
void bluetoothCommand(int i) {
  switch (i) {
    case '1':
      sendCal();
      break;
    case '2':
      sendBaro();
      sendAcc();
      break;
    case '3':
      mySerial.write("Beginning Servo Test");
      myServo.writeMicroseconds(minServoMicro);
      delay(2000);
      myServo.writeMicroseconds(maxServoMicro);
      break;
    case '4':
      missionPhase = 1;
      mySerial.write("STANDBY Phase Initiated");
      break;
    case '5':
      missionPhase = 2;
      startTime = millis();
      mySerial.write("PAD Phase Initiated, Ready for Launch");
      break;
    case '6':
      mySerial.write("Current Mission Phase: ");
      mySerial.print(missionPhase);
      break;
    case '7':
      mySerial.write("Initalizing altimeter\n");
      onPadAlti = initializeAltitude();
      mySerial.write("Ground Altitude: ");
      mySerial.print(onPadAlti);
      break;
    case '8':
      mySerial.write("Menu:\n");
      mySerial.write("1. Calibration, 2. Altimeter, IMU, 3. Servo Test, 4. Phase 1, 5. Phase 2, 6. Current Phase, 7. Set Altimeter, 8. Menu");
      break;
    default:
      break;
  }
}

// SETTING GROUND ALTITUDE
float initializeAltitude() {
  int t = millis();
  int count = 0; 
  float sum = 0;
  while (millis() - t < 20.0*1000.0) {
    sum = sum+baro.getAltitude();
    count ++;
  }
  return sum/count;
}

// SAVING DATA 
void saveData() {
  if (writeHeader) {
    dataFile.println("time,accCal,gyroCal,magCal,systemCal,accX,accY,accZ,gyroX,gyroY,gyroZ,magX,magY,magZ,eulerX,eulerY,eulerZ,linearAccelX,linearAccelY,linearAccelZ,gravityX,gravityY,gravityZ,quatW,quatX,quatY,quatZ,imuTemp,baroTemp,pressure,altitude,missionPhase,onPadAlti");
    writeHeader = false;
  }

  // TIMESTAMP
  dataFile.print(millis());
  dataFile.write(",");
  
  // IMU CALIBRATION
  dataFile.print(accCal);
  dataFile.write(",");
  dataFile.print(gyroCal);
  dataFile.write(",");
  dataFile.print(magCal);
  dataFile.write(",");
  dataFile.print(systemCal);
  dataFile.write(",");

  // ACCELEROMETER DATA(m/s^2)
  dataFile.print(acc.x());
  dataFile.write(",");
  dataFile.print(acc.y());
  dataFile.write(",");
  dataFile.print(acc.z());
  dataFile.write(",");

  // GYROSCOPE DATA(rps)
  dataFile.print(gyro.x());
  dataFile.write(",");
  dataFile.print(gyro.y());
  dataFile.write(",");
  dataFile.print(gyro.z());
  dataFile.write(",");

  // MAGNETOMETER DATA(uT)
  dataFile.print(mag.x());
  dataFile.write(",");
  dataFile.print(mag.y());
  dataFile.write(",");
  dataFile.print(mag.z());
  dataFile.write(",");

  // EULER DATA
  dataFile.print(euler.x());
  dataFile.write(",");
  dataFile.print(euler.y());
  dataFile.write(",");
  dataFile.print(euler.z());
  dataFile.write(",");

  // LINEAR ACCELERATION DATA(m/s^2)
  dataFile.print(linearAccel.x());
  dataFile.write(",");
  dataFile.print(linearAccel.y());
  dataFile.write(",");
  dataFile.print(linearAccel.z());
  dataFile.write(",");

  // GRAVITY DATA(m/s^2)
  dataFile.print(gravity.x());
  dataFile.write(",");
  dataFile.print(gravity.y());
  dataFile.write(",");
  dataFile.print(gravity.z());
  dataFile.write(",");

  // QUATERNION DATA
  dataFile.print(quat.w(), 4);
  dataFile.write(",");
  dataFile.print(quat.x(), 4);
  dataFile.write(",");
  dataFile.print(quat.y(), 4);
  dataFile.write(",");
  dataFile.print(quat.z(), 4);
  dataFile.write(",");

  // TEMPERATURE DATA (degC)
  dataFile.print(imuTemp);
  dataFile.write(",");
  dataFile.print(temperature);
  dataFile.write(",");

  // ALTITUDE DATA
  dataFile.print(pressure); 
  dataFile.write(",");
  dataFile.print(altitude); 

  // MISSION PHASE
  dataFile.write(",");
  dataFile.print(missionPhase);

  // ON PAD ALTITUDE
  dataFile.write(",");
  dataFile.println(onPadAlti);
}

// Sending Calibration Data to Phone
void sendCal() {
  mySerial.write("A: ");
  mySerial.print(accCal);
  mySerial.write(", G: ");
  mySerial.print(gyroCal);
  mySerial.write(", M: ");
  mySerial.print(magCal);
  mySerial.write(", S: ");
  mySerial.print(systemCal);
}

// Sending Altimeter Data to Phone
void sendBaro() {
  mySerial.write("P: ");
  mySerial.print(pressure); // hPa
  mySerial.write(", A: ");
  mySerial.print(altitude); // m
  mySerial.write(", T: ");
  mySerial.print(temperature); // deg C
}

// Sending Accelerometer Data to Phone
void sendAcc() {
  mySerial.write("X: ");
  mySerial.print(acc.x());
  mySerial.write(", Y: ");
  mySerial.print(acc.y());
  mySerial.write(", Z: ");
  mySerial.print(acc.z());
}

void playBuzzer(int t) {
  tone(buzzer, 1000); 
  delay(t);
  noTone(buzzer);
}
