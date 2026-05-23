
/**************************************************/
/*     LCD12864 Drive based on software SPI       */
/* RS  - CS       - Device Select - Currently PA4 */
/* RW  - SID/MOSI - Data          - Currently PA5 */
/* E   - CLK      - Clock         - Currently PA6 */
/* RST - RST      - Reset         - Currently PA7 */
/**************************************************/

#include "LCD12864.h" // My header file
#include "DelayUs.h"
#include "cmsis_os.h" // FreeRTOS
#include "cmsis_os2.h"

static void
LCD_DelayMs(uint32_t ms)
{
    if(osKernelGetState() == osKernelRunning)
        osDelay(ms);
    else
        HAL_Delay(ms);
}

/***************************************************************************************************/
/* BASIC */

/*
/ @ Description: Send a single byte using software SPI
/ @ Parameter:
/ @ Return Value:
*/

void
LCD_SendByte(uint8_t byte)
{
    for(int i = 0; i < 8; i++)
    {
        if((byte << i) & 0x80)
            HAL_GPIO_WritePin(LCD12864_MOSI_GPIO_Port, LCD12864_MOSI_Pin, GPIO_PIN_SET);
        else
            HAL_GPIO_WritePin(LCD12864_MOSI_GPIO_Port, LCD12864_MOSI_Pin, GPIO_PIN_RESET);

        HAL_GPIO_WritePin(LCD12864_CLK_GPIO_Port, LCD12864_CLK_Pin, GPIO_PIN_SET);
        // clock high - bit start
        HAL_GPIO_WritePin(LCD12864_CLK_GPIO_Port, LCD12864_CLK_Pin, GPIO_PIN_RESET);
        // clock low - bit end
    }
    // a byte contains 8 bits so the loop runs 8 times
    // byte << i move current bit to the highest bit
    // compare this bit with 0x80, or 1000 0000
    // if output 1 set MOSI high cause we want to send 1
}

/*
/ @ Description: Send a command / data using software SPI
/ @ Parameter:
/ @ Return Value:
*/

void
LCD_SendCommand(uint8_t cmd)
{
    HAL_GPIO_WritePin(LCD12864_CS_GPIO_Port, LCD12864_CS_Pin, GPIO_PIN_SET);
    // set CS high - start sending command / data

    LCD_SendByte(SYNCCMD);
    // start synchrnozing and set to command mode

    LCD_SendByte(cmd & 0xf0);
    // send higher bits

    LCD_SendByte((cmd << 4) & 0xf0);
    // move lower bits to the higher bit
    // send lower bits

    Delay_us(50);
    // wait for LCD to process

    HAL_GPIO_WritePin(LCD12864_CS_GPIO_Port, LCD12864_CS_Pin, GPIO_PIN_RESET);
    // set CS high - stop  sending command / data
}

void
LCD_SendData(uint8_t data)
{
    HAL_GPIO_WritePin(LCD12864_CS_GPIO_Port, LCD12864_CS_Pin, GPIO_PIN_SET);

    LCD_SendByte(SYNCDATA);
    // start synchrnozing and set to data mode

    LCD_SendByte(data & 0xf0);
    LCD_SendByte((data << 4) & 0xf0);

    Delay_us(50);

    HAL_GPIO_WritePin(LCD12864_CS_GPIO_Port, LCD12864_CS_Pin, GPIO_PIN_RESET);
}

/***************************************************************************************************/
/* APPLICATION */

/*
/ @ Description: Get LCD ready to work
/ @ Parameter:
/ @ Return Value:
*/

void
LCD_Init(void)
{
    HAL_GPIO_WritePin(LCD12864_RST_GPIO_Port, LCD12864_RST_Pin, GPIO_PIN_RESET); // RESET=0
    LCD_DelayMs(10);                                                             // wait for 10ms
    HAL_GPIO_WritePin(LCD12864_RST_GPIO_Port, LCD12864_RST_Pin, GPIO_PIN_SET);   // RESET=1

    LCD_DelayMs(50); // wait for >40 ms

    LCD_SendCommand(0x30); // 8bit mode
    Delay_us(110);         //  >100us delay

    LCD_SendCommand(0x30); // 8bit mode
    Delay_us(40);          // >37us delay

    LCD_SendCommand(0x08); // D=0, C=0, B=0
    Delay_us(110);         // >100us delay

    LCD_SendCommand(0x01); // clear screen
    LCD_DelayMs(12);       // >10 ms delay

    LCD_SendCommand(0x06); // cursor increment right no shift
    LCD_DelayMs(1);        // 1ms delay

    LCD_SendCommand(0x0C); // D=1, C=0, B=0
    LCD_DelayMs(1);        // 1ms delay

    LCD_SendCommand(0x02); // return to home
    LCD_DelayMs(1);        // 1ms delay
}

/*
/ @ Description: Full screen clear
/ @ Parameter:
/ @ Return Value:
*/
void
LCD_Clear()
{
    LCD_SendCommand(CLEAR);
    LCD_DelayMs(1); // wait for the clear command to finish
}

/*
/ @ Description: Display a char / string on (row,col)
/ @ Parameter:
/ @ Return Value:
*/

void
LCD_DispChar(uint8_t row, uint8_t col, char ch)
{
    uint16_t addr = row + (col - 1);
    LCD_SendCommand(addr);
    LCD_SendData(ch);
}

void
LCD_DispString(uint8_t row, uint8_t col, const char *str)
{
    uint8_t written = 0;

    if(col < COL1 || col > COL16)
        return;

    uint16_t addr = row + (col - 1);
    LCD_SendCommand(addr);

    while(*str && (uint8_t)(col - 1 + written) < COL16)
    {
        LCD_SendData((uint8_t)*str++);
        written++;
    }

    while((uint8_t)(col - 1 + written) < COL16)
    {
        LCD_SendData(' ');
        written++;
    }
}

/***************************************************************************************************/
/* ADVANCED */

/*
/ @ Description: Display Chinese
/ @ Parameter:
/ @ Return Value:
/ @ Notes: Only if your LCD12846 has a Chinese Character Font Lib
*/

bool
isEven(uint8_t num)
{
    if(num % 2 == 0)
        return true;
    else
        return false;
}

void
LCD_DispCN(uint8_t row, uint8_t col, char *str)
{
    for(int i = 0; i < strlen(str) - 1; i += 2)
    {
        if(i < 14)
        {
            uint16_t new_col = (col - 1 + (i / 2));
            uint16_t addr = (row) + new_col;
            ;
            LCD_SendCommand(addr);
            LCD_SendData(str[i]);
            LCD_SendData(str[i + 1]);
        }
    }
}
