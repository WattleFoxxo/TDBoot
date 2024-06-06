#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Update.h>
#include <esp_ota_ops.h>

#include "config.h"
#include "pins.h"
#include <Arduino_GFX_Library.h>

#define GFX_DEV_DEVICE LILYGO_T_DECK
#define GFX_BL TDECK_TFT_BACKLIGHT

Arduino_DataBus *bus = new Arduino_ESP32SPI(TDECK_TFT_DC, TDECK_TFT_CS, TDECK_SPI_SCK, TDECK_SPI_MOSI, TDECK_SPI_MISO);
Arduino_GFX *gfx = new Arduino_ST7789(bus, GFX_NOT_DEFINED, 1, false);

void print_menu();
void print_apps();
void start_app(int index, char* name);

int current_selection = 0;
int number_of_apps = 0;

struct app {
    int id;
    char name[MAX_NAME_SIZE];
};

app apps[MAX_APPS];

void setup() {
    Serial.begin(115200);

    Serial.println("[BOOT_MENU] keyboard init");

    pinMode(TDECK_PERI_POWERON, OUTPUT);
    pinMode(TDECK_TFT_CS, OUTPUT);

    digitalWrite(TDECK_PERI_POWERON, HIGH);
    digitalWrite(TDECK_TFT_CS, HIGH);

    delay(500);

    Wire.begin(TDECK_I2C_SDA, TDECK_I2C_SCL);

    Wire.requestFrom(TDECK_KEYBOARD_ADDR, 1);
    if (Wire.read() == -1) {
        while (1) {
            Serial.println("[BOOT_MENU] keyboard not responding, waiting 1000 ms");
            delay(1000);
        }
    }

    Serial.println("[BOOT_MENU] display init");
    if (!gfx->begin()) {
        Serial.println("[BOOT_MENU] display init failed, trying to continue");
    }

    gfx->fillScreen(GUI_BACKGROUND);
    gfx->setTextColor(GUI_FOREGROUND);

    #ifdef GFX_BL
        pinMode(GFX_BL, OUTPUT);
        digitalWrite(GFX_BL, HIGH);
    #endif

    print_menu();
}

void loop() {
    char keyValue = 0;
    Wire.requestFrom(TDECK_KEYBOARD_ADDR, 1);
    while (Wire.available() > 0) {
        keyValue = Wire.read();
        if (keyValue != (char)0x00) {
            if (keyValue == 'w' && current_selection > 0) current_selection -= 1;
            if (keyValue == 's' && current_selection < number_of_apps) current_selection += 1;
            if (keyValue == 13) start_app(apps[current_selection].id, apps[current_selection].name);

            gfx->fillRect(0, 35, 15, gfx->height(), GUI_BACKGROUND);
            print_apps();
        }
    }

    delay(5);
}

// Print partition table
void print_menu() {
    gfx->fillScreen(GUI_BACKGROUND);
    gfx->setCursor(0, 10);

    gfx->println(" TD-Boot 0.1.0\n Use W and S keys to select an app, then press enter.\n");

    print_apps();
}

void print_apps() {
    gfx->setCursor(0, 35);

    esp_partition_iterator_t iterator = NULL;
    const esp_partition_t *next_partition = NULL;

    iterator = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);

    int index = 0;
    while (iterator) {
        next_partition = esp_partition_get(iterator);

        // Skip boot menu app
        if (next_partition != NULL && index != 0 && index <= MAX_APPS) {

            int app_index = index - 1;
            bool selected = (app_index == current_selection);

            gfx->printf(" %s%d.   %s (0x%06x)\n", selected ? "> " : "  ", app_index, next_partition->label, next_partition->address);

            // Add app to the list
            app newApp;
            newApp.id = app_index;
            strcpy(newApp.name, next_partition->label);

            apps[app_index] = newApp;
            number_of_apps = app_index;
        }

        index += 1;
        iterator = esp_partition_next(iterator);
    }
}

void start_app(int index, char* name) {
    const esp_partition_t *next_partition = esp_ota_get_next_update_partition(NULL);

    if (index > 0 && index <= 4) {
        for (int i = 0; i < index; i++) {
            next_partition = esp_ota_get_next_update_partition(next_partition);
            if (!next_partition) break;
        }
    }

    if (next_partition && esp_ota_set_boot_partition(next_partition) == ESP_OK) {
        printf("[BOOT_MENU] starting: %s (%s)\n", name, next_partition->label);

        gfx->setCursor(0, 10);
        gfx->fillScreen(GUI_BACKGROUND);
        gfx->printf(" Starting %s...\n", name);

        Wire.end(); // STOP KEYBOARD TO AVOID ERRORS!!!!

        esp_restart();

    } else {
        printf("[BOOT_MENU] failed to set partition\n");

        gfx->setCursor(0, 10);
        gfx->fillScreen(GUI_BACKGROUND);
        gfx->printf(" Faild to start %s.\n", name);

        delay(500);

        print_menu();
    }
}
