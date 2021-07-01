/*
 * Rocket Project Prometheus
 * 2020-2021
 * BunkerControls
 */

// pin numbers
#define PIN_SHUTOFF 12
#define PIN_MPV 11
#define PIN_IGNITE 10
#define PIN_DISCONNECT 9
#define PIN_VENT 8
#define PIN_DUMP 7
#define PIN_NITROGEN 6
#define PIN_NITROUS 5

// control signal flags
#define SIG_SHUTOFF     0b000000001 //1
#define SIG_MPV         0b000000010 //2
#define SIG_IGNITE      0b000000100 //4
#define SIG_DISCONNECT  0b000001000 //8
#define SIG_VENT        0b000010000 //16
#define SIG_DUMP        0b000100000 //32
#define SIG_NITROGEN    0b001000000 //64
#define SIG_NITROUS     0b010000000 //128
//#define SIG_OTHER     0b100000000 //256 // THIS INDICATES A TEMPLATE FOR FUTURE SIGNALS IF THEY SHOULD BE ADDED

// configurable parameters
#define BAUD_RATE 57600 // bits per second being transmitted
#define OFFSET_DELAY_TX 25 // specify additional flat "delay" added to minimum calculated delay to avoid corruption
#define SERIAL_TRANSFER_DEBUGGING true
#define SERIAL_TRANSFER_TIMEOUT 50
#define SIG_DECODING true
#define DEBUGGING true

// macros
#define MIN_DELAY_TX (1/(BAUD_RATE/10))*1000*2 // refer to `https://github.com/UCLA-Rocket-Project/prometheus-groundsys-2021/blob/main/docs/safer_serial_transmission_practices.md`
#define DELAY_TX MIN_DELAY_TX + OFFSET_DELAY_TX

// link necessary libraries
#include <SerialTransfer.h>
#include <SoftwareSerial.h>

// init SoftwareSerial connection to pad
SoftwareSerial to_pad_connection(3, 2); // RX, TX

// init SerialTransfer
SerialTransfer transfer_to_pad;

// data packet structure
struct Datapacket
{
  // representation of state of switches
  int sig; // since our number of signals is smaller than 16, this will work fine (2 bytes, 16 bits => at most 16 signals); should this number ever increase, we can use a bigger datatype (like long, 4 bytes)
};

// init buffer for datapacket
Datapacket dp;

void setup()
{
  // specify digital pin modes
  pinMode(PIN_IGNITE, INPUT_PULLUP);
  pinMode(PIN_DISCONNECT, INPUT_PULLUP);
  pinMode(PIN_VENT, INPUT_PULLUP);
  pinMode(PIN_DUMP, INPUT_PULLUP);
  pinMode(PIN_NITROGEN, INPUT_PULLUP);
  pinMode(PIN_NITROUS, INPUT_PULLUP);
  pinMode(PIN_MPV, INPUT_PULLUP);
  pinMode(PIN_SHUTOFF, INPUT_PULLUP);

  // start Serial connections
  to_pad_connection.begin(BAUD_RATE);
  transfer_to_pad.begin(to_pad_connection, SERIAL_TRANSFER_DEBUGGING, Serial, SERIAL_TRANSFER_TIMEOUT);
  transfer_to_pad.reset();

  if (DEBUGGING)
  {
    Serial.begin(9600);
  }
}

void loop()
{
  // reset data buffers
  reset_buffers(dp);

  // update state variable with current state of switches
  update_state(dp, PIN_SHUTOFF, SIG_SHUTOFF);
  update_state(dp, PIN_MPV, SIG_MPV);
  update_state(dp, PIN_IGNITE, SIG_IGNITE);
  update_state(dp, PIN_DISCONNECT, SIG_DISCONNECT);
  update_state(dp, PIN_VENT, SIG_VENT);
  update_state(dp, PIN_DUMP, SIG_DUMP);
  update_state(dp, PIN_NITROGEN, SIG_NITROGEN);
  update_state(dp, PIN_NITROUS, SIG_NITROUS);

  // transmit packet
  transfer_to_pad.sendDatum(dp);

  // output to Serial debugging and signal decoding information
  if (DEBUGGING)
  {
    if (SIG_DECODING)
    {
      Serial.print(dp.sig, BIN);
      Serial.print(", corresponds to: ");
      display_decoded_signals(dp.sig);
    }
    else
      Serial.println(dp.sig, BIN);
  }

  // delay so we don't sample too fast :)
  delay(DELAY_TX);
}

/*
 * reset_buffers()
 * -------------------------
 * Resets all fields of global data buffer to 0.
 */
void reset_buffers(Datapacket& dp)
{
  dp.sig = 0;
}

/*
 * update_state()
 * -------------------------
 * Reads from a specified digital pin and updates an integer-represented
 * state with the specfied state encoding 
 * 
 * Inputs:
 *   - state: pointer to an integer variable representing the state of the switches
 *   - pin: pin to read from
 *   - state_encoding: binary encoding/mask for the state associated with the pin's
 *       connected switch
 */
void update_state(Datapacket& dp, int pin, int state_encoding)
{
  if (digitalRead(pin) == LOW)
    dp.sig |= state_encoding;
}

/*
 * display_decoded_signals()
 * -------------------------
 * Interprets an integer-representation of the state of switches and displays
 * a human-readable decoding of the flipped switches.
 * 
 * Inputs:
 *   - sig: integer variable representing the state of the switches
 */
void display_decoded_signals(int sig)
{
  if (sig & SIG_SHUTOFF)
    Serial.print("SHUTOFF ");
  if (sig & SIG_MPV)
    Serial.print("MPV ");
  if (sig & SIG_IGNITE)
    Serial.print("IGNITE ");
  if (sig & SIG_DISCONNECT)
    Serial.print("DISCONNECT ");
  if (sig & SIG_VENT)
    Serial.print("VENT ");
  if (sig & SIG_DUMP)
    Serial.print("DUMP ");
  if (sig & SIG_NITROGEN)
    Serial.print("NITROGEN ");
  if (sig & SIG_NITROUS)
    Serial.print("NITROUS ");

  //...add new signals here...

  Serial.println();
}
