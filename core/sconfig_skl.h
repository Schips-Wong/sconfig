/** @file         sconfig_skl.h
 *  @brief        Session、 key 、val 相关函数接口
 *  @author       Schips
 *  @date         2021-02-02 13:58:54
 *  @version      v1.0
 *  @copyright    Copyright By Schips, All Rights Reserved
 */

#ifndef __SCONFIG_SKL__
#define __SCONFIG_SKL__
#include "sconfig.h"

// 清空临时缓冲区
void init_tmp_var_buff(void);
// 获取临时缓冲区入口
char *get_tmp_buff_entry(void);

// 把字符传入临时缓冲区
char save_ch_in_tmp_var(char ch);

// 将临时缓冲区的内容保存出来
#define clean_tmp_var_snapshot init_tmp_var_buff
#define parser_clean_up init_tmp_var_buff
#define tmp_var_snapshot get_tmp_buff_entry

int try_insert_section_in_config(Config * conf, char * section_name);

// 设置当前 节
void set_cur_section_name(char* section_name);
char* get_cur_section_name(void);

void set_cur_key_name(char* section_name);
char* get_cur_key_name(void);

void set_cur_val(char* val);
void* get_cur_val(void);



int try_insert_item_in_section(Config * conf,
                               char* section_name, 
                               char *key_name);

struct values* get_vals_head(void);
void tmp_var_switch(void);
struct section * find_section_in_config(Config * conf, char * section_name);
struct item * find_item_in_section(struct section *section, char * key_name);
#endif /* Head define end*/

