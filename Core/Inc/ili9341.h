#ifndef INC_ILI9341_H_
#define INC_ILI9341_H_

#include "fonts.h"
#include <string.h>
#include "nmea.h"


#define HORIZONTAL_IMAGE	0
#define VERTICAL_IMAGE		1

#define FONT_WIDTH(font)   ((font)[1])
#define FONT_HEIGHT(font)  ((font)[2])

void ILI9341_DrawHollowCircle(uint16_t X, uint16_t Y, uint16_t radius, uint16_t color);
void ILI9341_DrawFilledCircle(uint16_t X, uint16_t Y, uint16_t radius, uint16_t color);
void ILI9341_DrawHollowRectangleCoord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t color);
void ILI9341_DrawFilledRectangleCoord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t color);
void ILI9341_DrawChar(char ch, const uint8_t font[], uint16_t X, uint16_t Y, uint16_t color);
void ILI9341_DrawText(const char* str, const uint8_t font[], uint16_t X, uint16_t Y, uint16_t color);
void ILI9341_DrawImage(const uint8_t* image, uint8_t orientation);
void ILI9341_ClearRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ILI9341_DrawTextClean(uint16_t x,  uint16_t y, const char *text, const uint8_t font[], uint16_t color, uint16_t bg);
void spi_la_demo(void);
uint32_t device_id(void);
extern uint8_t lcd_is_landscape;

#endif /* INC_ILI9341_H_ */
