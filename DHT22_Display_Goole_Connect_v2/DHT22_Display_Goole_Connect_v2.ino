//
//  FILE : DHT22_Display_Google_Connect
//  AUTHOR : nobug (nobug007@gmail.com)
//  CREATED : 15.7.2021
//  HW : NodeMCU & Google ID & SSD1306 & DHT22
//


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

// Initialize the OLED display using Wire library
SSD1306Wire  display(0x3c, D3, D4);  //D3=SDA  D4=SCL  As per labeling on NodeMCU

DHT dht(DHTPIN, DHTTYPE);


////  WiFI connection

WiFiClientSecure client;
char ssid[20] = "";
char password[20] = "";

String DevName= "거실";
String Loc = "서울";
String Owner = "방효식";
String Info = "테스트";
String Mac = "";
String GAS_ID = "AKfycbzm2u6Smvu4WpU-DRM4zPJ586Wa6P8hgzr6ptAJiC3X0OIJG9YbEn09Ld2OlEv1V3I5cg";  // Replace by your GAS service id

//=======================================================================
//                    Power on setup
//=======================================================================

void setup() {
  Serial.begin(115200);
  WiFi_Connect();
  display_init();
}

//=======================================================================
//                    Display SSD1306 init
//=======================================================================

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

   ESP8266WiFiClass Wifi8266;
   Mac = Wifi8266.macAddress();


  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.drawRect(0, 0, 128, 64);
  display.drawString(20,6, "Connecting...");
  display.display();
    
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  int i = 0;
  String icon_c[4] = { "/","-","\\","|"};
  while (WiFi.status() != WL_CONNECTED) {
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
  drawStatus(t,h);
  sendData2Server(t, h);
  delay(1000*60*10);
}

//=========================================================================
//   Display Temp & Humi
//=========================================================================

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

//=========================================================================
//   Send Data to Google
//=========================================================================


void sendData2Server(float x, float y)
{
  const char* host = "script.google.com";
  const int httpsPort = 443;

  Serial.print("connecting to ");
  Serial.println(host); 
  client.setInsecure();
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
 
  char string_x[10];
  char string_y[10];

  sprintf(string_x,"%0.2f",x);
  sprintf(string_y,"%0.2f",y);
 
  String url = "/macros/s/" + GAS_ID + "/exec?Name="+urlencode(DevName)+"&Temp=" + string_x + "&Humi=" + string_y;
  url = url + "&Loc="+ urlencode(Loc) + "&Owner=" + urlencode(Owner) + "&Info=" + urlencode(Info)+ "&Mac=" + Mac;
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
      break;
    }
  }
  String line = client.readStringUntil('\n');
  Serial.println(line);
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
