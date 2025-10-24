#include "myheaderfile.h"
// 引入简化后的图形框架头文件，包含图形绘制、动画控制等相关声明

// -------------------------- 队列与任务句柄声明（仅保留必要项） --------------------------
// 按键队列：用于传递按键状态（保留2个队列以兼容原有逻辑）
QueueHandle_t queue_keyscan_handler;

// 任务句柄（仅保留必要任务，用于任务管理和控制）
TaskHandle_t task_keyscan_Handler;      // 按键扫描任务句柄
TaskHandle_t task_oled_Handler;         // OLED显示任务句柄
TaskHandle_t task_main_process_Handler; // 主处理任务句柄

// -------------------------- 任务函数声明 --------------------------
void task_keyscan(void *pvParameters);      // 按键扫描任务：负责检测按键状态并发送到队列
void task_oled(void *pvParameters);         // OLED显示任务：处理图形指令并更新显示
void task_main_process(void *pvParameters); // 主处理任务：接收按键指令，控制图形动画逻辑

// -------------------------- 主函数：初始化硬件与任务 --------------------------
int main(void)
{
    // 1. 硬件初始化（如GPIO等外设）
    GPIO_PP_Init(PC_13); // 初始化PC_13引脚（示例：假设为LED或其他外设）

    // 2. 队列初始化（删除旧的block队列，新增图形指令队列）
    // 创建按键队列，容量10，每个元素为uint8_t类型
    queue_keyscan_handler = xQueueCreate(10, sizeof(uint8_t));

    // 初始化图形指令队列及相关互斥锁（框架内部函数）
    CMDandMUTEX_queue_init();

    // 3. 创建任务
    // 按键扫描任务：栈大小128，优先级1
    xTaskCreate(task_keyscan, "task_keyscan", 128, NULL, 1, &task_keyscan_Handler);
    // 主处理任务：栈大小128，优先级1
    xTaskCreate(task_main_process, "task_main_process", 128, NULL, 1, &task_main_process_Handler);
    // OLED显示任务：栈大小128，优先级1
    xTaskCreate(task_oled, "task_oled", 128, NULL, 1, &task_oled_Handler);

    // 4. 启动FreeRTOS任务调度器
    vTaskStartScheduler();

    // 若调度器启动失败，进入死循环（通常不会执行到此处）
    while (1)
    {
    }
}

/**
 * 按键扫描任务：周期性扫描按键状态，并将状态发送到按键队列
 */
void task_keyscan(void *pvParameters)
{
    // 定义并初始化按键1的参数
    Key_ObjectTypeDef Key1;
    Key1.GPIO_Pin = PA_0;                // 按键连接的引脚为PA_0
    Key1.Pull_Mode = KEY_PULL_MODE_DOWN; // 按键下拉模式
    Key1.Long_Start_ms = 300;            // 长按判定起始时间：300ms
    Key1.Long_Interval_ms = 1;           // 长按状态间隔：1ms（连续触发）
    Key1.Func_Sel = KEY_FUNC_CLICK_ONLY; // 按键功能：仅检测单击
    Key_Init(&Key1);                     // 初始化按键1（配置GPIO、状态变量等）

    while (1)
    {
        // 扫描Key1的状态（返回值：0=无操作，1=短按，2=长按等）
        uint8_t state = Key_Scan_General(&Key1);

        // 若按键有状态变化（非0），发送到队列
        if (state)
        {
            // 非阻塞发送到按键队列（队列满时不阻塞，直接跳过）
            xQueueSend(queue_keyscan_handler, &state, 0);
            state = 0; // 清空状态，准备下一次扫描
        }

        // 任务延时（调整扫描周期，避免频繁占用CPU）
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/**
 * 主处理任务：从队列接收按键状态，生成图形控制指令（如创建/启动/暂停动画）
 */
void task_main_process(void *pvParameters)
{
    uint8_t key_state = 0; // 存储接收到的按键状态

    // 注册图形1：数字显示（ID=1，内容类型=数字，层级=2）
    GeneralGraph_t *block1 = Graph_ID_Register(1, CONTENT_TYPE_NUM, 2);
    if (block1 != NULL)
    {
        // 图形位置与属性配置
        block1->x = 0;                           // X坐标起始位置
        block1->y = 0;                           // Y坐标起始位置
        block1->param.num.num_val = 1234;        // 初始显示的数字值
        block1->param.num.num_count = 4;         // 数字位数（1234为4位）
        block1->param.num.font_size = OLED_8X16; // 字体大小：8x16像素

        // 动画属性配置
        block1->anim_dir = 4;         // 动画方向：右移（4代表右移方向）
        block1->anim_target_val = 80; // 动画目标：移动到80像素位置
        block1->anim_speed = 50;      // 动画速度：每帧间隔50ms（约20fps）
    }

    // 注册图形2：方块显示（ID=2，内容类型=方块，层级=2）
    GeneralGraph_t *block2 = Graph_ID_Register(2, CONTENT_TYPE_BLOCK, 2);
    if (block2 != NULL)
    {
        // 图形位置与属性配置
        block2->x = 0;  // X坐标起始位置
        block2->y = 0;  // Y坐标起始位置
        block2->w = 20; // 方块宽度：20像素
        block2->h = 20; // 方块高度：20像素

        // 动画属性配置
        block2->anim_dir = 2;         // 动画方向：右移（2代表右移方向，与block1定义可能不同）
        block2->anim_target_val = 40; // 动画目标：移动到40像素位置
        block2->anim_speed = 0;       // 动画速度：初始为0（暂停状态）
    }

    while (1)
    {
        // 从按键队列接收状态（阻塞等待，直到有数据）
        if (xQueueReceive(queue_keyscan_handler, &key_state, portMAX_DELAY) == pdPASS)
        {
            // 短按（key_state=1）：切换动画状态（启动/暂停/恢复）
            if (key_state == 1)
            {
                GeneralCmd_t cmd1; // 针对图形1的指令
                cmd1.target_id = 1;
                GeneralCmd_t cmd2; // 针对图形2的指令
                cmd2.target_id = 2;

                // 根据当前状态设置指令类型
                if (block1->state == STATE_READY || block2->state == STATE_READY)
                {
                    // 若图形处于就绪状态，发送启动动画指令
                    cmd1.cmd_type = CMD_ANIM_START;
                    cmd2.cmd_type = CMD_ANIM_START;
                }
                else if (block1->state == STATE_ANIM_RUNNING || block2->state == STATE_ANIM_RUNNING)
                {
                    // 若动画正在运行，发送暂停指令
                    cmd1.cmd_type = CMD_ANIM_PAUSE;
                    cmd2.cmd_type = CMD_ANIM_PAUSE;
                }
                else if (block1->state == STATE_ANIM_PAUSED || block2->state == STATE_ANIM_PAUSED)
                {
                    // 若动画已暂停，发送恢复指令
                    cmd1.cmd_type = CMD_ANIM_RESUME;
                    cmd2.cmd_type = CMD_ANIM_RESUME;
                }

                // 发送指令到图形指令队列
                CMD_send(&cmd2);
                CMD_send(&cmd1);
                key_state = 0; // 清空按键状态
            }

            // 长按（key_state=2）：删除图形1并重新启动（示例逻辑）
            else if (key_state == 2)
            {
                GeneralCmd_t cmd;
                cmd.target_id = 1;

                // 发送删除图形指令
                cmd.cmd_type = CMD_DLETE_GRAPH_ONECE;
                CMD_send(&cmd);

                // 延时100ms等待删除完成
                vTaskDelay(100 / portTICK_PERIOD_MS);

                // 重新发送启动动画指令
                cmd.cmd_type = CMD_ANIM_START;
                CMD_send(&cmd);
                key_state = 0; // 清空按键状态
            }
        }
    }
}

/**
 * OLED任务：处理图形指令队列中的指令，并执行动画帧更新
 */
void task_oled(void *pvParameters)
{
    OLED_Init(); // 初始化OLED显示屏（配置I2C、初始化寄存器等）

    while (1)
    {
        GeneralCmd_t recv_cmd; // 存储接收到的图形指令

        // 循环接收并执行所有待处理的指令（非阻塞）
        while (CMD_receive(&recv_cmd) == pdPASS)
        {
            cmd_excute(recv_cmd); // 执行指令（如启动/暂停动画、删除图形等）
        }

        // 更新OLED显示：执行动画帧计算并刷新屏幕
        oled_excute();

        // 任务延时（控制帧率，避免频繁刷新）
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}