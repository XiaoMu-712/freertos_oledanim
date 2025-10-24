#include "OLED.h"
#include "OLED_basicAnim.h"
#include "gpio_common.h"
#include "Undecoupled_RTOS.h"
// 1. 唯一全局内存池（容量按需调整，如16个对象）
const uint8_t MAX_GRAPH_TOTAL = 16;
volatile GeneralGraph_t  g_graph_static_pool[MAX_GRAPH_TOTAL] = {0};





/**
 * @brief 色块弹出单帧执行（新结构体适配版，无残留，清旧区→画新区→局部刷新）
 * @param graph：要执行的图形对象（GeneralGraph_t 类型）
 */



void Disp_BlockPopOneFrame(GeneralGraph_t *graph)
{
    // 入参+状态校验：对象空/未显示/未运行，直接返回
    if (graph == NULL || graph->is_show != GRAPH_SHOW || graph->state != STATE_ANIM_RUNNING)
    {
        return;
    }
   
    // 1. 帧间隔控制（严格按 anim_speed 控制速度，修复“一直退出”问题）
    uint32_t curr_tick
    = OLED_gettime();
    

  

   

    if (curr_tick - graph->anim_last_tick < graph->anim_speed)
    {
        return;
    }
  
    graph->anim_last_tick = curr_tick; // 必须更新时间戳，否则永远卡帧
    
    // 2. 初始化上一帧旧区域（用于清残留，默认取当前坐标和尺寸）
    uint8_t last_old_x = graph->x;
    uint8_t last_old_y = graph->y;
    uint8_t last_old_w = graph->anim_curr_val; // 旧宽度=上一帧进度（关键！）
    uint8_t last_old_h = graph->h;             // 高度默认不变（按旧逻辑）

    // 3. 更新当前帧进度（每次+1像素，到目标值则停止动画）
    graph->anim_curr_val++;
    if (graph->anim_curr_val > graph->anim_target_val)
    {
        graph->anim_curr_val = graph->anim_target_val;
        graph->state = STATE_ANIM_PAUSED; // 动画结束，切换为暂停状态
    }

    // 4. 按方向计算「旧区域（清残留）」和「新区域（当前帧）」（完全对齐旧逻辑）
    uint8_t new_x = graph->x;             // 新区域X默认值
    uint8_t new_y = graph->y;             // 新区域Y默认值
    uint8_t new_w = graph->anim_curr_val; // 新宽度=当前进度（关键！）
    uint8_t new_h = graph->h;             // 新高度默认不变

    switch (graph->anim_dir)
    {
    case 1: // 1=上弹出：从下往上扩展（高度变化，Y偏移）
        // 旧区域：上一帧高度=上一帧进度，Y=目标Y + (目标高度-上一帧进度)
        last_old_h = (graph->anim_curr_val > 1) ? (graph->anim_curr_val - 1) : 0;
        last_old_y = graph->y + (graph->anim_target_val - last_old_h);
        last_old_w = graph->w; // 宽度不变
        // 新区域：当前高度=当前进度，Y=目标Y + (目标高度-当前进度)
        new_h = graph->anim_curr_val;
        new_y = graph->y + (graph->anim_target_val - new_h);
        break;

    case 2: // 2=下弹出：从上往下扩展（高度变化，Y不变）
        // 旧区域：上一帧高度=上一帧进度，Y=目标Y
        last_old_h = (graph->anim_curr_val > 1) ? (graph->anim_curr_val - 1) : 0;
        last_old_y = graph->y;
        last_old_w = graph->w; // 宽度不变
        // 新区域：当前高度=当前进度，Y=目标Y
        new_h = graph->anim_curr_val;
        break;

    case 3: // 3=左弹出：从左往右扩展（宽度变化，X不变）
        // 旧区域：上一帧宽度=上一帧进度，X=目标X
        last_old_w = (graph->anim_curr_val > 1) ? (graph->anim_curr_val - 1) : 0;
        last_old_x = graph->x;
        last_old_h = graph->h; // 高度不变
        // 新区域：当前宽度=当前进度，X=目标X
        new_w = graph->anim_curr_val;
        break;

    case 4: // 4=右弹出：从右往左扩展（宽度变化，X偏移）
        // 旧区域：上一帧宽度=上一帧进度，X=目标X + (目标宽度-上一帧进度)
        last_old_w = (graph->anim_curr_val > 1) ? (graph->anim_curr_val - 1) : 0;
        last_old_x = graph->x + (graph->anim_target_val - last_old_w);
        last_old_h = graph->h; // 高度不变
        // 新区域：当前宽度=当前进度，X=目标X + (目标宽度-当前进度)
        new_w = graph->anim_curr_val;
        new_x = graph->x + (graph->anim_target_val - new_w);
        break;

    default: // 默认左弹出（兼容异常方向）
        last_old_w = (graph->anim_curr_val > 1) ? (graph->anim_curr_val - 1) : 0;
        last_old_x = graph->x;
        last_old_h = graph->h;
        new_w = graph->anim_curr_val;
        break;
    }

    // 5. 清上一帧旧区域（关键：避免残留，必须用反色恢复背景）
    if (last_old_w > 0 && last_old_h > 0) // 旧区域有效才清，避免无效操作
    {
        OLED_ReverseArea(last_old_x, last_old_y, last_old_w, last_old_h);
    }

    // 6. 画当前帧新区域（用反色绘制，与旧逻辑一致）
    OLED_ReverseArea(new_x, new_y, new_w, new_h);

    // 7. 局部刷新（只刷必要区域，比全局快，修复“无法显示”问题）
    uint8_t refresh_x = (last_old_x < new_x) ? last_old_x : new_x; // 刷新起始X（取小值）
    uint8_t refresh_y = (last_old_y < new_y) ? last_old_y : new_y; // 刷新起始Y（取小值）
    // 刷新结束X/Y（取旧区和新区的最大边界）
    uint8_t refresh_end_x = (last_old_x + last_old_w) > (new_x + new_w) ? (last_old_x + last_old_w) : (new_x + new_w);
    uint8_t refresh_end_y = (last_old_y + last_old_h) > (new_y + new_h) ? (last_old_y + last_old_h) : (new_y + new_h);
    // 执行局部刷新（宽=结束X-起始X，高=结束Y-起始Y）
    OLED_UpdateArea(refresh_x, refresh_y,
                    refresh_end_x - refresh_x,
                    refresh_end_y - refresh_y);
}

void Disp_VideoFramePlay(GeneralGraph_t *graph)
{
    // 1. 入参+状态+类型+动画类型四重校验（严格匹配最新定义）
    if (graph == NULL)
        return; // 空对象过滤
    if (graph->is_show != GRAPH_SHOW)
        return; // 未显示过滤
    if (graph->state != STATE_ANIM_RUNNING)
        return; // 非运行状态过滤
    if (graph->content_type != CONTENT_TYPE_VIDEO)
        return; // 仅视频类型执行
    if (graph->anim_type != 3)
        return; // 复用anim_type=3标记视频动画
    if (graph->param.video.vd_mod == NULL)
        return; // 视频数据为空过滤（修正：param.video）

    // 2. 帧间隔控制（复用anim_speed，与其他动画速度逻辑一致）
    uint32_t curr_tick = OLED_gettime();
    if (curr_tick - graph->anim_last_tick < graph->anim_speed)
    {
        return; // 未到帧间隔，跳过当前帧
    }
    graph->anim_last_tick = curr_tick; // 更新时间戳

    // 3. 视频帧核心参数（复用现有字段，无新增）
    uint8_t curr_frame_idx = graph->serial_num;              // 当前帧索引（用serial_num记录）
    const uint8_t *video_frames = graph->param.video.vd_mod; // 视频数据首地址（修正：param.video.vd_mod）
    uint8_t frame_w = graph->w;                              // 单帧宽度（复用图形w字段）
    uint8_t frame_h = graph->h;                              // 单帧高度（复用图形h字段）
    uint8_t frame_byte_len = (frame_w * frame_h + 7) / 8;    // 单帧字节数（8像素1字节，适配OLED字模格式）

    // 4. 清除上一帧残留（固定坐标，直接清除显示区域，避免拖尾）
    OLED_ClearArea(graph->x, graph->y, frame_w, frame_h);

    // 5. 绘制当前帧（按帧索引计算偏移，复用图片显示函数）
    // 视频帧格式：连续存储的多帧图片，单帧格式与CONTENT_TYPE_IMAGE完全一致
    const uint8_t *curr_frame_data = video_frames + (curr_frame_idx * frame_byte_len);
    OLED_ShowImage(graph->x, graph->y, frame_w, frame_h, curr_frame_data);

    // 6. 局部刷新（仅刷新视频区域，减少闪烁）
    OLED_UpdateArea(graph->x, graph->y, frame_w, frame_h);

    // 7. 帧索引更新（处理循环/非循环，复用cycle_en和anim_target_val）
    uint8_t total_frames = graph->anim_target_val; // 总帧数（用anim_target_val存储）
    if (curr_frame_idx + 1 >= total_frames)
    {
        if (graph->param.video.cycle_en == 1) // 循环播放（修正：param.video.cycle_en）
        {
            graph->serial_num = 0; // 重置为第0帧
        }
        else // 非循环播放：停在最后一帧
        {
            graph->serial_num = total_frames - 1;
            graph->state = STATE_ANIM_PAUSED; // 切换为暂停状态
        }
    }
    else
    {
        graph->serial_num++; // 下一帧索引
    }
}
/**
 * 通用平移动画单帧执行函数（适配所有content_type，输入为原GeneralGraph_t结构体）
 * @param graph：要执行的图形对象指针（需先注册+设为STATE_ANIM_RUNNING状态+anim_type=2）
 */
void Disp_CommonMoveOneFrame(GeneralGraph_t *graph)
{
    // -------------------------- 1. 共性：基础校验（所有平移函数通用） --------------------------
    if (graph == NULL)
        return; // 空对象过滤
    if (graph->is_show != GRAPH_SHOW)
        return; // 未显示过滤
    if (graph->state != STATE_ANIM_RUNNING)
        return; // 非运行状态过滤
    if (graph->anim_type != 2)
        return; // 仅处理移动动画（anim_type=2）

    // -------------------------- 2. 共性：帧间隔控制（确保速度稳定） --------------------------
    uint32_t curr_tick = OLED_gettime();
    if (curr_tick - graph->anim_last_tick < graph->anim_speed)
    {
        return; // 未到帧间隔，跳过当前帧
    }
    graph->anim_last_tick = curr_tick; // 更新时间戳，避免卡帧

    // -------------------------- 3. 共性：记录旧区域+首帧标记（解决残留） --------------------------
    uint8_t last_old_x = graph->x;                                // 旧区域X坐标
    uint8_t last_old_y = graph->y;                                // 旧区域Y坐标
    uint8_t content_w = 0;                                        // 内容总宽度（差异化计算）
    uint8_t content_h = 0;                                        // 内容总高度（差异化计算）
    uint8_t is_first_frame = (graph->anim_curr_val == 0) ? 1 : 0; // 首帧标记（首帧不清理旧区）

    // -------------------------- 4. 差异化：按content_type计算宽高（核心分支1） --------------------------
    switch (graph->content_type)
    {
    case CONTENT_TYPE_BLOCK: // 色块：宽高直接读结构体
        content_w = graph->w;
        content_h = graph->h;
        break;

    case CONTENT_TYPE_IMAGE: // 图片：宽高直接读结构体
        content_w = graph->w;
        content_h = graph->h;
        // 额外校验图片数据（避免空指针）
        if (graph->param.image.img_mod == NULL)
            return;
        break;

    case CONTENT_TYPE_CHINESE: // 汉字：16x16字体，宽=16×个数，高=16
        content_w = 16 * graph->param.chinese.ch_count;
        content_h = 16;
        // 额外校验汉字字符串（避免空指针）
        if (graph->param.chinese.ch_mod == NULL)
            return;
        break;

    case CONTENT_TYPE_ASCII: // ASCII：按字体大小算宽高
    {
        uint8_t font_size = graph->param.ascii.font_size;
        uint8_t char_w = (font_size == OLED_6X8) ? 6 : 8;
        content_w = char_w * strlen(graph->param.ascii.str);
        content_h = (font_size == OLED_6X8) ? 8 : 16;
        // 额外校验ASCII字符串（避免空指针）
        if (graph->param.ascii.str == NULL)
            return;
    }
    break;

    case CONTENT_TYPE_NUM: // 数字：按字体大小+位数算宽高
    {
        uint8_t font_size = graph->param.num.font_size;
        uint8_t num_single_w = (font_size == OLED_6X8) ? 6 : 8;
        content_w = num_single_w * graph->param.num.num_count;
        content_h = (font_size == OLED_6X8) ? 8 : 16;
    }
    break;

    default: // 未知类型：直接返回，避免异常
        return;
    }

    // -------------------------- 5. 共性：更新进度与坐标（逐像素移动） --------------------------
    graph->anim_curr_val++;
    // 按anim_dir更新X/Y坐标（方向定义不变：1=上/2=下/3=左/4=右）
    switch (graph->anim_dir)
    {
    case 1:
        graph->y--;
        break;
    case 2:
        graph->y++;
        break;
    case 3:
        graph->x--;
        break;
    case 4:
        graph->x++;
        break;
    default:
        graph->x;
        break; // 默认右移，兼容异常方向
    }

    // 进度达标：切换为暂停状态
    if (graph->anim_curr_val > graph->anim_target_val)
    {
        graph->anim_curr_val = graph->anim_target_val;
        graph->state = STATE_ANIM_PAUSED;
    }

    // -------------------------- 6. 共性：清除旧区域残留（非首帧执行） --------------------------
    if (!is_first_frame && content_w > 0 && content_h > 0)
    {
        OLED_ClearArea(last_old_x, last_old_y, content_w, content_h);
    }

    // -------------------------- 7. 差异化：绘制新区域（核心分支2） --------------------------
    uint8_t new_x = graph->x; // 新区域X坐标
    uint8_t new_y = graph->y; // 新区域Y坐标
    switch (graph->content_type)
    {
    case CONTENT_TYPE_BLOCK:
        OLED_ReverseArea(new_x, new_y, content_w, content_h);
        break;

    case CONTENT_TYPE_IMAGE:
        OLED_ShowImage(new_x, new_y, content_w, content_h, graph->param.image.img_mod);
        break;

    case CONTENT_TYPE_CHINESE:
        OLED_ShowChinese(new_x, new_y, (char *)graph->param.chinese.ch_mod);
        break;

    case CONTENT_TYPE_ASCII:
        OLED_ShowString(new_x, new_y, (char *)graph->param.ascii.str, graph->param.ascii.font_size);
        break;

    case CONTENT_TYPE_NUM:
        OLED_ShowNum(new_x, new_y, graph->param.num.num_val,
                     graph->param.num.num_count, graph->param.num.font_size);
        break;

    default:
        break;
    }

    // -------------------------- 8. 共性：局部刷新（合并旧区+新区，减少闪烁） --------------------------
    uint8_t refresh_x, refresh_y, refresh_end_x, refresh_end_y;
    if (is_first_frame)
    {
        // 首帧：仅刷新新区域
        refresh_x = new_x;
        refresh_y = new_y;
        refresh_end_x = new_x + content_w;
        refresh_end_y = new_y + content_h;
    }
    else
    {
        // 非首帧：刷新旧区+新区的最小包围矩形
        refresh_x = (last_old_x < new_x) ? last_old_x : new_x;
        refresh_y = (last_old_y < new_y) ? last_old_y : new_y;
        refresh_end_x = (last_old_x + content_w) > (new_x + content_w) ? (last_old_x + content_w) : (new_x + content_w);
        refresh_end_y = (last_old_y + content_h) > (new_y + content_h) ? (last_old_y + content_h) : (new_y + content_h);
    }
    // 执行局部刷新
    OLED_UpdateArea(refresh_x, refresh_y,
                    refresh_end_x - refresh_x,
                    refresh_end_y - refresh_y);
}
void oled_excute(void)
{
    for (uint8_t i = 0; i < MAX_GRAPH_TOTAL; i++)
    {
        GeneralGraph_t *curr_graph = &g_graph_static_pool[i];

        if ((curr_graph->state == STATE_STATIC || (curr_graph->state == STATE_ANIM_PAUSED && curr_graph->anim_type != 1)) && curr_graph->is_show == GRAPH_SHOW && curr_graph->z_order == Z_BACKGROUND)
        {
            if (curr_graph->content_type == CONTENT_TYPE_BLOCK)
            {
                OLED_ReverseArea(curr_graph->x, curr_graph->y, curr_graph->w, curr_graph->h);
                OLED_ReverseArea(curr_graph->x, curr_graph->y, curr_graph->w, curr_graph->h);
                OLED_UpdateArea(curr_graph->x, curr_graph->y, curr_graph->w, curr_graph->h);
            }
            if (curr_graph->content_type == CONTENT_TYPE_CHINESE)
            {
                OLED_ShowChinese(curr_graph->x, curr_graph->y, curr_graph->param.chinese.ch_mod);

                OLED_UpdateArea(curr_graph->x, curr_graph->y, curr_graph->w, curr_graph->h);
            }
            if (curr_graph->content_type == CONTENT_TYPE_ASCII)
            {
                OLED_ShowString(curr_graph->x, curr_graph->y, curr_graph->param.ascii.str, curr_graph->param.ascii.font_size);
                OLED_UpdateArea(curr_graph->x, curr_graph->y, curr_graph->w, curr_graph->h);
            }
            if (curr_graph->content_type == CONTENT_TYPE_NUM)
            {
                OLED_ShowNum(curr_graph->x, curr_graph->y, curr_graph->param.num.num_val, curr_graph->param.num.num_count, curr_graph->param.num.font_size);
                OLED_UpdateArea(curr_graph->x, curr_graph->y, curr_graph->w, curr_graph->h);
            }
        }
        // 修复赋值运算符=为比较运算符==
        else if (curr_graph->state == STATE_ANIM_RUNNING && curr_graph->is_show == GRAPH_SHOW)
        {

            if (curr_graph->anim_type == 1)
            {
                Disp_BlockPopOneFrame(curr_graph); // 弹出动画
            }

            if (curr_graph->anim_type == 2)
            {
                Disp_CommonMoveOneFrame(curr_graph); // 通用平移动画
            }
            if (curr_graph->anim_type == 3)
            {
                Disp_VideoFramePlay(curr_graph); // 视频动画
            }
        }

        else if ((curr_graph->state == STATE_STATIC || (curr_graph->state == STATE_ANIM_PAUSED && curr_graph->anim_type != 1)) && curr_graph->is_show == GRAPH_SHOW && curr_graph->z_order == Z_PROSPECT)
        {
            if (curr_graph->content_type == CONTENT_TYPE_BLOCK)
            {
                OLED_ReverseArea(curr_graph->x, curr_graph->y, curr_graph->w, curr_graph->h);
                OLED_ReverseArea(curr_graph->x, curr_graph->y, curr_graph->w, curr_graph->h);
                OLED_UpdateArea(curr_graph->x, curr_graph->y, curr_graph->w, curr_graph->h);
            }
            if (curr_graph->content_type == CONTENT_TYPE_CHINESE)
            {
                OLED_ShowChinese(curr_graph->x, curr_graph->y, curr_graph->param.chinese.ch_mod);

                OLED_UpdateArea(curr_graph->x, curr_graph->y, curr_graph->w, curr_graph->h);
            }
            if (curr_graph->content_type == CONTENT_TYPE_ASCII)
            {
                OLED_ShowString(curr_graph->x, curr_graph->y, curr_graph->param.ascii.str, curr_graph->param.ascii.font_size);
                OLED_UpdateArea(curr_graph->x, curr_graph->y, curr_graph->w, curr_graph->h);
            }
            if (curr_graph->content_type == CONTENT_TYPE_NUM)
            {
                OLED_ShowNum(curr_graph->x, curr_graph->y, curr_graph->param.num.num_val, curr_graph->param.num.num_count, curr_graph->param.num.font_size);
                OLED_UpdateArea(curr_graph->x, curr_graph->y, curr_graph->w, curr_graph->h);
            }
        }
    }
}

void cmd_excute(GeneralCmd_t recv_cmd)
{
    GeneralGraph_t *graph = Graph_ID_Find(recv_cmd.target_id);
    if (graph == NULL)
    {
        return; // 未找到对应图形对象，直接返回
    }

    // 执行指令（包含新增的注销指令）
    switch (recv_cmd.cmd_type)
    {
    case CMD_ANIM_START:
        graph->x = 0;
        graph->y = 0;
        graph->is_show = GRAPH_SHOW;            // 显示图形
        graph->state = STATE_ANIM_RUNNING;      // 启动动画
        graph->anim_curr_val = 0;               // 重置进度
        graph->anim_last_tick = OLED_gettime(); // 重置时间戳
        break;

    case CMD_ANIM_PAUSE:
        if (graph->state == STATE_ANIM_RUNNING)
            graph->state = STATE_ANIM_PAUSED; // 暂停
        break;

    case CMD_ANIM_RESUME:
        if (graph->state == STATE_ANIM_PAUSED)
        {
            graph->state = STATE_ANIM_RUNNING;      // 恢复
            graph->anim_last_tick = OLED_gettime(); // 重置时间戳
        }
        break;

    case CMD_ANIM_NEXTSERIAL:
        graph->serial_num++;
        break;

    case CMD_ANIM_OVERSERIAL:
        graph->serial_num = 0;
        break;

    case CMD_ANIM_CHANGE_DIR:
        graph->anim_dir = recv_cmd.param.new_dir; // 修改方向
        break;

    case CMD_ANIM_CHANGE_SPEED:
        graph->anim_speed = recv_cmd.param.new_speed; // 修改速度
        break;

    case CMD_ANIM_CHANGE_TARGETVAL:
        graph->anim_target_val = recv_cmd.param.new_target_val; // 修改目标值
        break;

    case CMD_ANIM_CHANGE_TYPE:
        graph->anim_type = recv_cmd.param.new_anim_type; // 修改动画类型
        break;

    case CMD_ANIM_CHANGE_CONTENTTYPE:
        graph->content_type = recv_cmd.param.new_content_type; // 修改内容类型
        break;

    case CMD_DLETE_GRAPH_ONECE:

        OLED_Clear();
        OLED_Update();

        break;

    // ---------------- 新增：注销图形指令处理 ----------------
    case CMD_DESTRUCT_GRAPH:
        // 1. 先清除图形在屏幕上的显示（避免残留）
        if (graph->is_show == GRAPH_SHOW)
        {

            OLED_ClearArea(graph->x, graph->y, graph->w, graph->h);
            OLED_UpdateArea(graph->x, graph->y, graph->w, graph->h);
        }

        // 2. 重置图形状态为“未使用”（释放内存池位置）
        graph->is_show = GRAPH_HIDE; // 隐藏图形
        graph->state = STATE_READY;  // 重置为初始状态
        graph->id = 0;               // 清除ID标记（表示该位置空闲）
        graph->serial_num = 0;       // 重置串行编号
        graph->anim_curr_val = 0;    // 清空进度
        graph->anim_type = 0;        // 清空动画类型
        // 可根据需要重置其他字段（如x/y/w/h等，避免残留旧数据）
        break;
        // --------------------------------------------------------

    default:
        break;
    }
}