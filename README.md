# esp32-nrf52810-computer-lock

This project implements a Bluetooth Low Energy (BLE) RSSI-based system to automatically lock your computer when a paired BLE device moves out of range. The ESP32 acts as a BLE scanner to monitor the signal strength of a specific BLE device (NRF52810 BLE beacon), and if the signal strength drops below a defined threshold for a certain duration, the ESP32 sends a keyboard shortcut to lock the computer.

## Features

- Monitors the RSSI (signal strength) of a target BLE device in real time.
- Locks the computer using the "Win + L" shortcut when the target device moves out of range.
- Prevents repeated locking until the device comes back into range.
- Adjustable RSSI threshold and sensitivity settings.
- Verbose mode for detailed debugging output.

## Hardware Requirements

- ESP32 Development Board
  - Tested with ELEGOO ESP-WROOM-32 ESP32 ESP-32S Development Board
- BLE Beacon
  - Tested with NRF52810 BLE beacon

## Software Requirements

- CP210x USB to UART Bridge VCP Drivers
- Arduino IDE
- NRF Connect mobile application
- Latest beta version of the [ESP32-BLE-Keyboard library](https://github.com/T-vK/ESP32-BLE-Keyboard)

## How it Works

1. The ESP32 scans for BLE devices in its vicinity.
2. When the target device is found, the ESP32 measures its RSSI (signal strength).
3. If the RSSI falls below a predefined threshold (default: −70 dBm) for a specified number of consecutive readings, the ESP32 simulates the "Win + L" keyboard shortcut to lock the computer.
4. The system resets and waits for the target device to return within range before it can lock the computer again.

## Installation and Setup

1. Install the CP210x USB to UART Bridge VCP Drivers
2. Install Arduino IDE
3. Connect your ESP32 to your computer via USB
4. In Arduino IDE go to `File > Preferences > Additional board managers URLs` and add: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
5. In Arduino IDE click on `Select board and port` and select `ESP32 Dev Module` as well as the correct COM port for your board
6. Download the .zip of the latest beta version of the [ESP32-BLE-Keyboard library](https://github.com/T-vK/ESP32-BLE-Keyboard) and install it in Arduino IDE by going to `Sketch > Include Library > Add .ZIP Library...`
   - A bug in versions < 0.3.2 makes it so the ESP32 board has to be re-paired to the computer everytime it turns off or gets disconnected
7. Navigate to your Arduino Librairies path and modify the following lines in BleKeyboard.cpp
```cpp
BLEDevice::init(deviceName); --> BLEDevice::init(deviceName.c_str());
hid->manufacturer()->setValue(deviceManufacturer); --> hid->manufacturer()->setValue(deviceManufacturer.c_str());
```
8. Using the NRF Connect mobile application (or with another means) find your BLE beacon's MAC address and modify it in the `esp32-nrf52810-computer-lock.ino` file, the letters should be in lowercase
```cpp
const char* targetDeviceMAC = "xx:xx:xx:xx:xx:xx";
```
9. You can now configure the other parameters:
   - The number of readings used to calculate the RSSI average
   - RSSI value threshold at which the computer will be locked
   - The number of consecutive reads under the threshold after which the computer will be locked
   - Whether the debug messages are printed to the serial output or not (should be set to false when you are ready to use)
```cpp
#define NUM_READINGS 10
#define MAX_CONSECUTIVE_CROSSES 2
#define RSSI_THRESHOLD -70

bool verboseMode = true;
```
10. Upload the code to your ESP32
11. Pair the ESP32 as a bluetooth device on your computer
12. Put the BLE beacon in your pockets and walk off, your computer should get locked!

## Key Parameters

|        Parameter        |                              Description                              |    Default Value    |
| :---------------------: | :-------------------------------------------------------------------: | :-----------------: |
|     targetDeviceMAC     |                 MAC address of the target BLE device                  | "xx:xx:xx:xx:xx:xx" |
|     RSSI_THRESHOLD      |        Signal strength threshold below which the lock triggers        |       -70 dBm       |
| MAX_CONSECUTIVE_CROSSES | Number of consecutive readings below the threshold to trigger locking |          2          |
|      NUM_READINGS       |           Number of RSSI readings to average for stability            |         10          |

## Troubleshooting

1. No BLE Devices Detected:
   - Ensure the BLE device is powered on and within range.
   - Verify the MAC address of the target device and make sure the letters are lowercase in the code.

2. ESP32 Fails to Lock the Computer:
   - Ensure the computer is paired with the ESP32 as a Bluetooth keyboard.
   - Make sure you are using version 0.3.2 or higher of the [ESP32-BLE-Keyboard library](https://github.com/T-vK/ESP32-BLE-Keyboard).

3. Lock Triggering Too Often:
   - Adjust the RSSI_THRESHOLD to a lower value.
   - Increase MAX_CONSECUTIVE_CROSSES for more stability.

## License

This project is open-source and licensed under the [MIT License](LICENSE). Feel free to modify and distribute the code as needed.