#ifndef __SCONFIG_FSM__
#define __SCONFIG_FSM__

/** @file         sconfig_fsm_sub.h
 *  @brief        子状态机 section、item 对外接口
 *  @author       Schips
 *  @date         2021-01-28 10:52:19
 *  @version      v1.0
 *  @copyright    Copyright By Schips, All Rights Reserved
 */
#include "openfsm.h"

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

void* step_item_get_chars_start(void* this_fsm);
void* step_item_get_key(void* this_fsm);
void* step_item_get_val(void* this_fsm);
// 单引号 SQM : single quotation marks
// 双引号 DQM : double quotation marks
void* step_item_get_val_sqm(void* this_fsm);
void* step_item_get_val_dqm(void* this_fsm);
void* step_item_head_done(void* this_fsm);

static Procedure item_procedure_list[] = { 
    step_item_get_chars_start,
    step_item_get_key,
    step_item_get_val,
    step_item_get_val_sqm,
    step_item_get_val_dqm,
    step_item_head_done,
};

enum item_procedure_id { 
    state_item_get_chars_start, 
    state_item_get_key, 
    state_item_get_val,
    state_item_get_val_sqm,
    state_item_get_val_dqm,
    state_item_head_done,
};

static inline state get_item_procedure_default_state(void)
{
    return state_item_get_chars_start;
}

static inline int is_item_procedure_done(FSM *this_fsm)
{
    return is_curr_state(this_fsm, state_item_head_done);
}

/* 额外需要提供的跳转序列接口 */
static inline Procedure* get_item_procedure_list(void)
{
    return item_procedure_list;
}

void* step_section_head_start(void* this_fsm);
void* step_section_get_chars_start(void* this_fsm);
void* step_section_get_chars_ing(void* this_fsm);
void* step_section_head_done(void* this_fsm);

static Procedure section_procedure_list[] = { 
    step_section_head_start, 
    step_section_get_chars_start,
    step_section_get_chars_ing,
    step_section_head_done};

enum section_procedure_id { 
    state_section_head_start, 
    state_section_get_chars_start, 
    state_section_get_chars_ing, 
    state_section_head_done
};

static inline state get_section_procedure_default_state(void)
{
    return state_section_head_start;
}

static inline int is_section_procedure_done(FSM *this_fsm)
{
    return is_curr_state(this_fsm, state_section_head_done);
}

/* 额外需要提供的跳转序列接口 */
static inline Procedure* get_section_procedure_list(void)
{
    return section_procedure_list;
}

#endif /* Head define end*/
