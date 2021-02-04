/** @file         sconfig.h
 *  @brief        sconfig 对外接口
 *  @author       Schips
 *  @date         2021-01-27 13:57:37
 *  @version      v1.0
 *  @copyright    Copyright By Schips, All Rights Reserved
 */

#ifndef __SCONFIG__
#define __SCONFIG__

/*
 
我们所说的配置项是指形如下方提到的这类文件格式配置

    [section1]
    key = val

*/
struct values {
    void           *value;
    int             value_len;
    struct values  *next;
};

struct item {
    char *          key_name;
    void           *value;
    //char          **value;
    struct values  *vals;
    struct item    *next;
};

// 定义
struct section {
    char           *section_name;
    struct item    *items;
    struct section *next;
};

#define DEFAULT_SECTION_NAME "ANONYMOUS"
#define CONFIG_NAME_MAX      512
#define CONFIG_TMP_BUF_MAX   1024
typedef struct 
{
	// 对应的配置项文件路径
    char            conf_path[CONFIG_NAME_MAX];
	// 所有的 配置节 (节中包括了n个配置项)
    struct section *sections;
    // 内置需要用来处理行数据的状态机
    FSM             parser_fsm;
    char           *p_tmp_buff;
} Config;

void init_tmp_var_buff(void);
char *get_tmp_buff_entry(void);

char save_ch_in_tmp_var(char ch);
int set_curr_section_name(void);

#endif /* Head define end*/

