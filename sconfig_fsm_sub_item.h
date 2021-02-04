/** @file         sconfig_sub_item.h
 *  @brief        简要说明
 *  @details      详细说明
 *  @author       Schips
 *  @date         2021-01-28 10:52:19
 *  @version      v1.0
 *  @copyright    Copyright By Schips, All Rights Reserved
 */

#ifndef __SCONFIG_ITEM__
#define __SCONFIG_ITEM__

#include "openfsm.h"

void* step_item_get_chars_start(void* this_fsm);
void* step_item_get_key(void* this_fsm);
void* step_item_get_val(void* this_fsm);
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

char *get_tmp_key_name(void);
void init_tmp_key_name(void);

#endif /* Head define end*/

