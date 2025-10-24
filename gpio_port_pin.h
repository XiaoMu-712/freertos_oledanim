#ifndef GPIO_PORT_PIN_H
#define GPIO_PORT_PIN_H

#include "stm32f10x.h"

// -------------------------- 定义GPIO_Pin结构体（包含中断线、中断通道和对应时钟） --------------------------
typedef struct
{
    GPIO_TypeDef *Port;     // 端口指针（如GPIOA）
    uint16_t Pin;           // 引脚掩码（如GPIO_Pin_0）
    uint8_t PortSource;     // AFIO端口源枚举（如GPIO_PortSourceGPIOA）
    uint8_t PinSource;      // AFIO引脚源枚举（如GPIO_PinSource0）
    uint32_t ExtiLine;      // 对应的外部中断线（如EXTI_Line0）
    IRQn_Type IRQChannel;   // 对应的中断通道枚举（如EXTI0_IRQn）
    uint32_t RccApb2Periph; // 对应端口的APB2时钟（如RCC_APB2Periph_GPIOA）
} GPIO_Pin;

// -------------------------- 引脚宏定义（包含中断线、中断通道和对应时钟） --------------------------

// PA系列
#define PA_0 ((GPIO_Pin){GPIOA, GPIO_Pin_0, GPIO_PortSourceGPIOA, GPIO_PinSource0, EXTI_Line0, EXTI0_IRQn, RCC_APB2Periph_GPIOA})
#define PA_1 ((GPIO_Pin){GPIOA, GPIO_Pin_1, GPIO_PortSourceGPIOA, GPIO_PinSource1, EXTI_Line1, EXTI1_IRQn, RCC_APB2Periph_GPIOA})
#define PA_2 ((GPIO_Pin){GPIOA, GPIO_Pin_2, GPIO_PortSourceGPIOA, GPIO_PinSource2, EXTI_Line2, EXTI2_IRQn, RCC_APB2Periph_GPIOA})
#define PA_3 ((GPIO_Pin){GPIOA, GPIO_Pin_3, GPIO_PortSourceGPIOA, GPIO_PinSource3, EXTI_Line3, EXTI3_IRQn, RCC_APB2Periph_GPIOA})
#define PA_4 ((GPIO_Pin){GPIOA, GPIO_Pin_4, GPIO_PortSourceGPIOA, GPIO_PinSource4, EXTI_Line4, EXTI4_IRQn, RCC_APB2Periph_GPIOA})
#define PA_5 ((GPIO_Pin){GPIOA, GPIO_Pin_5, GPIO_PortSourceGPIOA, GPIO_PinSource5, EXTI_Line5, EXTI9_5_IRQn, RCC_APB2Periph_GPIOA})
#define PA_6 ((GPIO_Pin){GPIOA, GPIO_Pin_6, GPIO_PortSourceGPIOA, GPIO_PinSource6, EXTI_Line6, EXTI9_5_IRQn, RCC_APB2Periph_GPIOA})
#define PA_7 ((GPIO_Pin){GPIOA, GPIO_Pin_7, GPIO_PortSourceGPIOA, GPIO_PinSource7, EXTI_Line7, EXTI9_5_IRQn, RCC_APB2Periph_GPIOA})
#define PA_8 ((GPIO_Pin){GPIOA, GPIO_Pin_8, GPIO_PortSourceGPIOA, GPIO_PinSource8, EXTI_Line8, EXTI9_5_IRQn, RCC_APB2Periph_GPIOA})
#define PA_9 ((GPIO_Pin){GPIOA, GPIO_Pin_9, GPIO_PortSourceGPIOA, GPIO_PinSource9, EXTI_Line9, EXTI9_5_IRQn, RCC_APB2Periph_GPIOA})
#define PA_10 ((GPIO_Pin){GPIOA, GPIO_Pin_10, GPIO_PortSourceGPIOA, GPIO_PinSource10, EXTI_Line10, EXTI9_5_IRQn, RCC_APB2Periph_GPIOA})
#define PA_11 ((GPIO_Pin){GPIOA, GPIO_Pin_11, GPIO_PortSourceGPIOA, GPIO_PinSource11, EXTI_Line11, EXTI9_5_IRQn, RCC_APB2Periph_GPIOA})
#define PA_12 ((GPIO_Pin){GPIOA, GPIO_Pin_12, GPIO_PortSourceGPIOA, GPIO_PinSource12, EXTI_Line12, EXTI9_5_IRQn, RCC_APB2Periph_GPIOA})
#define PA_13 ((GPIO_Pin){GPIOA, GPIO_Pin_13, GPIO_PortSourceGPIOA, GPIO_PinSource13, EXTI_Line13, EXTI9_5_IRQn, RCC_APB2Periph_GPIOA})
#define PA_14 ((GPIO_Pin){GPIOA, GPIO_Pin_14, GPIO_PortSourceGPIOA, GPIO_PinSource14, EXTI_Line14, EXTI9_5_IRQn, RCC_APB2Periph_GPIOA})
#define PA_15 ((GPIO_Pin){GPIOA, GPIO_Pin_15, GPIO_PortSourceGPIOA, GPIO_PinSource15, EXTI_Line15, EXTI9_5_IRQn, RCC_APB2Periph_GPIOA})

// PB系列
#define PB_0 ((GPIO_Pin){GPIOB, GPIO_Pin_0, GPIO_PortSourceGPIOB, GPIO_PinSource0, EXTI_Line0, EXTI0_IRQn, RCC_APB2Periph_GPIOB})
#define PB_1 ((GPIO_Pin){GPIOB, GPIO_Pin_1, GPIO_PortSourceGPIOB, GPIO_PinSource1, EXTI_Line1, EXTI1_IRQn, RCC_APB2Periph_GPIOB})
#define PB_2 ((GPIO_Pin){GPIOB, GPIO_Pin_2, GPIO_PortSourceGPIOB, GPIO_PinSource2, EXTI_Line2, EXTI2_IRQn, RCC_APB2Periph_GPIOB})
#define PB_3 ((GPIO_Pin){GPIOB, GPIO_Pin_3, GPIO_PortSourceGPIOB, GPIO_PinSource3, EXTI_Line3, EXTI3_IRQn, RCC_APB2Periph_GPIOB})
#define PB_4 ((GPIO_Pin){GPIOB, GPIO_Pin_4, GPIO_PortSourceGPIOB, GPIO_PinSource4, EXTI_Line4, EXTI4_IRQn, RCC_APB2Periph_GPIOB})
#define PB_5 ((GPIO_Pin){GPIOB, GPIO_Pin_5, GPIO_PortSourceGPIOB, GPIO_PinSource5, EXTI_Line5, EXTI9_5_IRQn, RCC_APB2Periph_GPIOB})
#define PB_6 ((GPIO_Pin){GPIOB, GPIO_Pin_6, GPIO_PortSourceGPIOB, GPIO_PinSource6, EXTI_Line6, EXTI9_5_IRQn, RCC_APB2Periph_GPIOB})
#define PB_7 ((GPIO_Pin){GPIOB, GPIO_Pin_7, GPIO_PortSourceGPIOB, GPIO_PinSource7, EXTI_Line7, EXTI9_5_IRQn, RCC_APB2Periph_GPIOB})
#define PB_8 ((GPIO_Pin){GPIOB, GPIO_Pin_8, GPIO_PortSourceGPIOB, GPIO_PinSource8, EXTI_Line8, EXTI9_5_IRQn, RCC_APB2Periph_GPIOB})
#define PB_9 ((GPIO_Pin){GPIOB, GPIO_Pin_9, GPIO_PortSourceGPIOB, GPIO_PinSource9, EXTI_Line9, EXTI9_5_IRQn, RCC_APB2Periph_GPIOB})
#define PB_10 ((GPIO_Pin){GPIOB, GPIO_Pin_10, GPIO_PortSourceGPIOB, GPIO_PinSource10, EXTI_Line10, EXTI9_5_IRQn, RCC_APB2Periph_GPIOB})
#define PB_11 ((GPIO_Pin){GPIOB, GPIO_Pin_11, GPIO_PortSourceGPIOB, GPIO_PinSource11, EXTI_Line11, EXTI9_5_IRQn, RCC_APB2Periph_GPIOB})
#define PB_12 ((GPIO_Pin){GPIOB, GPIO_Pin_12, GPIO_PortSourceGPIOB, GPIO_PinSource12, EXTI_Line12, EXTI9_5_IRQn, RCC_APB2Periph_GPIOB})
#define PB_13 ((GPIO_Pin){GPIOB, GPIO_Pin_13, GPIO_PortSourceGPIOB, GPIO_PinSource13, EXTI_Line13, EXTI9_5_IRQn, RCC_APB2Periph_GPIOB})
#define PB_14 ((GPIO_Pin){GPIOB, GPIO_Pin_14, GPIO_PortSourceGPIOB, GPIO_PinSource14, EXTI_Line14, EXTI9_5_IRQn, RCC_APB2Periph_GPIOB})
#define PB_15 ((GPIO_Pin){GPIOB, GPIO_Pin_15, GPIO_PortSourceGPIOB, GPIO_PinSource15, EXTI_Line15, EXTI9_5_IRQn, RCC_APB2Periph_GPIOB})

// PC、PD系列
#define PC_0 ((GPIO_Pin){GPIOC, GPIO_Pin_0, GPIO_PortSourceGPIOC, GPIO_PinSource0, EXTI_Line0, EXTI0_IRQn, RCC_APB2Periph_GPIOC})
#define PC_1 ((GPIO_Pin){GPIOC, GPIO_Pin_1, GPIO_PortSourceGPIOC, GPIO_PinSource1, EXTI_Line1, EXTI1_IRQn, RCC_APB2Periph_GPIOC})
#define PC_2 ((GPIO_Pin){GPIOC, GPIO_Pin_2, GPIO_PortSourceGPIOC, GPIO_PinSource2, EXTI_Line2, EXTI2_IRQn, RCC_APB2Periph_GPIOC})
#define PC_3 ((GPIO_Pin){GPIOC, GPIO_Pin_3, GPIO_PortSourceGPIOC, GPIO_PinSource3, EXTI_Line3, EXTI3_IRQn, RCC_APB2Periph_GPIOC})
#define PC_4 ((GPIO_Pin){GPIOC, GPIO_Pin_4, GPIO_PortSourceGPIOC, GPIO_PinSource4, EXTI_Line4, EXTI4_IRQn, RCC_APB2Periph_GPIOC})
#define PC_5 ((GPIO_Pin){GPIOC, GPIO_Pin_5, GPIO_PortSourceGPIOC, GPIO_PinSource5, EXTI_Line5, EXTI9_5_IRQn, RCC_APB2Periph_GPIOC})
#define PC_6 ((GPIO_Pin){GPIOC, GPIO_Pin_6, GPIO_PortSourceGPIOC, GPIO_PinSource6, EXTI_Line6, EXTI9_5_IRQn, RCC_APB2Periph_GPIOC})
#define PC_7 ((GPIO_Pin){GPIOC, GPIO_Pin_7, GPIO_PortSourceGPIOC, GPIO_PinSource7, EXTI_Line7, EXTI9_5_IRQn, RCC_APB2Periph_GPIOC})
#define PC_8 ((GPIO_Pin){GPIOC, GPIO_Pin_8, GPIO_PortSourceGPIOC, GPIO_PinSource8, EXTI_Line8, EXTI9_5_IRQn, RCC_APB2Periph_GPIOC})
#define PC_9 ((GPIO_Pin){GPIOC, GPIO_Pin_9, GPIO_PortSourceGPIOC, GPIO_PinSource9, EXTI_Line9, EXTI9_5_IRQn, RCC_APB2Periph_GPIOC})
#define PC_10 ((GPIO_Pin){GPIOC, GPIO_Pin_10, GPIO_PortSourceGPIOC, GPIO_PinSource10, EXTI_Line10, EXTI9_5_IRQn, RCC_APB2Periph_GPIOC})
#define PC_13 ((GPIO_Pin){GPIOC, GPIO_Pin_13, GPIO_PortSourceGPIOC, GPIO_PinSource13, EXTI_Line13, EXTI9_5_IRQn, RCC_APB2Periph_GPIOC})
#define PC_14 ((GPIO_Pin){GPIOC, GPIO_Pin_14, GPIO_PortSourceGPIOC, GPIO_PinSource14, EXTI_Line14, EXTI9_5_IRQn, RCC_APB2Periph_GPIOC})
#define PC_15 ((GPIO_Pin){GPIOC, GPIO_Pin_15, GPIO_PortSourceGPIOC, GPIO_PinSource15, EXTI_Line15, EXTI9_5_IRQn, RCC_APB2Periph_GPIOC})
#define PD_0 ((GPIO_Pin){GPIOD, GPIO_Pin_0, GPIO_PortSourceGPIOD, GPIO_PinSource0, EXTI_Line0, EXTI0_IRQn, RCC_APB2Periph_GPIOD})
#define PD_1 ((GPIO_Pin){GPIOD, GPIO_Pin_1, GPIO_PortSourceGPIOD, GPIO_PinSource1, EXTI_Line1, EXTI1_IRQn, RCC_APB2Periph_GPIOD})

#endif


