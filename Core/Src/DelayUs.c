#include "DelayUs.h"

#include "tim.h"

void
Delay_Init(void)
{
    HAL_TIM_Base_Start(&htim3);
}

void
Delay_us(uint16_t nus)
{
    uint16_t start = __HAL_TIM_GET_COUNTER(&htim3);
    uint16_t curr;

    while(1)
    {
        curr = __HAL_TIM_GET_COUNTER(&htim3);

        if((uint16_t)(curr - start) >= nus)
            break;
    }
}