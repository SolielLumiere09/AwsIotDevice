#include <ESP8266WiFi.h>
#include <Arduino.h>
#include "AwsIotDevice.hpp"

#define LED_RED D1
#define LED_YELLOW D2
#define LED_WHITE D3

String fileToString(File* ptr);
void msgReceived(char* topic, uint8_t* payload, unsigned int length);


const char* awsEndpoint PROGMEM = "YOUR AWS ENDPOINT";
const char* ssid PROGMEM = "YOUR WIFI SSID";
const char* password PROGMEM = "YOUR WIFI PASSWORD";
uint16_t port = 8883;

AwsIotDevice* client;


void turnOffLeds(){
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_WHITE, HIGH);
}

void turnOnLed(uint8 led){
  digitalWrite(led, LOW);
}

void setup() {

  File cert, key, ca;

  Serial.begin(9600);

  if(!SPIFFS.begin()){
    Serial.println(F("Can't mount fileSystem"));
  }

  cert = SPIFFS.open(F("/certificate.pem.crt"), "r");
  key = SPIFFS.open(F("/private.pem.key"), "r");
  ca = SPIFFS.open(F("/AmazonRootCA1.pem"), "r");
  
  if(!cert || !key || !ca){
    while(true){
      Serial.println(F("Failed to read the certificates"));
      delay(10000);
    }
  }
  
  client = new AwsIotDevice(
    ssid,
    password,
    awsEndpoint,
    port,
    msgReceived,
    fileToString(&cert).c_str(),
    fileToString(&key).c_str(),
    fileToString(&ca).c_str()
  );


  if(client->subscribe("aws/image/doorPermission")){
    Serial.println("Subscribed!");
  }
  else {
    Serial.println("Can't Subscribe");
  }

  cert.close();
  key.close();
  ca.close();

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_WHITE, OUTPUT);
  turnOffLeds();
}

void loop() {
  client->update();

}


String fileToString(File* ptr){

  String buff;

  while(ptr->available()){
    buff += (char)ptr->read();
  }

  return buff;

}
void msgReceived(char* topic, uint8_t* payload, unsigned int length) {
  String msg;
  msg.reserve(length);

  Serial.print(F("Message received on ")); 
  Serial.print(topic);
  Serial.print(": ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    msg += (char)payload[i];
  }
  Serial.println();

  turnOffLeds();
  if(msg.toInt() == 0){
    turnOnLed(LED_RED);
  }
  if(msg.toInt() == 1){
    turnOnLed(LED_YELLOW);
  }
  if(msg.toInt() == 2){
    turnOnLed(LED_WHITE);
  }
  msg.clear();
}