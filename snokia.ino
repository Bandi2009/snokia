#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#define FINAL

#ifdef FINAL
const int CLK = 18;
#else
const int CLK = 22;
#endif
const int DIN = 21;
const int DC = 17;
const int CE = 16;
#ifdef FINAL
const  int RST = 22;
#else
const  int RST = 18;
#endif

const int joystick_y = 36;
const int joystick_x = 26;
const int joystick_s = 19;
const int JOYSTICK_LOW = 30;
const int JOYSTICK_HIGH = 4000;

// 84 x 48 matrix display
Adafruit_PCD8544 display = Adafruit_PCD8544(CLK,DIN,DC,CE,RST);

const int MATRIX_WIDTH = 14;
const int MATRIX_HEIGTH = 8;

struct Point
{
  int x;
  int y;
};

const int MAXKIGXOHOSSZ = 64;
const unsigned MINWAIT = 1;
const int TARGET_KIGYO_MIN_TAV = 2;
struct Kigyo
{
  int hossz = 3;
  const unsigned LASSUSAG = 500;
  char iranyitas = 's';
  Point test[MAXKIGXOHOSSZ];
  int fej = 0;
  int farok = 0;
  int akt_hossz = 0;
};

Kigyo kigyo;

class LedEmulator
{
  static const int dotWidth = 6;

  public:
    void setLed(int dummy, unsigned int x, unsigned int y, bool on)
    {
       display.fillRect(x * dotWidth, y * dotWidth, dotWidth, dotWidth, on ? BLACK : WHITE);
    }

};

LedEmulator lc;

void setup()
{
  Serial.begin(115200);
  display.begin();
  display.setContrast(50);
  display.clearDisplay();
  display.display();
  pinMode(joystick_s, INPUT_PULLUP);

  kigyo.fej = 0;
  kigyo.farok = 0;
  kigyo.akt_hossz = 1;
  kigyo.test[kigyo.fej].x = 4;
  kigyo.test[kigyo.fej].y = 0;
  lc.setLed(0, kigyo.test[kigyo.fej].x, kigyo.test[kigyo.fej].y, true);
  display.display();
  delay(kigyo.LASSUSAG);

}

int score = 0;

bool target_displayed = false;
int target_x, target_y;

void AddNewTarget()
{
  if (target_displayed)
    return;

  bool found = false;
  while(!found)
  {
    target_x = random(0, MATRIX_WIDTH -1);
    target_y = random(0, MATRIX_HEIGTH -1);
    target_displayed = true;

    found = true;
    int p = kigyo.farok;
    while (true)
    {
      if (abs(target_x - kigyo.test[p].x) < TARGET_KIGYO_MIN_TAV && abs(target_y - kigyo.test[p].y) < TARGET_KIGYO_MIN_TAV)
      {
        found = false;
        break;
      }

      if (p == kigyo.fej)
        break;

      if (++p >= MAXKIGXOHOSSZ)
        p = 0;
    }
  }

  lc.setLed(0, target_x, target_y, true);
}

void loop()
{
  AddNewTarget();

//  display.printf("x:%d y:%d %d",analogRead(joystick_x),analogRead(joystick_y), digitalRead(joystick_s));
  int joystick_x_state = analogRead(joystick_x);
  int joystick_y_state = analogRead(joystick_y);

  if (joystick_x_state > JOYSTICK_HIGH && kigyo.iranyitas != 'a')
  {
    kigyo.iranyitas = 'd';
  }
  else if (joystick_x_state < JOYSTICK_LOW && kigyo.iranyitas != 'd')
  {
    kigyo.iranyitas = 'a';
  }
  else if (joystick_y_state > JOYSTICK_HIGH && kigyo.iranyitas != 's')
  {
    kigyo.iranyitas = 'w';
  }
  else if (joystick_y_state < JOYSTICK_LOW && kigyo.iranyitas != 'w')
  {
    kigyo.iranyitas = 's';
  }

  static unsigned long lastRun = 0;
  if (lastRun != 0 && millis() - lastRun < kigyo.LASSUSAG)
  {
    delay(MINWAIT);
    return;
  }
  lastRun = millis();

  if (kigyo.akt_hossz == kigyo.hossz)
  {
    lc.setLed(0, kigyo.test[kigyo.farok].x, kigyo.test[kigyo.farok].y, false);
    if (++kigyo.farok >= MAXKIGXOHOSSZ)
      kigyo.farok = 0;
  }
  else
    kigyo.akt_hossz++;

  Point ujfej = kigyo.test[kigyo.fej];
  switch (kigyo.iranyitas)
  {
  case 's':
    ujfej.y++;
    if (ujfej.y > MATRIX_HEIGTH - 1)
      ujfej.y = 0;
    break;
  case 'a':
    ujfej.x--;
    if (ujfej.x < 0)
      ujfej.x = MATRIX_WIDTH - 1;
    break;

  case 'w':
    ujfej.y--;
    if (ujfej.y < 0)
      ujfej.y = MATRIX_HEIGTH - 1;
    break;

  case 'd':
    ujfej.x++;
    if (ujfej.x > MATRIX_WIDTH - 1)
      ujfej.x = 0;
    break;

  default:
    break;
  }

  if (++kigyo.fej >= MAXKIGXOHOSSZ)
    kigyo.fej = 0;
  
  kigyo.test[kigyo.fej] = ujfej;

  lc.setLed(0, kigyo.test[kigyo.fej].x, kigyo.test[kigyo.fej].y, true);

  if (kigyo.test[kigyo.fej].x == target_x && kigyo.test[kigyo.fej].y == target_y)
  {
    score++;
    target_displayed = false;
    kigyo.hossz++;
  }
  else
  {
      int p = kigyo.farok;
      while (p != kigyo.fej)
      {
        if ( kigyo.test[p].x == kigyo.test[kigyo.fej].x && kigyo.test[p].y == kigyo.test[kigyo.fej].y)
        {
          display.clearDisplay();
          display.setCursor(0,0);
          display.printf("Hits:%d\n\n Press reset\n button to\n restart!", score);
          display.display();
          delay(4000000);
        }

        if (++p >= MAXKIGXOHOSSZ)
          p = 0;
      }
  }

  display.display();

  delay(MINWAIT);
}