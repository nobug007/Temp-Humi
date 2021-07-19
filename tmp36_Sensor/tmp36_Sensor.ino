#include <string.h>
#include <stdio.h>

void setup() {
  Serial.begin(9600);
}

void loop() {
  int readValue = analogRead(A0);
  float voltage = readValue * 5.0/1024.0;
  float temp = voltage *100 - 50;
  temp = ( temp - 32 ) * 5 / 9 ;
  Serial.print("Temp: ");
  Serial.println(temp);

  delay(1000);
}
