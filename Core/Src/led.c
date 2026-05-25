#include "led.h"
#include "main.h"
#include "stm32f1xx_hal_gpio.h"
#include <sys/types.h>

// 对于此项目(FinalEXP)，无需使用 LED12 ~ LED15，因此相关代码已加上注释，避免误操作导致不必要的麻烦

// 输入数字控制流水灯状态
void
LED_Flow(uint8_t state)
{
    // 共 16 个 LED

    // 清空 LED 状态
    HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED6_GPIO_Port, LED6_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED7_GPIO_Port, LED7_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED8_GPIO_Port, LED8_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED9_GPIO_Port, LED9_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED10_GPIO_Port, LED10_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED11_GPIO_Port, LED11_Pin, GPIO_PIN_SET);
    // HAL_GPIO_WritePin(LED12_GPIO_Port, LED12_Pin, GPIO_PIN_SET);
    // HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, GPIO_PIN_SET);
    // HAL_GPIO_WritePin(LED14_GPIO_Port, LED14_Pin, GPIO_PIN_SET);
    // HAL_GPIO_WritePin(LED15_GPIO_Port, LED15_Pin, GPIO_PIN_SET);

    // 根据输入状态点亮对应 LED，如 state = 0，则点亮 LED0 到 LED7
    if(state >= 0 && state < 12)
    {
        uint8_t counter = 0;
        while(counter < 8)
        {
            switch((state + counter) % 12)
            {
                case 0:
                    HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
                    break;
                case 1:
                    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
                    break;
                case 2:
                    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
                    break;
                case 3:
                    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
                    break;
                case 4:
                    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET);
                    break;
                case 5:
                    HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, GPIO_PIN_RESET);
                    break;
                case 6:
                    HAL_GPIO_WritePin(LED6_GPIO_Port, LED6_Pin, GPIO_PIN_RESET);
                    break;
                case 7:
                    HAL_GPIO_WritePin(LED7_GPIO_Port, LED7_Pin, GPIO_PIN_RESET);
                    break;
                case 8:
                    HAL_GPIO_WritePin(LED8_GPIO_Port, LED8_Pin, GPIO_PIN_RESET);
                    break;
                case 9:
                    HAL_GPIO_WritePin(LED9_GPIO_Port, LED9_Pin, GPIO_PIN_RESET);
                    break;
                case 10:
                    HAL_GPIO_WritePin(LED10_GPIO_Port, LED10_Pin, GPIO_PIN_RESET);
                    break;
                case 11:
                    HAL_GPIO_WritePin(LED11_GPIO_Port, LED11_Pin, GPIO_PIN_RESET);
                    break;
                    // case 12:
                    //     HAL_GPIO_WritePin(LED12_GPIO_Port, LED12_Pin, GPIO_PIN_RESET);
                    //     break;
                    // case 13:
                    //     HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, GPIO_PIN_RESET);
                    //     break;
                    // case 14:
                    //     HAL_GPIO_WritePin(LED14_GPIO_Port, LED14_Pin, GPIO_PIN_RESET);
                    //     break;
                    // case 15:
                    //     HAL_GPIO_WritePin(LED15_GPIO_Port, LED15_Pin, GPIO_PIN_RESET);
                    //     break;
            }
            counter++;
        }
    }
}

// 输入数字，以二进制形式显示到 LED0 ~ LED15 上
void
LED_Binary_Display(uint16_t value)
{
    for(int i = 0; i < 12; i++)
    {
        if(value & (1 << i))
        {
            // 点亮对应 LED
            switch(i)
            {
                case 0:
                    HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
                    break;
                case 1:
                    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
                    break;
                case 2:
                    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
                    break;
                case 3:
                    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
                    break;
                case 4:
                    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET);
                    break;
                case 5:
                    HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, GPIO_PIN_RESET);
                    break;
                case 6:
                    HAL_GPIO_WritePin(LED6_GPIO_Port, LED6_Pin, GPIO_PIN_RESET);
                    break;
                case 7:
                    HAL_GPIO_WritePin(LED7_GPIO_Port, LED7_Pin, GPIO_PIN_RESET);
                    break;
                case 8:
                    HAL_GPIO_WritePin(LED8_GPIO_Port, LED8_Pin, GPIO_PIN_RESET);
                    break;
                case 9:
                    HAL_GPIO_WritePin(LED9_GPIO_Port, LED9_Pin, GPIO_PIN_RESET);
                    break;
                case 10:
                    HAL_GPIO_WritePin(LED10_GPIO_Port, LED10_Pin, GPIO_PIN_RESET);
                    break;
                case 11:
                    HAL_GPIO_WritePin(LED11_GPIO_Port, LED11_Pin, GPIO_PIN_RESET);
                    break;
                    // case 12:
                    //     HAL_GPIO_WritePin(LED12_GPIO_Port, LED12_Pin, GPIO_PIN_RESET);
                    //     break;
                    // case 13:
                    //     HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, GPIO_PIN_RESET);
                    //     break;
                    // case 14:
                    //     HAL_GPIO_WritePin(LED14_GPIO_Port, LED14_Pin, GPIO_PIN_RESET);
                    //     break;
                    // case 15:
                    //     HAL_GPIO_WritePin(LED15_GPIO_Port, LED15_Pin, GPIO_PIN_RESET);
                    //     break;
            }
        }
        else
        {
            // 熄灭对应 LED
            switch(i)
            {
                case 0:
                    HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
                    break;
                case 1:
                    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
                    break;
                case 2:
                    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
                    break;
                case 3:
                    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
                    break;
                case 4:
                    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);
                    break;
                case 5:
                    HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, GPIO_PIN_SET);
                    break;
                case 6:
                    HAL_GPIO_WritePin(LED6_GPIO_Port, LED6_Pin, GPIO_PIN_SET);
                    break;
                case 7:
                    HAL_GPIO_WritePin(LED7_GPIO_Port, LED7_Pin, GPIO_PIN_SET);
                    break;
                case 8:
                    HAL_GPIO_WritePin(LED8_GPIO_Port, LED8_Pin, GPIO_PIN_SET);
                    break;
                case 9:
                    HAL_GPIO_WritePin(LED9_GPIO_Port, LED9_Pin, GPIO_PIN_SET);
                    break;
                case 10:
                    HAL_GPIO_WritePin(LED10_GPIO_Port, LED10_Pin, GPIO_PIN_SET);
                    break;
                case 11:
                    HAL_GPIO_WritePin(LED11_GPIO_Port, LED11_Pin, GPIO_PIN_SET);
                    break;
                    // case 12:
                    //     HAL_GPIO_WritePin(LED12_GPIO_Port, LED12_Pin, GPIO_PIN_SET);
                    //     break;
                    // case 13:
                    //     HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, GPIO_PIN_SET);
                    //     break;
                    // case 14:
                    //     HAL_GPIO_WritePin(LED14_GPIO_Port, LED14_Pin, GPIO_PIN_SET);
                    //     break;
                    // case 15:
                    //     HAL_GPIO_WritePin(LED15_GPIO_Port, LED15_Pin, GPIO_PIN_SET);
                    //     break;
            }
        }
    }
}

// 熄灭所有 LED
void
LED_Turn_off_All(void)
{
    LED_Binary_Display(0);
}