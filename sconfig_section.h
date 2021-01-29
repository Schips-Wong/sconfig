/** @file         sub_fsm.h
 *  @brief        子状态机对外提供的状态接口
 *  @author       Schips
 *  @date         2021-01-22 19:50:04
 *  @version      v1.0
 *  @copyright    Copyright By Schips, All Rights Reserved
 */

#ifndef __SECOND_ORDER_FSM_DEMO_SUB_STATE__
#define __SECOND_ORDER_FSM_DEMO_SUB_STATE__

#include "openfsm.h"

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

char *get_tmp_section_name(void);
void init_tmp_section_name(void);


#endif /* Head define end*/

