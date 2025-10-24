#ifndef OLED_BASICANIM_H
#define OLED_BASICANIM_H

#include "FreeRTOS.h"
#include "task.h"
#include "stdint.h"
#include "string.h" // 提供memset等字符串操作函数

// -------------------------- 1. 指令相关定义（用于任务间传递图形控制图形的指令格式） --------------------------

/**
 * @brief 指令类型枚举
 * 定义所有支持的图形控制指令，后续
展时可在此处新增
 */
typedef enum
{
    CMD_ANIM_START = 0,              // 启动动画（激活显示并重置进度）
    CMD_ANIM_PAUSE = 1,              // 暂停动画（保留当前进度）
    CMD_ANIM_RESUME = 2,             // 恢复动画（从暂停进度继续运行）
    CMD_ANIM_CHANGE_SPEED = 3,       // 修改动画速度（调整帧间隔时间）
    CMD_ANIM_CHANGE_DIR = 4,         // 修改动画方向（仅对移动类动画生效）
    CMD_ANIM_NEXTSERIAL = 5,         // 切换到下一个序列号（用于多帧同步）
    CMD_ANIM_OVERSERIAL = 6,         // 停止动画（隐藏图形并重置状态）
    CMD_DESTRUCT_GRAPH = 7,          // 注销图形（释放内存池资源并清除显示）
    CMD_DLETE_GRAPH_ONECE = 8,       // 单次删除显示（清除静态图形及残留）
    CMD_ANIM_CHANGE_TARGETVAL = 9,   // 修改动画目标值（如移动距离、弹出尺寸）
    CMD_ANIM_CHANGE_TYPE = 10,       // 修改动画类型（切换弹出/移动/视频等）
    CMD_ANIM_CHANGE_CONTENTTYPE = 11 // 修改内容类型（切换数字/文字/图片等）
} CmdType;

/**
 * @brief 指令参数联合体
 * 根据不同指令类型存储对应的参数，避免内存浪费
 */
typedef union
{
    uint16_t new_speed;       // 对应CMD_ANIM_CHANGE_SPEED：新帧间隔（单位：ms）
    uint8_t new_dir;          // 对应CMD_ANIM_CHANGE_DIR：新方向（1=上/2=下/3=左/4=右）
    uint8_t new_target_val;   // 对应CMD_ANIM_CHANGE_TARGETVAL：新目标值（单位：像素）
    uint8_t new_anim_type;    // 对应CMD_ANIM_CHANGE_TYPE：新动画类型
    uint8_t new_content_type; // 对应CMD_ANIM_CHANGE_CONTENTTYPE：新内容类型
    struct
    {              // 可选：用于位置控制场景（后续扩展）
        uint8_t x; // 目标X坐标
        uint8_t y; // 目标Y坐标
    } pos;
} CmdParam;

/**
 * @brief 通用指令结构体
 * 任务间传递图形控制指令的统一格式，包含目标ID、指令类型和参数
 */
typedef struct
{
    uint8_t target_id; // 目标图形ID（关联内存池中的具体对象）
    CmdType cmd_type;  // 指令类型（决定执行的操作）
    CmdParam param;    // 指令参数（仅对应类型的参数有效）
} GeneralCmd_t;

// -------------------------- 2. 核心枚举定义（状态、层级、内容类型） --------------------------

/**
 * @brief 图形状态枚举
 * 定义图形对象的生命周期状态
 */
typedef enum
{
    STATE_IDLE = 0,         // 空闲（未注册，内存池空闲位置）
    STATE_READY = 1,        // 就绪（已注册但未启动）
    STATE_STATIC = 2,       // 静态显示（无动画）
    STATE_ANIM_RUNNING = 3, // 动画运行中
    STATE_ANIM_PAUSED = 4   // 动画暂停（保留当前状态）
} GraphState;

/**
 * @brief Z轴层级枚举
 * 控制图形显示的前后层级关系
 */
typedef enum
{
    Z_BACKGROUND = 0, // 背景层（先绘制）
    Z_PROSPECT = 1    // 前景层（后绘制，覆盖背景）
} Anim_Z;

/**
 * @brief 内容类型枚举
 * 定义图形支持的内容格式
 */
typedef enum
{
    CONTENT_TYPE_NONE = 0,    // 无效类型
    CONTENT_TYPE_NUM = 1,     // 数字
    CONTENT_TYPE_ASCII = 2,   // ASCII字符
    CONTENT_TYPE_CHINESE = 3, // 汉字
    CONTENT_TYPE_IMAGE = 4,   // 图片
    CONTENT_TYPE_BLOCK = 5,   // 色块
    CONTENT_TYPE_VIDEO = 6    // 视频（多帧图片序列）
} ContentType;

// -------------------------- 3. 通用图形结构体（内存池对象的核心定义） --------------------------

/**
 * @brief 通用图形结构体
 * 统一描述所有可显示对象的属性，包括显示参数、动画参数和内容数据
 */
typedef struct
{
    uint8_t id;               // 唯一ID（注册时分配，用于指令定位）
    ContentType content_type; // 内容类型（决定绘制方式）
    GraphState state;         // 状态（控制生命周期）
    uint8_t is_show;          // 显示开关（1=显示，0=隐藏）

    // 基础显示属性（坐标和尺寸）
    uint8_t x; // 左上角X坐标
    uint8_t y; // 左上角Y坐标
    uint8_t w; // 宽度（像素）
    uint8_t h; // 高度（像素）

    // 动画专属属性（仅动画状态有效）
    uint8_t anim_type;       // 动画类型（0=无，1=弹出，2=移动，3=视频）
    uint8_t anim_dir;        // 动画方向（1=上，2=下，3=左，4=右）
    uint8_t anim_curr_val;   // 当前进度（像素，如移动距离、弹出尺寸）
    uint8_t anim_target_val; // 目标进度（像素，动画结束条件）
    uint16_t anim_speed;     // 帧间隔（ms，控制动画速度）
    uint32_t anim_last_tick; // 上一帧时间戳（用于控制帧间隔）
    uint8_t serial_num;      // 序列号（用于多帧同步，如视频帧索引）
    Anim_Z z_order;          // Z轴层级（前景/背景，控制绘制顺序）

    // 类型专属参数（联合体，根据content_type选择使用）
    union
    {
        // 数字类型参数
        struct
        {
            uint32_t num_val;  // 数字值
            uint8_t font_size; // 字体大小（如6x8、8x16）
            uint8_t num_count; // 位数（显示的数字长度）
        } num;

        // ASCII字符类型参数
        struct
        {
            char *str;         // 字符串指针
            uint8_t font_size; // 字体大小
        } ascii;

        // 汉字类型参数
        struct
        {
            char *ch_mod;      // 汉字字模指针
            uint8_t font_size; // 字体大小（通常16x16）
            uint8_t ch_count;  // 汉字个数
        } chinese;

        // 图片类型参数
        struct
        {
            const uint8_t *img_mod; // 图片字模指针
        } image;

        // 视频类型参数
        struct
        {
            const uint8_t *vd_mod; // 视频帧序列指针
            uint8_t cycle_en;      // 循环使能（1=循环播放，0=播放一次）
        } video;
    } param;

} GeneralGraph_t;

// -------------------------- 4. 核心函数声明（动画执行与指令处理） --------------------------

/**
 * @brief 色块弹出动画单帧执行函数
 * 处理弹出动画的一帧逻辑，包括清残留、画新帧和局部刷新
 * @param graph 图形对象指针
 */
void Disp_BlockPopOneFrame(GeneralGraph_t *graph);

/**
 * @brief 视频播放单帧执行函数
 * 处理视频动画的一帧逻辑，包括切换帧、清屏和局部刷新
 * @param graph 图形对象指针
 */
void Disp_VideoFramePlay(GeneralGraph_t *graph);

/**
 * @brief 通用平移动画单帧执行函数
 * 处理所有类型内容的平移动画，支持多方向移动
 * @param graph 图形对象指针
 */
void Disp_CommonMoveOneFrame(GeneralGraph_t *graph);

/**
 * @brief OLED显示执行函数
 * 遍历内存池所有图形对象，根据状态执行绘制或动画逻辑
 */
void oled_excute(void);

/**
 * @brief 指令执行函数
 * 解析并执行收到的图形控制指令
 * @param recv_cmd 收到的指令结构体
 */
void cmd_excute(GeneralCmd_t recv_cmd);

// -------------------------- 5. 全局变量声明与基础宏定义 --------------------------

extern volatile GeneralGraph_t g_graph_static_pool[]; // 全局图形内存池（外部定义）
extern const uint8_t MAX_GRAPH_TOTAL;                 // 内存池最大容量（外部定义）

// 基础宏定义（复用FreeRTOS接口，确保跨平台一致性）
#define OLED_delay vTaskDelay          // OLED延迟函数（封装FreeRTOS延迟）
#define OLED_gettime xTaskGetTickCount // 获取当前时间戳（封装FreeRTOS滴答计数）
#define GRAPH_SHOW 1                   // 图形显示状态标记
#define GRAPH_HIDE 0                   // 图形隐藏状态标记

#endif