/** @file         sconfig_item.c
 *  @brief        解析 键值对的有关实现
 *  @details      详细说明
 *  @author       Schips
 *  @date         2021-01-28 10:52:17
 *  @version      v1.0
 *  @copyright    Copyright By Schips, All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "openfsm.h"
#include "sconfig.h"
#include "sconfig_skl.h"
#include "sconfig_fsm_sub_item.h"

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
            // 保存 之前的内容为 key
            set_cur_key_name(tmp_var_snapshot());
            clean_tmp_var_snapshot();
            //save_tmp_var_as_cur_key();
            set_next_state(this_fsm, state_item_get_val);
            break;

        default :
            // 累计到item中
            save_ch_in_tmp_var(conf->p_tmp_buff[0]);
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
            //tmp_var_snapshot();
            set_next_state(this_fsm, state_item_head_done);
            clean_tmp_var_snapshot();
            break;
        // 跳过 = 以后的 空白字符
        case '='  :
        case '\t' :
        case ' '  :
            conf->p_tmp_buff++;
            break;

        default :
            // 累计
            save_ch_in_tmp_var(conf->p_tmp_buff[0]);
            conf->p_tmp_buff++;
            break;
    }

    return NULL;
}

void* step_item_head_done(void* this_fsm)
{
    return NULL;
}

