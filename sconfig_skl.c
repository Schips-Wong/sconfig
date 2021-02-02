/** @file         sconfig_skl.c
 *  @brief        Session、 key 、val 相关函数
 *  @author       Schips
 *  @date         2021-02-02 13:39:45
 *  @version      v1.0
 *  @copyright    Copyright By Schips, All Rights Reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "openfsm.h"
#include "sconfig.h"

// 每次解析时用于存放解析字符的容器
static char tmp_var_buff[516]; 
static int  tmp_var_buff_index;
// 解析过程中需要的
static char cur_section_name[CONFIG_NAME_MAX];
//static char cur_key_name[CONFIG_NAME_MAX];


void set_cur_section_name(char* section_name)
{
    int len = strlen(section_name) + 1;

    memset(cur_section_name, 0, sizeof(cur_section_name));
    memcpy(cur_section_name, section_name, len);
    cur_section_name[len] = '\0';

    //printf("set_cur_section_name : %p\n", section_name);
}

char* get_cur_section_name(void)
{
    //printf("get_cur_section_name : %s\n", cur_section_name);
    return cur_section_name;
}

void init_tmp_var_buff(void)
{
    memset(tmp_var_buff, 0, sizeof(tmp_var_buff));
    tmp_var_buff_index = 0;
}

char *get_tmp_buff_entry(void)
{
    //printf("-->Current tmp buff has: {%s}\n", tmp_var_buff);
    return tmp_var_buff;
}

char save_ch_in_tmp_var(char ch)
{
    if (tmp_var_buff_index >= sizeof(tmp_var_buff))
    {
        return '\0';
    }

    tmp_var_buff[tmp_var_buff_index]     = ch;
    tmp_var_buff[sizeof(tmp_var_buff)-1] = '\0';

    tmp_var_buff_index++;
    return ch;
}

#if 0
// 清除 临时缓冲区
void clean_tmp_var_snapshot(void)
{
    init_tmp_var_buff();
}

// 获取临时缓冲区 的数据入口
char* tmp_var_snapshot(void)
{
    return  get_tmp_buff_entry();
}
#endif

/* ------------------- Section 业务逻辑 有关内部接口 -----------------------------*/

struct section * find_section_in_config(Config * conf, char * section_name)
{
    struct section *cur_section =  conf->sections;

    if(!conf)         return NULL; 
    if(!section_name) return NULL; 

    while(cur_section)
    {
        printf("1, %s\n", cur_section->section_name);
        printf("2, %s\n", section_name);

        if(!strcmp(cur_section->section_name, section_name))
        {
            return cur_section;
        }
        cur_section = cur_section->next;
    }
    return NULL;
}

struct section * new_section(void)
{
    return malloc(sizeof(struct section));
}

int try_insert_section_in_config(Config * conf, char * section_name)
{
    struct section *cur_section =  conf->sections;
    int len = strlen(section_name) + 1;

    if(!conf) return -1;
    if(!section_name) return -1;

    while(cur_section)
    {
        if(!strcmp(cur_section->section_name, section_name))
        {
            return -1; // exist
        }
        cur_section = cur_section->next;
    }

    cur_section = new_section();
    if(!cur_section) return -1;

    // 保存名字
    cur_section->section_name = malloc(CONFIG_NAME_MAX);
    if(!cur_section->section_name) return -1;
    memset(cur_section->section_name, 0, CONFIG_NAME_MAX);
    memcpy(cur_section->section_name, section_name, len);

    // 初始化item
    cur_section->items = NULL;
    // 头插
    cur_section->next = conf->sections;
    conf->sections    = cur_section;

    return 0;
}

int try_insert_item_in_section(Config * conf,
                               struct section* section, 
                               char *item_name)
{
    if(!conf)      return -1;
    if(!section)   return -1;
    if(!item_name) return -1;
    //find_section_in_config

    return 0;
}
