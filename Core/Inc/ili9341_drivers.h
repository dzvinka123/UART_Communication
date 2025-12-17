#ifndef INC_ILI9341_DRIVERS_H_
#define INC_ILI9341_DRIVERS_H_



#include <stdint.h>
#include "stm32f4xx_hal.h"

extern SPI_HandleTypeDef hspi2;

#define ILI9341_SCREEN_WIDTH   240
#define ILI9341_SCREEN_HEIGHT  320

#define TILE_W  80
#define TILE_H  16


/* PIN Configuration */
#define HSPI_INSTANCE			&hspi2
#define LCD_CS_PORT				GPIOB
#define LCD_CS_PIN				GPIO_PIN_12
#define LCD_DC_PORT				GPIOA
#define LCD_DC_PIN				GPIO_PIN_3
#define	LCD_RST_PORT			GPIOA
#define	LCD_RST_PIN				GPIO_PIN_2

#define BURST_MAX_SIZE 			500
#define BLACK      				0x0000
#define NAVY        			0x000F
#define DARKGREEN   			0x03E0
#define DARKCYAN    			0x03EF
#define MAROON      			0x7800
#define PURPLE      			0x780F
#define OLIVE       			0x7BE0
#define LIGHTGREY   			0xC618
#define DARKGREY    			0x7BEF
#define BLUE        			0x001F
#define GREEN       			0x07E0
#define CYAN        			0x07FF
#define RED         			0xF800
#define MAGENTA     			0xF81F
#define YELLOW      			0xFFE0
#define WHITE       			0xFFFF
#define ORANGE      			0xFD20
#define GREENYELLOW 			0xAFE5
#define PINK        			0xF81F

#define SCREEN_VERTICAL_1		0
#define SCREEN_HORIZONTAL_1		1
#define SCREEN_VERTICAL_2		2
#define SCREEN_HORIZONTAL_2		3

#define ILI9341_MADCTL_MY   0x80
#define ILI9341_MADCTL_MX   0x40
#define ILI9341_MADCTL_MV   0x20
#define ILI9341_MADCTL_BGR  0x08



static inline void DelayUs(uint32_t us);
void ILI9341_WriteCommand(uint8_t cmd);
void ILI9341_WriteData(uint8_t *buff, size_t buff_size);
//void ILI9341_WriteBuffer(uint8_t *buffer, uint16_t len);
void ILI9341_Reset(void);
void ILI9341_Enable(void);
void ILI9341_Init(void);
void ILI9341_SetRotation(uint8_t rotation);
void ILI9341_Set_Window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void ILI9341_DrawColor(uint16_t color);
void ILI9341_DrawColorBurst(uint16_t color, uint32_t size);
void ILI9341_FillScreen(uint16_t color);
void ILI9341_DrawPixel(uint16_t x,uint16_t y,uint16_t color);

void ILI9341_DrawTile(uint16_t x, uint16_t y);
void Tile_Fill(uint16_t color);
void ILI9341_WriteBufferDMA(const uint8_t *buf, uint32_t len);


extern uint16_t tile_buf[TILE_W * TILE_H];
extern volatile uint16_t LCD_HEIGHT;
extern volatile uint16_t LCD_WIDTH;

#endif /* INC_ILI9341_DRIVERS_H_ */
