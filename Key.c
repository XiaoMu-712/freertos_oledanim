#include "gpio_common.h" 
#include "Key.h"  

// -------------------------- 按键初始化 --------------------------  
uint8_t Key_Init(Key_ObjectTypeDef *key_obj) 
{
    if(key_obj->Pull_Mode == 1) 	
    {
        GPIO_PuIn_Init(key_obj->GPIO_Pin); 		
        key_obj->key_state = 0;
        key_obj->last_level = 1;
        return 1; 	
    } 	
    if(key_obj->Pull_Mode == 0) 	
    {
        GPIO_PdIn_Init(key_obj->GPIO_Pin);  		
        key_obj->key_state = 0;
        key_obj->last_level = 0;
        return 1; 	
    } 	
    else 
        return 0;    	
}  

// -------------------------- 下拉-仅点击扫描 -------------------------- 
uint8_t Key_Scan_ClickOnly(Key_ObjectTypeDef *key_obj) 
{        
    uint8_t curr_level = GPIO_ReadIn(key_obj->GPIO_Pin);
    Key_delay(2);
    if (curr_level != GPIO_ReadIn(key_obj->GPIO_Pin)) 
    {
        key_obj->last_level = curr_level;
        return KEY_NONE;
    }      

    switch (key_obj->key_state) 
    {
        case 0:              
            if (curr_level == 1 && key_obj->last_level == 0)
            {
                key_obj->press_tick = Key_gettime();
                key_obj->key_state = 1;
            }
            break;          

        case 1: 
            if (curr_level == 0)
            {
                uint32_t press_duration = Key_gettime() - key_obj->press_tick;
                key_obj->key_state = 0;
                key_obj->last_level = curr_level;                 

                return (press_duration >= key_obj->Long_Start_ms) ? KEY_CLICK_LONG : KEY_CLICK_SHORT;
            }
            break;
    }  

    key_obj->last_level = curr_level;
    return KEY_NONE;
}  

// -------------------------- 下拉-长按间隔扫描 -------------------------- 
uint8_t Key_Scan_LongLoop(Key_ObjectTypeDef *key_obj) 
{        
    uint8_t curr_level = GPIO_ReadIn(key_obj->GPIO_Pin);
    Key_delay(2);
    if (curr_level != GPIO_ReadIn(key_obj->GPIO_Pin)) 
    {
        key_obj->last_level = curr_level;
        return KEY_NONE;
    }  

    switch (key_obj->key_state) 
    {
        case 0: 
            if (curr_level == 1 && key_obj->last_level == 0)
            {
                key_obj->press_tick = Key_gettime();
                key_obj->last_send_tick = key_obj->press_tick;
                key_obj->key_state = 1;
            }
            break;          

        case 1: 
            if (curr_level == 0) 
            {
                key_obj->key_state = 0;
                key_obj->last_level = curr_level;
                return KEY_CLICK_SHORT;
            }
            else if ((Key_gettime() - key_obj->press_tick) >= key_obj->Long_Start_ms) 
            {
                key_obj->key_state = 2;
                return KEY_LONG_LOOP;
            }
            break;          

        case 2: 
            if (curr_level == 0) 
            {
                key_obj->key_state = 0;
                key_obj->last_level = curr_level;
                return KEY_NONE;
            }
            else if ((Key_gettime() - key_obj->last_send_tick) >= key_obj->Long_Interval_ms) 
            {
                key_obj->last_send_tick = Key_gettime();
                return KEY_LONG_LOOP;
            }
            break;
    }  

    key_obj->last_level = curr_level;
    return KEY_NONE;
}  

// -------------------------- 上拉-仅点击扫描 -------------------------- 
uint8_t Key_Scan_ClickOnly_PullUp(Key_ObjectTypeDef *key_obj) 
{    
    uint8_t curr_level = GPIO_ReadIn(key_obj->GPIO_Pin);
    Key_delay(2);
    if (curr_level != GPIO_ReadIn(key_obj->GPIO_Pin)) 
    {
        key_obj->last_level = curr_level;
        return KEY_NONE;
    }  

    switch (key_obj->key_state) 
    {
        case 0: 
            if (curr_level == 0 && key_obj->last_level == 1) 
            {
                key_obj->press_tick = Key_gettime();
                key_obj->key_state = 1;
            }
            break;          

        case 1: 
            if (curr_level == 1) 
            {
                uint32_t press_duration = Key_gettime() - key_obj->press_tick;
                key_obj->key_state = 0;
                key_obj->last_level = curr_level;                 

                return (press_duration >= key_obj->Long_Start_ms) ? KEY_CLICK_LONG : KEY_CLICK_SHORT;
            }
            break;
    }  

    key_obj->last_level = curr_level;
    return KEY_NONE;
}  

// -------------------------- 上拉-长按间隔扫描 -------------------------- 
uint8_t Key_Scan_LongLoop_PullUp(Key_ObjectTypeDef *key_obj) 
{    
    uint8_t curr_level = GPIO_ReadIn(key_obj->GPIO_Pin);
    Key_delay(2);
    if (curr_level != GPIO_ReadIn(key_obj->GPIO_Pin)) 
    {
        key_obj->last_level = curr_level;
        return KEY_NONE;
    }  

    switch (key_obj->key_state) 
    {
        case 0: 
            if (curr_level == 0 && key_obj->last_level == 1) 
            {
                key_obj->press_tick = Key_gettime();
                key_obj->last_send_tick = key_obj->press_tick;
                key_obj->key_state = 1;
            }
            break;          

        case 1: 
            if (curr_level == 1) 
            {
                key_obj->key_state = 0;
                key_obj->last_level = curr_level;
                return KEY_CLICK_SHORT;
            }
            else if ((Key_gettime() - key_obj->press_tick) >= key_obj->Long_Start_ms) 
            {
                key_obj->key_state = 2;
                return KEY_LONG_LOOP;
            }
            break;          

        case 2: 
            if (curr_level == 1) 
            {
                key_obj->key_state = 0;
                key_obj->last_level = curr_level;
                return KEY_NONE;
            }
            else if ((Key_gettime() - key_obj->last_send_tick) >= key_obj->Long_Interval_ms) 
            {
                key_obj->last_send_tick = Key_gettime();
                return KEY_LONG_LOOP;
            }
            break;
    }  

    key_obj->last_level = curr_level;
    return KEY_NONE;
}  

// -------------------------- 通用扫描函数 -------------------------- 
uint8_t Key_Scan_General(Key_ObjectTypeDef *key_obj) 
{
    if (key_obj == NULL || key_obj->Long_Start_ms == 0) 
    {
        return KEY_NONE;
    }  

    if (key_obj->Pull_Mode == 0) 
    {  // 下拉输入        
        if (key_obj->Func_Sel == 0) 
        {
            return Key_Scan_ClickOnly(key_obj);         
        } 
        else if (key_obj->Func_Sel == 1) 
        {
            return Key_Scan_LongLoop(key_obj);         
        }
        else if (key_obj->Func_Sel == 2) 
        {
            Exti_Pd_Init(key_obj->GPIO_Pin, key_obj->exti_priority);
        } 
    }
    else if (key_obj->Pull_Mode == 1) 
    {  // 上拉输入        
        if (key_obj->Func_Sel == 0) 
        {
            return Key_Scan_ClickOnly_PullUp(key_obj);
        } 
        else if (key_obj->Func_Sel == 1) 
        {
            return Key_Scan_LongLoop_PullUp(key_obj);         
        }
    }  

    return KEY_NONE;
}
