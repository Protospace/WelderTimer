# Welder Timer
Version 2.2

Arduino based MIG Welder timer for Hobart Handler 175
Connected to Hobart RC4 and RC14 connectors (+ resistor) via an octocoulpler (part number to be added). Octocoupler transitor controls a digital pin (set in firmware).

This code allows for stopwatch style timing of trigger presses. If the arduino is powered via wallward connected to 220V input to the welder (line in), the timer will display but not currently record trigger time and estimated cost at a set rate.

Record cumulative welding time, like a usage "Odometer" (saved to EEPROM) displayed at bottom of LCD.

Arduino has been replaced with Atmega328P and display is Reprapdiscount Smart LCD. 

To reset counter unplug and replug welder. 

NEW FEATURE:
- Displays info from previous session for 5 seconds at startup in case you unplug the welder and realized you didn't check the time.

Wishlist for features include:
- tracking of wire feed rate, and type of wire.
- Option to reset from physical controls
- log of previous sessions (unlikely to get SD card and RTC module connected to Atmega328P, but suggestions would be accepted)
    -option to scroll through previous sessions with date and time info if RTC can be added.

For now, it's a timer, one that can be reset by accident if someone wants to use another tool. It isn't perfect but it is better than we have.Lets try it out and see how it goes.
