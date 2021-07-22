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
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`

SSD1306Wire  display(0x3c, 2, 4);  

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "nobug_home";
const char* password = "bang8813";
const String devName = "nobug_방";
const String Loc = "서울";
const String Owner = "방효식";

//Your Domain name with URL path or IP address with path

String serverName = "https://script.google.com//macros/s/AKfycbxaMa6KkfUqfQWLctcGq6J_-SuD6ETeTkjzAVkjEVR8F_GXcGPqI1r7TIZW7YzMExf7/exec";

void setup() {
  Serial.begin(115200); 
  WiFi_Connect();
  display_init();
}

void display_init() {
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_24);
  display.drawRect(0, 0, 128, 64);
  display.drawLine(1, 32, 128, 32);     
 
  display.drawString(10,4, "T : ");
  display.drawString(10,36, "H : ");

  display.display();
}

void WiFi_Connect() {
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.drawRect(0, 0, 128, 64);
  display.drawString(20,6, "Connecting...");
  display.display();
  
  int i = 0;
  String icon_c[4] = { "/","-","\\","|"};

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
        display.setColor(WHITE);
        display.drawString(60,36, icon_c[(i%4)]);
        display.display();
        Serial.print(".");
        delay(500);
        display.setColor(BLACK);
        display.drawString(60,36, icon_c[(i%4)]);

        if ( i > 60 ) {
          i = 0;
          display.setColor(WHITE);
          display.drawString(45,36, "Fail !");
          display.display();
          delay(1000*10);
          display.setColor(BLACK);
          display.drawString(45,36, "Fail !");
          break;
        }
        i++;
   }
   display.setColor(WHITE);
   Serial.println("");
   Serial.print("Connected to WiFi network with IP Address: ");
   Serial.println(WiFi.localIP());
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
  drawStatus(t,h);
   sendData2Server(t, h);
   delay(1000*60*10);
}

void drawStatus(float t, float h) {

  char cT[10];
  char cH[10];
  int sH;

  sprintf(cT,"%2.1fºC",t);
  sprintf(cH,"%2.1f",h);

  sH = 4; // sizeof(cH);
  cH[sH+1] = '\0';
  cH[sH] = '%';
 
  display.setColor(WHITE);
  display.drawString(50,4, cT);
  display.drawString(50,34, cH);

  display.display();

  display.setColor(BLACK);
  display.drawString(50,4, cT);
  display.drawString(50,34, cH);
}

void    sendData2Server(float t, float h){

    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      String serverPath = serverName + "?Name=" + urlencode(devName) + "&Temp=" + t + "&Humi=" + h  + "&Loc=" + urlencode(Loc) +"&Owner=" + urlencode(Owner);

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

String urlencode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;  
}
