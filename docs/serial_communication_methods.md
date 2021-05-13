
# Ground System's Serial Communication Methods

In order to implement much of the safer serial transmission practices that was outlined in the [Safer Serial Transmission Practices](safer_serial_transmission_practices.md) document, we utilize the third-party [`SerialTransfer` Arduino library](https://github.com/PowerBroker2/SerialTransfer), which provides functionality to transfer packetized data fast and reliably via UART, I2C, SPI, or Serial connection.

In this document, we provide a bit additional background on why we chose to use the `SerialTransfer` library, as well as provide the simple datapacket payload structure that is transmitted using the `SerialTransfer` API.

## Why Use `SerialTransfer`?
As noted on its documentation, the `SerialTransfer` library provides support for a number of key features, some of which we especially wanted to integrate into our system software. These features are listed below, with the especially desirable features for our purposes **bolded** accordingly:
- **works with `software-serial`**, `software-I2C`, and `software-SPI` libraries
- **is non-blocking**
- **uses packet delimiters**
- **uses consistent overhead byte stuffing**
- **uses CRC-8 (Polynomial 0x9B with lookup table) for checksum functionality**
- allows the use of dynamically sized packets (packets can have payload lengths anywhere from 1 to 254 bytes)
- supports user-specified callback functions
- **can transfer bytes, ints, floats, structs**, even large files like JPEGs and CSVs!!

## Datapacket Payload Structure

In this section, we outline the the datapacket payload structure utilized by our ground systems support software to perform all of the necessary communication of data over serial connection.

**NOTE:** The term **payload** refers to the actual "data-portion" that we care about when transmitting a packet. The `SerialTransfer` library defines their own set structure for the entire datapacket (including packet delimiters, various metadata fields, etc.). These can be found in more detail by referring to the [`SerialTransfer` documentation](https://github.com/PowerBroker2/SerialTransfer#readme).

### DAQ

Tx, Rx: Pad, Bunker

```c
// datapacket payload structure
struct Datapacket
{
  // metadata
  int valid; // valid flags for our various data fields 

  // data-portion
  unsigned long timestamp; // timestamp of data collection on PAD Arduino (ms since startup)
  float pt0_data;
  float pt1_data;
};
```

### Controls

Tx, Rx: Bunker, Pad

Currently, we do not utilize the `SerialTransfer` library for our Controls subsystem. This is primarily due to the minimal amount of data being transmitted, as well as our legacy code (that worked perfectly) adopted from earlier in the development season. Eventually, this software will adopt the `SerialTransfer` library for consistency's and safety's sake.