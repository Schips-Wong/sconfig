/** @file         sconfig_fsm.c
 *  @brief        二阶状态所有状态机的实现（顶层部分），负责处理每行的数据，决定解析什么类型
 *  @author       Schips
 *  @date         2021-01-22 19:03:20
 *  @version      v1.0
 *  @copyright    Copyright By Schips, All Rights Reserved
 */

#include <stdlib.h>

#include "openfsm.h"
#include "sconfig.h"
#include "sconfig_fsm.h"
#include "sconfig_skl.h"

/* TOP FSM for Line
 *   解析每一行，判断其类型
 *    - 1. 节     格式为 : '[section name]'
 *    - 2. 键值对  格式为 : '<key> = <value>'
 * */


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
    char *p_tmp_buff;

    if(!this_fsm)    return NULL;
    if(!conf_itself) return NULL;

    p_tmp_buff = conf_itself->p_tmp_buff;
    //sections = conf_itself->sections;

    switch(p_tmp_buff[0])
    {
        // 跳过 空格
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

// 解析Section
void* top_step_find_section(void* this_fsm) // 解析section头部信息
{
    Config *conf_itself = get_data_entry(this_fsm);
    FSM *section_sub_fsm = get_section_sub_fsm();

    if(!this_fsm)    return NULL;
    if(!conf_itself) return NULL;

    run_state_machine_once(section_sub_fsm);
    if(is_fsm_error(section_sub_fsm))
    {
        //printf("top_step_find_section err\n");
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

// 解析 item
void* top_step_find_item(void* this_fsm) 
{
    Config *conf_itself = get_data_entry(this_fsm);
    FSM *item_sub_fsm = get_item_sub_fsm();

    if(!this_fsm)    return NULL;
    if(!conf_itself) return NULL;

    run_state_machine_once(item_sub_fsm);
    if(is_fsm_error(item_sub_fsm))
    {
        //printf("top_step_find_item err\n");
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

/* Sub FSM for key-value
 *   解析键值对，根据赋值语法，解析出它们
 *    - 1. 节    被'[]'括起的是section，中间的空白符号会被跳过
 *    - 2. 键    赋值号（=）左边的是key
 *    - 3. 值    赋值号（=）右边的是value， 这个部分可以用单/双引号括起
 * */

/** @file         sconfig_fsm_sub.c
 *  @brief        子状态机 section、item 解析实现
 *  @author       Schips
 *  @date         2021-01-28 10:52:17
 *  @version      v1.0
 *  @copyright    Copyright By Schips, All Rights Reserved
 */

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
            
            init_tmp_var_buff();
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
        // 引号 实现了相互包含
        case '\''  :
            set_next_state(this_fsm, state_item_get_val_sqm);
            conf->p_tmp_buff++;
            break;
        case '\"'  :
            set_next_state(this_fsm, state_item_get_val_dqm);
            conf->p_tmp_buff++;
            break;
        case '\n' :
        case '\0' :
            // 结束。处理
            set_next_state(this_fsm, state_item_head_done);
            clr_fsm_error_flag(this_fsm);
            set_cur_val(tmp_var_snapshot());
            //clean_tmp_var_snapshot();
            break;
        // 跳过 = 以后的 空白字符
        case '='  :
        case '\t' :
        case ' '  :
            conf->p_tmp_buff++;
            break;
        // 逗号 视为 一键多值
        case ','  :
            // 保存到新的地方
            //printf("Before switch : %s\n", (char*)get_tmp_buff_entry());
            tmp_var_switch();
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

// 单引号
void* step_item_get_val_sqm(void* this_fsm)
{
    Config *conf = get_data_entry(this_fsm);

    switch(conf->p_tmp_buff[0])
    {
        // 提前结尾(只有一个引号)
        case '\n' :
        case '\0' :
        // 再次遇见时，代表结束
        case '\''  :
            conf->p_tmp_buff++;
            set_next_state(this_fsm, state_item_head_done);
            set_cur_val(tmp_var_snapshot());
            //clean_tmp_var_snapshot();
            break;

        // 引号中的 空格
        case ' '  :
        case '\t' :
        default :
            // 累计
            save_ch_in_tmp_var(conf->p_tmp_buff[0]);
            conf->p_tmp_buff++;
            break;
    }

    return NULL;
}

// 双引号
void* step_item_get_val_dqm(void* this_fsm)
{
    Config *conf = get_data_entry(this_fsm);

    switch(conf->p_tmp_buff[0])
    {
        // 提前结尾(只有一个引号)
        case '\n' :
        case '\0' :
        // 再次遇见时，代表结束
        case '\"'  :
            conf->p_tmp_buff++;
            set_next_state(this_fsm, state_item_head_done);
            set_cur_val(tmp_var_snapshot());
            //clean_tmp_var_snapshot();
            break;

        // 引号中的 空格
        case ' '  :
        case '\t' :
        default :
            // 累计
            save_ch_in_tmp_var(conf->p_tmp_buff[0]);
            conf->p_tmp_buff++;
            break;
    }
    return NULL;
}
