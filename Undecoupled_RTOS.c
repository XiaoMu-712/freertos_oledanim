#include "Undecoupled_RTOS.h"

static QueueHandle_t g_cmd_queue = NULL;
#define CMD_QUEUE_LENGTH 10
SemaphoreHandle_t g_graph_pool_mutex; // 内存池互斥锁
/**
 * @brief 初始化指令队列
 */
BaseType_t CMDandMUTEX_queue_init(void)
{
    if (g_cmd_queue != NULL)
        return pdPASS;
    g_graph_pool_mutex = xSemaphoreCreateMutex();
        g_cmd_queue = xQueueCreate(CMD_QUEUE_LENGTH, sizeof(GeneralCmd_t));
    return (g_cmd_queue != NULL) ? pdPASS : errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;
}

/**
 * @brief 先延时，后发送指令（非阻塞）
 * @note 延时后发送，确保前序硬件操作（如IIC）完成后再发新指令
 */
BaseType_t CMD_send(GeneralCmd_t *cmd)
{
    if (g_cmd_queue == NULL || cmd == NULL)
        return pdFALSE;

   

    // 非阻塞发送指令
    return xQueueSend(g_cmd_queue, cmd, 0);
}

/**
 * @brief 从队列接收指令（非阻塞）
 */
BaseType_t CMD_receive(GeneralCmd_t *cmd)
{
    if (g_cmd_queue == NULL || cmd == NULL)
        return pdFALSE;

    return xQueueReceive(g_cmd_queue, cmd, 0);
}

QueueHandle_t CMD_queue_get_handle(void)
{
    return g_cmd_queue;
}

/**
 * 注册函数：分配内存池空间+创建对象+初始化核心属性
 * @param target_id：用户指定唯一ID（需确保不重复）
 * @param content_type：内容类型（如CONTENT_TYPE_BLOCK）
 * @param anim_type：初始动画类型（0=无，1=弹出，2=移动）
 * @return 成功：对象指针；失败：NULL（内存满/ID重复/参数错）
 */
GeneralGraph_t *
Graph_ID_Register(uint8_t target_id, ContentType content_type, uint8_t anim_type)
{
    // 入参校验：ID不能为0，类型/动画类型有效
    if (target_id == 0)
    {
        return NULL;
    }

    // 检查ID是否重复（避免同一ID注册多次）
    for (uint8_t i = 0; i < MAX_GRAPH_TOTAL; i++)
    {
        if (g_graph_static_pool[i].state != STATE_IDLE && g_graph_static_pool[i].id == target_id)
        {
            return NULL;
        }
    }

    // 找内存池空闲块，初始化对象
    for (uint8_t i = 0; i < MAX_GRAPH_TOTAL; i++)
    {
        if (g_graph_static_pool[i].state == STATE_IDLE)
        {
            GeneralGraph_t *new_graph = &g_graph_static_pool[i];

            xSemaphoreTake(g_graph_pool_mutex, portMAX_DELAY); // 申请互斥锁
                memset(new_graph, 0, sizeof(GeneralGraph_t)); // 清空旧数据
                                                              // 假设按键短按时翻转LED状态
            // 核心属性初始化（注册即绑定）
            new_graph->id = target_id;
            new_graph->content_type = content_type;
            new_graph->anim_type = anim_type;
            new_graph->state = STATE_READY;  // 初始状态：就绪
            new_graph->is_show = GRAPH_HIDE; // 初始隐藏

            // 动画默认参数（用户后续可直接修改结构体赋值）
            new_graph->anim_speed = 10; // 默认10ms/帧
            new_graph->anim_last_tick = 0;
            xSemaphoreGive(g_graph_pool_mutex); // 释放互斥锁
            return new_graph;
        }
    }

    return NULL; // 内存池满
}

/**
 * 查找函数：按ID从内存池找对象
 * @param target_id：要找的ID
 * @return 成功：对象指针；失败：NULL（ID不存在/未注册）
 */
GeneralGraph_t *Graph_ID_Find(uint8_t target_id)
{
    if (target_id == 0)
    {
        return NULL;
    }

    // 遍历内存池，找非空闲且ID匹配的对象
    for (uint8_t i = 0; i < MAX_GRAPH_TOTAL; i++)
    {
        if (g_graph_static_pool[i].state != STATE_IDLE && g_graph_static_pool[i].id == target_id)
        {
            return &g_graph_static_pool[i];
        }
    }

    return NULL;
}
