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
// Section 有关
static char *cur_section = NULL;


void set_cur_section_name(char* section_name)
{
    cur_section = section_name;
    printf("set_cur_section_name : %p\n", cur_section);
}

char* get_cur_section_name(void)
{
    printf("get_cur_section_name : %s\n", cur_section);
    return cur_section;
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

// 将临时缓冲区的内容保存出来
// 注意：这里申请的内存 要在别的地方释放
char* tmp_var_snapshot(void)
{
    // 获取缓冲区数据
    char * tmp_val = get_tmp_buff_entry();
    int len = strlen(tmp_val) + 1;
    static char *cur;

    // 这样的做法会导致 分段的配置项存在内存泄漏的风险 TODO
    cur = malloc(len + 1);
    if(cur)
    {
        memcpy(cur, tmp_val, len);
        cur[len] = '\0';
        init_tmp_var_buff();
        printf("Current : %s\n", cur);
    }

    return cur;
}

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

    if(!conf) return -1;
    if(!section_name) return -1;
    printf("try_insert_section_in_config\n");

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

    cur_section->section_name = section_name;
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
