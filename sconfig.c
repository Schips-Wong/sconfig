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
    FSM *fsm = &conf->parser_fsm;

    if(init_flag) return;
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
    if (!conf) return -1;

    if(!conf_path)
    {
        memset(conf->conf_path, 0, CONFIG_NAME_MAX);
    } else 
    {
        path_len = strlen(conf_path);
        if (path_len >= CONFIG_NAME_MAX) path_len = CONFIG_NAME_MAX -1;

        strncpy(conf->conf_path, conf_path, path_len);
        conf->conf_path[path_len] = '\0';
    }

    //printf("Using [%s] as config\n", conf->conf_path);

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

int sconfig_init(Config * conf, char *conf_path)
{
    FILE *fp;
    static char buff[1024];
    struct section *sections;
    struct section *ssafe;
    struct item *items;
    struct item *isafe;
    struct values *values;
    struct values *vsafe;
    if (!conf)      return -1;
    //printf("Set path %p\n", conf_path);
    __sconfig_set_path(conf, conf_path);
    __sconfig_parser_init(conf);

    //printf("Free all (if have)\n");
    sections = conf->sections; 
    while(sections)
    {
        ssafe = sections->next;
        //printf("Free Section: %p\n", sections);
        items = sections->items;
        while(items)
        {
            isafe = items->next;
            //printf("--Free item: %p\n", items);

            values = items->vals;
            while(values)
            {
                vsafe = values->next;
                //printf("----Free val: %p\n", values);
                free(values);
                values = vsafe;
            }
            free(items);
            //printf("--Next : %p\n", isafe);
            items= isafe;
        }
        free(sections);
        sections = ssafe;
    }

    conf->sections = NULL;

    // 如果没有提供配置，则默认正常结束
    if (!conf_path)
    {
        return 0;
    }

    fp = fopen(conf->conf_path, "r");
    if(!fp) 
    {
        printf("%s: ", conf->conf_path);
        perror("");
        return -1;
    }

    memset(buff, 0, sizeof(buff));
    // 按行为单位来处理
    while (fgets(buff, sizeof(buff), fp) != NULL)
    {
        //printf("Handling : |%s", buff);
        sconfig_parse_line(conf, buff);
    }

    return 0;
}

void sconfig_deinit(Config * conf)
{
    //printf("sconfig_deinit\n");
    sconfig_init(conf, NULL);
}

void sconfig_reload(Config * conf)
{
    //printf("sconfig_reload is %s\n", conf->conf_path);
    sconfig_init(conf, conf->conf_path);
}

void sconfig_dump_vals(struct values * head)
{
    struct values * cur_vals  = head;

    while(cur_vals)
    {
        printf("  %p  #%3d : [%s]\n", cur_vals, cur_vals->value_len, (char*)cur_vals->value);
        //printf("Going to next, %p/%p\n", cur_vals, cur_vals->next);
        cur_vals = cur_vals->next;
    }
}

void sconfig_dump_item(struct item * head)
{
    struct item * cur = head;
    while(cur)
    {
        printf("-key : %s\n", cur->key_name);
        sconfig_dump_vals(cur->vals);
        cur = cur->next;
    }
}

void sconfig_dump_session(struct section * head)
{
    struct section * cur = head;
    while(cur)
    {
        //printf("[%s](%p)\n", cur->section_name, cur->section_name);
        printf("[%s]\n", cur->section_name);
        sconfig_dump_item(cur->items);
        cur = cur->next;
        printf("\n");
    }
}

void sconfig_dump(Config * conf) 
{
    if(!conf) return;
    printf("=======================\n");
    printf("-- Conf path : %s\n", conf->conf_path);
    printf("\n");

    printf("sconfig_dump_sessionn");
    sconfig_dump_session(conf->sections);
    printf("=======================\n\n");

    return ;
}

int main(int argc, char *argv[])
{
    int ret;
    DECLARE_CONFIG(conf);

    // 通过指定的配置文件进行初始化，会将所有能够解析的节点读进内存
    ret = sconfig_init(&conf, "./test.ini");
    //sconfig_read_all_config(&conf);

    if(ret)
    {
        printf("sconfig init error\n");
    }

    struct item* host;
    host = sconfig_get_item_from_section(&conf, "section1", "host");

    if(!host) {
        printf("section/item not found\n");
    }

    printf("host = %s\n", (char *) sconfig_get_item_val(host));
    printf("\n");

    printf("------------------sconfig_reload\n");
    sconfig_reload(&conf);
    sconfig_dump(&conf);

    printf("------------------sconfig_deinit\n");
    sconfig_deinit(&conf);
    sconfig_dump(&conf);

    printf("------------------sconfig_init\n");
    ret = sconfig_init(&conf, "./test.ini");
    sconfig_dump(&conf);
    return 0;
}

