/** @file         sconfig_skl.h
 *  @brief        简要说明
 *  @details      详细说明
 *  @author       Schips
 *  @date         2021-02-02 13:58:54
 *  @version      v1.0
 *  @copyright    Copyright By Schips, All Rights Reserved
 *
 **********************************************************
 *
 *  @attention    NOTE
 *
 *
 *  @par 修改日志:
 *  <table>
 *  <tr><th>Date       <th>Version   <th>Author    <th>Description
 *  <tr><td>2021-02-02 <td>1.0       <td>Schips    <td>创建初始版本
 *  </table>
 *
 **********************************************************
 */

#ifndef __SCONFIG_SKL__
#define __SCONFIG_SKL__

// 清空临时缓冲区
void init_tmp_var_buff(void);

// 获取临时缓冲区入口
char *get_tmp_buff_entry(void);

// 把字符传入临时缓冲区
char save_ch_in_tmp_var(char ch);

// 将临时缓冲区的内容保存出来
//char* tmp_var_snapshot(void);
#define clean_tmp_var_snapshot init_tmp_var_buff
#define tmp_var_snapshot get_tmp_buff_entry

int try_insert_section_in_config(Config * conf, char * section_name);

// 设置当前 节
void set_cur_section_name(char* section_name);
char* get_cur_section_name(void);

#endif /* Head define end*/

