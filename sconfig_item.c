/** @file         sconfig_item.c
 *  @brief        简要说明
 *  @details      详细说明
 *  @author       Schips
 *  @date         2021-01-28 10:52:17
 *  @version      v1.0
 *  @copyright    Copyright By Schips, All Rights Reserved
 *
 **********************************************************
 *
 *  @attention    NOTE
 *                SDK: 
 *                ENV: 
 *  @par 修改日志:
 *  <table>
 *  <tr><th>Date       <th>Version   <th>Author    <th>Description
 *  <tr><td>2021-01-28 <td>1.0       <td>Schips    <td>创建初始版本
 *  </table>
 *
 **********************************************************
 */

#include <stdio.h>
#include "openfsm.h"
#include "sconfig.h"
#include "sconfig_item.h"

// 每次解析时用于存放解析字符的容器
static char tmp_item_name[128]; 
static int  tmp_item_name_index;
//static int  tmp_item_flag;

void init_tmp_item_name(void)
{
    //printf("init_tmp_item_name\n");
    memset(tmp_item_name, 0, sizeof(tmp_item_name));
    tmp_item_name_index = 0;
}

char *get_tmp_item_name(void)
{
    //printf("get_tmp_item_name : [%s]\n", tmp_item_name);
    return tmp_item_name;
}

char push_item_name(char ch)
{
    if (tmp_item_name_index > sizeof(tmp_item_name))
    {
        return;
    }
    tmp_item_name[tmp_item_name_index]   = ch;
    //tmp_item_name[tmp_item_name_index+1] = '\0';
    tmp_item_name_index++;
    //printf("%c\n", ch);
}

void* step_item_get_chars_start(void* this_fsm) //计数
{
    Config *conf = get_data_entry(this_fsm);

    switch(conf->p_tmp_buff[0])
    {
        // 跳过 '[' 以后到字符之前的空格
        case '\t' :
        case ' '  :
            conf->p_tmp_buff++;
            break;
        // 如果遇到 结尾视为错误结束
        case '\0' :
        case ']'  :
        case '\n' :
            set_next_state(this_fsm, state_item_head_done);
            set_fsm_error_flag(this_fsm);
            break;
        // 如果遇到 '=' 则视为获取 键完成
        case '=' :
            set_next_state(this_fsm, state_item_get_val);
            break;
        // 否则进入获取连续字符模式
        default:
            set_next_state(this_fsm, state_item_get_key);
            //push_item_name(conf->p_tmp_buff[0]);
            //conf->p_tmp_buff++;
            break;
    }

    return NULL;
}

void* step_item_get_key(void* this_fsm) //计数完成
{
    Config *conf = get_data_entry(this_fsm);

    switch(conf->p_tmp_buff[0])
    {
        // 跳过 键之间的 空白字符
        case '\t' :
        case ' '  :
            //printf("Skip {%x}\n", conf->p_tmp_buff[0]);
            conf->p_tmp_buff++;
            break;
        // 正常结束
        case '='  :
            //push_item_name('\0');
            printf("[===], key is {%s}\n", get_tmp_item_name());
            init_tmp_item_name();
            set_next_state(this_fsm, state_item_get_val);
            break;

        default :
            // 累计到item中
            push_item_name(conf->p_tmp_buff[0]);
            conf->p_tmp_buff++;
            break;
    }
    return NULL;
}

void* step_item_get_val(void* this_fsm) //计数完成
{
    Config *conf = get_data_entry(this_fsm);
    //printf("get val\n");
    switch(conf->p_tmp_buff[0])
    {
#if 1
        case '\n' :
        case '\0' :
            printf("[EOL], val is {%s}\n", get_tmp_item_name());
            set_next_state(this_fsm, state_item_head_done);
            init_tmp_item_name();
            break;
        // 跳过 = 以后的 空白字符
        case '='  :
        case '\t' :
        case ' '  :
            conf->p_tmp_buff++;
            //push_item_name('-');
            break;
        // 正常结束
            break;

        default :
            // 累计
            push_item_name(conf->p_tmp_buff[0]);
            conf->p_tmp_buff++;
            break;
#endif
    }

    return NULL;
}

void* step_item_head_done(void* this_fsm)
{
    //init_tmp_item_name();
    //printf("step_item_head_done\n" );
    return NULL;
}

