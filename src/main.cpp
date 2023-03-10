#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristic;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  BLEDevice::init("ELCOBEACON");

  pServer = BLEDevice::createServer();
  pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                              CHARACTERISTIC_UUID,
                              BLECharacteristic::PROPERTY_READ |
                              BLECharacteristic::PROPERTY_WRITE
                              );

  pCharacteristic->setValue("Hello, World! TPM");
  pService->start();
  //BLEAdvertising *pAdvertising = pServer->getAdvertising();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  //pAdvertising->start();
  Serial.println("Characteristic defined! Now you can read it in the Client!");
}

void loop() {
  std::string value = pCharacteristic->getValue();
  Serial.print("The new characteristic value is: ");
  Serial.println(value.c_str());
  delay(2000);
}