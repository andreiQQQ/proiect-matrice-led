
#include "LedControl.h" //  need the library
#include <stdio.h>
#include <time.h>
#include <LiquidCrystal.h>
#define L 20

#define waitUserInput 0
#define runGame 1
#define lost 2
#define win 3

LedControl lc=LedControl(12,11,10,1); // 
LiquidCrystal lcd(8, 9, 6, 5, 4, 3);
// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219
int xPin = A1;
int yPin = A0;
int buttonPin = 2;

int xPosition = 0;
int yPosition = 0;
int buttonState = 0;

const char m[L][L] = {
  {1,1,0,0,0,1,0,0,0,0,0,0,1,1,1,1,0,0,1,1},
  {1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {1,0,0,0,1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0},
  {1,1,1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0},
  {0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,1},
  {0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,1},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
  {1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
  {0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1},
  {1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,1,1},
};
short windowLine, windowColoumn, personLine = 6, personColumn = 9, pointLine = 3, pointColoumn = 9, points=0, lifes = 3, lcdCase = 99;
short dl[] = {-1, -1, 0, 1, 1, 1, 0, -1}, dc[] = {0, 1, 1, 1, 0, -1, -1, -1}, enemyTime = 0;
short enemyLine, enemyColoumn;
int time0, seconds;
bool buton;
short go = 0;
long long Time = 0;
void punct() {
  bool ok;
  do {
    pointLine = (Time + rand()) % 20;
    pointColoumn = (Time + rand()) % 20;
    ok = 1;
    if (m[pointLine][pointColoumn] != 0 || (personLine == pointLine && personColumn == pointColoumn)) {
      ok = 0;
    }
    int lv, cv;
    for (int i = 0; i < 7; ++i) {
      lv = pointLine + dl[i];
      cv = pointColoumn + dc[i];
      if (lv > 0 && cv > 0 && lv < L && cv < L && m[lv][cv] == 1) ok = 0;
    }
    if(pointLine > 0 && m[pointLine - 1][pointColoumn] == 1) ok = 0;
    if(pointColoumn > 0 && m[pointLine][pointColoumn - 1] == 1) ok = 0;
    if(pointLine < L - 1 && m[pointLine + 1][pointColoumn] == 1) ok = 0;
    if(pointColoumn < L - 1 && m[pointLine][pointColoumn + 1] == 1) ok = 0;
  } while (!ok);
}
void setup()
{
  lcd.begin(16, 2);
  Time = 0;
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0,false);// turn off power saving, enables display
  lc.setIntensity(0,3);// sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen

  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);

  //activate pupersonLine-up resistor on the push-button pin
  pinMode(buttonPin, INPUT_PULLUP); 
  srand(time(NULL));
  // For versions prior to Arduino 1.0.1
  // pinMode(buttonPin, INPUT);
  // digitalWrite(buttonPin, HIGH);
}
void afisare(short Time) {
  if(Time % 70 == 0) {
    windowLine = personLine - 4;
    windowColoumn = personColumn - 3;
    if (windowLine < 0) {
      windowLine = 0;
    }
    if (windowColoumn < 0) {
      windowColoumn = 0;
    }
    if (windowLine + 7 >= L) {
      windowLine = L - 8;
    }
    if (windowColoumn + 7 >= L) {
      windowColoumn = L - 8;
    }
    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 8; ++j) {
        if((windowLine + i != personLine || windowColoumn + j != personColumn) && (windowLine + i != pointLine || windowColoumn + j != pointColoumn) && (windowLine + i != enemyLine || windowColoumn + j != enemyColoumn)) {
          if (m[windowLine + i][windowColoumn + j] == 1) {
            lc.setLed(0, i, j, true);
          } else {
            lc.setLed(0, i, j, false);
          }
        }
      }
    }
  }
  if (Time % 71 == 0) {
    enemyTime = (enemyTime + 1) % 8;
    enemyLine = pointLine + dl[enemyTime];
    enemyColoumn = pointColoumn + dc[enemyTime];
    if(personLine == enemyLine && personColumn == enemyColoumn) {
      --lifes;
      lcd.setCursor(9, 1);
      lcd.print(lifes);
      if (lifes == 0) {
        go = lost;
      }
    }
    if (windowLine <= enemyLine && enemyLine <= windowLine + 7 && windowColoumn <= enemyColoumn && enemyColoumn <= windowColoumn + 7) {
      lc.setLed(0, enemyLine - windowLine, enemyColoumn - windowColoumn, true);
    }
  }
  if (Time % 70 == 0) {
    if (Time % 280 == 0) {
     lc.setLed(0, personLine - windowLine, personColumn - windowColoumn, false);
    }
    else {
      lc.setLed(0, personLine - windowLine, personColumn - windowColoumn, true);
    }
    if (windowLine <= pointLine && pointLine <= windowLine + 7 && windowColoumn <= pointColoumn && pointColoumn <= windowColoumn + 7) {
      lc.setLed(0, pointLine - windowLine, pointColoumn - windowColoumn, true);
    }
  }
}
void pers(short Time) {
  if(Time % 101) return;
  int xPosition, yPosition;
  xPosition = analogRead(xPin);
  yPosition = analogRead(yPin);
  if (xPosition < 300 && personLine < L - 1 && m[personLine + 1][personColumn] == 0) {
    ++personLine;
  }
  if (yPosition < 300 && personColumn > 0 && m[personLine][personColumn - 1] == 0) {
    --personColumn;
  }
  if (xPosition > 700 && personLine > 0 && m[personLine - 1][personColumn] == 0) {
    --personLine;
  }
  if (yPosition > 700 && personColumn < L - 1 && m[personLine][personColumn + 1] == 0) {
    ++personColumn;
  }
  if(personLine == pointLine && personColumn == pointColoumn) {
    ++points;
    if (points == 10) {
      go = win;
    }
    lcd.setCursor(9, 0);
    lcd.print(points);
    punct();
  }
  /*if(personLine == enemyLine && personColumn == enemyColoumn) {
    --lifes;
    lcd.setCursor(9, 1);
    lcd.print(lifes);
    if (lifes == 0) {
      go = lost;
    }
  }*/
}


void loop()
{
  switch (go) {
    case waitUserInput:
      if (lcdCase != waitUserInput) {
        lcdCase = waitUserInput;
        lcd.setCursor(0, 0);
        lcd.print(" Press to start");
      }
      buton = digitalRead(buttonPin);
      if (buton == 0) {
        go = runGame;
      }
    break;
    case runGame:
      if (lcdCase != runGame) {
        lcdCase = runGame;
        lcd.setCursor(0, 0);
        lcd.print("Points:         ");
        lcd.setCursor(0, 1);
        lcd.print("Lifes:          ");
        lcd.setCursor(9, 0);
        lcd.print(points + " / 10");
        lcd.setCursor(9, 1);
        lcd.print(lifes);
        time0 = millis();
      }
      afisare(Time);
      pers(Time);
      if (Time == 9223372036854775807) {
        go = lost;
      }
    break;
    case lost:
      if (lcdCase != lost) {
        lcdCase = lost;
        lcd.setCursor(0, 0);
        lcd.print("  You looost! ");
        lcd.setCursor(0, 1);
        lcd.print("points:       ");
        lcd.setCursor(9, 1);
        lcd.print(points);
      }
      buton = digitalRead(buttonPin);
      if (buton == 0) {
         go = runGame;
         personLine = 6, personColumn = 9, pointLine = 3, pointColoumn = 9, points=0, lifes = 3, lcdCase = 99;
         Time = 0;
      }
    break;
    case win:
      if (lcdCase != win) {
        lcdCase = win;
        lcd.setCursor(0, 0);
        lcd.print("  You win...    ");

        for (int i = 0; i < 8; ++i) {
          for (int j = 0; j < 8; ++j) {
            lc.setLed(0, i, j, true);
          }
        }
        
        lcd.setCursor(0, 1);
        seconds = (millis() - time0) / 1000;
        lcd.print("seconds:           ");
        lcd.setCursor(10, 1);
        lcd.print(seconds);
      }
      buton = digitalRead(buttonPin);
      if (buton == 0) {
         go = runGame;
         personLine = 6, personColumn = 9, pointLine = 3, pointColoumn = 9, points=0, lifes = 3, lcdCase = 99;
         Time = 0;
      }
    break;
  }
  ++Time;
  delay(1);
}

