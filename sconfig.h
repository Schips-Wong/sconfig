/** @file         sconfig.h
 *  @brief        简要说明
 *  @details      详细说明
 *  @author       Schips
 *  @date         2021-01-27 13:57:37
 *  @version      v1.0
 *  @copyright    Copyright By Schips, All Rights Reserved
 */

#ifndef __SCONFIG__
#define __SCONFIG__

/*

[section1]
item1 = val

*/


struct item {
    char *item_name;
    void *value;
    struct item *next;
};

// 定义
struct section {
    char *section_name;
    char *item;
    struct section *next;
};

#define CONFIG_NAME_MAX 512
typedef struct 
{
	// 对应的配置项文件路径
    char conf_path[CONFIG_NAME_MAX];
	// 所有的 配置节 (节中包括了n个配置项)
    struct section * sections;
    // 内置需要用来处理行数据的状态机
    FSM parser_fsm;
    char *p_tmp_buff;
} Config;

#endif /* Head define end*/

