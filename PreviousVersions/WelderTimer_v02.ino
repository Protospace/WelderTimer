// MIG Welder trigger timer

//https://groups.google.com/forum/#!msg/protospace-discuss/mmDGLw4DFE4/Ga710BHeAwAJ

#include <LiquidCrystal.h>
#include <EEPROM.h>


const float PRICE_PER_MINUTE = 0.4; //Dollars per minute

const int POSTFLOW_DURATION = 0; // How long to keep the timer running after trigger is released, to account for extra gas flow
const int DISPLAY_REFRESH_INTERVAL=-1; //Set to -1 to dissable occassional refresh (might not need this anymore)
const unsigned long SLEEP_DELAY = 30 * 60000; // Incase we ever want to turn off the scree due to inactivity

// Welder and Trigger Status options
const int TRIGGER_PULLED = LOW; // User is Welding!
const int TRIGGER_OFF = HIGH; // User is not currently welding

const int STATE_OFF = 0;
const int STATE_WELDING = 1;
const int STATE_POSTFLOW = 2;

// Pins
const int LCD_PINS_RS = 8; //previous value 12
const int LCD_PINS_E = 7; // previous value 11
const int LCD_PINS_D4 = 5;
const int LCD_PINS_D5 = 6; // previous value 4
const int LCD_PINS_D6 = 3;
const int LCD_PINS_D7 = 2;

const int PIN_WELDERSIGNAL = 9; // Pin receiving on/off signal from welder (through optocoupler)


// Define Variables
unsigned long timerStart = 0; //millis reading when timer starts. Rollover safe due to unsigned
unsigned long currentTime, postflowStart = 0;
unsigned long odometer;
unsigned long displayRefreshLast = 0;

long totalWeldTime = 0; //time elapsed in milliseconds

int previousTriggerStatus, currentTriggerStatus = 0;
int currentWelderState = STATE_OFF;



//Setup LCD
LiquidCrystal lcd(LCD_PINS_RS, \
                  LCD_PINS_E, \
                  LCD_PINS_D4, \
                  LCD_PINS_D5, \
                  LCD_PINS_D6, \
                  LCD_PINS_D7);



// Print time to LCD with "##m ##s" format
void printTime(long timeValue, boolean hours, boolean minutes, boolean seconds){
	float h, m, s;
	unsigned long over;
	over = timeValue;
	
	if (hours){ //show hours
		h = over/360000;
		over = over % 360000;
		lcd.print(int(h)); lcd.print("h ");
	}
	if (minutes){ //show minutes
		m = over/60000;
		over = over % 60000;
		lcd.print(int(m)); lcd.print("m ");
	}
	if (seconds){ //show seconds
		s = over/1000;
		over = over % 1000;
		lcd.print(int(s)); lcd.print("s");
	}
}


// Returns the price for the time provided as timeValue.
float calculatePrice(long timeValue){
	return (timeValue/60000.0)*PRICE_PER_MINUTE;
}

// Update what is seen on the LCD
void updateDisplay(){
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Rate: $");
	lcd.print(PRICE_PER_MINUTE, 2);
	lcd.print("/min");
	
	lcd.setCursor(0,1);
	lcd.print("Session: ");
	printTime(totalWeldTime, false, true, true);
	
	lcd.setCursor(0,2);
	lcd.print("Price: $");
	lcd.print(calculatePrice(totalWeldTime), 2);
	
	// welder state information
	lcd.setCursor(0,3);
	if (currentWelderState==STATE_WELDING){
		lcd.print("----- WELDING -----");
	}
	else if(currentWelderState==STATE_POSTFLOW){
		lcd.print("---- POST FLOW ----");
	}
	else if(currentWelderState==STATE_OFF){
		// I don't think we need to display anything about the state the welder is in when it's not doing anything
    lcd.print("(Odo: ");
    printTime(odometer+totalWeldTime, true, true, true);
    lcd.print(")");
	}
}

// Simple trigger debounce test. Was getting weird stuff occasionally without this, but it was like 3am at the time and it could have been due to something else. Also the rest of the code has been revised since then
boolean debounceOK(){
	delay(10);
	return currentTriggerStatus == digitalRead(PIN_WELDERSIGNAL);
}



// Things to do when Arduino starts up
void setup() {
  pinMode(PIN_WELDERSIGNAL, INPUT);
  
  lcd.begin(20, 4);
  
  currentTriggerStatus = digitalRead(PIN_WELDERSIGNAL);
  previousTriggerStatus = currentTriggerStatus;

  EEPROM.get(0,odometer);
  
  updateDisplay();
}

void loop() 
{
  currentTriggerStatus = digitalRead(PIN_WELDERSIGNAL); // read the status of the trigger pin
	currentTime = millis(); // record time of last trigger check
  if (currentTriggerStatus != previousTriggerStatus) // If trigger status has changed then...
  	{
		if(debounceOK()) 
		{
			switch(currentTriggerStatus){
				case TRIGGER_PULLED:
					if(currentWelderState==STATE_OFF){ // Avoids clearing the timer if it hasn't finished purging gas
						timerStart=currentTime;
					}
					currentWelderState=STATE_WELDING;
					updateDisplay();
					break;
				case TRIGGER_OFF:
					currentWelderState = STATE_POSTFLOW;
					postflowStart = currentTime;
					updateDisplay();
					break;
			}
			previousTriggerStatus=currentTriggerStatus;
		}
		else{ // Trigger debounce test failed. Put things back to what it was before, and if the trigger has in fact changed status, it will be caught the next time through the loop.
			currentTriggerStatus = previousTriggerStatus;
		}
	}
	else{ //This is what normally occurs, as changes in trigger status are infrequent
		if(currentWelderState==STATE_POSTFLOW && currentTime - postflowStart > POSTFLOW_DURATION){ // Checks to see if gas post flow should end, and if so record time and update price, odometer reading, and update displate
			currentWelderState = STATE_OFF; // post flow over, therefore next stage is welder is doing nothing

			totalWeldTime += currentTime - timerStart; // calculate total weld time for this session

      EEPROM.put(0,odometer+totalWeldTime);
      
			timerStart=currentTime; // reset timer so it can be used for the sleep option if nothing happens for a long time.

			updateDisplay();
	  }
 }
}
