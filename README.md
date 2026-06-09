# 嵌入式综合设计作业

本项目是一个基于 STM32F103ZET6 微控制器和 FreeRTOS 实时操作系统的多外设协同控制系统，集成 17 种外设模块，通过 FreeRTOS 任务实现并行调度。

## 硬件平台

- **MCU**：STM32F103ZET6（Cortex-M3, 72MHz）
- **OS**：FreeRTOS（CMSIS-RTOS v2）
- **工具链**：STM32CubeMX + HAL 库 + VS Code + CMake + Ninja

## 设计亮点

- **系统级整合**：17 个功能模块在单个 FreeRTOS 工程中协同工作
- **红外遥控集成**：NEC 协议状态机区分普通帧与重复码，红外解码后直接控制系统逻辑
- **DMA 广泛应用**：ADC 连续采集、UART 收发、SPI DAC 数据发送均使用 DMA
- **状态机设计**：按键消抖、红外解码采用有限状态机，逻辑清晰易于维护
- **单总线驱动**：DHT11/DS18B20 在时序关键段关中断保护，兼顾实时性与可靠性

## 功能模块

| 模块               | 说明                                         |
| ------------------ | -------------------------------------------- |
| LED 指示           | 12 路 LED，二进制显示 ADC 采样值             |
| 按键输入           | PA0 按键 + 4 路独立按键，状态机消抖          |
| 交通灯模拟         | 6 路 LED，四状态循环切换（3s 周期）          |
| ADC 采集           | DMA 连续采集光敏电阻（PA1）和热敏电阻（PA2） |
| UART 通信          | USART1 DMA + 空闲中断收发，消息队列缓冲      |
| LCD12864 显示      | 软件 SPI 驱动，4 行实时刷新                  |
| CH451 数码管       | 4 线串行接口，8 位数码管 + 矩阵键盘扫描      |
| 温度传感器 DHT11   | 单总线协议，定时采集温湿度并上报             |
| 温度传感器 DS18B20 | 单总线协议，9 位精度                         |
| 红外遥控           | NEC 协议状态机解码，支持重复码识别           |
| 继电器控制         | 2 路独立通断控制                             |
| TLC5615 DAC        | SPI3 DMA 驱动，呼吸灯效果                    |
| 直流电机           | TIM4 PWM 调速 + TIM3 编码器测速              |
| 步进电机           | 四相八拍驱动，角度精确控制                   |
| 舵机               | TIM8 50Hz PWM，多角度摆动模式                |
| 蜂鸣器             | 红外遥控按键反馈                             |
| 拨码开关           | 8 位 DIP 开关状态读取与 LCD 显示             |

## 系统架构

```
┌────────────────────────────────────────────────────────┐
│                    FreeRTOS Kernel                     │
│  SensorTask  UARTTask  ADCTask  BtnTask  TrafficTask   │
│  DisplayTask MotorTask InfraredTask CH451Task          │
│  SwitchTask  DACTask                                   │
│       ┌──────────────────┐  ┌──────────────────┐       │
│       │  UARTTXQueue (8) │  │ DIP2LCDQueue (2) │       │
│       └──────────────────┘  └──────────────────┘       │
├────────────────────────────────────────────────────────┤
│                    STM32 HAL Layer                     │
│  GPIO  ADC  DMA  USART  SPI  TIM  EXTI                 │
└────────────────────────────────────────────────────────┘
```

- **共 11 个任务**，通过消息队列解耦通信
- 传感器数据通过 `UARTTXQueue` 汇集到 UARTTask，DMA 发送至上位机
- 多模式设备控制：继电器 / 步进电机 / 直流电机 / 舵机，通过独立按键或红外遥控切换

## 硬件资源

| 资源         | 用途                            |
| ------------ | ------------------------------- |
| TIM3         | 直流电机编码器接口（四倍频）    |
| TIM4         | 直流电机 PWM（CH1/CH2）         |
| TIM5         | 交通灯 3s 周期中断              |
| TIM6         | HAL 系统时基（1ms）             |
| TIM7         | 微秒延时 + 红外解码时基（1MHz） |
| TIM8         | 舵机 PWM（CH4, 50Hz）           |
| DMA1 CH1     | ADC1 双通道连续采集             |
| DMA1 CH4/CH5 | USART1 TX/RX                    |
| DMA2 CH2     | SPI3 TX（TLC5615 DAC）          |
| SPI3         | TLC5615 通信                    |
| 软件 SPI     | LCD12864 液晶驱动               |

## 构建与烧录

```bash
# 使用 CMake + Ninja 构建
cmake -B build -G Ninja
cmake --build build

# 使用 CubeMX CLI 烧录
# 或通过 IDE（VS Code + Cortex-Debug）直接下载
```

## 目录结构

```
FinalEXP/
├── Core/
│   ├── Inc/                     # 头文件
│   │   ├── main.h               # 主头文件（类型定义、引脚宏、全局变量声明）
│   │   ├── FreeRTOSConfig.h     # FreeRTOS 配置文件
│   │   ├── btn.h                # PA0 按键状态机
│   │   ├── KeyScan.h            # 4 路独立按键扫描
│   │   ├── led.h                # LED 控制
│   │   ├── LCD12864.h           # LCD12864 液晶驱动
│   │   ├── ch451.h              # CH451 数码管+矩阵键盘
│   │   ├── dht11.h              # DHT11 温湿度传感器
│   │   ├── ds18b20.h            # DS18B20 温度传感器
│   │   ├── ir_receiver.h        # 红外 NEC 协议解码
│   │   ├── dc_motor.h           # 直流电机 PWM+编码器
│   │   ├── stepper_motor.h      # 步进电机驱动
│   │   ├── servo_motor.h        # 舵机驱动
│   │   ├── DelayUs.h            # 微秒延时
│   │   └── gpio.h/adc.h/dma.h/spi.h/tim.h/usart.h  # CubeMX 生成
│   └── Src/                     # 源文件
│       ├── main.c               # 主程序 + 中断回调
│       ├── freertos.c           # FreeRTOS 任务实现
│       ├── btn.c                # PA0 按键状态变量定义
│       ├── KeyScan.c            # 独立按键扫描实现
│       ├── led.c                # LED 控制实现
│       ├── LCD12864.c           # LCD12864 软件 SPI 驱动
│       ├── ch451.c              # CH451 4 线串行驱动
│       ├── dht11.c              # DHT11 单总线驱动
│       ├── ds18b20.c            # DS18B20 单总线驱动
│       ├── ir_receiver.c        # 红外 NEC 解码状态机
│       ├── dc_motor.c           # 直流电机控制+编码器测速
│       ├── stepper_motor.c      # 步进电机四相八拍驱动
│       ├── servo_motor.c        # 舵机 PWM 控制
│       ├── DelayUs.c            # 微秒延时（TIM7）
│       └── ...                  # CubeMX 生成的 HAL 映射文件
├── Drivers/                     # HAL 库 + CMSIS
├── Middleware/                  # FreeRTOS 内核
├── FinalEXP.ioc                 # CubeMX 工程文件
└── README.md
```
