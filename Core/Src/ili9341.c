#include "ili9341.h"
#include "main.h"
#include "ili9341_drivers.h"
#include "nmea.h"




void ILI9341_DrawChar(char ch,
                      const uint8_t font[],
                      uint16_t X,
                      uint16_t Y,
                      uint16_t color)
{
    if (ch < 32 || ch > 127) return;

    uint8_t fOffset = font[0];
    uint8_t fWidth  = font[1];
    uint8_t fHeight = font[2];
    uint8_t fBPL    = font[3];

    /* Pointer to current character data */
    const uint8_t *charData =
        &font[(ch - 0x20) * fOffset + 4];

    for (uint8_t y = 0; y < fHeight; y++)
    {
        for (uint8_t x = 0; x < fWidth; x++)
        {
            uint8_t byte =
                charData[fBPL * x + (y >> 3) + 1];
            uint8_t bit = 1 << (y & 0x07);

            if (byte & bit)
            {
                ILI9341_DrawPixel(X + x, Y + y, color);
            }
        }
    }
}

void ILI9341_ClearRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if (w == 0 || h == 0) return;

    Tile_Fill(color);

    uint16_t y_end = y + h;
    uint16_t x_end = x + w;

    for (uint16_t yy = y; yy < y_end; yy += TILE_H)
    {
        uint16_t th = (yy + TILE_H <= y_end) ? TILE_H : (y_end - yy);

        for (uint16_t xx = x; xx < x_end; xx += TILE_W)
        {
            uint16_t tw = (xx + TILE_W <= x_end) ? TILE_W : (x_end - xx);

            ILI9341_Set_Window(xx, yy, xx + tw - 1, yy + th - 1);

            uint32_t bytes = (uint32_t)tw * th * 2;

            ILI9341_WriteBufferDMA((uint8_t*)tile_buf, bytes);
        }
    }
}

void ILI9341_DrawTextClean(uint16_t x,  uint16_t y, const char *text, const uint8_t font[], uint16_t color, uint16_t bg)
{
    uint16_t w = strlen(text) * FONT_WIDTH(font);
    uint16_t h = FONT_HEIGHT(font) + 2;

    ILI9341_ClearRect(x, y, w, h, bg);
    ILI9341_DrawText(text, font, x, y, color);
}


void ILI9341_DrawText(const char* str,
                      const uint8_t font[],
                      uint16_t X,
                      uint16_t Y,
                      uint16_t color)
{
    uint8_t fOffset = font[0];
    uint8_t fWidth  = font[1];

    while (*str)
    {
        ILI9341_DrawChar(*str, font, X, Y, color);

        uint8_t *tempChar =
            (uint8_t*)&font[((*str - 0x20) * fOffset) + 4];
        uint8_t charWidth = tempChar[0];

        uint16_t step = (charWidth + 2 < fWidth)
                        ? (charWidth + 2)
                        : fWidth;

        if (!lcd_is_landscape)
        {
            X += step;
        }
        else
        {
            Y += step;
        }

        str++;

    }
}

uint32_t device_id(void) {

    uint32_t *uid = (uint32_t*)UID_BASE;
    uint32_t id = uid[0] ^ uid[1] ^ uid[2];
    return id;

}




