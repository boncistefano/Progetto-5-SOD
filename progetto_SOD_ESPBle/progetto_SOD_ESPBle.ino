#include "RTClib.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include "BluetoothSerial.h"
#include "esp_bt_device.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <BH1750.h>
#include <string.h>
#include <string>
#include "doAdjust.h"

#define BMP_SCK (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS (10)

#define SEA_PRESSURE_LEVEL 1015.7  //Ancona

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run 'make menuconfig' to enable it
#endif

// Creazione degli oggetti
TaskHandle_t taskHandler;
BluetoothSerial SerialBT;
Adafruit_BMP280 bmp;
RTC_PCF8523 rtc;
BH1750 lightMeter;

// Dichiarazione delle variabili globali
float temp;
float light;
float press;
float alt;
bool execAdjust = false;

// Task per aggiornare data e orario tramite comando proveniente da Raspberry
void TaskTimeReading(void *pvParameters) {
  (void)pvParameters;

  TickType_t xLastWakeTime;

  for (;;) 
  {
    xLastWakeTime = xTaskGetTickCount();

    // Questo if viene eseguito se il settaggio è stato già fatto una prima volta
    if (execAdjust == true) 
    {
      String a = SerialBT.readString();
      if (!a.isEmpty()) 
      {
        doAdjust(rtc, a); // Questa funzione viene richiamata per sincronizzare data e ora
      }
    }

    // Questo blocco di codice viene eseguito se si tratta del primo settaggio dell'orario dall'avvio
    else {
      String a = SerialBT.readString();
      if (a.isEmpty()) 
      {
        SerialBT.print("Ho bisogno dell'orario"); // Viene inviata una richiesta alla RPi
        unsigned long startTime = millis();
        while (a.isEmpty() && millis() - startTime < 10000) {
          a = SerialBT.readString();
        }
      }
      if (!a.isEmpty()) {

        doAdjust(rtc, a);
        execAdjust = true;

        // Creazione degli altri task
        
        xTaskCreate(TaskBMPReading,
                    "BMP Reading",
                    4000,
                    NULL,
                    1,
                    &taskHandler);


        xTaskCreate(TaskBHReading,
                    "BH Reading",
                    4000,
                    NULL,
                    2,
                    &taskHandler);

        xTaskCreate(TaskSensorSending,
                    "Sensor Sending",
                    4000,
                    NULL,
                    3,
                    &taskHandler);
      }
    }
    vTaskDelayUntil(&xLastWakeTime, 100);
  }
}

// Task per la lettura dei dati provenienti dal sensore BMP280 (temperatura, pressione e altitudine)
void TaskBMPReading(void *pvParameters) {
  (void)pvParameters;

  TickType_t xLastWakeTime;

  for (;;) 
  {
    xLastWakeTime = xTaskGetTickCount();
    temp = bmp.readTemperature();
    press = bmp.readPressure();
    alt = bmp.readAltitude(SEA_PRESSURE_LEVEL);
    Serial.print("La temperatura è: ");
    Serial.println(temp);
    Serial.print("La pressione è: ");
    Serial.println(press);
    Serial.print("L'altitudine è: ");
    Serial.println(alt);
    vTaskDelayUntil(&xLastWakeTime, 3000);
  }
}

// Task per la lettura dei dati provenienti dal sensore BH1750 (luminosità)
void TaskBHReading(void *pvParameters) {
  (void)pvParameters;

  TickType_t xLastWakeTime;

  for (;;) 
  {
    xLastWakeTime = xTaskGetTickCount();
    light = lightMeter.readLightLevel();
    Serial.print("La luminosità è: ");
    Serial.println(light);
    vTaskDelayUntil(&xLastWakeTime, 3000);
  }
}

// Task per l'invio dei datti letti (sotto forma di stringa) alla Raspberry Pi tramite bluetooth
void TaskSensorSending(void *pvParameters) {

  for (;;) 
  {
    DateTime now = rtc.now();
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    SerialBT.print(String(((int)(now.day()))) + "/" + String(((int)(now.month()))) + "/"
                   + String(((int)(now.year()))) + ";" + String(((int)(now.hour()))) + ":"
                   + String(((int)(now.minute()))) + ":" + String(((int)(now.second()))) + ";"
                   + String(temp) + " *C" + ";" + String(press) + " Pa" + ";" + String(alt) + " m" + ";"
                   + String(light) + " lx");

    vTaskDelayUntil(&xLastWakeTime, 3000);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n---Start---");
  SerialBT.begin("ESP32test");

  unsigned status;
  status = bmp.begin();

  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);

  if (!rtc.begin()) 
  {
    Serial.println("Impossibile trovare l'RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (!rtc.initialized() || rtc.lostPower()) 
  {
    Serial.println("RTC non inizializzato");
  }

  rtc.start();
  Wire.begin();
  lightMeter.begin();

  // Creazione dei task
  xTaskCreate(TaskTimeReading,  // Funzione relativa al task
              "Time Reading",   // Nome del task
              4000,             // Stack size
              NULL,             // *pvParameters
              4,                // Priorità
              &taskHandler);    // Task handler
}

void loop() {
}
