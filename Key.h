#ifndef KEY_H
#define KEY_H

#include "gpio_port_pin.h"  
#define KEY_PULL_MODE_DOWN    0   // 下拉输入模式（按键未按时引脚为低电平，按下时为高电平）
#define KEY_PULL_MODE_UP      1   // 上拉输入模式（按键未按时引脚为高电平，按下时为低电平）


// 2. 按键功能选择宏（对应 Func_Sel 成员）
#define KEY_FUNC_CLICK_ONLY   0   // 仅支持“短按/长按”（需松键后才返回结果，适合单次触发场景）
#define KEY_FUNC_LONG_LOOP    1   // 支持“短按+长按间隔触发”（按住期间循环返回长按信号，适合连续操作）
#define KEY_FUNC_EXTI         2  // 外部中断模式（触发中断，需自行在中断处理函数中读取状态，适合响应速度要求高的场景）
// 按键结构体：存储按键硬件配置、功能参数及运行状态（每个按键独立实例化）
typedef struct {
    GPIO_Pin GPIO_Pin;           // 按键GPIO引脚，直接赋值如PA_0（需与gpio_port_pin.h定义匹配）
    uint8_t  Pull_Mode;          // 按键输入模式：0=下拉输入，1=上拉输入
    uint8_t  Func_Sel;           // 按键功能选择：0=仅支持短按/长按（松键后返回结果），1=支持短按+长按间隔触发（按住期间循环返回）
    uint8_t exti_priority;       // 外部中断优先级（仅Func_Sel=2时生效）
    uint16_t Long_Start_ms;      // 长按判定阈值（ms）：按下持续时间≥此值判定为长按
    uint16_t Long_Interval_ms;   // 长按间隔触发周期（ms）：仅Func_Sel=1时生效，按住期间每隔此时间返回一次长按信号
    uint8_t  key_state;          // 按键运行状态：0=未按下，1=按下中，2=长按（内部维护，外部无需赋值）
    uint32_t press_tick;         // 按下时刻时间戳（ms）（内部维护，外部无需赋值）
    uint32_t last_send_tick;     // 上次长按间隔信号发送时刻（ms）（内部维护，外部无需赋值）
    uint8_t  last_level;         // 上一次GPIO电平值（内部维护，外部无需赋值）

} Key_ObjectTypeDef;

/**
 * @brief  按键初始化函数
 * @param  key_obj: 指向按键对象结构体的指针（需提前配置GPIO_Pin和Pull_Mode成员）
 * @retval 1: 初始化成功；0: 初始化失败（Pull_Mode配置错误，仅支持0或1）
 * @note   初始化时会根据Pull_Mode配置GPIO为上拉或下拉输入，其他结构体成员（如Func_Sel）无需在此阶段初始化
 */
uint8_t Key_Init(Key_ObjectTypeDef *key_obj);

/**
 * @brief  下拉输入按键-仅短按/长按扫描（松键后返回结果）
 * @param  key_obj: 指向按键对象结构体的指针（需确保Pull_Mode=0，Long_Start_ms已配置）
 * @retval KEY_NONE(0): 无动作；KEY_CLICK_SHORT(1): 短按（按下时间<Long_Start_ms）；KEY_CLICK_LONG(2): 长按（按下时间≥Long_Start_ms）
 * @note   仅支持下拉输入模式，扫描逻辑依赖结构体内部状态变量（key_state、press_tick等），无需外部维护状态
 */
uint8_t Key_Scan_ClickOnly(Key_ObjectTypeDef *key_obj);

/**
 * @brief  下拉输入按键-短按+长按间隔扫描（按住期间循环返回）
 * @param  key_obj: 指向按键对象结构体的指针（需确保Pull_Mode=0，Long_Start_ms和Long_Interval_ms已配置）
 * @retval KEY_NONE(0): 无动作；KEY_CLICK_SHORT(1): 短按（未达长按阈值松键）；KEY_LONG_LOOP(3): 长按触发（达阈值时首次返回+按住期间每隔Long_Interval_ms返回）
 * @note   仅支持下拉输入模式，长按触发后会持续间隔返回信号，直到按键松开
 */
uint8_t Key_Scan_LongLoop(Key_ObjectTypeDef *key_obj);

/**
 * @brief  上拉输入按键-仅短按/长按扫描（松键后返回结果）
 * @param  key_obj: 指向按键对象结构体的指针（需确保Pull_Mode=1，Long_Start_ms已配置）
 * @retval KEY_NONE(0): 无动作；KEY_CLICK_SHORT(1): 短按（按下时间<Long_Start_ms）；KEY_CLICK_LONG(2): 长按（按下时间≥Long_Start_ms）
 * @note   仅支持上拉输入模式，扫描逻辑依赖结构体内部状态变量，无需外部维护状态
 */
uint8_t Key_Scan_ClickOnly_PullUp(Key_ObjectTypeDef *key_obj);

/**
 * @brief  上拉输入按键-短按+长按间隔扫描（按住期间循环返回）
 * @param  key_obj: 指向按键对象结构体的指针（需确保Pull_Mode=1，Long_Start_ms和Long_Interval_ms已配置）
 * @retval KEY_NONE(0): 无动作；KEY_CLICK_SHORT(1): 短按（未达长按阈值松键）；KEY_LONG_LOOP(3): 长按触发（达阈值时首次返回+按住期间每隔Long_Interval_ms返回）
 * @note   仅支持上拉输入模式，长按触发后会持续间隔返回信号，直到按键松开
 */
uint8_t Key_Scan_LongLoop_PullUp(Key_ObjectTypeDef *key_obj);

/**
 * @brief  按键通用扫描函数（自动适配输入模式和功能）
 * @param  key_obj: 指向按键对象结构体的指针（需提前配置所有必要成员：GPIO_Pin、Pull_Mode、Func_Sel、Long_Start_ms、Long_Interval_ms）
 * @retval KEY_NONE(0): 无动作；KEY_CLICK_SHORT(1): 短按；KEY_CLICK_LONG(2)/KEY_LONG_LOOP(3): 长按（根据Func_Sel区分）
 * @note   无需手动选择具体扫描函数，函数会根据key_obj的配置自动调用对应模式的扫描接口，推荐外部优先使用此函数
 */
uint8_t Key_Scan_General(Key_ObjectTypeDef *key_obj);

// 统一返回值宏定义（不变）
#define KEY_NONE         0   // 无动作
#define KEY_CLICK_SHORT  1   // 普通点击（按→松，时长<长按阈值）
#define KEY_CLICK_LONG   2   // 长按点击（按→松，时长≥长按阈值）
#define KEY_LONG_LOOP    3   // 长按间隔触发（与长按点击复用值，不同时用）

// 时间函数宏定义（适配FreeRTOS，如需切换系统需修改此处）
#include "FreeRTOS.h"
#include "task.h"
#define Key_delay vTaskDelay    // 延时函数（单位：FreeRTOS tick，需确保tick周期与ms匹配）
#define Key_gettime xTaskGetTickCount  // 时间戳函数（返回值：FreeRTOS tick）

#endif



