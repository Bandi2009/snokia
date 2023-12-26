#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

const int CLK = 22;
const int DIN = 21;
const int DC = 17;
const int CE = 16;
const  int RST = 18;

const int joystick_y = 36;
const int joystick_x = 26;
const int joystick_s = 19;

Adafruit_PCD8544 display = Adafruit_PCD8544(CLK,DIN,DC,CE,RST);

void setup()
{
  Serial.begin(115200);
  display.begin();
  display.setContrast(50);
  display.clearDisplay();
  display.fillRect(10,10,6,6, BLACK);
  display.display();
  pinMode(joystick_s, INPUT_PULLUP);
}

void loop()
{
  display.clearDisplay();
  display.printf("x:%d y:%d %d",analogRead(joystick_x),analogRead(joystick_y), digitalRead(joystick_s));
  display.display();
  delay(500);
}