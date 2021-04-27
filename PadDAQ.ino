/*
 * Rocket Project Prometheus
 * 2020-2021
 * PadDAQ
 */

// pin numbers
#define PIN_PT A0

// configurable parameters
#define DEBUGGING false

// global constants
const int PT_OFFSET = 0.996;
const int PT_SCALE = 0.00408;

// link necessary libraries
#include <SoftwareSerial.h>

// init SoftwareSerial connection to bunker
SoftwareSerial to_bunker_connection(4, 8); // RX, TX

// init `float` variable used as buffer for PT data
float pt_data = -1;
  
void setup()
{
  // start Serial connections
  if (DEBUGGING)
  {
      Serial.begin(9600);
  }

  to_bunker_connection.begin(57600);
}

void loop() {

  // clear data buffers
  pt_data = -1;

  // get and convert current PT readings
  pt_data = get_psi_from_raw_pt_data(analogRead(PIN_PT));

  // send metadata portion of datapacket
  //to_bunker_connection.write('s'); // indicate start of transmission
  //to_bunker_connection.write(sizeof(pt_data)); // send single byte representing size of data transmitted

  // send data portion of datapacket
  to_bunker_connection.println(pt_data); // send single float of PT1 data

  // output to Serial debugging information
  if (DEBUGGING)
  {
    Serial.println(pt_data);
  }

  // delay so we don't sample too fast :)
  delay(10);
}

/*
 * get_psi_from_raw_pt_data()
 * -------------------------
 * Converts raw voltage reading from PT to human-readable PSI value
 * for pressure. Utilizes pre-set calibration factors (scale and offset).
 * 
 * Inputs:
 *   - raw_data: integer representing raw voltage reading from PT
 * 
 * Returns: float representing pressure (in PSI) corresponding to PT reading
 */
float get_psi_from_raw_pt_data(int raw_data)
{
  // convert to voltage
  float voltage = raw_data * (5.0/1023.0); // arduino-defined conversion from raw analog value to voltage

  // convert to psi (consider calibration factors)
  float psi = (voltage - PT_OFFSET) / PT_SCALE; 

  // return value
  return psi;
}
