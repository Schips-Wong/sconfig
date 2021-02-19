/** @file         sconfig_top_fsm.c
 *  @brief        二阶状态机的实现（顶层部分），负责处理每行的数据，决定解析什么类型
 *  @author       Schips
 *  @date         2021-01-22 19:03:20
 *  @version      v1.0
 *  @copyright    Copyright By Schips, All Rights Reserved
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "openfsm.h"
#include "sconfig.h"
#include "sconfig_skl.h"
#include "sconfig_fsm_top.h"
#include "sconfig_fsm_sub_section.h"
#include "sconfig_fsm_sub_item.h"

/* ------------------- Functions -----------------------------*/
// 子状态 相关
static FSM section_sub_fsm = {0};
static FSM item_sub_fsm = {0};

FSM *get_section_sub_fsm(void)
{
    return &section_sub_fsm;
}

FSM *get_item_sub_fsm(void)
{
    return &item_sub_fsm;
}

// 判断这一行是什么内容 : 注释， 节 还是 项
void* top_step_check_type(void* this_fsm)
{
    Config *conf_itself = get_data_entry(this_fsm);
    //struct section * sections; 
    char *p_tmp_buff;

    if(!this_fsm)    return NULL;
    if(!conf_itself) return NULL;

    p_tmp_buff = conf_itself->p_tmp_buff;
    //sections = conf_itself->sections;

    switch(p_tmp_buff[0])
    {
        // 跳过 '['  之前的 空格
        case '\t':
        case ' ':
            conf_itself->p_tmp_buff++;
            set_next_state(this_fsm, top_state_check_type);
            break;
        // 跳过尾部、注释
        case '\0':
        case '\n':
        case '#':
            set_next_state(this_fsm, top_state_done);
            break;
        // 如果遇到 [ ，意味着这一行可能是 节名
        case '[':
            //printf("found section head : %s", p_tmp_buff);
            init_tmp_var_buff();
            set_next_state(this_fsm, top_state_find_section);
            break;
        // 如果遇到 ] ，意味着 节名 分析结束
        case ']':
            set_next_state(this_fsm, top_state_done);

            break;
        // 其他情况则是匹配的字符
        default :
            // 此时需要与值进行判断
            set_next_state(this_fsm, top_state_find_item);
            break;
    }

    return NULL;
}

void* top_step_find_section(void* this_fsm) // 解析section头部信息
{
    Config *conf_itself = get_data_entry(this_fsm);
    FSM *section_sub_fsm = get_section_sub_fsm();
    //char * cur_section;

    if(!this_fsm)    return NULL;
    if(!conf_itself) return NULL;

    run_state_machine_once(section_sub_fsm);
    if(is_fsm_error(section_sub_fsm))
    {
        printf("top_step_find_section err\n");
        set_next_state(section_sub_fsm, get_section_procedure_default_state());
    }

    // 如果判断完成则继续判断其他部分
    if(is_section_procedure_done(section_sub_fsm))
    {
        set_next_state(this_fsm,         top_state_check_type);
        set_next_state(section_sub_fsm, get_section_procedure_default_state());
        // 记住当前的节名
        set_cur_section_name(tmp_var_snapshot());
        // 尝试插入配置中
        try_insert_section_in_config(conf_itself , get_cur_section_name());
        parser_clean_up();
    }

    return NULL;
}

void* top_step_find_item(void* this_fsm) // 解析 item 头部信息
{
    Config *conf_itself = get_data_entry(this_fsm);
    FSM *item_sub_fsm = get_item_sub_fsm();

    if(!this_fsm)    return NULL;
    if(!conf_itself) return NULL;

    run_state_machine_once(item_sub_fsm);
    if(is_fsm_error(item_sub_fsm))
    {
        printf("top_step_find_item err\n");
        set_next_state(item_sub_fsm, get_item_procedure_default_state());
    }

    // 如果判断完成则继续判断其他部分
    if(is_item_procedure_done(item_sub_fsm))
    {
        set_next_state(this_fsm,         top_state_check_type);
        set_next_state(item_sub_fsm, get_item_procedure_default_state());

        // 尝试插入配置中
        try_insert_item_in_section(conf_itself, get_cur_section_name(), get_cur_key_name());
        parser_clean_up();
    }

    return NULL;
}

void* top_step_done(void* this_fsm)
{
    return NULL;
}

