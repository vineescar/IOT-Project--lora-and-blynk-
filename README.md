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








