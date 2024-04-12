#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

////////////////////////////////////////////////////////////////////////

#define  KEY_UP     1
#define  KEY_DOWN   2
#define  KEY_RIGHT  0
#define  KEY_LEFT   3
#define  KEY_SELECT 4

int adc_key_val[5] = { 50, 150, 300, 450, 700 };
int adc_key_in;
int NUM_KEYS = 5;
int key = -1;
int oldkey = -1;

int read_LCD_buttons(unsigned int input)
{
  int value;
  for (value = 0; value < NUM_KEYS; value++)
  {
    if (input < adc_key_val[value])
    {
      return value;
    }
  }
  if (value >= NUM_KEYS)
  {
    value = -1;
  }
  return value;
}

////////////////////////////////////////////////////////////////////////

byte spaceship[8] = { B00000,
                      B00100,
                      B11110,
                      B01111,
                      B01111,
                      B11110,
                      B00100,
                      B00000,
                    };

byte bullet[8] = { B00000,
                   B00000,
                   B00010,
                   B11111,
                   B00010,
                   B00000,
                   B00000,
                 };

byte block[8] = { B00000,
                  B10001,
                  B01110,
                  B01110,
                  B01110,
                  B10001,
                  B00000,
                };

byte sad[8] = { B00000,
                B11011,
                B11011,
                B00000,
                B00000,
                B01110,
                B10001,
              };

byte ammo[8] = { B00000,
                 B00100,
                 B00100,
                 B11111,
                 B00100,
                 B00100,
                 B00000,
               };

////////////////////////////////////////////////////////////////////////

#define  STATE_GAME       1
#define  STATE_GAME_LOST  2
#define  STATE_MENU       0

int state = STATE_MENU;
int lcd_width = 12;

int interval = 300;
long previousMillis = 0;

int spacePosition_x = 0;
int spacePosition_y = 0;

int enemies_x[100];
int enemies_y[100];

int munition_x[100];
int munition_y[100];

int number_of_enemies = 100;

unsigned int score = 0;
int munition = 5;
boolean fire = false;
int negate = 0;

////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(9600);

  lcd.begin(16, 2);

  lcd.createChar(0, spaceship);
  lcd.createChar(1, bullet);
  lcd.createChar(2, block);
  lcd.createChar(3, sad);
  lcd.createChar(4, ammo);

  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Welcome!");
  delay(1000);
  lcd.setCursor(1, 1);
  lcd.print("Are you ready?");

  randomSeed(analogRead(1));

  int accumulation = lcd_width;

  for (int i = 0; i < number_of_enemies; i++)
  {
    accumulation += random(2, 8);
    enemies_x[i] = accumulation;
    enemies_y[i] = random(0, 2);
  }

  accumulation = lcd_width;

  for (int i = 0; i < number_of_enemies; i++)
  {
    accumulation += random(4, 50);
    munition_x[i] = accumulation;
    munition_y[i] = random(0, 2);
  }

  delay(2000);
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > interval)
  {
    previousMillis = currentMillis;

    if (state == STATE_MENU)
    {
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Press select");
      lcd.setCursor(4, 1);
      lcd.print("to start");
    }
    else if (state == STATE_GAME)
    {
      lcd.clear();
      lcd.setCursor(spacePosition_x, spacePosition_y);
      lcd.write(byte(0));
      lcd.setCursor(14, 0);
      lcd.print(score);
      lcd.setCursor(14, 1);
      lcd.print(munition);
      for (int i = 0; i < number_of_enemies; i++)
      {                 
        enemies_x[i] = enemies_x[i] - 1;
        munition_x[i] = munition_x[i] - 1;

        if (enemies_x[i] >= 0 && enemies_x[i] <= lcd_width)
        {
          lcd.setCursor(spacePosition_x, spacePosition_y);
          lcd.write(byte(0));  
          lcd.setCursor(enemies_x[i], enemies_y[i]);
          lcd.write(2);

          if (fire == true && spacePosition_y == enemies_y[i])
          {
            negate = enemies_x[i];
            for (int i = 1; i <= negate; i++)
            {
              lcd.setCursor(i, spacePosition_y);
              lcd.write(1);
            }

            enemies_x[i] = 0;
            score += 3;

            fire = false;

            if (munition > 0)
            {
              munition--;
            }
          }
        }
        
        if (munition_x[i] >= 0 && munition_x[i] <= lcd_width)
        {
          lcd.setCursor(munition_x[i], munition_y[i]);
          lcd.write(4);
        }

        if ((spacePosition_y == 0 && enemies_y[i] == 0 && enemies_x[i] == 1) || (spacePosition_y == 1 && enemies_y[i] == 1 && enemies_x[i] == 1))
        {
          state = STATE_GAME_LOST;
        }

        if ((enemies_y[i] == 0 || enemies_y[i] == 1) && enemies_x[i] == 1)
        {
          score++;
        }

        if (munition_y[i] == spacePosition_y && munition_x[i] == 0)
        {
          munition++;
          lcd.setCursor(spacePosition_x, spacePosition_y);
          lcd.blink();
          delay(100);
          lcd.noBlink();
          lcd.write(byte(0));

        }


      }
    }
    else if (state == STATE_GAME_LOST)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("GAME OVER!");
      lcd.setCursor(11, 0);
      lcd.write(3);
      lcd.setCursor(13, 0);
      lcd.write(3);
      lcd.setCursor(15, 0);
      lcd.write(3);
      lcd.setCursor(0, 1);
      lcd.print("Your score is:");
      lcd.print(score);
    }
  }

  adc_key_in = analogRead(0);
  key = read_LCD_buttons(adc_key_in);

  if (key != oldkey)
  {
    delay(50);
    adc_key_in = analogRead(0);
    key = read_LCD_buttons(adc_key_in);

    if (key != oldkey)
    {
      oldkey = key;
      if (key >= 0)
      {
        if (key == KEY_DOWN)
        {
          if (state == STATE_GAME)
          {
            spacePosition_y = 1;
          }
        }
        else if (key == KEY_UP)
        {
          if (state == STATE_GAME)
          {
            spacePosition_y = 0;
          }
        }
        else if (key == KEY_SELECT && state != STATE_GAME_LOST)
        {
          state = STATE_GAME;
        }
        else if (key == KEY_RIGHT && munition > 0)
        {
          fire = true;
        }
      }
    }
  }
}
