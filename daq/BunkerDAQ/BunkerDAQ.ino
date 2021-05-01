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

// init `float` variable used as buffer for PT1 and PT2 data
float data1 = -1;
float data2 = -1;

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
  data1 = -1;
  data2 = -1;

  // only attempt to read data from connection if data is available and the first byte indicates the start of a packet we expect ('s')
  if (from_pad_connection.available()) // && from_pad_connection.read() == 's'
  {
    // read and parse float value transmitted (representing PT data)
    data1 = from_pad_connection.parseFloat();
    data2 = from_pad_connection.parseFloat();

    // display PT data value to Serial monitor
    Serial.print(data1);
    Serial.print(",");
    Serial.println(data2);
  }

  // delay because we don't expect data transmitting that fast
  delay(10);
}
