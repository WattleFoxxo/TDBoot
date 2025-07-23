#include <Adafruit_SharpMem.h>

const uint8_t bayer4x4[4][4] = {
    {  0,  8,  2, 10 },
    { 12,  4, 14,  6 },
    {  3, 11,  1,  9 },
    { 15,  7, 13,  5 }
};
  
class Adafruit_SharpMem_Dither : public Adafruit_SharpMem {
    public:
        Adafruit_SharpMem_Dither(
            uint8_t clk,
            uint8_t mosi,
            uint8_t cs,
            uint16_t w,
            uint16_t h,
            uint32_t freq = 2000000
        ): Adafruit_SharpMem(clk, mosi, cs, w, h, freq) {}
    
        void drawPixel(int16_t x, int16_t y, uint16_t color) override {
            if (x < 0 || y < 0 || x >= width() || y >= height()) return;
    
            uint8_t threshold = bayer4x4[y % 4][x % 4];
            uint8_t onebit_color = color > threshold;

            if (color == 0 || color == 15) onebit_color = color; // fix solid colours not being solid

            Adafruit_SharpMem::drawPixel(x, y, onebit_color);
        }
};