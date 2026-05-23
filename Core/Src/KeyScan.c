#include "KeyScan.h"

#include "main.h"

#define KEY_DEBOUNCE_DELAY_MS 40U
#define KEY_SCAN_PERIOD_MS 20U

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} KeyInput_t;

static const KeyInput_t keyInputs[KEY_COUNT] = {
    { KEY0_GPIO_Port, KEY0_Pin },
    { KEY1_GPIO_Port, KEY1_Pin },
    { KEY2_GPIO_Port, KEY2_Pin },
    { KEY3_GPIO_Port, KEY3_Pin },
};

static BtnState_t keyState[KEY_COUNT] = {
    BTN_STATE_IDLE,
    BTN_STATE_IDLE,
    BTN_STATE_IDLE,
    BTN_STATE_IDLE,
};

static uint8_t keyDebounceCounter[KEY_COUNT] = { 0 };

volatile uint8_t keyEvent[KEY_COUNT] = { 0 };

static void
ScanOneKey(uint8_t keyIndex)
{
    uint8_t keyPressed =
      HAL_GPIO_ReadPin(keyInputs[keyIndex].port, keyInputs[keyIndex].pin) == GPIO_PIN_RESET;

    switch(keyState[keyIndex])
    {
        case BTN_STATE_IDLE:
            if(keyPressed)
            {
                keyState[keyIndex] = BTN_STATE_DEBOUNCE;
                keyDebounceCounter[keyIndex] = 0;
            }
            break;

        case BTN_STATE_DEBOUNCE:
            if(keyPressed)
            {
                keyDebounceCounter[keyIndex]++;
                if(keyDebounceCounter[keyIndex] >= (KEY_DEBOUNCE_DELAY_MS / KEY_SCAN_PERIOD_MS))
                {
                    keyState[keyIndex] = BTN_STATE_PRESSED;
                    keyEvent[keyIndex] = KEY_EVENT_CLICK;
                }
            }
            else
            {
                keyState[keyIndex] = BTN_STATE_IDLE;
            }
            break;

        case BTN_STATE_PRESSED:
            if(!keyPressed)
            {
                keyState[keyIndex] = BTN_STATE_IDLE;
            }
            break;
    }
}

void
KeyScan_Init(void)
{
    for(uint8_t keyIndex = 0; keyIndex < KEY_COUNT; keyIndex++)
    {
        keyState[keyIndex] = BTN_STATE_IDLE;
        keyDebounceCounter[keyIndex] = 0;
        keyEvent[keyIndex] = KEY_EVENT_NONE;
    }
}

void
KeyScan_ScanAll(void)
{
    for(uint8_t keyIndex = 0; keyIndex < KEY_COUNT; keyIndex++)
    {
        ScanOneKey(keyIndex);
    }
}