#ifndef __BTN_H
#define __BTN_H

// 按键事件类型
typedef enum
{
    BTN_EVENT_NONE,
    BTN_EVENT_CLICK,
} BtnEvent_t;

// 按键状态机状态
typedef enum
{
    BTN_STATE_IDLE,
    BTN_STATE_DEBOUNCE,
    BTN_STATE_PRESSED,
} BtnState_t;

void Btn_Init(void);

extern BtnEvent_t btnEvent; // 当前按键事件
extern BtnState_t btnState; // 当前按键状态

#endif /* __BTN_H */