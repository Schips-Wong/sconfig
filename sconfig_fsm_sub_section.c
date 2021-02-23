/** @file         sconfig_section.c
 *  @brief        fsm解析 节 有关
 *  @author       Schips
 *  @date         2021-01-22 19:03:20
 *  @version      v1.0
 *  @copyright    Copyright By Schips, All Rights Reserved
 */

#include <stdlib.h>
#include "openfsm.h"
#include "sconfig.h"
#include "sconfig_skl.h"
#include "sconfig_fsm_sub_section.h"

/* ------------------- 解析配置的状态机的有关实现 -----------------------------*/
// 在获取section时，必须是判断出合法的头部
void* step_section_head_start(void* this_fsm)
{
    Config *conf = get_data_entry(this_fsm);

    // 进入初始状态：认为以 节名 开头的才是真正的状态
    switch(conf->p_tmp_buff[0])
    {
        // 跳过[
        case '['  :
            conf->p_tmp_buff++;
            break;
        case ']'  :
        // 如果 在一开始处理就遇到 行尾，则认为错误（不允许空节名）
        case '\0' :
        case '\n' :
            set_next_state(this_fsm, state_section_head_done);
            set_fsm_error_flag(this_fsm);
            break;
        default:
            set_next_state(this_fsm, state_section_get_chars_start);
            break;
    }

    return NULL;
}

// 跳过 Section 之前的字符
void* step_section_get_chars_start(void* this_fsm)
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
            // 错误，结束
            set_next_state(this_fsm, state_section_head_done);
            set_fsm_error_flag(this_fsm);
            break;
        // 否则进入获取连续字符模式
        default:
            set_next_state(this_fsm, state_section_get_chars_ing);
            break;
    }
    return NULL;
}

// 获取 实际的 section（ Section 名中的空格 会被合并）
void* step_section_get_chars_ing(void* this_fsm)
{
    Config *conf = get_data_entry(this_fsm);

    switch(conf->p_tmp_buff[0])
    {
        // 跳过 节名之间的 空白字符
        case '\t' :
        case ' '  :
            conf->p_tmp_buff++;
            break;
        // 正常结束
        case ']'  :
            set_next_state(this_fsm, state_section_head_done);
            save_ch_in_tmp_var('\0');
            clr_fsm_error_flag(this_fsm);
            break;

        // 认为收到的是 组成的节名 的 字符
        default :
            save_ch_in_tmp_var(conf->p_tmp_buff[0]);
            conf->p_tmp_buff++;
            break;
    }

    return NULL;
}

void* step_section_head_done(void* this_fsm)
{
    return NULL;
}

