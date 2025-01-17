#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BleKeyboard.h>

#define NUM_READINGS 10
#define MAX_CONSECUTIVE_CROSSES 2
#define RSSI_THRESHOLD -70

const char* targetDeviceMAC = "xx:xx:xx:xx:xx:xx";
bool verboseMode = false;

int consecutiveCrosses = 0;
bool isLocked = false;

BLEScan* pBLEScan;
BleKeyboard bleKeyboard;

void verbosePrint(const char* message) {
    if (verboseMode) {
        Serial.println(message);
    }
}

void verbosePrintf(const char* format, ...) {
    if (verboseMode) {
        va_list args;
        va_start(args, format);
        Serial.vprintf(format, args);
        va_end(args);
    }
}

int getAverageRSSI(BLEAdvertisedDevice &advertisedDevice) {
    int totalRSSI = 0;
    for (int i = 0; i < NUM_READINGS; i++) {
        totalRSSI += advertisedDevice.getRSSI();
        delay(25);
    }
    return totalRSSI / NUM_READINGS;
}

void lockComputer() {
    if(bleKeyboard.isConnected()) {
      verbosePrint("Locking computer...");
      bleKeyboard.press(KEY_LEFT_GUI);
      bleKeyboard.press('l');
      delay(100);
      bleKeyboard.releaseAll();
      isLocked = true;
    }
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        if (advertisedDevice.getAddress().toString() == targetDeviceMAC) {

            int rssi = getAverageRSSI(advertisedDevice);
            String deviceName = advertisedDevice.haveName() ? advertisedDevice.getName() : "Unknown";
            
            verbosePrintf("Found device with MAC %s, RSSI: %d dBm, Name: %s\n",
                          targetDeviceMAC, rssi, deviceName.c_str());

            if (rssi < RSSI_THRESHOLD) {
                consecutiveCrosses++;

                if (consecutiveCrosses >= MAX_CONSECUTIVE_CROSSES && !isLocked) {
                    lockComputer();
                }

            } else {
                consecutiveCrosses = 0;
                isLocked = false;
            }
        }
    }
};

void setup() {
    Serial.begin(115200);

    BLEDevice::init(String("ESP32"));
    bleKeyboard.begin();
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);
}

void loop() {
    BLEScanResults* scanResults = pBLEScan->start(3);
    pBLEScan->clearResults();
    delay(1500);
}
