/** @file         sconfig_item.c
 *  @brief        解析 键值对的有关实现
 *  @details      详细说明
 *  @author       Schips
 *  @date         2021-01-28 10:52:17
 *  @version      v1.0
 *  @copyright    Copyright By Schips, All Rights Reserved
 */

#include <stdio.h>
#include <string.h>
#include "openfsm.h"
#include "sconfig.h"
#include "sconfig_item.h"

// 每次解析时用于存放解析字符的容器
static char tmp_var_buff[516]; 
static int  tmp_var_buff_index;
//static int  tmp_item_flag;

void init_tmp_var_buff(void)
{
    memset(tmp_var_buff, 0, sizeof(tmp_var_buff));
    tmp_var_buff_index = 0;
}

char *get_tmp_buff_entry(void)
{
    printf("-->Current tmp buff has: {%s}\n", tmp_var_buff);
    return tmp_var_buff;
}

char push_ch_to_tmp_buff(char ch)
{
    if (tmp_var_buff_index >= sizeof(tmp_var_buff))
    {
        return '\0';
    }

    tmp_var_buff[tmp_var_buff_index]     = ch;
    tmp_var_buff[sizeof(tmp_var_buff)-1] = '\0';

    tmp_var_buff_index++;
    return ch;
}

// 把临时缓冲区中的数据复制作为当前的key
int save_tmp_var_as_cur_key(void)
{
    get_tmp_buff_entry();
    init_tmp_var_buff();

    return 0;
}

// 把临时缓冲区中的数据复制作为当前的key
int save_tmp_var_as_cur_val(void)
{
    get_tmp_buff_entry();
    init_tmp_var_buff();

    return 0;
}

void* step_item_get_chars_start(void* this_fsm)
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
            break;
    }

    return NULL;
}

void* step_item_get_key(void* this_fsm)
{
    Config *conf = get_data_entry(this_fsm);

    switch(conf->p_tmp_buff[0])
    {
        // 跳过 键之间的 空白字符(键内的空格会被合并)
        case '\t' :
        case ' '  :
            conf->p_tmp_buff++;
            break;
        // 获取 键 完成, 准备获取 值
        case '='  :
            save_tmp_var_as_cur_key();
            set_next_state(this_fsm, state_item_get_val);
            break;

        default :
            // 累计到item中
            push_ch_to_tmp_buff(conf->p_tmp_buff[0]);
            conf->p_tmp_buff++;
            break;
    }
    return NULL;
}

void* step_item_get_val(void* this_fsm)
{
    Config *conf = get_data_entry(this_fsm);

    switch(conf->p_tmp_buff[0])
    {
        case '\n' :
        case '\0' :
            save_tmp_var_as_cur_val();
            set_next_state(this_fsm, state_item_head_done);
            break;
        // 跳过 = 以后的 空白字符
        case '='  :
        case '\t' :
        case ' '  :
            conf->p_tmp_buff++;
            //push_ch_to_tmp_buff('-');
            break;

        default :
            // 累计
            push_ch_to_tmp_buff(conf->p_tmp_buff[0]);
            conf->p_tmp_buff++;
            break;
    }

    return NULL;
}

void* step_item_head_done(void* this_fsm)
{
    return NULL;
}

