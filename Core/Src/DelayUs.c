#include "DelayUs.h"

#include "tim.h"

void
Delay_Init(void)
{
    HAL_TIM_Base_Start(&htim7);
}

void
Delay_us(uint16_t nus)
{
    uint16_t start = __HAL_TIM_GET_COUNTER(&htim7);
    uint16_t curr;

    while(1)
    {
        curr = __HAL_TIM_GET_COUNTER(&htim7);

        if((uint16_t)(curr - start) >= nus)
            break;
    }
}