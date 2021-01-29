/** @file         top_fsm.c
 *  @brief        二阶状态机的实现（顶层部分）
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

/*
 *  说明
 *
 *  关于修改状态机的状态，需要按照 如下的 函数类型声明与实现
 *
 *      其次，在状态函数的最后需要用 return 返回 ns(next state)
 *
 * */
/* ------------------- Functions -----------------------------*/
/* 状态机 函数指针 区域 */
void* top_step_check_type(void* this_fsm);
void* top_step_section_get_head(void* this_fsm);//计数
void* top_step_done(void* this_fsm);//计数完成
void* top_step_find_item(void* this_fsm);//错误过程


/* 状态定义(这里的顺序要求与 procedure_list[] 一致) */
enum procedure_id{top_state_check_type, top_state_section_get_head, top_state_done, top_state_find_item};
/* 状态机调整列表 */
static Procedure procedure_list[] = { top_step_check_type, top_step_section_get_head, top_step_done, top_step_find_item };

// 子状态 相关
static FSM section_sub_fsm = {0};

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
            //printf("Skip this line:  %s", p_tmp_buff);
            set_next_state(this_fsm, top_state_done);
            break;
        case '[':
            //printf("found section head : %s", p_tmp_buff);
            init_tmp_section_name();
            set_next_state(this_fsm, top_state_section_get_head);
            break;
        case ']':
            //printf("found section end %s", p_tmp_buff);
            set_next_state(this_fsm, top_state_done);
            get_tmp_section_name();
            break;

        // 其他情况则是匹配的字符
        default :
            // 此时需要与值进行判断
            set_next_state(this_fsm, top_state_done);
            break;
    }

    return NULL;
}

void *try_insert_section(void *this_fsm, char *section_name)
{
    printf("try_insert_section: %s\n", section_name);
    return NULL;
}

void* top_step_section_get_head(void* this_fsm) // 解析section头部信息
{
    Config *conf_itself = get_data_entry(this_fsm);
    struct section * sections; 
    char *p_tmp_buff;
    char *section_name;

    if(!this_fsm)    return NULL;
    if(!conf_itself) return NULL;

    p_tmp_buff = conf_itself->p_tmp_buff;
    sections = conf_itself->sections;
    section_name = p_tmp_buff;

    //已经将完整的数据给到了set_data_entry(&section_sub_fsm, conf_itself->p_tmp_buff);
    
    run_state_machine_once(&section_sub_fsm);
    if(is_fsm_error(&section_sub_fsm))
    {
        printf("top_step_section_get_head err\n");
        set_next_state(&section_sub_fsm, get_section_procedure_default_state());
    }

    // 如果判断完成则继续判断其他部分
    if(is_section_procedure_done(&section_sub_fsm))
    {
        set_next_state(this_fsm,         top_state_check_type);
        set_next_state(&section_sub_fsm, get_section_procedure_default_state());
    }

    return NULL;
}

void* top_step_done(void* this_fsm)//计数完成
{
#if 0
    state sub_fsm_cur_state;
    step_ret ret;

    // 二阶状态机 调用
    if(is_curr_state(&sub_fsm, sub_state_done))
    {
        set_next_state(this_fsm, top_state_done);
    } else 
    {
        printf(" ├──Sub fsm : Before runing: %d\n", get_curr_state(&sub_fsm));

        sub_fsm_cur_state = run_state_machine_once(&sub_fsm);

        printf(" ├──Sub fsm : Ran :%d\n", sub_fsm_cur_state);
        printf(" └──Sub fsm : Next :%d\n", get_next_state(&sub_fsm));
        if(is_fsm_error(&sub_fsm))
        {
            printf("Error in sub\n");
            ret = (step_ret) get_step_retval(&sub_fsm);
            printf("Get err : %lld\n", ret);
            set_next_state(this_fsm, top_state_done);
        }
    }
#endif
    return NULL;
}

void* top_step_find_item(void* this_fsm) //默认
{
    set_next_state(this_fsm, top_state_check_type);
    return NULL;
}

static void __sconfig_init_sub_fsm_once(Config * conf)
{
    static int init_flag = 0;
    if(init_flag) return;

    // 为 子状态机 设置 需要的 过程序列、参数
    set_procedures(&section_sub_fsm, get_section_procedure_list());
    set_data_entry(&section_sub_fsm, conf);
    set_default_state(&section_sub_fsm, get_section_procedure_default_state());
}

static void __sconfig_init_top_fsm_once(Config * conf)
{
    FSM *fsm = &conf->parser_fsm;
    if(!fsm) return ;
    set_procedures(fsm, procedure_list);
    set_data_entry(fsm, conf);
    set_default_state(fsm, top_state_check_type);
}

static void __sconfig_parser_init(Config * conf)
{
    void * data_entry = conf->sections;
    __sconfig_init_top_fsm_once(conf);
    __sconfig_init_sub_fsm_once(conf);
}

static int __sconfig_set_path(Config * conf, char * conf_path)
{
    int path_len;
    if (!conf_path) return -1;

	//conf->conf_path
    path_len = strlen(conf_path);
    if (path_len >= CONFIG_NAME_MAX) path_len = CONFIG_NAME_MAX -1;

    strncpy(conf->conf_path, conf_path, path_len);
    conf->conf_path[path_len] = '\0';
    return 0;
}

int sconfig_init(Config * conf, char *conf_path)
{
    if (!conf)      return -1;
    if (!conf_path) return -1;
    __sconfig_set_path(conf, conf_path);
    printf("Using [%s] as config\n", conf->conf_path);
    __sconfig_parser_init(conf);

    return 0;
}

// 每次处理一行的数据
int sconfig_parse_line(Config * conf, char * buff)
{
    FSM *fsm = &conf->parser_fsm;
    state cur_state;
    if (!conf)       return -1;
    if (!buff)       return -1;

    // 记录数据所在的指针
    conf->p_tmp_buff = buff;
    while(1)
    {
        // 状态机内，每次处理以字符为单位
        cur_state = run_state_machine_once(fsm);
        //printf("FSM:%x\n", cur_state);
        if(is_curr_state(fsm, top_state_done))
        {
            reset_state_machine(fsm);
            break;
        }
    }
    return 0;

}

int sconfig_read_all_config(Config * conf)
{
    int read_section_cnt = 0;
    //int read_size;
    FILE *fp;
    char buff[1024];
    if (!conf)       return -1;
    fp = fopen(conf->conf_path, "r");
    if(!fp) 
    {
        perror("");
        return -1;
    }

    while (fgets(buff, sizeof(buff), fp) != NULL)
    {
        printf("Handling : |%s", buff);
        sconfig_parse_line(conf, buff);
    }

    return read_section_cnt;
}

void sconfig_dump_section(Config * conf) 
{
    if(!conf) return;
    return ;
}


int main(int argc, char *argv[])
{
    Config conf; 
    sconfig_init(&conf, "./test.ini");
    sconfig_read_all_config(&conf);
    sconfig_dump_section(&conf);
    //sconfig_dump_item_int_section(&conf);
#if 0
    int i = 10;
    state cur_state;
    FSM fsm_1 = {0};


    while(i--)
    {
        printf("=================\n");
        printf("Before runing: %d\n", get_curr_state(&fsm_1));

        cur_state = run_state_machine_once(&fsm_1);

        printf("Ran :%d\n", cur_state);
        printf("Next :%d\n", get_next_state(&fsm_1));

        // 停止条件
        if(is_curr_state(&fsm_1, top_state_done))
        {
            printf("Done\n");
            reset_state_machine(&fsm_1);
            break;
        }
    }
#endif
    return 0;
}
