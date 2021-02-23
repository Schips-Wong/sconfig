/** @file         sconfig_fsm_top.h
 *  @brief        状态机的对外接口（顶层部分），负责处理每行的数据，决定解析什么类型
 *  @author       Schips
 *  @date         2021-01-22 19:03:20
 *  @version      v1.0
 *  @copyright    Copyright By Schips, All Rights Reserved
 */

#ifndef __SCONFIG_TOP__
#define __SCONFIG_TOP__

/* 状态机 函数指针 区域 */
void* top_step_check_type(void* this_fsm);
void* top_step_find_section(void* this_fsm);
void* top_step_find_item(void* this_fsm);
void* top_step_done(void* this_fsm);

/* 状态机调整列表 */
static Procedure top_procedure_list[] = { 
    top_step_check_type, 
    top_step_find_section, 
    top_step_find_item,
    top_step_done, 
};

enum top_procedure_id {
    top_state_check_type, 
    top_state_find_section, 
    top_state_find_item,
    top_state_done,
};

static inline Procedure* get_top_procedure_list(void)
{
    return top_procedure_list;
}

static inline state get_top_procedure_default_state(void)
{
    return top_state_check_type;
}

FSM *get_section_sub_fsm(void);
FSM *get_item_sub_fsm(void);

#endif /* Head define end*/

