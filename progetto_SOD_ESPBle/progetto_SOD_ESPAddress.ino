#include "BluetoothSerial.h"
#include "esp_bt_device.h"
#include <Wire.h>
#include <SPI.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Creazione degli oggetti
BluetoothSerial SerialBT;

// Funzione per estrapolare e stampare l'indirizzo dell'ESP32 per la connessione bluetooth
void printDeviceAddress() 
{
  const uint8_t* point = esp_bt_dev_get_address();
 
  for (int i = 0; i < 6; i++) 
  {
    char str[3];
 
    sprintf(str, "%02X", (int)point[i]);
    Serial.print(str);
 
    if (i < 5)
    {
      Serial.print(":");
    }
  }
}

void setup()
{
  Serial.begin(115200);
  SerialBT.begin();
  Serial.println("Device Name: ESP32test");
  Serial.print("BT MAC: ");
  printDeviceAddress();
  Serial.println();
}

void loop() {
}
