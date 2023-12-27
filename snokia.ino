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

struct snake
{
  int x;
  int y;
};
typedef struct snake snake;

const int MAXKIGXOHOSSZ = 64;

int kigyohossza = 3;
const unsigned KIGYOLASSUSAG = 500;
static char iranyitas = 's';
snake kigyo[MAXKIGXOHOSSZ];
static char pi[MAXKIGXOHOSSZ];

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

  for (int i = kigyohossza - 1; i >= 0; i--)
  {
    kigyo[i].x = 4;
    kigyo[i].y = 0 + kigyohossza - 1 - i;
    pi[i] = 's';
    lc.setLed(0, kigyo[i].x, kigyo[i].y, true);
    display.display();
    delay(KIGYOLASSUSAG);
  }
}

int hitnum = 0;

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
    for (size_t p = 0; p < kigyohossza; p++)
    {
      if (abs(target_x - kigyo[p].x) < 2 && abs(target_y - kigyo[p].y) < 2)
        found = false;
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

  lc.setLed(0, kigyo[kigyohossza - 1].x, kigyo[kigyohossza - 1].y, false);

  for (size_t i = kigyohossza - 1; i > 0; i--)
  {
    pi[i] = pi[i-1];
  }

  pi[0] = iranyitas;

  for (size_t p = 0; p < kigyohossza; p++)
  {
    switch (pi[p])
    {
    case 's':
      kigyo[p].y++;
      break;
    case 'a':
      kigyo[p].x--;
      break;

    case 'w':
      kigyo[p].y--;
      break;

    case 'd':
      kigyo[p].x++;
      break;

    default:
      break;
    }

    if (kigyo[p].y > MATRIX_HEIGTH - 1 )
    {
      kigyo[p].y = 0;
    }
    if (kigyo[p].x > MATRIX_WIDTH - 1)
    {
      kigyo[p].x = 0;
    }
    if (kigyo[p].y < 0)
    {
      kigyo[p].y = MATRIX_HEIGTH - 1;
    }
    if (kigyo[p].x < 0)
    {
      kigyo[p].x = MATRIX_WIDTH - 1;
    }
  }
  lc.setLed(0, kigyo[0].x, kigyo[0].y, true);

  if (kigyo[0].x == target_x && kigyo[0].y == target_y)
  {
    hitnum++;
    target_displayed = false;
//    kigyohossza++;
  }
  else
  {
      for (size_t p = 1; p < kigyohossza; p++)
      {
        if ( kigyo[p].x == kigyo[0].x && kigyo[p].y == kigyo[0].y)
        {
          display.clearDisplay();
          display.setCursor(0,0);
          display.printf("Hits:%d\n\n Press reset\n button to\n restart!", hitnum);
          display.display();
          delay(4000000);
        }
      }
  }

  display.display();

  delay(KIGYOLASSUSAG);
}