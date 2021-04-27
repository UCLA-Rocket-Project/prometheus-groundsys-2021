/*
 * Rocket Project Prometheus
 * 2020-2021
 * BunkerDAQ
 */

// configurable parameters
#define DEBUGGING true

// link necessary libraries
#include <SoftwareSerial.h>

// init Software Serial connection from pad
SoftwareSerial from_pad_connection(3, 2); // RX, TX

// init `float` variable used as buffer for PT1 data
float data = -1;

void setup()
{
  // start Serial connections
  if (DEBUGGING)
  {
    Serial.begin(9600);    
  }

  from_pad_connection.begin(57600);
}

void loop()
{
  // clear data buffers
  data = -1;

  // only attempt to read data from connection if data is available and the first byte indicates the start of a packet we expect ('s')
  if (from_pad_connection.available()) // && from_pad_connection.read() == 's'
  {
    // read and parse float value transmitted (representing PT data)
    data = from_pad_connection.parseFloat();

    // display PT data value to Serial monitor
    Serial.println(data);
  }

  // delay because we don't expect data transmitting that fast
  delay(10);
}
