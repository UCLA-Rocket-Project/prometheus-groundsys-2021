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

// defined global constants
#define NUM_OF_SIGNALS 8
#define SIGNAL_LENGTH ((NUM_OF_SIGNALS % 8) ? NUM_OF_SIGNALS/8 + 1 : NUM_OF_SIGNALS/8)

// configurable parameters
#define SIG_DECODING true
#define DEBUGGING true

// link necessary libraries
#include <SoftwareSerial.h>

// init SoftwareSerial connection to pad
SoftwareSerial to_pad_connection(3, 2); // RX, TX

// init `sig` to represent state of switches
int sig = 0; // since NUM_OF_SIGNALS smaller than 16, this will work fine (2 bytes, 16 bits => at most 16 signals); should this number ever increase, we can use a bigger datatype (like long, 4 bytes)

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
  if (DEBUGGING)
  {
    Serial.begin(9600);
  }

  to_pad_connection.begin(57600);
}

void loop()
{
  // reset state variable
  sig = 0;

  // update state variable with current state of switches
  update_state(&sig, PIN_SHUTOFF, SIG_SHUTOFF);
  update_state(&sig, PIN_MPV, SIG_MPV);
  update_state(&sig, PIN_IGNITE, SIG_IGNITE);
  update_state(&sig, PIN_DISCONNECT, SIG_DISCONNECT);
  update_state(&sig, PIN_VENT, SIG_VENT);
  update_state(&sig, PIN_DUMP, SIG_DUMP);
  update_state(&sig, PIN_NITROGEN, SIG_NITROGEN);
  update_state(&sig, PIN_NITROUS, SIG_NITROUS);

  // send metadata portion of datapacket
  to_pad_connection.write('s'); // indicate start of transmission
  to_pad_connection.write(SIGNAL_LENGTH); // send single byte representing size of data transmitted

  // send data portion of datapacket
  for (int i = 0; i < SIGNAL_LENGTH*8; i+=8)
    to_pad_connection.write(sig >> i); // send byte of signal state data

  // output to Serial debugging and signal decoding information
  if (DEBUGGING)
  {
    if (SIG_DECODING)
    {
      Serial.print(sig, BIN);
      Serial.print(", corresponds to: ");
      display_decoded_signals(sig);
    }
    else
      Serial.println(sig, BIN);
  }

  // delay so we don't sample too fast :)
  delay(10);
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
void update_state(int* state, int pin, int state_encoding)
{
  if (digitalRead(pin) == LOW)
    (*state) |= state_encoding;
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
