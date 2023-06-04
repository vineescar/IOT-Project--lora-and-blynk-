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

