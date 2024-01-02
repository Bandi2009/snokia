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

int kigyohossza = 3;
const unsigned KIGYOLASSUSAG = 500;
const unsigned MINWAIT = 1;
static char iranyitas = 's';
Point kigyo[MAXKIGXOHOSSZ];
int kigyo_fej = 0;
int kigyo_farok = 0;
int kigyo_akt_hossz = 0;
const int TARGET_KIGYO_MIN_TAV = 2;

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

  kigyo_fej = 0;
  kigyo_farok = 0;
  kigyo_akt_hossz = 1;
  kigyo[kigyo_fej].x = 4;
  kigyo[kigyo_fej].y = 0;
  lc.setLed(0, kigyo[kigyo_fej].x, kigyo[kigyo_fej].y, true);
  display.display();
  delay(KIGYOLASSUSAG);

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
    int p = kigyo_farok;
    while (true)
    {
      if (abs(target_x - kigyo[p].x) < TARGET_KIGYO_MIN_TAV && abs(target_y - kigyo[p].y) < TARGET_KIGYO_MIN_TAV)
      {
        found = false;
        break;
      }

      if (p == kigyo_fej)
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

  if (joystick_x_state > JOYSTICK_HIGH && iranyitas != 'a')
  {
    iranyitas = 'd';
  }
  else if (joystick_x_state < JOYSTICK_LOW && iranyitas != 'd')
  {
    iranyitas = 'a';
  }
  else if (joystick_y_state > JOYSTICK_HIGH && iranyitas != 's')
  {
    iranyitas = 'w';
  }
  else if (joystick_y_state < JOYSTICK_LOW && iranyitas != 'w')
  {
    iranyitas = 's';
  }

  static unsigned long lastRun = 0;
  if (lastRun != 0 && millis() - lastRun < KIGYOLASSUSAG)
  {
    delay(MINWAIT);
    return;
  }
  lastRun = millis();

  if (kigyo_akt_hossz == kigyohossza)
  {
    lc.setLed(0, kigyo[kigyo_farok].x, kigyo[kigyo_farok].y, false);
    if (++kigyo_farok >= MAXKIGXOHOSSZ)
      kigyo_farok = 0;
  }
  else
    kigyo_akt_hossz++;

  Point ujfej = kigyo[kigyo_fej];
  switch (iranyitas)
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

  if (++kigyo_fej >= MAXKIGXOHOSSZ)
    kigyo_fej = 0;
  
  kigyo[kigyo_fej] = ujfej;

  lc.setLed(0, kigyo[kigyo_fej].x, kigyo[kigyo_fej].y, true);

  if (kigyo[kigyo_fej].x == target_x && kigyo[kigyo_fej].y == target_y)
  {
    score++;
    target_displayed = false;
    kigyohossza++;
  }
  else
  {
      int p = kigyo_farok;
      while (p != kigyo_fej)
      {
        if ( kigyo[p].x == kigyo[kigyo_fej].x && kigyo[p].y == kigyo[kigyo_fej].y)
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