#include "stm32f10x.h"

/**
 * @brief EXTI_Line0 中断服务函数（对应PA0/PB0/PC0等Pin0引脚）
 */
void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

/**
 * @brief EXTI_Line1 中断服务函数（对应PA1/PB1/PC1等Pin1引脚）
 */
void EXTI1_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

/**
 * @brief EXTI_Line2 中断服务函数（对应PA2/PB2/PC2等Pin2引脚）
 */
void EXTI2_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line2) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
}

/**
 * @brief EXTI_Line3 中断服务函数（对应PA3/PB3/PC3等Pin3引脚）
 */
void EXTI3_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line3) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line3);
    }
}

/**
 * @brief EXTI_Line4 中断服务函数（对应PA4/PB4/PC4等Pin4引脚）
 */
void EXTI4_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line4) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
}

/**
 * @brief EXTI_Line5~Line9 中断服务函数（对应所有Pin5~Pin9引脚）
 */
void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line5) != RESET)
        EXTI_ClearITPendingBit(EXTI_Line5);
    if (EXTI_GetITStatus(EXTI_Line6) != RESET)
        EXTI_ClearITPendingBit(EXTI_Line6);
    if (EXTI_GetITStatus(EXTI_Line7) != RESET)
        EXTI_ClearITPendingBit(EXTI_Line7);
    if (EXTI_GetITStatus(EXTI_Line8) != RESET)
        EXTI_ClearITPendingBit(EXTI_Line8);
    if (EXTI_GetITStatus(EXTI_Line9) != RESET)
        EXTI_ClearITPendingBit(EXTI_Line9);
}

/**
 * @brief EXTI_Line10~Line15 中断服务函数（对应所有Pin10~Pin15引脚）
 */
void EXTI15_10_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line10) != RESET)
        EXTI_ClearITPendingBit(EXTI_Line10);
    if (EXTI_GetITStatus(EXTI_Line11) != RESET)
        EXTI_ClearITPendingBit(EXTI_Line11);
    if (EXTI_GetITStatus(EXTI_Line12) != RESET)
        EXTI_ClearITPendingBit(EXTI_Line12);
    if (EXTI_GetITStatus(EXTI_Line13) != RESET)
        EXTI_ClearITPendingBit(EXTI_Line13);
    if (EXTI_GetITStatus(EXTI_Line14) != RESET)
        EXTI_ClearITPendingBit(EXTI_Line14);
    if (EXTI_GetITStatus(EXTI_Line15) != RESET)
        EXTI_ClearITPendingBit(EXTI_Line15);
}

// -------------------------- 定时器中断服务函数（仅含中断标志位清零） --------------------------
/**
 * @brief 定时器1 更新中断服务函数（TIM1_UP）
 */
void TIM1_UP_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}

/**
 * @brief 定时器1 捕获比较1中断服务函数（TIM1_CC1）
 */
void TIM1_CC_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET)
    {
        TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
    }
    if (TIM_GetITStatus(TIM1, TIM_IT_CC2) != RESET)
    {
        TIM_ClearITPendingBit(TIM1, TIM_IT_CC2);
    }
    if (TIM_GetITStatus(TIM1, TIM_IT_CC3) != RESET)
    {
        TIM_ClearITPendingBit(TIM1, TIM_IT_CC3);
    }
    if (TIM_GetITStatus(TIM1, TIM_IT_CC4) != RESET)
    {
        TIM_ClearITPendingBit(TIM1, TIM_IT_CC4);
    }
}

/**
 * @brief 定时器2 更新中断服务函数（TIM2_UP）
 */
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

/**
 * @brief 定时器3 更新中断服务函数（TIM3_UP）
 */
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
}

/**
 * @brief 定时器4 更新中断服务函数（TIM4_UP）
 */
void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}

/**
 * @brief 定时器5 更新中断服务函数（TIM5_UP，仅大容量STM32F1系列支持）
 */
void TIM5_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
    }
}

/**
 * @brief 定时器6 更新中断服务函数（TIM6_UP，仅大容量STM32F1系列支持）
 */
void TIM6_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }
}

/**
 * @brief 定时器7 更新中断服务函数（TIM7_UP，仅大容量STM32F1系列支持）
 */
void TIM7_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    }
}

/**
 * @brief 定时器8 更新中断服务函数（TIM8_UP，仅大容量STM32F1系列支持）
 */
void TIM8_UP_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
    }
}

/**
 * @brief 定时器8 捕获比较1中断服务函数（TIM8_CC1，仅大容量STM32F1系列支持）
 */
void TIM8_CC_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM8, TIM_IT_CC1) != RESET)
    {
        TIM_ClearITPendingBit(TIM8, TIM_IT_CC1);
    }
    if (TIM_GetITStatus(TIM8, TIM_IT_CC2) != RESET)
    {
        TIM_ClearITPendingBit(TIM8, TIM_IT_CC2);
    }
    if (TIM_GetITStatus(TIM8, TIM_IT_CC3) != RESET)
    {
        TIM_ClearITPendingBit(TIM8, TIM_IT_CC3);
    }
    if (TIM_GetITStatus(TIM8, TIM_IT_CC4) != RESET)
    {
        TIM_ClearITPendingBit(TIM8, TIM_IT_CC4);
    }
}