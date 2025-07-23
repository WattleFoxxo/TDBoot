#include "main.h"

Adafruit_SharpMem_Dither display(TDECK_SCLK, TDECK_MOSI, TDECK_LCD_CS, SCREEN_WIDTH, SCREEN_HEIGHT);
Adafruit_GFX* gfx = &display;

Adafruit_FlashTransport_SPI flashTransport(TDECK_SD_CS, SPI);

Adafruit_SPIFlash flash(&flashTransport);
FatVolume fatfs;
FatFile myFile;

App apps[MAX_APPS];
uint16_t apps_count = 0;
uint16_t selected_app = 1;

void setup() {
    Serial.begin(115200);

    // while (!Serial);
    delay(2000);
 
    Serial.println("Hello world!");

    initialize_keyboard();
    // initialize_partitions();

    // pinMode(TDECK_SD_CS, OUTPUT);
    // pinMode(TDECK_LCD_CS, OUTPUT);

    // digitalWrite(TDECK_SD_CS, HIGH);
    // digitalWrite(TDECK_LCD_CS, HIGH);

    delay(2000);

    for (size_t i = 0; i < 5; i++) {
        App newApp;
        newApp.name = String("Test App ") + String(i + 1);
        newApp.path = String("/test") + String(i + 1);
        newApp.flash_size = 250400;
        newApp.sd_size = 4294967296;

        apps[i] = newApp;
    }

    apps_count = 5;
    
    // while (true);

    display.begin();
    display.setRotation(2);
    display.clearDisplay();
    display.cp437(true);

    // delay(2000);

    // if (!flash.begin()) {
    //     Serial.println("Failed to initialise sdcard");
    // }

    // if (!fatfs.begin(&flash, true, 1)) {
    //     Serial.println("Failed to mount file system");
    // }

    // Serial.println("File system found");
    
    // if (fatfs.exists("test")) {
    //     Serial.println("/test/ ecists");
    // }

    // fatfs.ls();
}

uint8_t is_redraw = 1;

void loop() {
    // display.clearDisplay();
    gfx->fillScreen(COLOUR_WHITE);

    draw_ui();

    display.refresh();
    delay(50);
}

void draw_ui() {
    // gfx->fillRect(0, 0, SCREEN_WIDTH, 40, COLOUR_DARK_GREY);

    // Title bar
    draw_gradient_rect(gfx, 0, 0, SCREEN_WIDTH, 30, 1, 14);
    draw_text_left(gfx, "Boot Menu", 3, 23, COLOUR_WHITE, &FreeSansOblique12pt7b);

    // App info


    draw_app_info(&apps[selected_app]);

    int x_offest = 10;
    int y_offset = 45;

    GFXcanvas16 list_view(200, 210);

    list_view.fillScreen(COLOUR_WHITE);

    list_view.fillRect(0, selected_app * 30, 200, 30, 14);
    list_view.drawRect(0, selected_app * 30, 200, 30, COLOUR_BLACK);

    uint16_t scroll_offset = 0;

    for (size_t i = 0; i < apps_count; i++) {
        list_view.drawBitmap(5, 5 + (i * 30), icon_small, 20, 20, COLOUR_BLACK, COLOUR_WHITE);
        list_view.drawRect(5, 5 + (i * 30), 20, 20, COLOUR_BLACK);

        if(i == selected_app) list_view.drawRect(4, 4 + (i * 30), 22, 22, COLOUR_BLACK);

        draw_text_left(&list_view, apps[i].name.c_str(), 30, 20 + (i * 30), COLOUR_BLACK, i == selected_app ? &FreeSansBold9pt7b : &FreeSans9pt7b);
    }

    gfx->drawRGBBitmap(10, 40, list_view.getBuffer(), list_view.width(), list_view.height());
}

void draw_app_info(App* app) {
    // TODO: Draw Icon
    gfx->drawBitmap(SCREEN_WIDTH - 145, 40, icon_large, 110, 110, COLOUR_BLACK, COLOUR_WHITE);
    gfx->drawRect(SCREEN_WIDTH - 145, 40, 110, 110, COLOUR_BLACK);

    draw_text_center(gfx, app->name.c_str(), SCREEN_WIDTH - 92, 170, COLOUR_BLACK, &FreeSansBold9pt7b);
    draw_text_center(gfx, (String("/mnt/sd") + app->path).c_str(), SCREEN_WIDTH - 92, 190, COLOUR_DARK_GREY, &FreeSansBoldOblique9pt7b);
    draw_text_center(gfx, String(formatBytes(app->flash_size) + " flash").c_str(), SCREEN_WIDTH - 92, 210, COLOUR_DARK_GREY, &FreeSansBoldOblique9pt7b);
    draw_text_center(gfx, String(formatBytes(app->sd_size) + " sd").c_str(), SCREEN_WIDTH - 92, 230, COLOUR_DARK_GREY, &FreeSansBoldOblique9pt7b);
}

void draw_text_center(Adafruit_GFX *context, const char* text, uint16_t x, uint16_t y, uint16_t colour, const GFXfont* font, uint8_t size) {
    context->setTextSize(size);
    context->setFont(font);
    context->setTextColor(colour);

    int16_t x1, y1;
    uint16_t w, h;
    context->getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

    context->setCursor(x - w / 2, y);
    context->println(text);
}

void draw_text_left(Adafruit_GFX *context, const char* text, uint16_t x, uint16_t y, uint16_t colour, const GFXfont* font, uint8_t size) {
    context->setTextSize(size);
    context->setFont(font);
    context->setTextColor(colour);

    int16_t x1, y1;
    uint16_t w, h;
    context->getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

    context->setCursor(x, y);
    context->println(text);
}

void draw_text_right(Adafruit_GFX *context, const char* text, uint16_t x, uint16_t y, uint16_t colour, const GFXfont* font, uint8_t size) {
    context->setTextSize(size);
    context->setFont(font);
    context->setTextColor(colour);

    int16_t x1, y1;
    uint16_t w, h;
    context->getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

    context->setCursor(x - w, y);
    context->println(text);
}

void draw_gradient_rect(Adafruit_GFX *context, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t colour_from, uint16_t colour_to) {
    for (size_t i = 0; i < w; i++) {
        context->drawLine(i + x, y, i + x, h, map(i, 0, w, colour_from, colour_to));
    }
}

String formatBytes(uint64_t bytes) {
    char output[32];
    if (bytes >= 1073741824ULL)
        snprintf(output, sizeof(output), "%.2f GB", bytes / 1073741824.0);
    else if (bytes >= 1048576)
        snprintf(output, sizeof(output), "%.2f MB", bytes / 1048576.0);
    else if (bytes >= 1024)
        snprintf(output, sizeof(output), "%.2f KB", bytes / 1024.0);
    else
        snprintf(output, sizeof(output), "%llu B", bytes);
    return String(output);
  }

void initialize_keyboard() {
    pinMode(TDECK_POWERON, OUTPUT);
    digitalWrite(TDECK_POWERON, HIGH);

    pinMode(TDECK_TRACKBALL_UP, INPUT_PULLUP);
    pinMode(TDECK_TRACKBALL_DOWN, INPUT_PULLUP);
    pinMode(TDECK_TRACKBALL_LEFT, INPUT_PULLUP);
    pinMode(TDECK_TRACKBALL_RIGHT, INPUT_PULLUP);
    pinMode(TDECK_TRACKBALL_CLICK, INPUT_PULLUP);

    attachInterrupt(TDECK_TRACKBALL_UP, []() {
        if (selected_app - 1 < 0) return;

        selected_app -= 1;
    }, FALLING);

    attachInterrupt(TDECK_TRACKBALL_DOWN, []() {
        if (selected_app + 1 >= apps_count) return;

        selected_app += 1;
    }, FALLING);

    attachInterrupt(TDECK_TRACKBALL_LEFT, []() {

    }, FALLING);

    attachInterrupt(TDECK_TRACKBALL_RIGHT, []() {

    }, FALLING);

    attachInterrupt(TDECK_TRACKBALL_CLICK, []() {
        
    }, FALLING);

    delay(500); // Wait for the keyboard to power on

    Wire.begin(TDECK_I2C_SDA, TDECK_I2C_SCL);
    Wire.requestFrom(TDECK_KEYBOARD_ADDR, 1);
    if (Wire.read() == -1) {
        Serial.println("Keyboard failed to start.");

        while (true) {
            delay(1000);
        }
    }
}

// void initialize_partitions() {
//     esp_partition_iterator_t iterator = NULL;
//     const esp_partition_t *next_partition = NULL;

//     iterator = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);

//     int index = 0;
//     while (iterator) {
//         next_partition = esp_partition_get(iterator);

//         if (next_partition != NULL && index != 0 && index <= MAX_APPS) {
//             int app_index = index - 1;

//             App newApp;
//             // newApp.id = app_index;
//             newApp.partition = next_partition;

//             apps[app_index] = newApp;
//             apps_count = app_index;
//         }

//         index += 1;
//         iterator = esp_partition_next(iterator);
//     }
// }

int start_app(const esp_partition_t* partition) {
    int error = esp_ota_set_boot_partition(partition);
    
    if (error != ESP_OK) return error;

    Wire.end();
    esp_restart();

    return ESP_OK;
}
