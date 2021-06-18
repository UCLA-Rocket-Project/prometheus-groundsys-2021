/*
 * Rocket Project Prometheus
 * 2020-2021
 * BunkerDAQ
 */

// validity flag encodings
#define F_PT0 0b00000001 //1

//#define F_OTHER 0b00001000 //8

// configurable parameters
#define DEBUGGING true
#define BAUD_RATE 57600 // bits per second being transmitted
#define OFFSET_DELAY_RX 0 // specify additional flat "delay" added to minimum calculated (for transmitting)

// macros
#define MIN_DELAY_TX (1/(BAUD_RATE/10))*1000*2 // refer to `https://github.com/UCLA-Rocket-Project/prometheus-groundsys-2021/blob/main/docs/safer_serial_transmission_practices.md`
#define DELAY_RX MIN_DELAY_TX + OFFSET_DELAY_RX // minimum delay needed by TX end, but this generally is a good rule-of-thumb for receiving end as well :)

// link necessary libraries
#include <SerialTransfer.h>
#include <SoftwareSerial.h>

// init Software Serial connection from pad
SoftwareSerial from_pad_connection(3, 2); // RX, TX

// init SerialTransfer
SerialTransfer transfer_from_pad;

// data packet structure
struct Datapacket
{
  // data portion
  unsigned long timestamp;
  float pt0_data;

  // data checksum
  float checksum;
};

// init buffer for datapacket
Datapacket dp;

void setup()
{
  // start Serial connections
  from_pad_connection.begin(BAUD_RATE);
  transfer_from_pad.begin(from_pad_connection);
  transfer_from_pad.reset();

  if (DEBUGGING)
  {
    Serial.begin(9600);
  }
}

void loop()
{
  // reset data buffers
  reset_buffers(dp);

  // only attempt to read data from connection if data is available
  if (transfer_from_pad.available())
  {
    // receive datapacket
    transfer_from_pad.rxObj(dp);

    // format and display data to Serial monitor
    display_dp(dp);
  }

  // delay because we don't expect data transmitting that fast
  delay(DELAY_RX);
}

/*
 * reset_buffers()
 * -------------------------
 * Resets all fields of global data buffer to 0.
 */
void reset_buffers(Datapacket& dp)
{
  dp.timestamp = 0;
  dp.pt0_data = 0;
  dp.checksum = 0;
}

/*
 * display_dp()
 * -------------------------
 * Formats and displays received data in comma-separated form to Serial.
 */
void display_dp(const Datapacket& dp)
{
  Serial.print(dp.timestamp);
  Serial.print(',');
  Serial.print(dp.pt0_data);
  Serial.print(',');
  Serial.println("-42069");
}
