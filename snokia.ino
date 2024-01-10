#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

const int CLK = 18;
const int DIN = 21;
const int DC = 17;
const int CE = 16;
const int RST = 22;

// 84 x 48 matrix display
Adafruit_PCD8544 display = Adafruit_PCD8544(CLK,DIN,DC,CE,RST);

const int MATRIX_WIDTH = 14;
const int MATRIX_HEIGTH = 8;

struct Wasd_joystick
{
  static const int joystick_y = 36;
  static const int joystick_x = 26;
  static const int joystick_s = 19;
  static const int JOYSTICK_LOW = 30;
  static const int JOYSTICK_HIGH = 4000;

  void Setup()
  {
    pinMode(joystick_s, INPUT_PULLUP);
  }

  char AdjUjIranyt()
  {
    int joystick_x_state = analogRead(joystick_x);
    int joystick_y_state = analogRead(joystick_y);

    if (joystick_x_state > JOYSTICK_HIGH)
      return 'd';

    if (joystick_x_state < JOYSTICK_LOW)
      return 'a';

    if (joystick_y_state > JOYSTICK_HIGH)
      return 'w';

    if (joystick_y_state < JOYSTICK_LOW)
      return 's';

    return 0;  
  }

};
Wasd_joystick wasd_iranyitas;

struct Pont
{
  int x;
  int y;
};


const unsigned MINWAIT = 1;
const int TARGET_KIGYO_MIN_TAV = 2;

struct Kigyo
{
  static const int MAX_HOSSZ = 64;
  int hossz = 3;
  const unsigned LASSUSAG = 500;
  char iranyitas = 's';
  Pont test[MAX_HOSSZ];
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
  wasd_iranyitas.Setup();

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

struct Alma
{
  Pont p;
  bool latszik = false;
};

Alma alma;

void AdjUjAlmat()
{
  if (alma.latszik)
    return;

  bool found = false;
  while(!found)
  {
    alma.p.x = random(0, MATRIX_WIDTH -1);
    alma.p.y = random(0, MATRIX_HEIGTH -1);
    alma.latszik = true;

    found = true;
    int p = kigyo.farok;
    while (true)
    {
      if (abs(alma.p.x - kigyo.test[p].x) < TARGET_KIGYO_MIN_TAV && abs(alma.p.y - kigyo.test[p].y) < TARGET_KIGYO_MIN_TAV)
      {
        found = false;
        break;
      }

      if (p == kigyo.fej)
        break;

      if (++p >= Kigyo::MAX_HOSSZ)
        p = 0;
    }
  }

  lc.setLed(0, alma.p.x, alma.p.y, true);
}

void loop()
{
  AdjUjAlmat();

//  display.printf("x:%d y:%d %d",analogRead(joystick_x),analogRead(joystick_y), digitalRead(joystick_s));
  char uj_irany = wasd_iranyitas.AdjUjIranyt();
  
  do
  {
    if (uj_irany == 0)
      break;
    if (uj_irany == 'd' && kigyo.iranyitas == 'a')
      break;
    if (uj_irany == 'a' && kigyo.iranyitas == 'd')
      break;
    if (uj_irany == 'w' && kigyo.iranyitas == 's')
      break;
    if (uj_irany == 's' && kigyo.iranyitas == 'w')
      break;

    kigyo.iranyitas = uj_irany;
  } while(false);

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
    if (++kigyo.farok >= Kigyo::MAX_HOSSZ)
      kigyo.farok = 0;
  }
  else
    kigyo.akt_hossz++;

  Pont ujfej = kigyo.test[kigyo.fej];
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

  if (++kigyo.fej >= Kigyo::MAX_HOSSZ)
    kigyo.fej = 0;
  
  kigyo.test[kigyo.fej] = ujfej;

  lc.setLed(0, kigyo.test[kigyo.fej].x, kigyo.test[kigyo.fej].y, true);

  if (kigyo.test[kigyo.fej].x == alma.p.x && kigyo.test[kigyo.fej].y == alma.p.y)
  {
    score++;
    alma.latszik = false;
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

        if (++p >= Kigyo::MAX_HOSSZ)
          p = 0;
      }
  }

  display.display();

  delay(MINWAIT);
}