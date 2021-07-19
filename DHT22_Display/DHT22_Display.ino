//
//  FILE : NodeMCU_SSD1306
//  AUTHOR : nobug (nobug007@gmail.com)
//  CREATED : 23.6.2021
//  HW : NodeMCU & SSD1306
//

#include <SoftwareSerial.h>
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`

// Initialize the OLED display using Wire library
SSD1306Wire  display(0x3c, D3, D4);  //D3=SDA  D4=SCL  As per labeling on NodeMCU


//=======================================================================
//                    Power on setup
//=======================================================================


void setup() {
  delay(1000);
  Serial.begin(9600);
  display_init();
}


//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {

  float i;

  for(i=25.0;i<40.0;i+=0.5) {
    drawStatus(i,i+30.5);
    delay(1000);
  }
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

//=========================================================================
//
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
