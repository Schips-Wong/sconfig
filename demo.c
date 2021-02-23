
#include <stdio.h>
#include "sconfig.h"

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
    //printf("------------------sconfig_sync_to_file\n");
    //sconfig_sync_to_file(&conf);
    return 0;
}

