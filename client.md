# First use

Install wifi link on the ESP8266 chip. This will make sure the mega board can establish a wifi connection to the server.

# Install wifi link

The ESP8266 can be flashed with wifilink by using instructions as found on https://community.openhab.org/t/howto-arduinomega-esp8266-to-mqtt-server-great-for-multi-sensor-monitoring/37304.

## Dip switches

Ensure the board is in flashing mode for the ESP8266 chip.

| Dip switches | State |
| ------------ | ----- |
| 1 2 3 4 8    | OFF   |
| 5 6 7        | ON    |

## Download firmware and install

https://github.com/jandrassy/arduino-firmware-wifilink

Download the source code, and open the ArduinoFirmwareEsp.ino sketch. For detailed instructions, read the README in the link above. Flash it to the ESP8266 chip.

- Board setting: Generic ESP8266
- Baud rate: 115200
- Flash size: 4M, 1M SPIFFS
- CPU Frequency: 80MHz
- LwIP version: v2 Higher Bandwidth
- Debug options: Debug / None
- Uncomment in code: #define GENERIC_ESP8266
- Change in code: #define BAUDRATE_COMMUNICATION 115200
- "Erase flash" option is new in esp8266 Arduino package 2.4.1. To preserve SPIFFS and WiFi credentials use option "Sketch only". If changing from prebuild firmware, changing LwIP option or after a update to new version of esp8266 Arduino core package use "All flash content" to erase all parameters set be Espressif SDK. Erasing all SDK parameters can help if you experience WiFi connection stability issues.

Do not "Upload", but use menu option Tools > ESP8266 Sketch Data Upload.

## Connect to the wifi

Disconnect the arduino, and flip the dipswitches to exit flashing mode:

| Dip switches | State |
| ------------ | ----- |
| 1 2 3 4      | ON    |
| 5 6 7 8      | OFF   |

## Install the wifilink library

To be able to use wifilink on the arduino, making use of the connection in the esp8266 chip.

Use the instructions found here: https://github.com/jandrassy/arduino-library-wifilink. Make sure to downgrade Arduino to 2.4.1, see https://github.com/jandrassy/arduino-firmware-wifilink/issues/10.
This can be done through board manager -> search for esp8266.
