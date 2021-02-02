/** @file         sconfig.c
 *  @brief        简要说明
 *  @details      详细说明
 *  @author       Schips
 *  @date         2021-02-02 13:42:13
 *  @version      v1.0
 *  @copyright    Copyright By Schips, All Rights Reserved
 *
 **********************************************************
 *
 *  @attention    NOTE
 *                SDK: 
 *                ENV: 
 *  @par 修改日志:
 *  <table>
 *  <tr><th>Date       <th>Version   <th>Author    <th>Description
 *  <tr><td>2021-02-02 <td>1.0       <td>Schips    <td>创建初始版本
 *  </table>
 *
 **********************************************************
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "openfsm.h"
#include "sconfig.h"
#include "sconfig_fsm_top.h"
#include "sconfig_fsm_sub_section.h"
#include "sconfig_fsm_sub_item.h"

// 只需要初始化一次的有关动作
static void __sconfig_init_sub_fsm_once(Config * conf)
{
    static int init_flag = 0;
    FSM* section_sub_fsm = get_section_sub_fsm();
    FSM* item_sub_fsm    = get_item_sub_fsm();

    if(init_flag) return;

    // 为 子状态机 设置 需要的 过程序列、参数
    set_procedures(section_sub_fsm,     get_section_procedure_list());
    set_data_entry(section_sub_fsm,     conf);
    set_default_state(section_sub_fsm,  get_section_procedure_default_state());

    set_procedures(item_sub_fsm,        get_item_procedure_list());
    set_data_entry(item_sub_fsm,        conf);
    set_default_state(item_sub_fsm,     get_item_procedure_default_state());
    init_flag = 1;
}

// 初始化状态机
static void __sconfig_init_top_fsm_once(Config * conf)
{
    static int init_flag = 0;

    if(init_flag) return;
    FSM *fsm = &conf->parser_fsm;
    if(!fsm) return ;
    set_procedures(fsm,     get_top_procedure_list());
    set_data_entry(fsm,     conf);
    set_default_state(fsm,  get_top_procedure_default_state());

    init_flag = 1;
}

static void __sconfig_parser_init(Config * conf)
{
    //void * data_entry = conf->sections;
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
    //printf("Using [%s] as config\n", conf->conf_path);
    return 0;
}

int sconfig_init(Config * conf, char *conf_path)
{
    if (!conf)      return -1;
    if (!conf_path) return -1;
    __sconfig_set_path(conf, conf_path);
    __sconfig_parser_init(conf);

    return 0;
}

// 每次处理一行的数据
int sconfig_parse_line(Config * conf, char * buff)
{
    FSM *fsm = &conf->parser_fsm;
    //state cur_state;
    if (!conf)       return -1;

    if (!buff)       return -1;

    // 记录数据所在的指针
    conf->p_tmp_buff = buff;
    while(1)
    {
        // 状态机内，每次处理以字符为单位
        run_state_machine_once(fsm);

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

void sconfig_dump(Config * conf) 
{
    if(!conf) return;
    printf("Conf path : %s\n", conf->conf_path);

    //sconfig_dump_session()

    return ;
}

int main(int argc, char *argv[])
{
    Config conf; 
    sconfig_init(&conf, "./test.ini");
    sconfig_read_all_config(&conf);
    sconfig_dump(&conf);
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
