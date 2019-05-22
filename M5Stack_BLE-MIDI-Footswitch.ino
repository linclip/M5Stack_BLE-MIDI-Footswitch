#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <M5Stack.h>

//M5Stack SD-Updater
#include <M5StackUpdater.h>

///// Button
const uint8_t buttonPins[5] = {39, 38, 37, 21, 22    }; //A, B, C, 16,17(R2,T2)   18,19=top
boolean buttonFlags[5] = {false, false, false, false, false};
uint8_t buttonNote[5] = {60, 64, 67,   71, 72};
uint8_t buttonCC[5] = {79, 80, 81,   82, 83};

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

#define MIDI_SERVICE_UUID        "03b80e5a-ede8-4b33-a751-6ce34ec4c700"
#define MIDI_CHARACTERISTIC_UUID "7772e5db-3868-4112-a1a9-f2669d106bf3"

void printConnectionStatus(const char* message){
  M5.Lcd.fillRect(0, 210, 320, 20, BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(160,210);
  M5.Lcd.printf("%13s", message);
}

BLEServer *pServer;  // ■
BLESecurity *pSecurity;  //■

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    printConnectionStatus("connected");
    Serial.println("connected");
  }
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    printConnectionStatus("disconnected");
    Serial.println("disconnected");

    // Start advertising ■
    pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
    pSecurity->setCapability(ESP_IO_CAP_NONE);
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
  
    pServer->getAdvertising()->addServiceUUID(MIDI_SERVICE_UUID);
    pServer->getAdvertising()->start();
  }
};

void noteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) {
  uint8_t midiPacket[] = {0x80, 0x80,  (uint8_t)(0x90 | channel), pitch, velocity };
  //Serial.println((uint8_t)(0x90 | channel));
  pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
  pCharacteristic->notify();
}
void noteOff(uint8_t channel, uint8_t pitch, uint8_t velocity) {
  uint8_t midiPacket[] = {0x80, 0x80,  (uint8_t)(0x80 | channel), pitch, velocity };
  //Serial.println((uint8_t)(0x80 | channel));
  pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
  pCharacteristic->notify();
}
void controlChange(uint8_t  channel, uint8_t  control, uint8_t  value) {
  uint8_t midiPacket[] = {0x80, 0x80, (uint8_t)(0xB0 | channel), control, value };
  pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
  pCharacteristic->notify();
}

void setup() {  
  Serial.begin(115200);
  M5.begin();  //LCD

  //M5Stack SD-Updater
  if(digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }

  BLEDevice::init("M5Footswitch"); //Device Name ■

  // Create the BLE Server
  //BLEServer *pServer = BLEDevice::createServer(); // ■
  pServer = BLEDevice::createServer(); // ■
  pServer->setCallbacks(new MyServerCallbacks());
  BLEDevice::setEncryptionLevel((esp_ble_sec_act_t)ESP_LE_AUTH_REQ_SC_BOND);
    
  // Create the BLE Service
  BLEService *pService = pServer->createService(BLEUUID(MIDI_SERVICE_UUID));

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      BLEUUID(MIDI_CHARACTERISTIC_UUID),
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_WRITE_NR
                    );
  pCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);

  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  //BLESecurity *pSecurity = new BLESecurity(); //■
  pSecurity = new BLESecurity();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
  pSecurity->setCapability(ESP_IO_CAP_NONE);
  pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

  pServer->getAdvertising()->addServiceUUID(MIDI_SERVICE_UUID);
  pServer->getAdvertising()->start();

  //ボタン
  for (int i = 0; i < 3; i++) {
    pinMode(buttonPins[i], INPUT); //GPIO #37-39 は内部プルアップ無し
  }
  //外部ピン
  for (int i = 3; i < 5; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  M5.Lcd.setCursor(7,0);
  M5.Lcd.setTextSize(2);
  M5.Lcd.printf("BLE MIDI FootSwitch");

}  //setup

void loop() {  
    for (int i = 0; i < 5; i++) { // 3: button, 5: footswitch

      int buttonValue = digitalRead(buttonPins[i]);

      if (buttonValue == LOW && buttonFlags[i] == false) {
        if (deviceConnected) {
          // noteOn
          noteOn(0, buttonNote[i], 127);
          // controlChange
          //controlChange(0, buttonCC[i], 127);
        }
      
        M5.Lcd.setTextSize(3);
        M5.Lcd.fillRect(0, i * 32 + 32, 100, 24, RED);
        //delay(500);
        M5.Lcd.setCursor(0, i * 32 + 32);
        M5.Lcd.printf("%2d On", buttonPins[i]);

        delay(100); //
        buttonFlags[i] = true;
      } //if

      if (buttonValue == HIGH && buttonFlags[i] == true) {
        if (deviceConnected) {
          // noteOff
          noteOff(0, buttonNote[i], 127);
        }

        //M5.Lcd.setTextSize(3);
        M5.Lcd.fillRect(48, i * 32 + 32, 64, 24, BLUE);
        M5.Lcd.setCursor(48, i * 32 + 32);
        M5.Lcd.printf("Off");

        delay(100);
        M5.Lcd.fillRect(0, i * 32 + 32, 320, 24, BLACK);

        buttonFlags[i] = false;
      } // if
    } //for
}
