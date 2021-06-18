/*
 * Rocket Project Prometheus
 * 2020-2021
 * PadControls
 */

// pin numbers
#define PIN_SHUTOFF 7
#define PIN_OX 6
#define PIN_IGNITE 8
#define PIN_DISCONNECT 9
#define PIN_VENT 10
#define PIN_DUMP 11
#define PIN_NITROGEN 12
#define PIN_NITROUS 13

// control signal flags
#define SIG_SHUTOFF     0b000000001 //1
#define SIG_OX          0b000000010 //2
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
#define STATE_CHANGE_SIG_RUN_THRESHOLD 10 // indicates the number of consistent signals from bunker needed to change state on pad
#define SIG_DECODING true
#define DEBUGGING true

// link necessary libraries
#include <SoftwareSerial.h>

// init SoftwareSerial connection from bunker
SoftwareSerial from_bunker_connection(3, 2); // RX, TX

// init variables used by main loop
int data_size = 0;
int prev_sig = 0;
int sig = 0;
int sig_run_count = 0;
int cur_state = 0;

void setup()
{
  // specify digital pin modes
  pinMode(PIN_NITROUS, OUTPUT);
  pinMode(PIN_NITROGEN, OUTPUT);
  pinMode(PIN_DUMP, OUTPUT);
  pinMode(PIN_VENT, OUTPUT);
  pinMode(PIN_DISCONNECT, OUTPUT);
  pinMode(PIN_IGNITE, OUTPUT);
  pinMode(PIN_OX, OUTPUT);
  pinMode(PIN_SHUTOFF, OUTPUT);

  // start Serial connections
  if (DEBUGGING)
  {
    Serial.begin(9600);
  }

  from_bunker_connection.begin(57600);
}

void loop()
{
  // only attempt to read data from connection if data is available and the first byte indicates the start of a packet we expect ('s')
  if (from_bunker_connection.read() == 's')
  {
    // second byte of packet represents our data size
    data_size = from_bunker_connection.read();

    prev_sig = sig; // propogate backward the previous signal state
    sig = 0; // setup buffer to hold incoming signal state

    // read all data for this packet
    for (int i = 0; i < data_size; i++)
      sig += from_bunker_connection.read() >> (i << 3); // --> i*8 but ~optimized~

    // handle received signal if attempting to change from current state
    if (sig != cur_state)
    {
      // check if continuing previous run
      if (sig == prev_sig)
      {
        // increment run count, and check if reached threshold of state change
        sig_run_count++;
        
        if (sig_run_count >= STATE_CHANGE_SIG_RUN_THRESHOLD)
        {
          // analyze all data for this packet
          analyze_state(sig, SIG_SHUTOFF,     PIN_SHUTOFF);
          analyze_state(sig, SIG_OX,          PIN_OX);
          analyze_state(sig, SIG_IGNITE,      PIN_IGNITE);
          analyze_state(sig, SIG_DISCONNECT,  PIN_DISCONNECT);
          analyze_state(sig, SIG_VENT,        PIN_VENT);
          analyze_state(sig, SIG_DUMP,        PIN_DUMP);
          analyze_state(sig, SIG_NITROGEN,    PIN_NITROGEN);
          analyze_state(sig, SIG_NITROUS,     PIN_NITROUS);

          // display state change message (with signal decoding if SIG_DECODING set to true)
          // NOTE: done after analyzing/reacting to data to not delay response time if there's an I/O error
          if (DEBUGGING)
          {
            Serial.print("STATE CHANGED: ");
            Serial.print(cur_state, BIN);
            Serial.print(" ---> ");
            Serial.print(sig, BIN);
          }
          
          if (DEBUGGING)
          {
            if (SIG_DECODING)
            {
              Serial.print(", corresponds to: ");
              display_decoded_signals(sig);
            }
            else
              Serial.println();
          }

          // update current state tracker
          cur_state = sig;

          // reset run count for next run of a state-changing signal
          sig_run_count = 1;
        }
      }
      else // if not continuing prevous run...
      {
        // restart run counter, as we've encountered a new signal (different from previous)
        sig_run_count = 1; // start at 1 to account for first of next signal appearing
      }
    }

    // display what we received
    // NOTE: done after analyzing/reacting to data to not delay response time if there's an I/O error
    if (DEBUGGING)
    {
      if (SIG_DECODING)
      {
        Serial.print("RECEIVED: ");
        Serial.print(sig, BIN);
        Serial.print(", corresponds to: ");
        display_decoded_signals(sig);
      }
      else
        Serial.println(sig, BIN);
    }
  }
}

/*
 * analyze_state()
 * -------------------------
 * Interprets a signal to determine if a specified signal/switch has been
 * transmitted/switched. If so, sets the specified digital output pin to
 * HIGH, otherwise to LOW.
 * 
 * Inputs:
 *   - sig_data: signal that represents the state of the switches
 *   - sig_mask: binary mask (or signal encoding) for the state associated
 *       with the specified pin
 *   - output_pin: digital pin to set to HIGH/LOW based on signal's presence
 */
void analyze_state(int sig_data, int sig_mask, int output_pin)
{
  if (sig_data & sig_mask)
    digitalWrite(output_pin, HIGH);
  else
    digitalWrite(output_pin, LOW);
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
  if (sig & SIG_OX)
    Serial.print("OX ");
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
