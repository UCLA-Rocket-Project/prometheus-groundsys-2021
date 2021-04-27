/*
 * Rocket Project Prometheus
 * 2020-2021
 * Loadcell_PrintRawReadings
 * 
 * Adapted heavily from https://github.com/bogde/HX711
 */

// pin numbers
#define PIN_LOADCELL_DOUT 2
#define PIN_LOADCELL_SCK 3

// global constants
const float LOADCELL_DIVIDER = 1; //example code gave: 5895655
const float LOADCELL_OFFSET = 0; //example code gave: 50682624

// link necessary libraries
#include "HX711.h"

// init loadcell object
HX711 loadcell;

void setup()
{
  // start Serial connection
  Serial.begin(9600);

  // init loadcell object
  loadcell.begin(PIN_LOADCELL_DOUT, PIN_LOADCELL_SCK);

  // reset internal (scale, offset) to (1,0) to read raw outputted values (for the sake of calibration)
  loadcell.set_scale(LOADCELL_DIVIDER);
  loadcell.set_offset(LOADCELL_OFFSET);
}

void loop()
{
  // acquire and print out value read by load cell (if available)
  if (loadcell.wait_ready_timeout(1000))
  {
    long reading = loadcell.get_units();
    Serial.println(reading, 2);
  }
  else
  {
    Serial.println("HX711 not found.");
  }
}