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
//static char *cur_section = NULL;

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

struct section * find_section_in_config(struct section *head, char * section_name)
{
#if 0
    struct section *next =  head->next;

    if(!head) return NULL;
    if(!section_name) return NULL;

    while(next)
    {
        printf("1, %s\n", next->section_name);
        printf("2, %s\n", section_name);

        if(!strcmp(next->section_name, section_name))
        {
            return next;
        }
        next = next->next;
    }

#endif
    return NULL;
}

int try_insert_section_in_config(Config * conf, struct section* section)
{
    return 0;
}

int try_insert_item_in_section(struct section* section, struct item* item)
{
    if(!section) return -1;
    if(!item) return -1;

    return 0;
}
