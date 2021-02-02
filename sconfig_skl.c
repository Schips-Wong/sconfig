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
static int  cur_section_name_len;

static char cur_key_name[CONFIG_NAME_MAX];
static int  cur_key_name_len;

static char cur_val[CONFIG_NAME_MAX];
static int  cur_val_len;

int get_tmp_var_buff_len(void);

void set_cur_val(char* val)
{
    int len = get_tmp_var_buff_len();

    memset(cur_val, 0, sizeof(cur_val));
    memcpy(cur_val, val, len);

    cur_val_len = get_tmp_var_buff_len();
}

void* get_cur_val(void)
{
    return cur_val;
}

void set_cur_section_name(char* section_name)
{
    int len = strlen(section_name) + 1;

    memset(cur_section_name, 0, sizeof(cur_section_name));
    memcpy(cur_section_name, section_name, len);
    cur_section_name[len] = '\0';

    cur_section_name_len = len;
}

char* get_cur_section_name(void)
{
    return cur_section_name;
}

void set_cur_key_name(char* section_name)
{
    int len = strlen(section_name) + 1;

    memset(cur_key_name, 0, sizeof(cur_key_name));
    memcpy(cur_key_name, section_name, len);
    cur_key_name[len] = '\0';

    cur_key_name_len = len;
}

char* get_cur_key_name(void)
{
    return cur_key_name;
}

void init_tmp_var_buff(void)
{
    memset(tmp_var_buff, 0, sizeof(tmp_var_buff));
    tmp_var_buff_index = 0;
}

int get_tmp_var_buff_len(void)
{
    return  tmp_var_buff_index;
}

char *get_tmp_buff_entry(void)
{
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

/* ------------------- Section 业务逻辑 有关内部接口 -----------------------------*/

struct section * find_section_in_config(Config * conf, char * section_name)
{
    struct section *cur_section =  conf->sections;

    if(!conf)         return NULL; 
    if(!section_name) return NULL; 

    while(cur_section)
    {
        //printf("1, %s\n", cur_section->section_name);
        //printf("2, %s\n", section_name);

        if(!strcmp(cur_section->section_name, section_name))
        {
            return cur_section;
        }
        cur_section = cur_section->next;
    }
    return NULL;
}

struct item * find_key_in_section(struct section *section, char * key_name)
{
    struct item *cur_item =  section->items;

    if(!section)     return NULL; 
    if(!key_name) return NULL; 

    while(cur_item)
    {
        printf("1, %s\n", cur_item->key_name);
        printf("2, %s\n", key_name);

        if(!strcmp(cur_item->key_name, key_name))
        {
            return cur_item;
        }
        cur_item = cur_item->next;
    }
    return NULL;
}

struct section * new_section(void)
{
    return malloc(sizeof(struct section));
}

struct item * new_item(void)
{
    return malloc(sizeof(struct item));
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
    cur_section->section_name = malloc(cur_section_name_len);

    if(!cur_section->section_name) return -1;
    memcpy(cur_section->section_name, section_name, len);
    cur_section->section_name[cur_section_name_len-1] = '\0';

    // 初始化item
    cur_section->items = NULL;
    // 头插
    cur_section->next = conf->sections;
    conf->sections    = cur_section;
    //printf("new section %s\n", section_name);

    return 0;
}

int try_insert_item_in_section(Config * conf,
                               char* section_name, 
                               char *key_name)
{
    struct section *cur_section = NULL;
    struct item    *cur_item = NULL;

    if(!conf)      return -1;
    if(!section_name)   return -1;
    if(!key_name) return -1;

    cur_section = find_section_in_config(conf, section_name);
    //printf("find_section_in_config :%p\n", cur_section);
    if(!cur_section) 
    {
        printf("[%s] Not found\n", section_name);
        return 0;
        printf("=================todo not found and as DEFAULT_SECTION_NAME\n");
        set_cur_section_name(DEFAULT_SECTION_NAME);
        cur_section = find_section_in_config(conf, DEFAULT_SECTION_NAME);
    }

    cur_item = cur_section->items;

    while(cur_item)
    {
        if(!strcmp(cur_item->key_name, key_name))
        {
            return -1; // exist
        }
        cur_item = cur_item->next;
    }

    // 填入名字、值
    cur_item = new_item();
    if(!cur_item) return -1;

    cur_item->key_name = malloc(cur_key_name_len);
    if(!cur_item->key_name) return -1;

    memcpy(cur_item->key_name, key_name, strlen(key_name));
    cur_item->key_name[cur_key_name_len-1] = '\0';

    cur_item->value = malloc(cur_val_len);
    memcpy(cur_item->key_name, key_name, strlen(key_name));
    memcpy(cur_item->value, get_cur_val(), cur_val_len);

    // 头插到链表中
    cur_item->next     = cur_section->items;
    cur_section->items = cur_item;

    return 0;
}
