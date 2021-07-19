//
//  FILE : DHT22_ESP32_Google_Connect
//  AUTHOR : nobug (nobug007@gmail.com)
//  CREATED : 13.7.2021
//  HW : ESP32 & Google ID &  DHT22
//
#include "DHT.h"
#define DHTPIN 15
#define DHTTYPE DHT22

#include <WiFi.h>
#include <HTTPClient.h>

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "nobug_home";
const char* password = "bang8813";
const String devName = "nobug";
const String Loc = "Seoul";
const String Owner = "Bang";

//Your Domain name with URL path or IP address with path

String serverName = "https://script.google.com//macros/s/AKfycbxaMa6KkfUqfQWLctcGq6J_-SuD6ETeTkjzAVkjEVR8F_GXcGPqI1r7TIZW7YzMExf7/exec";

void setup() {
  Serial.begin(115200); 

  WiFi_Connect();
}

void WiFi_Connect() {
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" C");
//  drawStatus(t,h);
   sendData2Server(t, h);
   delay(1000*60*10);
}

void    sendData2Server(float t, float h){

    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      String serverPath = serverName + "?Name=" + devName + "&Temp=" + t + "&Humi=" + h  + "&Loc=" + Loc +"&Owner=" + Owner;

      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();

      if (httpResponseCode>0) {
        Serial.println(serverPath);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
}
