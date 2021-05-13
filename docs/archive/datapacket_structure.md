## Ground System's Datapacket Structure

In order to ensure a clear unambiguous separation between datapackets, we define a hardcoded structure for datapackets being transmitted over our systems' Serial connections. Any junk data existing between subsequent datapackets should (and will) be ignored.

We provide the datapacket structure for our system's below, where each system obeys this datapacket structure when encoding during transmission and decoding during reception:

### DAQ

Tx, Rx: Pad, Bunker

| Byte # | Description |
|--|--|
| 0 | Start of packet indicator (some single character). For now, `s` for "start". |
| 1 | Data validity information, where each individual bit specifies whether or not a particular datastream's data value is valid (`1` == valid, `0` == invalid). Data may be invalidated due to either the sensor not being able to be polled from or errors in the Arduino script. |
| 2 | Size of packet's data in number of distinct data fields, where the first field is always the timestamp, and the rest of the fields are `float` data values. These fields are comma-separated. This size integer is 8-bits, which has range 0-255 in values. This can be increased very easily if we have packets with data portions' down the road that have larger than 255 fields. |
| 3+ | Packet's raw data |
| (N-3)-N | Checksum, CRC32, computed using the datapackets' data field, which includes a timestamp and all sensor data. |

### Controls

Tx, Rx: Bunker, Pad

| Byte # | Description |
|--|--|
| 0 | Start of packet indicator (some single character). For now, `s` for "start". |
| 1 | Size of packet's data in bytes (an 8-bit integer, which has range 0-255 in values). Currently, this will always be of value `1`, since we have less than 8 switches to represent in our state signal (at most 8 can be represented in each byte of our state variable). This can be increased very easily if we have packets down the road that could be larger than 255 bytes. |
| 2+ | Packet's raw data |
| (N-3)-N | Checksum, CRC32, computed using the datapackets' data field (state of switches). |