/*
 * Rocket Project Prometheus
 * 2020-2021
 * BunkerDAQ
 */

// buffer index
#define ID_PT0 0
#define ID_PT1 1
//#define ID_OTHER 2

// validity flag encodings
#define F_PT0 0b00000001 //1
#define F_PT1 0b00000010 //2
//#define F_OTHER 0b00000100 //4

// configurable parameters
#define DEBUGGING true
#define BAUD_RATE 57600 // bits per second being transmitted
#define OFFSET_DELAY_RX 0 // specify additional flat "delay" added to minimum calculated (for transmitting)
#define NUM_OF_PT 2
#define NUM_OF_TC 0
#define NUM_OF_LC 0

// macros
#define MIN_DELAY_TX (1/(BAUD_RATE/10))*1000*2 // refer to `https://github.com/UCLA-Rocket-Project/prometheus-groundsys-2021/blob/main/docs/safer_serial_transmission_practices.md`
#define DELAY_RX MIN_DELAY_TX + OFFSET_DELAY_RX // minimum delay needed by TX end, but this generally is a good rule-of-thumb for receiving end as well :)
#define DATA_BUF_SIZE NUM_OF_PT*1 + NUM_OF_TC*0 + NUM_OF_LC*0

// link necessary libraries
#include <SoftwareSerial.h>
#include <CRC32.h>

// init Software Serial connection from pad
SoftwareSerial from_pad_connection(3, 2); // RX, TX

// init buffers for data
float buf[DATA_BUF_SIZE];
unsigned long timestamp;
int valid; // since our total number of sensors is smaller than 16, this will work fine (2 bytes, 16 bits => at most 16 signals); should this number ever increase, we can use a bigger datatype (like long, 4 bytes)
int data_size;
unsigned long received_checksum;
CRC32 checksum;

void setup()
{
  // start Serial connections
  if (DEBUGGING)
  {
    Serial.begin(9600);    
  }

  from_pad_connection.begin(BAUD_RATE);
}

void loop()
{
  // reset data buffers
  reset_buffers();

  // only attempt to read data from connection if data is available and the first byte indicates the start of a packet we expect ('s')
  if (from_pad_connection.read() == 's')
  {
    // read metadata information (refer to `https://github.com/UCLA-Rocket-Project/prometheus-groundsys-2021/blob/main/docs/datapacket_structure.md`)
    valid = from_pad_connection.read(); // safe, as we expect only 1 byte transmitted for valid field
    data_size = from_pad_connection.read(); // safe, as we expect only 1 byte transmitted for size field

    // read timestamp (and update checksum)
    timestamp = (unsigned long) from_pad_connection.parseInt();
    checksum.update(timestamp);

    // read/parse rest of data (floats), and update checksum
    for (int i = 0; i < data_size-1; i++) // -1 from timestamp already processed
    {
      buf[i] = from_pad_connection.parseFloat();
      checksum.update(buf[i]);
    }

    // read datapackets' sent checksum
    received_checksum = (unsigned long) from_pad_connection.parseInt();

    // confirm checksum
    if (received_checksum != checksum.finalize())
    {
      // ...do something...
      Serial.print("CHECKSUM MISMATCH: ")
    }

    // format and display data to Serial monitor
    Serial.print(valid, BIN);
    Serial.print(",");
    Serial.print(timestamp);
    
    for (int i = 0; i < data_size-1; i++)
    {
      Serial.print(",");
      Serial.print(buf[i]);
    }

    Serial.println();
  }

  // delay because we don't expect data transmitting that fast
  delay(DELAY_RX);
}

/*
 * reset_buffers()
 * -------------------------
 * Resets all used global data buffers to 0.
 */
void reset_buffers()
{
  timestamp = 0;
  valid = 0;
  data_size = 0;
  received_checksum = 0;
  checksum.reset();

  // clear data buffer
  for (int i = 0; i < DATA_BUF_SIZE; i++)
    data[i] = 0;
}
