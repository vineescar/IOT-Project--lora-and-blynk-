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




