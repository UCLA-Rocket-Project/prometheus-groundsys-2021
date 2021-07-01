/*
 * Rocket Project Prometheus
 * 2020-2021
 * PadControls
 */

// pin numbers
#define PIN_SHUTOFF 7
#define PIN_MPV 6
#define PIN_IGNITE 13 //8
#define PIN_DISCONNECT 12 //9
#define PIN_VENT 11 //10
#define PIN_DUMP 10 //11
#define PIN_NITROGEN 9 //12
#define PIN_NITROUS 8 //13

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
#define STATE_CHANGE_SIG_RUN_THRESHOLD 10 // indicates the number of consistent signals from bunker needed to change state on pad
#define SIG_DECODING true
#define DEBUGGING false
#define BAUD_RATE 57600 // bits per second being transmitted
#define OFFSET_DELAY_RX 10 // specify additional flat "delay" added to minimum calculated (for transmitting)

// macros
#define MIN_DELAY_TX (1/(BAUD_RATE/10))*1000*2 // refer to `https://github.com/UCLA-Rocket-Project/prometheus-groundsys-2021/blob/main/docs/safer_serial_transmission_practices.md`
#define DELAY_RX MIN_DELAY_TX + OFFSET_DELAY_RX // minimum delay needed by TX end, but this generally is a good rule-of-thumb for receiving end as well :)

// link necessary libraries
#include <SerialTransfer.h>
#include <SoftwareSerial.h>

// init SoftwareSerial connection from bunker
SoftwareSerial from_bunker_connection(3, 2); // RX, TX

// init SerialTransfer
SerialTransfer transfer_from_bunker;

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
  pinMode(PIN_NITROUS, OUTPUT);
  pinMode(PIN_NITROGEN, OUTPUT);
  pinMode(PIN_DUMP, OUTPUT);
  pinMode(PIN_VENT, OUTPUT);
  pinMode(PIN_DISCONNECT, OUTPUT);
  pinMode(PIN_IGNITE, OUTPUT);
  pinMode(PIN_MPV, OUTPUT);
  pinMode(PIN_SHUTOFF, OUTPUT);

  // start Serial connections
  from_bunker_connection.begin(BAUD_RATE);
  transfer_from_bunker.begin(from_bunker_connection);
  transfer_from_bunker.reset();

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
  if (transfer_from_bunker.available())
  {
    // receive datapacket
    transfer_from_bunker.rxObj(dp);

    // analyze all data for this packet
    analyze_state(dp, SIG_SHUTOFF,     PIN_SHUTOFF);
    analyze_state(dp, SIG_MPV,         PIN_MPV);
    analyze_state(dp, SIG_IGNITE,      PIN_IGNITE);
    analyze_state(dp, SIG_DISCONNECT,  PIN_DISCONNECT);
    analyze_state(dp, SIG_VENT,        PIN_VENT);
    analyze_state(dp, SIG_DUMP,        PIN_DUMP);
    analyze_state(dp, SIG_NITROGEN,    PIN_NITROGEN);
    analyze_state(dp, SIG_NITROUS,     PIN_NITROUS);

    if (DEBUGGING)
    {
      // format and display data to Serial monitor
      display_dp(dp);
    }

    // delay because we don't expect data transmitting that fast
    delay(DELAY_RX);
  }
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
 * display_dp()
 * -------------------------
 * Formats and displays received data in comma-separated form to Serial.
 */
void display_dp(const Datapacket& dp)
{
  Serial.print("RECEIVED: ");
  Serial.print(dp.sig, BIN);
  Serial.print(", corresponds to: ");
  display_decoded_signals(dp.sig);
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
void analyze_state(Datapacket& dp, int sig_mask, int output_pin)
{
  if (dp.sig & sig_mask)
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
