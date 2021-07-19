//
//  FILE : NodeMCU_WiFi_Google_Sheet
//  AUTHOR : nobug (nobug007@gmail.com)
//  CREATED : 4.1.2020
//  HW : NodeMCU & Google ID
//

#define LED_Red D4
#define LED_Green D5
#define LED_Blue D6

#include "DHT.h"
#define DHTPIN D2
#define DHTTYPE DHT22

#include <string.h>
#include <ctype.h>
#include <ESP8266WiFi.h>
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include <SoftwareSerial.h>
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
#include <WiFiClientSecure.h>


DHT dht(DHTPIN, DHTTYPE);


////  WiFI connection

WiFiClientSecure client;



//=======================================================================
//                    Power on setup
//=======================================================================


void setup() {
  delay(1000);
  Serial.begin(9600);
  pinMode(LED_Green, OUTPUT);
  pinMode(LED_Blue, OUTPUT);
  pinMode(LED_Red, OUTPUT);
  digitalWrite(LED_Blue, HIGH);
 // WiFi Setup
  WiFi_Connect();
}

void WiFi_Connect() {
  char ssid[20] = "nobug_home";
  char password[20] = "bang8813";
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
        digitalWrite(LED_Blue, LOW);    // turn the LED off by making the voltage LOW
        delay(500);
        Serial.print(".");
        digitalWrite(LED_Blue, HIGH);    // turn the LED off by making the voltage LOW
        delay(500);
   }
     digitalWrite(LED_Blue, LOW);    // turn the LED off by making the voltage LOW
}

//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" C");
   sendData2Server(t, h);
   delay(1000*60*10);
}
//=========================================================================


void sendData2Server(float x, float y)
{
  const char* host = "script.google.com";
  const int httpsPort = 443;
  char DevName[20]= "nobug";
  String GAS_ID = "AKfycbwssR8XseF7Q274UCu-pjHNwSeBG0eDg3hBDimuFsn0LeNpkUQ1";  // Replace by your GAS service id

  Serial.print("connecting to ");
  Serial.println(host); 
  client.setInsecure();
  if (!client.connect(host, httpsPort)) {
    digitalWrite(LED_Green, LOW);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_Red, HIGH);   // turn the LED on (HIGH is the voltage level)
    Serial.println("connection failed");
    return;
  }
    digitalWrite(LED_Red,LOW);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_Green, HIGH);   // turn the LED on (HIGH is the voltage level)

/*
  if (client.verify(fingerprint, host)) {
  Serial.println("certificate matches");
  } else {
  Serial.println("certificate doesn't match");
  }

  https://script.google.com/macros/s/AKfycbzmgQFpjl7ZJGSUBHP19Q8-u8L_I2gvmqIP-v0dDrMwoQEqaIj7BmagGldFfF9NR3L3RA/exec?Name=nobug&Temp=31&Humi=61
  */

/*  
  String string_x     =  String(x, DEC);
  String string_y     =  String(y, DEC);
*/
  char string_x[10];
  char string_y[10];

  sprintf(string_x,"%0.2f",x);
  sprintf(string_y,"%0.2f",y);
  
  
  String url = "/macros/s/" + GAS_ID + "/exec?Name="+DevName+"&Temp=" + string_x + "&Humi=" + string_y;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.print("==========");
      Serial.print(line);
      Serial.println("==========");
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.println("reply was:");
  Serial.print("==========");
  Serial.print(line);
  Serial.println("==========");
  Serial.println("closing connection");
}
