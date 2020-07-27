#include <TinyGPS++.h>

TinyGPSPlus gps;

#include "Firebase_Arduino_WiFiNINA.h"

// Firebase Credentials 
#define FIREBASE_HOST "smart-seat-f37f8.firebaseio.com"
#define FIREBASE_AUTH "1OYRy3lyyPMozMXJ4GGuQPrM3Btjoerfc4oVzjpk"
#define WIFI_SSID "AlanDell"
#define WIFI_PASSWORD "0603021a"


//Define Firebase data object
FirebaseData firebaseData;


// Accelerometer readings from Adafruit LIS3DH

#include <Wire.h>

#include <SPI.h>

#include <Adafruit_LIS3DH.h>

#include <Adafruit_Sensor.h>

// Used for software SPI
#define LIS3DH_CLK 13# define LIS3DH_MISO 12# define LIS3DH_MOSI 11
// Used for hardware & software SPI
# define LIS3DH_CS 10

Adafruit_LIS3DH lis = Adafruit_LIS3DH();



//PIR Motion Variables
int pirInput = 5; // choose the input pin (for PIR sensor)
int pirState = false; // we start, assuming no motion detected
int pirVal = 0; // variable for reading the pin status

// LED Variable Declaration -- Change to Analog
const int forceLED = 2;
const int motionLED = 3;
const int accLED = 4;
void setup() {
    pinMode(A1, INPUT);
  // Controls the PIR Sensorinput
  pinMode(pirInput, INPUT); // declare sensor as input
  // LED Set as outputs
  pinMode(forceLED, OUTPUT);
  pinMode(motionLED, OUTPUT);
  pinMode(accLED, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
    Serial1.begin(9600);
  Serial.begin(115200);
     while (!Serial) delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("LIS3DH test!");

  if (!lis.begin(0x18)) { // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1) yield();
  }
  Serial.println("LIS3DH found!");
digitalWrite(LED_BUILTIN,HIGH);
delay(500);
digitalWrite(LED_BUILTIN,LOW);
delay(120);
digitalWrite(LED_BUILTIN,LOW);
  lis.setRange(LIS3DH_RANGE_4_G); // 2, 4, 8 or 16 G!

  Serial.print("Range = ");
  Serial.print(2 << lis.getRange());
  Serial.println("G");

  //  WiFi and Firebase Setup
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  //Provide the autntication data
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, WIFI_SSID, WIFI_PASSWORD);
  Firebase.reconnectWiFi(true);
  // Controls the force sensor input
  pinMode(A1, INPUT);
  // Controls the PIR Sensorinput
  pinMode(pirInput, INPUT); // declare sensor as input
  // LED Set as outputs
  pinMode(forceLED, OUTPUT);
  pinMode(motionLED, OUTPUT);
  digitalWrite(accLED, HIGH);
  digitalWrite(LED_BUILTIN,HIGH);

}

void loop() {
   int count = 0;
 // Accelerometer Variables
  float accXY[] = {0,0, 0,0, 0, 0, 0,0,0, 0};
  float accX[] = {0,0, 0,0, 0, 0, 0,0,0, 0};
// Tracks the averages of the X and Y m/s^2
  float averageArray = 0;
  float averageArrayXY = 0;
  float averageArray2 = 0;
  float averageArrayXY2 = 0;
  // Timer
  int waitTime = 0;
  int weight = analogRead(A1);
  weight /= 37;
  if(weight >2){
    Serial.println("Child placed on seat");
    digitalWrite(forceLED,HIGH);
    // Determine if there is a person in the vehicle
    // Need time for them to get into the car after placing the child on the seat
    delay(5000);
    pirVal = digitalRead(pirInput);
    // The sensor is active
   if(pirVal == HIGH){
      pirState= true;
    }
      while(pirState){
        pirVal = digitalRead(pirInput);
       if (pirVal == HIGH){
       pirState = false;
        }
       pirVal = digitalRead(pirInput);
       Serial.println("Data: "+ pirVal);
      Serial.println("DETECTION");
      alert(0);
      accData();
      // Accelerometer Reading when a person is in the car
    delay(1600);
      
      }if(pirVal != HIGH){
        pirState = false;
          digitalWrite(motionLED,HIGH);
          Serial.println("No Detection");
          while(true){

            waitTime +=1;
            delay(5000);
            Serial.println(waitTime);
          if(waitTime > 1 && waitTime < 5  ){
            //Send Active Response
            alert(1);
            int weight = analogRead(A1);
            weight /= 37;
            pirVal = digitalRead(pirInput);
            if(pirVal == HIGH || weight < 7){
                Serial.println("BROKEN1");
                break;
              }
              }else if(waitTime > 5){
            // Send emergency response
            alert(2);
            int weight = analogRead(A1);
            weight /= 37;
            pirVal = digitalRead(pirInput);
              if(pirVal == HIGH && weight < 7){
                    alert(0);
                    Serial.println("BROKEN2");
                    break;
                    }
                }
            }  
          
        }
    }else{
      // System turns off
  digitalWrite(forceLED,LOW);
  digitalWrite(motionLED,LOW);
  digitalWrite(accLED,LOW);
      delay(100);
      }

}

void alert(int i) {
  String path = "/tempandgps";
  String jsonStr;

  float lati = 2;

  while (Serial1.available()) {

    if (gps.encode(Serial1.read())) {
      if (gps.location.isValid()) {
      digitalWrite(LED_BUILTIN, HIGH);
        Serial.println("-----------------------------------");
        Serial.println("----------Begin Set Test-----------");
        Serial.println("-----------------------------------");
        Serial.println();
 // if (gps.speed.isUpdated())
//  {
        if (Firebase.setInt(firebaseData, path + "/Int/driving",gps.speed.mph() )) {
          Serial.println("----------Set result-----------");
          Serial.println("PATH: " + firebaseData.dataPath());
          Serial.println("TYPE: " + firebaseData.dataType());
          Serial.print("VALUE: ");
          if (firebaseData.dataType() == "int")
            Serial.println(firebaseData.intData());
          else if (firebaseData.dataType() == "float")
            Serial.println(firebaseData.floatData());
          else if (firebaseData.dataType() == "boolean")
            Serial.println(firebaseData.boolData() == 1 ? "true" : "false");
          else if (firebaseData.dataType() == "string")
            Serial.println(firebaseData.stringData());
          else if (firebaseData.dataType() == "json")
            Serial.println(firebaseData.jsonData());
          Serial.println("--------------------------------");
          Serial.println();
        } else {
          Serial.println("----------Can't set data--------");
          Serial.println("REASON: " + firebaseData.errorReason());
          Serial.println("--------------------------------");
          Serial.println();
        }
 // }
        // SEND THE LONGITUDE COORDINATES

        if (Firebase.setInt(firebaseData, path + "/Int/safe", i)) {
          Serial.println("----------Set result-----------");
          Serial.println("PATH: " + firebaseData.dataPath());
          Serial.println("TYPE: " + firebaseData.dataType());
          Serial.print("VALUE: ");
          if (firebaseData.dataType() == "int")
            Serial.println(firebaseData.intData());
          else if (firebaseData.dataType() == "float")
            Serial.println(firebaseData.floatData());
          else if (firebaseData.dataType() == "boolean")
            Serial.println(firebaseData.boolData() == 1 ? "true" : "false");
          else if (firebaseData.dataType() == "string")
            Serial.println(firebaseData.stringData());
          else if (firebaseData.dataType() == "json")
            Serial.println(firebaseData.jsonData());
          Serial.println("--------------------------------");
          Serial.println();
        } else {
          Serial.println("----------Can't set data--------");
          Serial.println("REASON: " + firebaseData.errorReason());
          Serial.println("--------------------------------");
          Serial.println();
        }

        if (Firebase.setFloat(firebaseData, path + "/Int/LONG", gps.location.lng())) {
          Serial.println("----------Set result-----------");
          Serial.println("PATH: " + firebaseData.dataPath());
          Serial.println("TYPE: " + firebaseData.dataType());
          Serial.print("VALUE: ");
          if (firebaseData.dataType() == "int")
            Serial.println(firebaseData.intData());
          else if (firebaseData.dataType() == "float")
            Serial.println(firebaseData.floatData());
          else if (firebaseData.dataType() == "boolean")
            Serial.println(firebaseData.boolData() == 1 ? "true" : "false");
          else if (firebaseData.dataType() == "string")
            Serial.println(firebaseData.stringData());
          else if (firebaseData.dataType() == "json")
            Serial.println(firebaseData.jsonData());
          Serial.println("--------------------------------");
          Serial.println();
        } else {
          Serial.println("----------Can't set data--------");
          Serial.println("REASON: " + firebaseData.errorReason());
          Serial.println("--------------------------------");
          Serial.println();
        }

        // SEND THE LATITIUDE COORDINATES 

        if (Firebase.setFloat(firebaseData, path + "/Int/LAT", gps.location.lat())) {
          Serial.println("----------Set result-----------");
          Serial.println("PATH: " + firebaseData.dataPath());
          Serial.println("TYPE: " + firebaseData.dataType());
          Serial.print("VALUE: ");
          if (firebaseData.dataType() == "int")
            Serial.println(firebaseData.intData());
          else if (firebaseData.dataType() == "float")
            Serial.println(firebaseData.floatData());
          else if (firebaseData.dataType() == "boolean")
            Serial.println(firebaseData.boolData() == 1 ? "true" : "false");
          else if (firebaseData.dataType() == "string")
            Serial.println(firebaseData.stringData());
          else if (firebaseData.dataType() == "json")
            Serial.println(firebaseData.jsonData());
          Serial.println("--------------------------------");
          Serial.println();
        } else {
          Serial.println("----------Can't set data--------");
          Serial.println("REASON: " + firebaseData.errorReason());
          Serial.println("--------------------------------");
          Serial.println();
        }

        break;
      }
      // Delay between updates
      delay(2000);
    }
  }

  // TEMP
  float temperature = 0.0; // stores the calculated temperature
  int sample; // counts through ADC samples
  float ten_samples = 0.0; // stores sum of 10 samples

  // take 10 samples from the MCP9700
  for (sample = 0; sample < 10; sample++) {
    // convert A0 value to temperature
    // convert A0 value to temperature
        temperature = ((float)analogRead(A0) * 5.0 / 1024.0) - 0.5;
        temperature = temperature / 0.01;
        // sample every 0.1 seconds
    // sample every 0.1 seconds
    delay(250);
    // sum of all samples
    ten_samples = ten_samples + temperature;
  }
  // get the average value of 10 temperatures
  temperature = ten_samples / 10.0;
  // send temperature out of serial port
  Serial.print(temperature);
  Serial.println(" deg. F");
  ten_samples = 0.0;

  // PUSH TEMP

  if (Firebase.setInt(firebaseData, path + "/Int/TEMP", temperature)) {
    Serial.println("----------Set result-----------");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.print("VALUE: ");
    if (firebaseData.dataType() == "int")
      Serial.println(firebaseData.intData());
    else if (firebaseData.dataType() == "float")
      Serial.println(firebaseData.floatData());
    else if (firebaseData.dataType() == "boolean")
      Serial.println(firebaseData.boolData() == 1 ? "true" : "false");
    else if (firebaseData.dataType() == "string")
      Serial.println(firebaseData.stringData());
    else if (firebaseData.dataType() == "json")
      Serial.println(firebaseData.jsonData());
    Serial.println("--------------------------------");
    Serial.println();
  } else {
    Serial.println("----------Can't set data--------");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("--------------------------------");
    Serial.println();
  }

digitalWrite(LED_BUILTIN, LOW);

}


void accData(){
  
  String path = "/accData";
  String jsonStr;

  Serial.println("-----------------------------------");
  Serial.println("----------Begin Set Test-----------");
  Serial.println("-----------------------------------");
  Serial.println();

float x = 0;
float userSpeed = 0;
float breakSpeed = 0;
int count = 0;
int breakFlag=0; 
float acc[] = {0,0,0,0,0,0,0,0,0,0};
float pi = 3.1415926535;
float degree_val = 0;
 float averageVal = 0;
 sensors_event_t event; 
 lis.getEvent(&event);
 float angleX = event.acceleration.x ; 
 float angleY = event.acceleration.y ; 
 float angleZ = event.acceleration.z ; 
 float angleX2 = atan(angleX / sqrt(pow(angleY,2) + pow(angleZ,2) ));
 float degreeX = (angleX2 / pi)  * 180;

for(int i = 0; i < 10; i++){
 sensors_event_t event; 
 lis.getEvent(&event);
  float valX = event.acceleration.x ; 
 float valY = event.acceleration.y ; 
 float valZ = event.acceleration.z ; 
 averageVal = sqrt(pow(valX,2) + pow(valY,2) + pow(valZ,2));
 acc[i] = averageVal;
 delay(400);
  }
  x = mphRead();
  
 if(x > 10){
 // Serial.println("REACHED OVER 20 MPH");
  userSpeed = x;
  Serial.println("Current Speed");
  Serial.println(userSpeed);
 // Serial.println(userSpeed);
  for(int i = 0; i < 5; i++){
    delay(2000);
    breakSpeed = mphRead();
    Serial.println("new speed");
    Serial.println(breakSpeed);
    if(breakSpeed < userSpeed  ){
        count++;
        Serial.print("Count increased");
       // Serial.println(breakSpeed);
      }
    delay(300);
    }
   if(count > 1){
    Serial.println("HARD BREAK");
        if(degreeX < -13){
      Serial.println("RIGHT TURN");
       breakFlag = 3;
      }
     else if(degreeX > 5){
      Serial.println("LEFT TURN");
       breakFlag = 2;
      }
     else{
      Serial.println("STRAIGHT");
       breakFlag = 1;
      }
   
    }else{
    breakFlag = 0;
    Serial.println("SOFT BREAK");
      }
  }


    if (Firebase.setInt(firebaseData, path + "/Int/break" , breakFlag))
    {
      Serial.println("----------Set result-----------");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.println("TYPE: " + firebaseData.dataType());
      Serial.print("VALUE: ");
      if (firebaseData.dataType() == "int")
        Serial.println(firebaseData.intData());
      else if (firebaseData.dataType() == "float")
        Serial.println(firebaseData.floatData());
      else if (firebaseData.dataType() == "boolean")
        Serial.println(firebaseData.boolData() == 1 ? "true" : "false");
      else if (firebaseData.dataType() == "string")
        Serial.println(firebaseData.stringData());
      else if (firebaseData.dataType() == "json")
        Serial.println(firebaseData.jsonData());
      Serial.println("--------------------------------");
      Serial.println();
    }
    else
    {
      Serial.println("----------Can't set data--------");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("--------------------------------");
      Serial.println();
    }
  
  for (uint8_t i = 0; i < 10; i++)
  {
    

     
    if (Firebase.setFloat(firebaseData, path + "/Int/Data" + (i), acc[i]))
    {
      Serial.println("----------Set result-----------");
      Serial.println("PATH: " + firebaseData.dataPath());
      Serial.println("TYPE: " + firebaseData.dataType());
      Serial.print("VALUE: ");
      if (firebaseData.dataType() == "int")
        Serial.println(firebaseData.intData());
      else if (firebaseData.dataType() == "float")
        Serial.println(firebaseData.floatData());
      else if (firebaseData.dataType() == "boolean")
        Serial.println(firebaseData.boolData() == 1 ? "true" : "false");
      else if (firebaseData.dataType() == "string")
        Serial.println(firebaseData.stringData());
      else if (firebaseData.dataType() == "json")
        Serial.println(firebaseData.jsonData());
      Serial.println("--------------------------------");
      Serial.println();
    }
    else
    {
      Serial.println("----------Can't set data--------");
      Serial.println("REASON: " + firebaseData.errorReason());
      Serial.println("--------------------------------");
      Serial.println();
    }
  }

  }


float mphRead(){
    while (Serial1.available()) {
        if (gps.encode(Serial1.read())) {
      
//      if(gps.speed.isValid()){
       // Serial.print(gps.speed.kmph());
        return gps.speed.mph();
//      }
  
   

  }  
  }
  }
