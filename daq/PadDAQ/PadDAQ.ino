/*
 * Rocket Project Prometheus
 * 2020-2021
 * PadDAQ
 */

// pin numbers
#define PIN_PT0 A0
#define PIN_PT1 A1

// buffer index
#define ID_PT0 0
#define ID_PT1 1
//#define ID_OTHER 2

// validity flag encodings
#define F_PT0 0b00000001 //1
#define F_PT1 0b00000010 //2
//#define F_OTHER 0b00000100 //4

// configurable parameters
#define DEBUGGING false
#define BAUD_RATE 57600 // bits per second being transmitted
#define OFFSET_DELAY_TX 25 // specify additional flat "delay" added to minimum calculated delay to avoid corruption
#define SERIAL_TRANSFER_TIMEOUT 50
#define SERIAL_TRANSFER_DEBUGGING true
#define NUM_OF_PT 2
#define NUM_OF_TC 0
#define NUM_OF_LC 0

/* NOTES:
 *  - TX_DELAY, the lower it is, leads to more often CRC errors, and eventually stale packet issues
 *  - 25 ms seems to be not bad, with the default TIMEOUT value (50)
 *  - TIMEOUT is used to determine a timeout threshold for parsing a datapacket from the Serial connection
 *     - if timeout too short for the length of packet, then will repeatedly cause "ERROR: STALE PACKET"
 *  - errors in datastream of the form of "ERROR: ..."; this CAN be disabled, but we should probably leave as is so we can do easy debugging
 */


// macros
#define MIN_DELAY_TX (1/(BAUD_RATE/10))*1000*2 // refer to `https://github.com/UCLA-Rocket-Project/prometheus-groundsys-2021/blob/main/docs/safer_serial_transmission_practices.md`
#define DELAY_TX MIN_DELAY_TX + OFFSET_DELAY_TX
#define DATA_BUF_SIZE NUM_OF_PT*1 + NUM_OF_TC*0 + NUM_OF_LC*0

// calibration factors
const float PT_OFFSET[NUM_OF_PT] = {-209.38, 11.924};
const float PT_SCALE[NUM_OF_PT] = {244.58, 178.51};

// link necessary libraries
#include <SerialTransfer.h>
#include <SoftwareSerial.h>

// init SoftwareSerial connection to bunker
SoftwareSerial to_bunker_connection(4, 8); // RX, TX

// init SerialTransfer
SerialTransfer transfer_to_bunker;

// data packet structure
struct Datapacket
{
  // metadata
  int valid;

  // data-portion
  unsigned long timestamp;
  float pt0_data;
  float pt1_data;
};

// init buffer for datapacket
Datapacket dp;

void setup()
{
  // start Serial connections
  to_bunker_connection.begin(BAUD_RATE);
  transfer_to_bunker.begin(to_bunker_connection, SERIAL_TRANSFER_DEBUGGING, Serial, SERIAL_TRANSFER_TIMEOUT);
  transfer_to_bunker.reset();

  if (DEBUGGING)
  {
    Serial.begin(9600);
  }
}

void loop()
{
  // reset data buffers
  reset_buffers(dp);

  // poll data, preprocess (if needed), and store in datapacket
  dp.timestamp = millis();
  dp.pt0_data = get_psi_from_raw_pt_data(analogRead(PIN_PT0), 0);
  dp.pt1_data = get_psi_from_raw_pt_data(analogRead(PIN_PT1), 1);

  // properly update valid flags in datapacket's metadata
  update_valid(F_PT0); // PT0
  update_valid(F_PT1); // PT1

  // transmit packet
  transfer_to_bunker.sendDatum(dp);

  // output to Serial debugging information
  if (DEBUGGING)
  {
    Serial.print(dp.valid);
    Serial.print(',');
    Serial.print(dp.timestamp);
    Serial.print(',');
    Serial.print(dp.pt0_data);
    Serial.print(',');
    Serial.println(dp.pt1_data);
  }

  // delay so we don't sample/transmit too fast :)
  delay(DELAY_TX);
}

/*
 * update_valid()
 * -------------------------
 * Update inputed valid signal `valid_sig` with specified encoding
 * `valid_encoding` if `Data` instance at index `i` in buffer `buf`
 * is valid (member == true). Otherwise, do nothing.
 * 
 * Inputs:
 *   - valid_sig: pointer to integer representing valid signal transmitted
 *   - buf: `Data` buffer to lookup into
 *   - i: index of `Data` instance to analyze
 *   - valid_encoding: bitmask encoding for valid signal for particular data entry
 */
void update_valid(int valid_encoding)
{
  // check if data is valid
  dp.valid |= valid_encoding;
}

/*
 * reset_buffers()
 * -------------------------
 * Resets all used global data buffers to 0 (and/or false).
 */
void reset_buffers(Datapacket& dp)
{
  dp.timestamp = 0;
  dp.valid = 0;
  dp.pt0_data = 0;
  dp.pt1_data = 0;
}

/*
 * get_psi_from_raw_pt_data()
 * -------------------------
 * Converts raw voltage reading from PT to human-readable PSI value
 * for pressure. Utilizes pre-set calibration factors (scale and offset).
 * 
 * Inputs:
 *   - raw_data: integer representing raw voltage reading from PT
 *   - pt_num: integer representing index number of PT (0 to NUM_OF_PT)
 * 
 * Returns: `Data` instance, containing float representing pressure (in PSI) corresponding to PT reading
 */
float get_psi_from_raw_pt_data(int raw_data, int pt_num)
{
  // get calibration factors for specified PT
  float offset = PT_OFFSET[pt_num];
  float scale = PT_SCALE[pt_num];

  // convert to voltage
  float voltage = raw_data * (5.0/1023.0); // arduino-defined conversion from raw analog value to voltage

  // convert to psi (consider calibration factors)
  float psi = voltage*scale + offset; 

  // return value
  return voltage;
}
