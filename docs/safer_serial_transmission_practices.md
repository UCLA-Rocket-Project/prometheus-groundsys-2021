

# Safer Serial Transmission Practices

During Prometheus' Hot-Fire of the hybrid rocket system on May 1st, 2021, due to issues with data transmission that arose during initial checks, a simplified and bare-bones version of the complete DAQ system was utilized.

Due to Prometheus' consideration for a second Hot-Fire, as well as a more robust DAQ system for our eventual launch, this document has been made to discuss and reflect on some of the safer options.

We note that these practices can (and will, if not already) be considered and implemented for the Controls portion of Prometheus' Ground Systems.

## Context (Brief Overview of System)
The DAQ system, at the state utilized at the Hot-Fire, was composed of 2 Arduinos, one on our Wired Pad DAQ subsystem, and the other on our Wired Bunker DAQ subsystem. These Arduinos communicated over Serial connection using the `RS-485` protocol. Software Serial, enabled with Arduinos' `SoftwareSerial` library, was used over Hardware Serial due to ability to quickly and easily debug/re-upload code without having to disconnect wired portions of the transmission system.

Looking at the Pad-side in more detail, this Serial connection was composed of (software-defined) TX and RX pins connected from each Arduino to the driver input port of a `MAX485`, a low-power slew-rate-limited transceiver which enables communication using the `RS-485`  protocol for longer distances  in a safer way to preserve data integrity. The `MAX485` module can be wired in slightly different ways to perform transmission or reception (which can be found at this according to [this article](https://create.arduino.cc/projecthub/maurizfa-13216008-arthur-jogy-13216037-agha-maretha-13216095/modbus-rs-485-using-arduino-c055b5)). For our system, the Pad side's `MAX485` was wired to become a transmitter, where the data output pins were then connected to an `RJ45` module, which provides a port for an Ethernet cable connection.

On the Bunker-side, the setup is virtually the same, however the `MAX485` is wired as a receiver, where the Receiver Output pin is connected to the Bunker Arduino's RX pin.

For more information (as well as schematics of the wired DAQ system), please refer to the [`Prometheus Electronics PDR 2021 Slides`](https://docs.google.com/presentation/d/1Jdpi-pk0Zy6jeL-ZxyfDw0a2y5S16jKSPS87sjIwKoI/edit).

## Considerations
We  dive into multiple different considerations, some of which are already integrated into our DAQ/Controls system, and others which should or will be implemented before further testing and launch.

### Long-Distance Optimized Serial Communication Protocols

We aim to have our two separate subsystems (Pad and Bunker DAQ) be able to be connected over virtually any distance, whether it be on the same table in the lab or a mile away in the middle of the desert connected via merely an Ethernet cable. To accomplish this, it is important to ensure that the integrity of our data is preserved throughout its transmission over longer distances, which present the data with possibilities of interference.

As noted above in the `Context (Brief Overview of System)` section of this document, we already have integrated this in the form of the `RS-485` communication protocol using the `MAX485` hardware modules. This setup avoids the need to perform software decoding of the transmitted bits, therefore abstracting away the potential computational and integrity issues associated with handling raw transmission/reception ourselves.

### Delay in Transmission

Through experience in the development of other Arduino-related projects, having a hardcoded-delay at the end of your `loop()` method—especially if working with any form of data polling or transmission—is critical to avoid data corruption due to overwriting of internal buffers used to stage data before its processed.

If you start listening to serial data in the middle of a stream, it is quite possible that a bit in the middle of the stream will be interpreted as a start bit, and then the receiver will interpret everything after that incorrectly. Therefore, the only real way to recover from this is to have a large enough gap, from time to time, that this cannot happen.

In order to properly avoid any data corruption when transmitting over Serial, a minimum delay between transmissions is computed as follows:

```c
baudrate = 9600; // bits per second
com_rate = baudrate/10; // byte (plus metadata) per second
min_delay = 1/com_rate; // s per byte

min_delay_ms = min_delay*1000; // convert to milliseconds (ms)
safe_min_delay_ms = 2*min_delay_ms; // safety precaution, "just in case"
```

When setting up Arduinos for a Serial communication, a specified baudrate must be provided to the `Serial.begin()` method, with the lowest being `9600`. This value represents the bits per second at which data is transmitted/received over the Serial line.

In transmitting over Serial, Arduino prepends and appends a start and stop bit, respectively, to each byte that is transmitted. Therefore, since we know there is 8 bits in a byte, every "unit" of transmission is 10 bits (start bit, byte of data, stop bit).

We compute our so-called "rate of communication" to be our baudrate divided by the size of our "unit" of transmission (which is `10`). This represents how often a byte of our data is transmitted/received over the Serial line.

To be completely safe, and avoid any issues of a byte being corrupted when being transmitted, we want to wait at least a full "unit" of transmission (10 bits) before attempting to write again. Therefore, our minimum delay can be computed by taking the inverse of our communication rate, receiving our final value. We convert this rate to be in milliseconds, due to Arduino's `delay()` method taking a millisecond-unit argument.

As an extra precaution, we scale this delay value arbitrarily by a factor of `2` "just to be safe". This is likely unneeded due to the other micro-delays associated with running different portions of the script, but we'll do it anyway.

For the most common Arduino-defined baudrates, we provide the following table:

| Baudrate (bps) | Minimum Delay, scaled (ms) |
|--|--|
| 9600 | 2.08333 |
| 14400 | 1.38888 |
| 19200 | 1.04166 |
| 28800 | 0.69444 |
| 31250 | 0.64000 |
| 38400 | 0.52083 |
| 57600 | 0.37222 |
| 115200 | 0.17361 |

**NOTE**: Prometheus was having troubles trying to get Software Serial working with a baudrate of `115200`, despite it being supported according to Arduino.

**NOTE**: This need for delay seems to only be required on the transmission side when conducting some initial (limited) testing. This intuitively makes sense, as the corruption likely happens when bytes are actually being transmitted.

Additional Resources:
- [Information on delay computation, etc.](https://arduino.stackexchange.com/questions/1569/what-is-serial-begin9600/14146#14146)

## Formal Datapacket Structure

In order to ensure a clear unambiguous separation between datapackets, we seek to hardcode a structure for datapackets being transmitted over our Serial connection. In general, this structure involves some "start" indicator, metadata about the packet, and a raw-data section. Any junk data existing between subsequent datapackets should (and will) be ignored.

We note that in contexts where we are polling from sensors (perhaps through an API provided by a library), we should likely populate the datapacket with null values accordingly to ensure accurate decoding.

## Checksum

A checksum is a relatively smaller-sized piece of data generated through a checksum function or algorithm, where the input is that of some raw data that it intends to verify after its transmission.  
  
Essentially, both the transmitter and receiver ends of the communication system agree on the same checksum algorithm before beginning data transmission. Once communication begins, the transmitter generates the checksum for some message by passing its raw data through the checksum algorithm. This produced checksum is appended onto the message, and subsequently transmitted. The receiver proceeds to verify the data’s integrity by also computing the checksum for the received data. If the computed checksum matches the transmitted checksum, then there is a very high probability that the data has not been accidentally (or maliciously) altered or corrupted during transmission. If it does not match, then we assume some error occurred either in the transmission of the data or the checksum, and handle accordingly (discard data, or attempt to error correct if implemented).

Additional Resources:
- Other error detection (and correction) methods were considered more extensively in Prometheus' 2020-2021 trade studies, particularly in the [`Electronics Controls Trade Study` document](https://docs.google.com/document/d/1m9FSp2kkbptVq2HxTiYCMsausWmxnFye5hQNq3DEO6M/edit).

## Steady Signal Reception

When changing the state of any critical systems (such as that of our Pad Controls subsystem with regards to filling and ignition procedures), it is desirable to *not make any false positive assumptions* of the data/signal received.

One very simple precaution that can be employed is confirming that the data received properly reflects the signal sent multiple times before shifting state. In a one-way communication system, this can be done by having the transmitting system continually transmit a signal, while having the receiving system only shift to a  pre-determined critical state when receiving the same signal `X` number of times, where this `X` represents some user-defined threshold.

This system is currently adopted in Prometheus' Controls subsystem code, due to the occasional presence of some likely hardware-induced erroneous signals over our Serial communication line.

## Solutions Currently Adopted

Given the aforementioned considerations, our current ground systems software adopts the majority of these practices in both our DAQ and Controls subsystems. For more information on how this is done, refer to the [Serial Communication Methods document](serial_communication_methods.md).

## Author Notes
This document was compiled by Harrison Cassar, Class of 2022, in May of 2021. For any questions, feel free to directly message Harrison over Slack.

*It's been manifested.*
:cowboy_hat_emoji: