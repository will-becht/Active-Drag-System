#include <SoftwareSerial.h>
#include <LittleFS.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Adafruit_MPL3115A2.h>
#include <Servo.h>

// INITIALIZING VARIABLES
#define BNO055_SAMPLERATE_DELAY_MS (100)
#define MPL3115A2_SAMPLERATE_DELAY_MS (250)
Adafruit_MPL3115A2 baro;
Adafruit_BNO055 myIMU = Adafruit_BNO055();
LittleFS_SPIFlash myfs;
SoftwareSerial mySerial(0, 1);
const int chipSelect = 10;
File dataFile;
bool writing = false;
bool writeHeader = true;
Servo myServo;
int missionPhase = 1; // STANDBY, PAD, BOOST, COAST, APOGEE, DONE
uint8_t systemCal, gyroCal, accCal, magCal = 0;
imu::Vector<3> acc, gyro, mag, euler, linearAccel, gravity;
imu::Quaternion quat;
int8_t imuTemp;
float pressure;
float altitude;
float temperature;

// CONNECTING SERIAL PORTS AND INITIALIZING SENSORS
void setup() {
  mySerial.begin(9600);
  Serial.begin(9600);
  myServo.attach(9);
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  while (!mySerial) {
    ; // wait for serial port to connect.
  }
  Serial.println("Serial Ports Connected.");

  if (!myfs.begin(chipSelect)) {
    Serial.printf("Error starting %s\n", "SPI FLASH");
    while (1) {
      // Error, so don't do anything more - stay stuck here
    }
  }
  Serial.println("Flash Chip initialized.");

  if (!baro.begin()) {
    Serial.println("Could not find sensor. Check wiring.");
    while (1);
  }
  baro.setSeaPressure(1013.26);
  Serial.println("Baro Initialized.");

  if (!myIMU.begin()) {
    Serial.println("Could not find sensor. Check wiring.");
    while (1);
  }
  myIMU.setExtCrystalUse(true);
  Serial.println("IMU initialized.");
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
  altitude = baro.getAltitude();
  temperature = baro.getTemperature();

  // SAVE DATA TO FLASH CHIP
  if (writing) {
    if (dataFile) {
      saveHeader();
      saveData();
    } else {
      Serial.println("error opening datalog.txt");
    }
  }

  // ACT ACCORDING TO MISSION PHASE
  switch (missionPhase) {
    // STANDBY
    case 1:
      int i;
      if (mySerial.available()) {
        i = mySerial.read();
        bluetoothCommand(i);
      }
      if (writing) {
        writing = false;
        dataFile.close();
        mySerial.write("Stopped Logging Data");
      }
      break;

    // ON PAD (RECORDING DATA, READY FOR LAUNCH)
    case 2:
      if (mySerial.available()) {
        i = mySerial.read();
        bluetoothCommand(i);
      }
      if (!writing) {
        writing = true;
        dataFile = myfs.open("datalog.txt", FILE_WRITE);
        mySerial.write("Started Logging Data");
      }
      break;

    // MOTOR BURN
    case 3:
      break;

    // COASTING BEFORE APOGEE
    case 4:
      // deploy airbrakes until apogee
      break;

    // APOGEE (RETRACT TABS FULLY)
    case 5:
      break;
    
    // DONE (LAUNCH VEHICLE ON GROUND)
    case 6:
      if (writing) {
        mySerial.write("Stopped Logging Data");
        writing = false;
        dataFile.close();
      }
      break;
    default:
      break;
  }
  delay(MPL3115A2_SAMPLERATE_DELAY_MS);
}


void bluetoothCommand(int i) {
  switch (i) {
    case '1':
      sendCal();
      break;
    case '2':
      sendBaro();
      break;
    case '3':
      sendAcc();
      break;
    case '4':
      mySerial.write("Beginning Servo Test");
      myServo.write(90);
      break;
    case '5':
      missionPhase = 1;
      mySerial.write("STANDBY Phase Initiated");
      break;
    case '6':
      missionPhase = 2;
      mySerial.write("PAD Phase Initiated");
      break;
    default:
      break;
  }
}

void saveHeader() {
  if (writeHeader) {
    dataFile.println("accCal,gyroCal,magCal,systemCal,accX,accY,accZ,gyroX,gyroY,gyroZ,magX,magY,magZ,eulerX,eulerY,eulerZ,linearAccelX,linearAccelY,linearAccelZ,gravityX,gravityY,gravityZ,quatW,quatX,quatY,quatZ,imuTemp,baroTemp,pressure,altitude");
    writeHeader = false;
  }
}

void saveData() {
  //dataFile.write("IMU CALIBRATION // Accelerometer: ");
  dataFile.print(accCal);
  dataFile.write(",");
  //dataFile.write(", Gyroscope: ");
  dataFile.print(gyroCal);
  dataFile.write(",");
  //dataFile.write(", Magnetometer: ");
  dataFile.print(magCal);
  dataFile.write(",");
  //dataFile.write(", System: ");
  dataFile.print(systemCal);
  dataFile.write(",");

  //dataFile.write("ACCELEROMETER DATA(m/s^2) // X: ");
  dataFile.print(acc.x());
  dataFile.write(",");
  //dataFile.print(", Y: ");
  dataFile.print(acc.y());
  dataFile.write(",");
  //dataFile.print(", Z: ");
  dataFile.print(acc.z());
  dataFile.write(",");

  //dataFile.write("GYROSCOPE DATA(rps) // X: ");
  dataFile.print(gyro.x());
  dataFile.write(",");
  //dataFile.print(", Y: ");
  dataFile.print(gyro.y());
  dataFile.write(",");
  //dataFile.print(", Z: ");
  dataFile.print(gyro.z());
  dataFile.write(",");

  //dataFile.write("MAGNETOMETER DATA(uT) // X: ");
  dataFile.print(mag.x());
  dataFile.write(",");
  //dataFile.print(", Y: ");
  dataFile.print(mag.y());
  dataFile.write(",");
  //dataFile.print(", Z: ");
  dataFile.print(mag.z());
  dataFile.write(",");

  //dataFile.write("EULER DATA // X: ");
  dataFile.print(euler.x());
  dataFile.write(",");
  //dataFile.print(", Y: ");
  dataFile.print(euler.y());
  dataFile.write(",");
  //dataFile.print(", Z: ");
  dataFile.print(euler.z());
  dataFile.write(",");

  //dataFile.write("LINEAR ACCELERATION DATA(m/s^2) // X: ");
  dataFile.print(linearAccel.x());
  dataFile.write(",");
  //dataFile.print(", Y: ");
  dataFile.print(linearAccel.y());
  dataFile.write(",");
  //dataFile.print(", Z: ");
  dataFile.print(linearAccel.z());
  dataFile.write(",");

  //dataFile.write("GRAVITY DATA(m/s^2) // X: ");
  dataFile.print(gravity.x());
  dataFile.write(",");
  //dataFile.print(", Y: ");
  dataFile.print(gravity.y());
  dataFile.write(",");
  //dataFile.print(", Z: ");
  dataFile.print(gravity.z());
  dataFile.write(",");

  //dataFile.write("QUATERNION DATA // W: ");
  dataFile.print(quat.w(), 4);
  dataFile.write(",");
  //dataFile.print(", X: ");
  dataFile.print(quat.x(), 4);
  dataFile.write(",");
  //dataFile.print(", Y: ");
  dataFile.print(quat.y(), 4);
  dataFile.write(",");
  //dataFile.print(", Z: ");
  dataFile.print(quat.z(), 4);
  dataFile.write(",");

  //dataFile.write("TEMPERATURE DATA (degC) // IMU: ");
  dataFile.print(imuTemp);
  dataFile.write(",");
  //dataFile.write(", Baro: ");
  dataFile.print(temperature);
  dataFile.write(",");

  //dataFile.write("ALTITUDE DATA // Pressure(hPa): ");
  dataFile.print(pressure); // hPa
  dataFile.write(",");
  //dataFile.write(", Altitude(m): ");
  dataFile.println(altitude); // m
  //dataFile.println("---------------------");
}

void sendCal() {
  Serial.println("Sending Calibration Data to Phone");
  mySerial.write("A: ");
  mySerial.print(accCal);
  mySerial.write(", G: ");
  mySerial.print(gyroCal);
  mySerial.write(", M: ");
  mySerial.print(magCal);
  mySerial.write(", S: ");
  mySerial.print(systemCal);
}

void sendBaro() {
  Serial.println("Sending Baro Data to Phone");
  mySerial.write("P: ");
  mySerial.print(pressure); // hPa
  mySerial.write(", A: ");
  mySerial.print(altitude); // m
  mySerial.write(", T: ");
  mySerial.print(temperature); // deg C
}

void sendAcc() {
  Serial.println("Sending Acceleration Data to Phone");
  mySerial.write("X: ");
  mySerial.print(acc.x());
  mySerial.write(", Y: ");
  mySerial.print(acc.y());
  mySerial.write(", Z: ");
  mySerial.print(acc.z());
}


//sensors_event_t eulerFused , gyroFused , linearAccelFused, magFused, accFused, gravityFused;
//myIMU.getEvent(&eulerFused, Adafruit_BNO055::VECTOR_EULER);
//myIMU.getEvent(&gyroFused, Adafruit_BNO055::VECTOR_GYROSCOPE);
//myIMU.getEvent(&linearAccelFused, Adafruit_BNO055::VECTOR_LINEARACCEL);
//myIMU.getEvent(&magFused, Adafruit_BNO055::VECTOR_MAGNETOMETER);
//myIMU.getEvent(&accFused, Adafruit_BNO055::VECTOR_ACCELEROMETER);
//myIMU.getEvent(&gravityFused, Adafruit_BNO055::VECTOR_GRAVITY);

//sensors_event_t event;
//myIMU.getEvent(&event);
//Serial.print(event.orientation.x, 4);
//Serial.print(event.acceleration.x, 4);
//Serial.print(event.magnetic.x, 4);
//Serial.print(event.gyro.x, 4);
