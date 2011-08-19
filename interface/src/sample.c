#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "interface.h"

int main()
{
    conn * connection;

    if(cli_connection("127.0.0.1", 1959, &connection))
    {
        char resp[256], cmd[256];
        int i;
        
        memset(resp, 0, 256);
        sprintf(cmd, "create table gu(id1 varchar, id2 varchar)");
        cli_commit(connection, cmd, resp);
        printf("ret: %s\n", resp);

        memset(resp, 0, 256);
        memset(cmd, 0, 256);
        sprintf(cmd, "insert into gu(gggg%d, bbbb%d)", 1, 1);
        cli_commit(connection, cmd, resp);
        printf("cmd: %s, ret: %s\n", cmd, resp);

        memset(resp, 0, 256);
        memset(cmd, 0, 256);
        sprintf(cmd, "select gu(gggg%d)", 1);
        cli_commit(connection, cmd, resp);
        printf("cmd: %s, ret: %s\n", cmd, resp);

        memset(resp, 0, 256);
        memset(cmd, 0, 256);
        sprintf(cmd, "insert into gu(gggg%d, bbbb%d)", 1, 2);
        cli_commit(connection, cmd, resp);
        printf("cmd: %s, ret: %s\n", cmd, resp);

        memset(resp, 0, 256);
        memset(cmd, 0, 256);
        sprintf(cmd, "select gu(gggg%d)", 1);
        cli_commit(connection, cmd, resp);
        printf("cmd: %s, ret: %s\n", cmd, resp);

        exit(1);

        printf("###begin to insert!\n");
        for(i = 0; i < 10000; i ++)
        {
            memset(resp, 0, 256);
            memset(cmd, 0, 256);
            sprintf(cmd, "insert into gu(gggg%d, bbbb%d)", i, i);
            cli_commit(connection, cmd, resp);
            printf("cmd: %s, ret: %s\n", cmd, resp);

        }
        printf("***begin to select\n");
        for(i = 0; i < 100; i ++)
        {
            memset(resp, 0, 256);
            memset(cmd, 0, 256);
            sprintf(cmd, "select gu(gggg%d)", i*100);
            cli_commit(connection, cmd, resp);
            printf("cmd: %s, ret: %s\n", cmd, resp);
            memset(cmd, 0, 256);
            sprintf(cmd, "gggg%dbbbb%d", i*100, i*100);
            printf("%s, %s\n", cmd, resp);
        }

	cli_exit(connection);
    }
    return 0;
}
