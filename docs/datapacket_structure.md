## Ground System's Datapacket Structure

In order to ensure a clear unambiguous separation between datapackets, we define a hardcoded structure for datapackets being transmitted over our systems' Serial connections. Any junk data existing between subsequent datapackets should (and will) be ignored.

We provide the datapacket structure for our system's below, where each system obeys this datapacket structure when encoding during transmission and decoding during reception:

### DAQ

Tx, Rx: Pad, Bunker

| Byte # | Description |
|--|--|
| 0 | Start of packet indicator (some single character). For now, `s` for "start". |
| 1 | Size of packet's data in bytes (an 8-bit integer, which has range 0-255 in values). This can be increased very easily if we have packets down the road that could be larger than 255 bytes. |
| 2+ | Packet's raw data |

### Controls

Tx, Rx: Bunker, Pad

| Byte # | Description |
|--|--|
| 0 | Start of packet indicator (some single character). For now, `s` for "start". |
| 1 | Size of packet's data in bytes (an 8-bit integer, which has range 0-255 in values). This can be increased very easily if we have packets down the road that could be larger than 255 bytes. |
| 2+ | Packet's raw data |