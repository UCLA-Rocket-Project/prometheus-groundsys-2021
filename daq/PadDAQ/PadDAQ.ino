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

// calibration factors
const float PT_OFFSET[NUM_OF_PT] = {-209.38, 11.924};
const float PT_SCALE[NUM_OF_PT] = {244.58, 178.51};

// configurable parameters
#define DEBUGGING false
#define BAUD_RATE 57600 // bits per second being transmitted
#define OFFSET_DELAY_TX 0 // specify additional flat "delay" added to minimum calculated delay to avoid corruption
#define NUM_OF_PT 2
#define NUM_OF_TC 0
#define NUM_OF_LC 0

// macros
#define MIN_DELAY_TX (1/(BAUD_RATE/10))*1000*2 // refer to `https://github.com/UCLA-Rocket-Project/prometheus-groundsys-2021/blob/main/docs/safer_serial_transmission_practices.md`
#define DELAY_TX MIN_DELAY_TX + OFFSET_DELAY_TX
#define DATA_BUF_SIZE NUM_OF_PT*1 + NUM_OF_TC*0 + NUM_OF_LC*0

// link necessary libraries
#include <SoftwareSerial.h>
#include <CRC32.h>

// init SoftwareSerial connection to bunker
SoftwareSerial to_bunker_connection(4, 8); // RX, TX
 
// Data struct (packages data with valid flag)
struct Data
{
  bool valid;
  float data;
};

// init buffers for data/valid flags
Data buf[DATA_BUF_SIZE];
unsigned long timestamp;
int valid; // since our total number of sensors is smaller than 16, this will work fine (2 bytes, 16 bits => at most 16 signals); should this number ever increase, we can use a bigger datatype (like long, 4 bytes)
CRC32 checksum;

void setup()
{
  // start Serial connections
  if (DEBUGGING)
  {
    Serial.begin(9600);
  }

  to_bunker_connection.begin(BAUD_RATE);
}

void loop()
{
  // reset data buffers
  reset_buffers();

  // poll data and preprocess (if needed)
  timestamp = millis();
  data[ID_PT0] = get_psi_from_raw_pt_data(analogRead(PIN_PT0), 0);
  data[ID_PT1] = get_psi_from_raw_pt_data(analogRead(PIN_PT1), 1);

  // properly update all valid flags for packet metadata
  update_valid(&valid, pt_data, 0, F_PT0); // PT0
  update_valid(&valid, pt_data, 1, F_PT1); // PT1

  // compute data checksum
  checksum.update(timestamp);

  for (int i = 0; i < DATA_BUF_SIZE; i++)
  {
    checksum.update(buf[i]);
  }

  // send metadata portion of datapacket
  to_bunker_connection.write('s'); // indicate start of transmission
  to_bunker_connection.write(valid); // send byte of data validity information
  to_bunker_connection.write(DATA_BUF_SIZE + 1); // send single byte representing number of floats (data) transmitted (+ 1 for timestamp)

  // send data portion of datapacket
  to_bunker_connection.print(timestamp); // send timestamp of data

  for (int i = 0; i < DATA_BUF_SIZE; i++)
  {
    to_bunker_connection.print(',');
    to_bunker_connection.print(buf[i].data); // send single float of data
  }

  // send checksum
  to_bunker_connection.print(',');
  to_bunker_connection.print(checksum.finalize());

  // terminate this datapacket
  to_bunker_connection.println();

  // output to Serial debugging information
  if (DEBUGGING)
  {
    Serial.print(buf[0].data); // send first float of data

    for (int i = 1; i < DATA_BUF_SIZE; i++)
    {
      Serial.print(',');
      Serial.print(buf[i].data); // send single float of data
    }
  }

  // delay so we don't sample too fast :)
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
void update_valid(int* valid_sig, Data* buf, int i, int valid_encoding)
{
  // check if data is valid
  if (buf[i].valid)
    (*valid_sig) |= valid_encoding;
}

/*
 * reset_buffers()
 * -------------------------
 * Resets all used global data buffers to 0 (and/or false).
 */
void reset_buffers()
{
  timestamp = 0;
  valid = 0;
  checksum.reset();

  // invalidate data buffer (no need to reset data field since we've invalidated it)
  for (int i = 0; i < DATA_BUF_SIZE; i++)
    data[i].valid = false;
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
Data get_psi_from_raw_pt_data(int raw_data, int pt_num)
{
  // assume data is invalid (default)
  Data res;
  res.valid = false;
  res.data = -1;

  // check for proper inputs
  if (pt_num >= NUM_OF_PT || pt_num < 0 || raw_data < 0 || raw_data > 1023)
  {
    // data is already invalidated, so just return
    return res;
  }

  // get calibration factors for specified PT
  float offset = PT_OFFSET[pt_num];
  float scale = PT_SCALE[pt_num];

  // convert to voltage
  float voltage = raw_data * (5.0/1023.0); // arduino-defined conversion from raw analog value to voltage

  // convert to psi (consider calibration factors)
  float psi = voltage*scale + offset; 

  // store data and validate
  res.data = psi;
  res.valid = true;

  // return value
  return res;
}
