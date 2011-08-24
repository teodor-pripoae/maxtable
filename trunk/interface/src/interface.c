#include "global.h"
#include "utils.h"
#include "master/metaserver.h"
#include "region/rangeserver.h"
#include "netconn.h"
#include "conf.h"
#include "token.h"
#include "tss.h"
#include "parser.h"
#include "memcom.h"
#include "strings.h"
#include "trace.h"
#include "row.h"
#include "type.h"
#include "interface.h"

extern	TSS	*Tss;

extern int sel_resp_rejoin(char * src_buf, char * dest_buf, int src_len, int * dest_len, char *index_buf);

int validation_request(char * request)
{
    //to be do.....
    return TRUE;
}


/*
create one connection between cli and svr, return the connection
*/
int cli_connection(char * meta_ip, int meta_port, conn ** connection)
{
    mem_init_alloc_regions();
    tss_setup(TSS_OP_CLIENT);

    conn * new_conn = (conn *)MEMALLOCHEAP(sizeof(conn));
    new_conn->meta_server_port = meta_port;
    strcpy(new_conn->meta_server_ip, meta_ip);

    if((new_conn->connection_fd = conn_open(meta_ip, meta_port)) < 0)
    {
        perror("error in create connection: ");
        return FALSE;
    }
    new_conn->status = ESTABLISHED;
    new_conn->rg_list_len = 0;

    *connection = new_conn;

    return TRUE;
}

/*
close one connection between cli and svr
*/
void cli_exit(conn * connection)
{
    int i;
    close(connection->connection_fd);
    for(i = 0; i < connection->rg_list_len; i++)
    {
        if(connection->rg_list[i]->status == ESTABLISHED)
            close(connection->rg_list[i]->connection_fd);
        MEMFREEHEAP(connection->rg_list[i]);
    }
    MEMFREEHEAP(connection);
}

/*
commit one request
*/
int cli_commit(conn * connection, char * cmd, char * response, int * resp_len)
{
    LOCALTSS(tss);
	
    char send_buf[LINE_BUF_SIZE];
    char send_rg_buf[LINE_BUF_SIZE];
    char tab_name[64];
    int send_buf_size;

    RPCRESP	*resp;
    RPCRESP *rg_resp;
    int querytype;

    if(!validation_request(cmd))
        return FALSE;

    //querytype = par_get_query(cmd, &querytype_index);
	parser_open(cmd);

    querytype = ((TREE *)(tss->tcmd_parser))->sym.command.querytype;
    MEMSET(tab_name, 64);
    MEMCPY(tab_name, ((TREE *)(tss->tcmd_parser))->sym.command.tabname,
        ((TREE *)(tss->tcmd_parser))->sym.command.tabname_len);

    send_buf_size = strlen(cmd);
    memset(send_buf, 0, LINE_BUF_SIZE);
    memcpy(send_buf, RPC_REQUEST_MAGIC, RPC_MAGIC_MAX_LEN);
    memcpy(send_buf + RPC_MAGIC_MAX_LEN, cmd, send_buf_size);
			
    write(connection->connection_fd, send_buf, (send_buf_size + RPC_MAGIC_MAX_LEN));

    resp = conn_recv_resp(connection->connection_fd);
    if (resp->status_code != RPC_SUCCESS)
    {
        printf("\n ERROR in response \n");
        return FALSE;
    }

    if((querytype == INSERT) || (querytype == SELECT))
    {
        INSMETA	* resp_ins = (INSMETA *)resp->result;
        rg_conn * rg_connection;
        int i;

        for(i = 0; i < connection->rg_list_len; i++)
        {
            if((resp_ins->i_hdr.rg_info.rg_port == connection->rg_list[i]->rg_server_port)
                     &&(!strcmp(resp_ins->i_hdr.rg_info.rg_addr, connection->rg_list[i]->rg_server_ip))
                     &&(connection->rg_list[i]->status == ESTABLISHED))
            {
                rg_connection = connection->rg_list[i];
                break;
            }
        }
        if(i == connection->rg_list_len)
        {
            rg_connection = (rg_conn *)MEMALLOCHEAP(sizeof(rg_conn));
            rg_connection->rg_server_port = resp_ins->i_hdr.rg_info.rg_port;
            strcpy(rg_connection->rg_server_ip, resp_ins->i_hdr.rg_info.rg_addr);

            if((rg_connection->connection_fd = conn_open(rg_connection->rg_server_ip, rg_connection->rg_server_port)) < 0)
            {
                perror("error in create connection with rg server: ");
                return FALSE;
            }
            rg_connection->status = ESTABLISHED;

            connection->rg_list[connection->rg_list_len] = rg_connection;
            connection->rg_list_len++;
                
        }

        memcpy(resp->result, RPC_REQUEST_MAGIC, RPC_MAGIC_MAX_LEN);
            
        memset(send_rg_buf, 0, LINE_BUF_SIZE);
        memcpy(send_rg_buf, RPC_REQUEST_MAGIC, RPC_MAGIC_MAX_LEN);
        memcpy(send_rg_buf + RPC_MAGIC_MAX_LEN, resp->result, resp->result_length);
        memcpy(send_rg_buf + RPC_MAGIC_MAX_LEN + resp->result_length, cmd, send_buf_size);
          
        write(rg_connection->connection_fd, send_rg_buf, 
                        (resp->result_length + send_buf_size + RPC_MAGIC_MAX_LEN));

        rg_resp = conn_recv_resp(rg_connection->connection_fd);
        if (rg_resp->status_code != RPC_SUCCESS)
        {
            printf("\n ERROR in rg_server response \n");
            return FALSE;
        }

		if((querytype == INSERT) && (rg_resp->result_length))
        {
            char *cli_add_sstab = "addsstab into ";
            int new_size = resp->result_length + 64 + STRLEN(cli_add_sstab);

            char * new_buf = MEMALLOCHEAP(new_size);
            MEMSET(new_buf, new_size);

            char newsstabname[SSTABLE_NAME_MAX_LEN];

            MEMSET(newsstabname, SSTABLE_NAME_MAX_LEN);

            MEMCPY(newsstabname, resp->result, SSTABLE_NAME_MAX_LEN);

            sprintf(new_buf, "addsstab into %s (%s, %s)", tab_name, newsstabname,
                resp->result + SSTABLE_NAME_MAX_LEN);

			MEMSET(send_buf, LINE_BUF_SIZE);
            MEMCPY(send_buf, RPC_REQUEST_MAGIC, RPC_MAGIC_MAX_LEN);
            MEMCPY(send_buf + RPC_MAGIC_MAX_LEN, new_buf, new_size);

            write(connection->connection_fd, send_buf, 
                  (new_size + RPC_MAGIC_MAX_LEN));

            resp = conn_recv_resp(connection->connection_fd);
            if (resp->status_code != RPC_SUCCESS)
            {
                printf("\n ERROR in meta_server response \n");
                return FALSE;
            }

            MEMFREEHEAP(new_buf);

        }
				
    }

    if(querytype == SELECT)
    {
        sel_resp_rejoin(rg_resp->result, response, rg_resp->result_length, resp_len, resp->result);
        //strcpy(response, rg_resp->result);
    }
    else
    {
        *resp_len = sizeof(SUC_RET);
        strcpy(response, SUC_RET);
    }

    conn_destroy_resp(resp);
    if((querytype == INSERT) || (querytype == SELECT))
        conn_destroy_resp(rg_resp);

    parser_close();

    return TRUE;
}

int sel_resp_rejoin(char * src_buf, char * dest_buf, int src_len, int * dest_len, char *index_buf)
{
    char col_off_tab[COL_OFFTAB_MAX_SIZE];
    int col_off_idx = COL_OFFTAB_MAX_SIZE;

    INSMETA *ins_meta = (INSMETA *)index_buf;
	index_buf += sizeof(INSMETA);

	TABLEHDR *tab_hdr = (TABLEHDR *)index_buf;
	index_buf += sizeof(TABLEHDR);

	COLINFO *col_info = (COLINFO *)index_buf;

	int i, src_buf_index1, src_buf_index2, dest_buf_index;

	int volcol_count = 1;

	int offset = sizeof(ROWFMT);

	src_buf_index1 = 0;
	src_buf_index2 = src_len - sizeof(int);
	dest_buf_index = 0;

	col_off_idx -= sizeof(int);
    //column number
    *((int *)(col_off_tab + col_off_idx)) = tab_hdr->tab_col;

    for(i = 0; i < tab_hdr->tab_col; i++)
    {
        int col_type = (col_info+i)->col_type;
        if(TYPE_IS_FIXED(col_type))
        {
            MEMCPY(dest_buf + dest_buf_index, src_buf + src_buf_index1, TYPE_GET_LEN(col_type));

            col_off_idx -= sizeof(int);
            *((int *)(col_off_tab + col_off_idx)) = dest_buf_index;

            dest_buf_index += TYPE_GET_LEN(col_type);
            src_buf_index1 += TYPE_GET_LEN(col_type);
        }
        else
        {
            int valcol_len = (ins_meta->varcol_num == volcol_count)?
                                   src_buf_index2 - (*((int *)(src_buf + src_buf_index2)) - offset):
                                   *((int *)(src_buf + src_buf_index2 - sizeof(int))) - *((int *)(src_buf + src_buf_index2));
            MEMCPY(dest_buf + dest_buf_index, src_buf + *((int *)(src_buf + src_buf_index2)) - offset, valcol_len);

            col_off_idx -= sizeof(int);
            *((int *)(col_off_tab + col_off_idx)) = dest_buf_index;
            
            dest_buf_index += valcol_len;
            src_buf_index2 -= sizeof(int);
            volcol_count ++;
        }
    }

    if (COL_OFFTAB_MAX_SIZE > col_off_idx)
    {
        MEMCPY(dest_buf + dest_buf_index, col_off_tab + col_off_idx, COL_OFFTAB_MAX_SIZE - col_off_idx);
        dest_buf_index += (COL_OFFTAB_MAX_SIZE - col_off_idx);
    }

    *dest_len = dest_buf_index;

    return TRUE;
}


