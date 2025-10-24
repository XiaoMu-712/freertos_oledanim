#include "gpio_port_pin.h"
#include "gpio_common.h"
#include "stm32f10x_exti.h" // 补充EXTI相关头文件，解决EXTI_Trigger_TypeDef未定义问题

// -------------------------- 推挽输出初始化--------------------------
void GPIO_PP_Init(GPIO_Pin gpio)
{
    // 从结构体拆分端口、引脚和对应时钟
    GPIO_TypeDef *port = gpio.Port;
    uint16_t pin = gpio.Pin;
    uint32_t rcc = gpio.RccApb2Periph;

    /* 1. PC13/14/15特殊处理*/
    if (port == GPIOC && (pin == GPIO_Pin_13 || pin == GPIO_Pin_14 || pin == GPIO_Pin_15))
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
        PWR_BackupAccessCmd(ENABLE);
    }

    /* 2. 使能GPIO时钟（通过结构体成员简化判断）*/
    RCC_APB2PeriphClockCmd(rcc, ENABLE);

    /* 3. GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = (port == GPIOC && (pin == GPIO_Pin_13 || pin == GPIO_Pin_14 || pin == GPIO_Pin_15)) ? GPIO_Speed_2MHz : GPIO_Speed_50MHz;
    GPIO_Init(port, &GPIO_InitStruct);

    /* 4. 初始高电平*/
    GPIO_SetBits(port, pin);
}

// -------------------------- 电平控制函数 --------------------------
void GPIO_SetHigh(GPIO_Pin gpio)
{
    GPIO_TypeDef *port = gpio.Port;
    uint16_t pin = gpio.Pin;

    GPIO_SetBits(port, pin);
}

void GPIO_SetLow(GPIO_Pin gpio)
{
    GPIO_TypeDef *port = gpio.Port;
    uint16_t pin = gpio.Pin;

    GPIO_ResetBits(port, pin);
}

void GPIO_Toggle(GPIO_Pin gpio)
{
    GPIO_TypeDef *port = gpio.Port;
    uint16_t pin = gpio.Pin;

    if (GPIO_ReadOutputDataBit(port, pin) == Bit_SET)
    {
        GPIO_ResetBits(port, pin);
    }
    else
    {
        GPIO_SetBits(port, pin);
    }
}

// -------------------------- 上拉输入初始化 --------------------------
void GPIO_PuIn_Init(GPIO_Pin gpio)
{
    GPIO_TypeDef *port = gpio.Port;
    uint16_t pin = gpio.Pin;
    uint32_t rcc = gpio.RccApb2Periph;

    // 使能GPIO时钟（通过结构体成员简化判断）
    RCC_APB2PeriphClockCmd(rcc, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(port, &GPIO_InitStruct);
}

// -------------------------- 下拉输入初始化 --------------------------
void GPIO_PdIn_Init(GPIO_Pin gpio)
{
    GPIO_TypeDef *port = gpio.Port;
    uint16_t pin = gpio.Pin;
    uint32_t rcc = gpio.RccApb2Periph;

    // 使能GPIO时钟（通过结构体成员简化判断）
    RCC_APB2PeriphClockCmd(rcc, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(port, &GPIO_InitStruct);
}

// -------------------------- 输入电平读取 --------------------------
BitAction GPIO_ReadIn(GPIO_Pin gpio)
{
    GPIO_TypeDef *port = gpio.Port;
    uint16_t pin = gpio.Pin;

    return (BitAction)GPIO_ReadInputDataBit(port, pin);
}

// -------------------------- 外部中断上拉初始化（补充原文件中缺失的函数实现，避免编译警告） --------------------------
void Exti_Pu_Init(GPIO_Pin pin,  uint8_t preemptionPriority)
{
   
    // 1. 使能AFIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    // 2. 配置GPIO为上拉输入（复用GPIO_PuIn_Init）
    GPIO_PuIn_Init(pin);
    // 3. 连接EXTI中断线
    GPIO_EXTILineConfig(pin.PortSource, pin.PinSource);
    // 4. 配置EXTI
    EXTI_InitTypeDef EXTI_InitStruct;
    EXTI_InitStruct.EXTI_Line = pin.ExtiLine;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);
    // 5. 配置NVIC
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = pin.IRQChannel;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = preemptionPriority;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}


void Exti_Pd_Init(GPIO_Pin pin,  uint8_t preemptionPriority)
{

    // 1. 使能AFIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    // 2. 配置GPIO为下拉输入（复用GPIO_PdIn_Init）
    GPIO_PdIn_Init(pin);
    // 3. 连接EXTI中断线
    GPIO_EXTILineConfig(pin.PortSource, pin.PinSource);
    // 4. 配置EXTI
    EXTI_InitTypeDef EXTI_InitStruct;
    EXTI_InitStruct.EXTI_Line = pin.ExtiLine;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);
    // 5. 配置NVIC
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = pin.IRQChannel;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = preemptionPriority;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}
void Exti_PdandPu_Init(GPIO_Pin pin, uint8_t preemptionPriority)
{

    // 1. 使能AFIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    // 2. 配置GPIO为下拉输入（复用GPIO_PdIn_Init）
    GPIO_PdIn_Init(pin);
    // 3. 连接EXTI中断线
    GPIO_EXTILineConfig(pin.PortSource, pin.PinSource);
    // 4. 配置EXTI
    EXTI_InitTypeDef EXTI_InitStruct;
    EXTI_InitStruct.EXTI_Line = pin.ExtiLine;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);
    // 5. 配置NVIC
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = pin.IRQChannel;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = preemptionPriority;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}


