// 按键初始化
#include "btn.h"

BtnEvent_t btnEvent = BTN_EVENT_NONE; // 当前按键事件
BtnState_t btnState = BTN_STATE_IDLE; // 当前按键状态

void
Btn_Init(void)
{
    btnEvent = BTN_EVENT_NONE;
    btnState = BTN_STATE_IDLE;
}
