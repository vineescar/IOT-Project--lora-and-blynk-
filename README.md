# IOT-Project--lora-and-blynk-
IOT based water pollution monitoring system

OBJECTIVE
Building an unmanned boat with manual navigation and sensors to 
measure the parameters to determine the quality of water and 
logging it for real-time data acquisition regarding the water system.

METHODOLOGY
![methodology](https://github.com/vineescar/IOT-Project--lora-and-blynk-/assets/135414134/f3de3958-e7c7-4cd4-8fce-acea37ee387c)
 
 How it is working
 
 
https://github.com/vineescar/IOT-Project--lora-and-blynk-/assets/135414134/e404e8ed-80e6-45b9-bfce-b600c285bc34



In this project I used Arduino uno and ESP32 modules.
Arduino uno is with the boat which is responsible for collecting the sensor data and communicate with esp32 through the lora module.
ESP32 is responsible to connect with the blynk application to control the motor.
Here I used lora SX1278 which one module is connected in esp32 and another one in Arduino uno .
So only the esp32 need internet connection. We can control the boat about 10km range from esp32.
I am providing the code which is coded in esp32 and Arduino uno.
At lora module at a time one way communication will happen synchronize the communication is the most difficult part.

I attached a pdf file which contain all the details about the project

main problem in this project is Lora SX1278 module at a time we can communicate on one direction so synchronizing the both side for communiction is more challege one..

Arduino code,

#include <SPI.h>
#include <LoRa.h>

const int ledPin = 7;

#define SS_PIN 10
#define RST_PIN 9
#define DI0_PIN 2

//sensor start
#include <Wire.h> 

#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 8
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#define TURBIDITY_PIN A0//turbidity sensor pin
#define SENSOR_PIN A1  //conductivity sensor
#define ONE_WIRE_BUS 8 //  temperature sensor

float calibration_value = 21.34 - 0.7;  //ph
int phval = A5; 
unsigned long int avgval; 
int buffer_arr[10],tempk;


#define trigPin A3
#define echoPin A4
long duration, distance; // Variables for saving the values from ultrasonic sensor
//sensor end

const int temp = 8;  //temperature sensor

// Motor A connections
const int in3 = 3;
const int in4 = 4;

const int enA = 7;
const int in1 = 6;
const int in2 = 5;

int counter = 0;
int collectU=0;
int moveM=0;

void setup() {

  Serial.begin(9600);
  sensors.begin(); //for temperature sensor
  while (!Serial);

  pinMode(ledPin, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Set the motor control pins as outputs
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);


  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(temp, INPUT);

  pinMode(phval, INPUT);


  Serial.println("LoRa Duplex with Arduino Uno");

  LoRa.setPins(SS_PIN, RST_PIN, DI0_PIN);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);
  }
  Serial.println("LoRa Duplex init success");
}

void loop() {

digitalWrite(ledPin, HIGH);

if (collectU != 100)
{

  if (moveM == 10) {
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      Serial.println("Direction: Forward");
    } else if(moveM==20) {
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);
      Serial.println("Direction: right");
    } else if(moveM==30) {
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      Serial.println("Direction: left");
    }else if(moveM==40) {
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      Serial.println("Direction: backward");
    }else  {
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);
      Serial.println("Direction: no movement");
    }


  int packetSize = LoRa.parsePacket();
  if (packetSize) {
  Serial.print("Received packet: ");
  collectU = LoRa.read(); 
  moveM = LoRa.read();
  while (LoRa.available()) {
    Serial.print((char)LoRa.read());
  }
  Serial.println();   

  Serial.print("collectu no is :");
  Serial.println(collectU);

  Serial.print("motor control no is ");
  Serial.println(moveM); 
  }

} 

else if(collectU==100)
{
    digitalWrite(ledPin, LOW);
    
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    Serial.println("Direction: no movement");

    delay(2000);

    for (int i = 0; i < 10; i++){
        int sensorValue1 = analogRead(SENSOR_PIN);       //conductivity start
        float tdsValue = map(sensorValue1, 0, 1023, 0, 1000);  // Replace with your conversion function
        Serial.print("Conductivity: ");
        Serial.print(tdsValue);
        Serial.println(" ppm ");                        //conductivity end

        int turbidityValue = analogRead(TURBIDITY_PIN);          //Turbidity start
        float turbidity = map(turbidityValue, 600, 0, 0, 100); 
        Serial.print("Turbidity: ");
        Serial.print(turbidity);
        Serial.println("%");                                     //turbidity end

        for(int i=0;i<10;i++)                                  //ph start
        { 
        buffer_arr[i]=analogRead(A0);
        delay(30);
        }
        for(int i=0;i<9;i++)
        {
        for(int j=i+1;j<10;j++)
        {
        if(buffer_arr[i]>buffer_arr[j])
        {
        tempk=buffer_arr[i];
        buffer_arr[i]=buffer_arr[j];
        buffer_arr[j]=tempk;
        }
        }
        }
        avgval=0;
        for(int i=2;i<8;i++)
        avgval+=buffer_arr[i];
        float volt=(float)avgval*5.0/1024/6; 
        float ph_act = -5.70 * volt + calibration_value;
        Serial.print("pH Val: ");
        Serial.println(ph_act);                                //ph end

        Serial.print("Requesting temperatures...");              //temperatutre start
        sensors.requestTemperatures(); // Send the command to get temperatures
        Serial.println("DONE");
        float tempC = sensors.getTempCByIndex(0);
        if(tempC != DEVICE_DISCONNECTED_C) 
        {
          Serial.print("Temperature for the device is: ");
          Serial.println(tempC);
        } 
        else
        {
          Serial.println("Error: Could not read temperature data");
        }                                                              //temperature end

        digitalWrite(trigPin, LOW);           //ultrasonic sensor
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);     // send waves for 10 us
        delayMicroseconds(10);
        duration = pulseIn(echoPin, HIGH); // receive reflected waves
        distance = duration / 58.2;        // convert to distance
        Serial.print("distance: ");
        Serial.print(distance);
        Serial.println("cm");    

        Serial.print("Sending packet: ");
        Serial.println(counter);

        LoRa.beginPacket();
        LoRa.write(tdsValue);
        LoRa.write(turbidity);
        LoRa.write(ph_act);
        LoRa.write(tempC);
        LoRa.write(distance);
        LoRa.print("Hello from Arduino Uno ,");
        LoRa.print(counter);
        LoRa.endPacket();

        delay(2000);


}
collectU = 0 ; 
}
}


Esp32 side,

#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6cWTFiXrJ"
#define BLYNK_TEMPLATE_NAME "LoRaBoat"
#define BLYNK_AUTH_TOKEN "QTQedVDCVt8xu5L6ilrX7Jj4b4F26Mg8"


#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <WiFiClient.h>

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "Vinu 4G";
char pass[] = "AQYD3LYE3RY";


#include <SPI.h>
#include <LoRa.h>

#define SCK 18
#define MISO 23
#define MOSI 19
#define SS 5
#define RST 14
#define DIO0 2

int counter = 0;
int sender=0;
int moveF , moveB , moveL , moveR,collect ; // Variables for movement
int move;

int tds,turbidity,phvalue,temperature,distance;

BLYNK_WRITE(V0)                 // Front Movement
{
  moveF = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("MoveF value is: ");
  Serial.println(moveF);
}

BLYNK_WRITE(V1)                 // left Movement
{
  moveL = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("MoveL value is: ");
  Serial.println(moveL);
}

BLYNK_WRITE(V2)                 // right Movement
{
  moveR = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("MoveR value is: ");
  Serial.println(moveR);
}

BLYNK_WRITE(V3)                 // back Movement
{
  moveB = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("MoveB value is: ");
  Serial.println(moveB);
}
BLYNK_WRITE(V4)                 // Front Movement
{
  collect = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("collect value is: ");
  Serial.println(collect);
}


void setup() {
  Serial.begin(9600);
  while (!Serial);

  Blynk.begin(auth, ssid, pass);

  Serial.println("LoRa Duplex with ESP32");

  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);
    Serial.println("LoRa Duplex init success");
  }
}

void loop() {
  if (collect !=100)
  {
  Serial.print("MoveF value is: ");
  Serial.println(moveF);

  Serial.print("MoveL value is: ");
  Serial.println(moveL);

  Serial.print("MoveR value is: ");
  Serial.println(moveR);

  Serial.print("MoveB value is: ");
  Serial.println(moveB);

  Serial.print("collect value is: ");
  Serial.println(collect);

   Blynk.virtualWrite(V11, 300);
   Serial.println("wrighting testing");
  if(moveF>30 & moveR<30 & moveL<30 & moveB<30){
    move = 10;}
  else if(moveF<30 & moveR>30 & moveL<30 & moveB<30){
    move = 20;}
  else if(moveF<30 & moveR<30 & moveL>30 & moveB<30){
    move=30;}
  else if(moveF<30 & moveR<30 & moveL<30 & moveB>30){
    move = 40;}
  
  else{
    move=0;
  }

  Serial.println("Sending packets ");
 // Serial.println(counter);

  LoRa.beginPacket();
  LoRa.write(collect); 
  LoRa.write(move);
  LoRa.print("Hello from esp32 ,");
  LoRa.print(counter);
  LoRa.endPacket();
  delay(2000);
  //counter++;
  }

else if(collect==100)
{    
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Receiving : ");
    tds=LoRa.read();
    turbidity=LoRa.read();
    phvalue=LoRa.read();
    temperature=LoRa.read();
    distance=LoRa.read();
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }
    Serial.println();
   // Serial.println(sender);
    Blynk.virtualWrite(V6, tds);
    Blynk.virtualWrite(V5, phvalue);
    Blynk.virtualWrite(V7, turbidity);
    Blynk.virtualWrite(V8, distance);
    Blynk.virtualWrite(V9, temperature);
    Serial.println("wrighting sensor datas");
    }

}  

}










