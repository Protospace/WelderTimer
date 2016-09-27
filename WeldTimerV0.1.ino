// MIG Welder trigger timer
#include <LiquidCrystal.h>

const int GASPURGE_LENGTH = 0;

const int PIN_WELDERSIG = 9; // Pin receiving on/off signal from welder (through optocoupler)
const int TRIGGER_PULLED = LOW; // User is Welding!
const int TRIGGER_OFF = HIGH; // User is not currently welding

const int STATE_OFF = 0;
const int STATE_WELDING = 1;
const int STATE_GASPURGE = 2;

// LCD Pins
const int LCD_PINS_RS = 12;
const int LCD_PINS_E = 11;
const int LCD_PINS_D4 = 5;
const int LCD_PINS_D5 = 4;
const int LCD_PINS_D6 = 3;
const int LCD_PINS_D7 = 2;

// Define Variables
unsigned long millisStart = 0; //millis reading when timer starts. Rollover safe due to unsigned
unsigned long millisEnd = 0; //millis reading when timer starts (note this value may roll over). Rollover safe due to unsigned
unsigned long currentMillis, timer_purgeStart = 0;
long timer_totalWeldTime = 0; //time elapsed in milliseconds
int timer_running = false; // timer is running
int currentState, triggerStatus = 0;

//Setup LCD
LiquidCrystal lcd(LCD_PINS_RS, \
                  LCD_PINS_E, \
                  LCD_PINS_D4, \
                  LCD_PINS_D5, \
                  LCD_PINS_D6, \
                  LCD_PINS_D7);

void setup() {
  Serial.begin(9600);
  
  // put your setup code here, to run once:
  lcd.begin(20, 4);
  pinMode(PIN_WELDERSIG, INPUT);
}

void runTimer()
{
  if (!timer_running)
  {
    millisStart=millis();
    timer_running = true;
    Serial.println("Start!");
  }
  else
  {

  }
}
  
void stopTimer()
{
  if(!timer_running)
  {
    millisEnd=millis();
    timer_running = false; 
    timer_totalWeldTime += millisEnd - millisStart; // calculate amount of time trigger has been pressed this session
    Serial.println("Done Welding");

    Serial.print("Total time: ");
    Serial.println(timer_totalWeldTime);
    // updateDisplay();
  }
}

void purgeGas()
{
  if (currentState = STATE_WELDING)
  {
    currentState = STATE_GASPURGE;
    timer_purgeStart = millis();
    Serial.print("Purging");
  }
  else if (currentState = STATE_GASPURGE)
  {
    if (millis() - timer_purgeStart > GASPURGE_LENGTH){
      stopTimer();  
      currentState = STATE_OFF;
    }
  }
}

void loop() 
{
  triggerStatus = digitalRead(PIN_WELDERSIG);
  // Check if trigger status doesn't match what the timer is doing (aka state has changed):
  if (triggerStatus==TRIGGER_PULLED)
  {
     runTimer();
     currentState = STATE_WELDING;
  }
  else if (triggerStatus==TRIGGER_OFF)
  {
    purgeGas();

  }

}
