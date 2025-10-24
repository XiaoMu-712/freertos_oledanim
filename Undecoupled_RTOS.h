#ifndef __UNDECOUPLED_RTOS_H
#define __UNDECOUPLED_RTOS_H
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "OLED_basicAnim.h"
/**
 * @brief 初始化指令队列（必须在任务启动前调用）
 * @return 成功返回pdPASS，失败返回errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY
 */
BaseType_t CMDandMUTEX_queue_init(void);

/**
 * @brief 先延时后发送指令（非阻塞，带硬件同步延时）
 * @param cmd：待发送的指令指针（必须为有效指针）
 * @return 成功返回pdPASS，队列满/参数无效返回pdFALSE
 */
BaseType_t CMD_send(GeneralCmd_t *cmd);

/**
 * @brief 从队列接收指令（非阻塞）
 * @param cmd：存储接收指令的缓冲区指针（必须为有效指针）
 * @return 成功返回pdPASS，无指令/参数无效返回pdFALSE
 */
BaseType_t CMD_receive(GeneralCmd_t *cmd);

/**
 * @brief 获取指令队列句柄（仅用于调试）
 * @return 队列句柄，未初始化返回NULL
 */
QueueHandle_t CMD_queue_get_handle(void);
// 注册：分配内存+创建对象+绑定ID
GeneralGraph_t *Graph_ID_Register(uint8_t target_id, ContentType content_type, uint8_t anim_type);
// 查找：按ID从内存池找对象
GeneralGraph_t *Graph_ID_Find(uint8_t target_id);
#endif

