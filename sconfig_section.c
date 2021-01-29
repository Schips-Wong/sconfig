/** @file         sconfig_section.c
 *  @brief        处理 项目节
 *  @author       Schips
 *  @date         2021-01-22 19:03:20
 *  @version      v1.0
 *  @copyright    Copyright By Schips, All Rights Reserved
 */

#include <stdio.h>
#include "openfsm.h"
#include "sconfig.h"
#include "sconfig_section.h"

/* ------------------- 只需要完成具体的实现 -----------------------------*/
//static char section_name

// 在获取头部的阶段，必须是判断出合法的头部
void* step_section_head_start(void* this_fsm)
{
    Config *conf = get_data_entry(this_fsm);
    //conf->p_tmp_buff

    switch(conf->p_tmp_buff[0])
    {
        case '['  :
            conf->p_tmp_buff++;
            //printf("Skip [\n");
            break;
        case ']'  :
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
#if 0
    while(1)
    {
        if(*p_tmp_buff == '[')
            section_name++;
        if(*p_tmp_buff == ']')
            *p_tmp_buff = '\0';
        if(*p_tmp_buff == '\n')
            break;
        p_tmp_buff++;
    }
    printf("Get item name is {%s}\n", section_name);


    SM_DATA *pd = get_data_entry(this_fsm);
    int *err_var;

    // 状态处理
    //set_next_state(this_fsm, sub_state_count);
#endif
    return NULL;
}

// 每次解析时用于存放解析字符的容器
static char tmp_section_name[128]; 
static int  tmp_section_name_index;
//static int  tmp_section_flag;
// 如果有些配置写在 节 名之外，则默认的节为 "ambiguous"
static const char* const ambiguous_section_name = "ambiguous"; 

void init_tmp_section_name(void)
{
    //printf("init_tmp_section_name\n");
    memset(tmp_section_name, 0, sizeof(tmp_section_name));
    tmp_section_name_index = 0;
}

char *get_tmp_section_name(void)
{
    //printf("get_tmp_section_name : [%s]\n", tmp_section_name);
    return tmp_section_name;
}

char push_section_name(char ch)
{
    if (tmp_section_name_index > sizeof(tmp_section_name))
    {
        return;
    }
    tmp_section_name[tmp_section_name_index]   = ch;
    //tmp_section_name[tmp_section_name_index+1] = '\0';
    tmp_section_name_index++;
}

void* step_section_get_chars_start(void* this_fsm) //计数
{
    Config *conf = get_data_entry(this_fsm);

    switch(conf->p_tmp_buff[0])
    {
        // 跳过 '[' 以后到字符之前的空格
        case '\t' :
        case ' '  :
            conf->p_tmp_buff++;
            //printf("Skip Space\n");
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
            //push_section_name(conf->p_tmp_buff[0]);
            //conf->p_tmp_buff++;
            break;
    }
#if 0
    SM_DATA *pd = get_data_entry(this_fsm);
    int *err_var;

    if(!pd)
    {
        // 通知 调用者 有错误发生
        set_fsm_error_flag(this_fsm);

        // 把 错误值 设进 容器中（如果容器存在）
        err_var = get_err_var(this_fsm);
        if(err_var) 
            *err_var = 0xff;
        return (AS_STEP_RETVAL)-1;
    }
    pd->cnt++;
        //set_next_state(this_fsm, sub_state_done);
#endif

    return NULL;
}

void* step_section_get_chars_ing(void* this_fsm) //计数完成
{
    Config *conf = get_data_entry(this_fsm);

    switch(conf->p_tmp_buff[0])
    {
        // 跳过 节名之间的 空白字符
        case '\t' :
        case ' '  :
            conf->p_tmp_buff++;
            //push_section_name('-');
            break;
        // 正常结束
        case ']'  :
            set_next_state(this_fsm, state_section_head_done);
            push_section_name('\0');
            clr_fsm_error_flag(this_fsm);
            break;

        default :
            push_section_name(conf->p_tmp_buff[0]);
            conf->p_tmp_buff++;
            break;
    }

}

void* step_section_head_done(void* this_fsm)
{
    return NULL;
}

