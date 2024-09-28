#ifndef _espconfiguration
#define _espconfiguration

#include "driver/gpio.h"
#include "driver/uart.h"

#include "nvs_flash.h"

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_spiffs.h"


#define LED_PIN_ERROR 12
#define LED_PIN_WORKING 14
#define LED_PIN_SYNC 27
#define BUTTON_PIN 26


#define WIFI_SSID       "WIFI"
#define WIFI_PASSWORD   "PASSWORD"


void WIFIConfiguration();
void UARTConfiguration();
void GPIOConfiguration();
void SPIFFSConfiguration();
#endif