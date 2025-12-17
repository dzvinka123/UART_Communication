#include "ili9341_drivers.h"
#include "main.h"

volatile uint16_t LCD_HEIGHT = ILI9341_SCREEN_HEIGHT;
volatile uint16_t LCD_WIDTH	 = ILI9341_SCREEN_WIDTH;
uint8_t lcd_is_landscape = 0;
uint16_t tile_buf[TILE_W * TILE_H];
volatile uint8_t lcd_dma_busy = 0;

static inline void DelayUs(uint32_t us)
{
    uint32_t cycles = us * (SystemCoreClock / 1000000);
    while(cycles--);
}


void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* Deselect when Tx Complete */
  if(hspi == HSPI_INSTANCE)
  {
	  HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
	  lcd_dma_busy = 0;
  }
}

void ILI9341_WriteBufferDMA(const uint8_t *buf, uint32_t len)
{
    while (lcd_dma_busy) { }
    lcd_dma_busy = 1;

    HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);   // DATA
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET); // CS ↓
    DelayUs(1);

    HAL_SPI_Transmit_DMA(HSPI_INSTANCE, (uint8_t*)buf, len);
}

void Tile_Fill(uint16_t color)
{
    for (uint32_t i = 0; i < TILE_W * TILE_H; i++)
        tile_buf[i] = (color >> 8) | (color << 8);
}


void ILI9341_DrawTile(uint16_t x, uint16_t y)
{
    ILI9341_Set_Window(
        x,
        y,
        x + TILE_W - 1,
        y + TILE_H - 1
    );

    HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);

    HAL_SPI_Transmit_DMA(
        HSPI_INSTANCE,
        (uint8_t*)tile_buf,
        TILE_W * TILE_H * 2
    );
}


void ILI9341_WriteCommand(uint8_t cmd)
{
	HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_RESET);	//command
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);	// CS ↓
	DelayUs(1);
    HAL_SPI_Transmit(HSPI_INSTANCE, &cmd, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET); // CS ↑
    DelayUs(1);
}

void ILI9341_WriteData(uint8_t *buff, size_t buff_size)
{
	HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);	//data
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);	//select
	DelayUs(1);
    HAL_SPI_Transmit(HSPI_INSTANCE, buff, buff_size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET); // CS -> High
	DelayUs(1);
}


void ILI9341_Set_Window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    if (x2 >= LCD_WIDTH)  x2 = LCD_WIDTH - 1;
    if (y2 >= LCD_HEIGHT) y2 = LCD_HEIGHT - 1;

    uint8_t buffer[4];
    // Column address set (0x2A)
    buffer[0] = x1 >> 8; buffer[1] = x1 & 0xFF;
    buffer[2] = x2 >> 8; buffer[3] = x2 & 0xFF;
    ILI9341_WriteCommand(0x2A);
    ILI9341_WriteData(buffer, 4);

    // Page address set (0x2B)
    buffer[0] = y1 >> 8; buffer[1] = y1 & 0xFF;
    buffer[2] = y2 >> 8; buffer[3] = y2 & 0xFF;
    ILI9341_WriteCommand(0x2B);
    ILI9341_WriteData(buffer, 4);

    // Memory write (0x2C)
    ILI9341_WriteCommand(0x2C);
}


void ILI9341_Reset(void)
{
	  HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_RESET);
	  HAL_Delay(20);
	  HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_SET);
	  HAL_Delay(20);
}

void ILI9341_Init(void)
{
	ILI9341_Reset();

	//SOFTWARE RESET
	ILI9341_WriteCommand(0x01);
	HAL_Delay(10);

	//POWER CONTROL A
	ILI9341_WriteCommand(0xCB);
	uint8_t data_cba[] = {0x39, 0x2C, 0x00, 0x34, 0x02};
	ILI9341_WriteData(data_cba, sizeof(data_cba));

	//POWER CONTROL B
	ILI9341_WriteCommand(0xCF);
    uint8_t data_cf[] = {0x00, 0xC1, 0x30};
	ILI9341_WriteData(data_cf, sizeof(data_cba));


	//DRIVER TIMING CONTROL A
	ILI9341_WriteCommand(0xE8);
    uint8_t data_e8[] = {0x85, 0x00, 0x78};
	ILI9341_WriteData(data_e8, sizeof(data_e8));

	//DRIVER TIMING CONTROL B
	ILI9341_WriteCommand(0xEA);
    uint8_t data_ea[] = {0x00, 0x00};
	ILI9341_WriteData(data_ea, sizeof(data_ea));

	//POWER ON SEQUENCE CONTROL
	ILI9341_WriteCommand(0xED);
    uint8_t data_ed[] = {0x64, 0x03, 0x12, 0x81};
	ILI9341_WriteData(data_ed, sizeof(data_ed));

	//PUMP RATIO CONTROL
	ILI9341_WriteCommand(0xF7);
    uint8_t data_f7[] = {0x20};
	ILI9341_WriteData(data_f7, sizeof(data_f7));

	//POWER CONTROL,VRH[5:0]
	ILI9341_WriteCommand(0xC0);
	uint8_t data_c0[] = {0x23};
	ILI9341_WriteData(data_c0, sizeof(data_c0));

	//POWER CONTROL,SAP[2:0];BT[3:0]
	ILI9341_WriteCommand(0xC1);
	uint8_t data_c1[] = {0x10};
	ILI9341_WriteData(data_c1, sizeof(data_c1));

	//VCM CONTROL
	ILI9341_WriteCommand(0xC5);
    uint8_t data_c5[] = {0x3E, 0x28};
    ILI9341_WriteData(data_c5, sizeof(data_c5));

	//VCM CONTROL 2
	ILI9341_WriteCommand(0xC7);
    uint8_t data_c7[] = {0x86};
    ILI9341_WriteData(data_c7, sizeof(data_c7));

	//MEMORY ACCESS CONTROL
	ILI9341_WriteCommand(0x36);
    uint8_t data_36[] = {0x48};
    ILI9341_WriteData(data_36, sizeof(data_36));

	//PIXEL FORMAT
	ILI9341_WriteCommand(0x3A);
    uint8_t data_3a[] = {0x55};
    ILI9341_WriteData(data_3a, sizeof(data_3a));

	//FRAME RATIO CONTROL, STANDARD RGB COLOR
	ILI9341_WriteCommand(0xB1);
    uint8_t data_b1[] = {0x00, 0x18};
    ILI9341_WriteData(data_b1, sizeof(data_b1));

	//DISPLAY FUNCTION CONTROL
	ILI9341_WriteCommand(0xB6);
    uint8_t data_b6[] = {0x08, 0xA2, 0x27, 0x00};
    ILI9341_WriteData(data_b6, sizeof(data_b6));

	//3GAMMA FUNCTION DISABLE
	ILI9341_WriteCommand(0xF2);
    uint8_t data_f2[] = {0x00};
    ILI9341_WriteData(data_f2, sizeof(data_f2));

	//GAMMA CURVE SELECTED
	ILI9341_WriteCommand(0x26);
    uint8_t data_26[] = {0x01};
    ILI9341_WriteData(data_26, sizeof(data_26));

	//EXIT SLEEP
	ILI9341_WriteCommand(0x11);
	HAL_Delay(100);

	//TURN ON DISPLAY
	ILI9341_WriteCommand(0x29);
	HAL_Delay(20);

	//STARTING ROTATION
	ILI9341_SetRotation(SCREEN_VERTICAL_1);
	HAL_Delay(20);
}


void ILI9341_SetRotation(uint8_t rotation)
{
    uint8_t madctl = ILI9341_MADCTL_BGR;

    switch(rotation)
    {
    case SCREEN_VERTICAL_1:
        madctl = ILI9341_MADCTL_BGR;
        LCD_WIDTH  = 240;
        LCD_HEIGHT = 320;
        break;

    case SCREEN_HORIZONTAL_1:
        madctl = ILI9341_MADCTL_MX | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR;
        LCD_WIDTH  = 320;
        LCD_HEIGHT = 240;
        break;

    case SCREEN_VERTICAL_2:
        madctl = ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR;
        LCD_WIDTH  = 240;
        LCD_HEIGHT = 320;
        break;

    case SCREEN_HORIZONTAL_2:
        madctl = ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR;
        LCD_WIDTH  = 320;
        LCD_HEIGHT = 240;
        break;
    }

    ILI9341_WriteCommand(0x36);
    ILI9341_WriteData(&madctl, 1);

    ILI9341_Set_Window(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
}


void ILI9341_DrawColor(uint16_t color)
{
	uint8_t buffer[2] = {color>>8, color};
	ILI9341_WriteData(buffer, sizeof(buffer));
}

void ILI9341_DrawColorBurst(uint16_t color, uint32_t size)
{
	uint32_t BufferSize = 0;

	if((size*2) < BURST_MAX_SIZE)
	{
		BufferSize = size;
	}
	else
	{
		BufferSize = BURST_MAX_SIZE;
	}

	HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);
    DelayUs(1);

	uint8_t chifted = color>>8;
	uint8_t BurstBuffer[BufferSize];

	for(uint32_t j = 0; j < BufferSize; j+=2)
	{
		BurstBuffer[j] = chifted;
		BurstBuffer[j+1] = color;
	}

	uint32_t SendingSize = size * 2;
	uint32_t SendingInBlock = SendingSize / BufferSize;
	uint32_t RemainderFromBlock = SendingSize % BufferSize;

	if(SendingInBlock != 0)
	{
		for(uint32_t j = 0; j < (SendingInBlock); j++)
		{
			HAL_SPI_Transmit(HSPI_INSTANCE, BurstBuffer, BufferSize, 10);
		}
	}

	HAL_SPI_Transmit(HSPI_INSTANCE, BurstBuffer, RemainderFromBlock, 10);
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);
    DelayUs(1);
}

void ILI9341_FillScreen(uint16_t color)
{
	ILI9341_Set_Window(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    ILI9341_DrawColorBurst(color, LCD_WIDTH * LCD_HEIGHT);
}

void ILI9341_DrawPixel(uint16_t x,uint16_t y,uint16_t color)
{
	if((x >=LCD_WIDTH) || (y >=LCD_HEIGHT)) return;

	uint8_t bufferX[4] = {x>>8, x, (x+1)>>8, (x+1)};
	uint8_t bufferY[4] = {y>>8, y, (y+1)>>8, (y+1)};
	uint8_t bufferC[2] = {color>>8, color};

	ILI9341_WriteCommand(0x2A);						//ADDRESS
	ILI9341_WriteData(bufferX, sizeof(bufferX));	//XDATA

	ILI9341_WriteCommand(0x2B);						//ADDRESS
	ILI9341_WriteData(bufferY, sizeof(bufferY));	//YDATA

	ILI9341_WriteCommand(0x2C);						//ADDRESS
	ILI9341_WriteData(bufferC, sizeof(bufferC));	//COLOR
}


