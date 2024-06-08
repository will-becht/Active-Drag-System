// Use BLESerial Tiny App on IOS; connect to DSD TECH and then type 1 or 0
// to turn led on teensy on or off

#include <SoftwareSerial.h>
SoftwareSerial mySerial(0,1);
int ledpin=13;

void setup(){
  mySerial.begin(9600);
  Serial.begin(9600);
  pinMode(ledpin,OUTPUT);
}
void loop(){
  int i;
   
  if (mySerial.available()){
    i=mySerial.read();
    Serial.println("DATA RECEIVED:");
    
    if(i=='1'){
      digitalWrite(ledpin,1);
      Serial.println("led on");
      mySerial.write("turning led on");
    }
    
    if(i=='0'){
      digitalWrite(ledpin,0);
      Serial.println("led off");
      mySerial.write("turning led off");
    }
  }
}
