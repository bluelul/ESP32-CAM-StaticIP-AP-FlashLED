# ESP32-CAM-StaticIP-AP-FlashLED
### Author: `bluelul.com`

Reference: https://randomnerdtutorials.com/esp32-cam-video-streaming-web-server-camera-home-assistant/

## Environment version
- [Arduino IDE](https://www.arduino.cc/en/software) `1.8.13`
- [esp32 library](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html) `1.0.4`
- [WifiManager library](https://github.com/tzapu/WiFiManager) `2.0.3-alpha`

## Flash Wiring
- U0R <-> TX
- U0T <-> RX
- IO0 <-> GND (only when flashing, disconnect for operating)
- 5V  <-> 5V (better with external power supply ~2A)
- GND <-> GND

## Flash config
- **Board**: ESP32 Wrover Module
- **Upload Speed**: 921600
- **Flash Frequency**: 80 MHz
- **Flash Mode**: QIO
- **Partition Scheme**: Huge APP (3MB No OTA/1MB SPIFFS)
- **Core Debug Level**: None

## Wifi connect
<p align="center" width="100%"><img src="readmeAsset/uespcam.png"></p>