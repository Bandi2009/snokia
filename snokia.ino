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
static char iranyitas = 'w';
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
    pi[i] = 'w';
    lc.setLed(0, kigyo[i].x, kigyo[i].y, true);
    display.display();
    delay(KIGYOLASSUSAG);
  }
}

void loop()
{
//  display.printf("x:%d y:%d %d",analogRead(joystick_x),analogRead(joystick_y), digitalRead(joystick_s));

  if (Serial.available())
  {
    if (iranyitas == 'w')
    {
      switch (Serial.read())
      {
      case 'a':
        iranyitas = 'a';
        break;

      case 'd':
        iranyitas = 'd';
        break;

      default:
        break;
      }
    }

    if (iranyitas == 'a')
    {
      switch (Serial.read())
      {
      case 'w':
        iranyitas = 'w';
        break;

      case 's':
        iranyitas = 's';
        break;

      default:
        break;
      }
    }

    if (iranyitas == 's')
    {
      switch (Serial.read())
      {
      case 'a':
        iranyitas = 'a';
        break;

      case 'd':
        iranyitas = 'd';
        break;

      default:
        break;
      }
    }

    if (iranyitas == 'd')
    {
      switch (Serial.read())
      {
      case 'w':
        iranyitas = 'w';
        break;

      case 's':
        iranyitas = 's';
        break;

      default:
        break;
      }
    }
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
    case 'w':
      kigyo[p].y++;
      break;
    case 'a':
      kigyo[p].x--;
      break;

    case 's':
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

  display.display();

  delay(KIGYOLASSUSAG);
}