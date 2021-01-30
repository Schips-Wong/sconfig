/** @file         sconfig_section.c
 *  @brief        处理 项目节
 *  @author       Schips
 *  @date         2021-01-22 19:03:20
 *  @version      v1.0
 *  @copyright    Copyright By Schips, All Rights Reserved
 */

#include <stdio.h>
#include <string.h>
#include "openfsm.h"
#include "sconfig.h"
#include "sconfig_section.h"

/* ------------------- Section 业务逻辑 有关内部接口 -----------------------------*/

struct section * find_section_in_config(struct section *head, char * section_name)
{
    struct section *next =  head->next;

    if(!head) return NULL;
    if(!section_name) return NULL;

    while(next)
    {
        printf("1, %s\n", next->section_name);
        printf("2, %s\n", section_name);

        if(!strcmp(next->section_name, section_name))
        {
            return next;
        }
        next = next->next;
    }

    return NULL;
}

// 每次解析时用于存放解析字符的容器
static char tmp_section_name[128]; 
static int  tmp_section_name_index;
// 如果有些配置写在 节 名之外，则默认的节为 "ambiguous"
//static const char* const ambiguous_section_name = "ambiguous"; 

void init_tmp_section_name(void)
{
    memset(tmp_section_name, 0, sizeof(tmp_section_name));
    tmp_section_name_index = 0;
}

char *get_tmp_section_name(void)
{
    return tmp_section_name;
}

char save_tmp_section_ch(char ch)
{
    if (tmp_section_name_index >= sizeof(tmp_section_name))
    {
        return '\0';
    }
    tmp_section_name[tmp_section_name_index]     = ch;
    tmp_section_name[sizeof(tmp_section_name)-1] = '\0';

    tmp_section_name_index++;
    return ch;
}

int try_insert_section_in_config(Config * conf, struct section* section)
{
    return 0;
}

int try_insert_item_in_section(struct section* section, struct item* item)
{
    if(!section) return -1;
    if(!item) return -1;

    return 0;
}

/* ------------------- 解析配置的状态机的有关实现 -----------------------------*/
// 在获取section时，必须是判断出合法的头部
void* step_section_head_start(void* this_fsm)
{
    Config *conf = get_data_entry(this_fsm);
    //conf->p_tmp_buff

    switch(conf->p_tmp_buff[0])
    {
        // 跳过[
        case '['  :
            conf->p_tmp_buff++;
            break;
        case ']'  :
        // 如果 处理到遇到 行尾，则认为错误
        case '\0' :
        case '\n' :
            set_next_state(this_fsm, state_section_head_done);
            set_fsm_error_flag(this_fsm);
            break;
        default:
            //printf("get_char\n");
            set_next_state(this_fsm, state_section_get_chars_start);
            break;
    }

    return NULL;
}

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
            //save_tmp_section_ch(conf->p_tmp_buff[0]);
            //conf->p_tmp_buff++;
            break;
    }
    return NULL;
}

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
            save_tmp_section_ch('\0');
            clr_fsm_error_flag(this_fsm);
            break;

        // 认为收到的是 组成的节名 的 字符
        default :
            save_tmp_section_ch(conf->p_tmp_buff[0]);
            conf->p_tmp_buff++;
            break;
    }

    return NULL;
}

void* step_section_head_done(void* this_fsm)
{
    return NULL;
}

