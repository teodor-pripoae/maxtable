/*
** netconn.h 2010-06-28 xueyingfei
**
** Copyright flying/xueyingfei.
**
** This file is part of MaxTable.
**
** Licensed under the Apache License, Version 2.0
** (the "License"); you may not use this file except in compliance with
** the License. You may obtain a copy of the License at
**
** http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
** implied. See the License for the specific language governing
** permissions and limitations under the License.
*/
#ifndef NETCONN_H_
#define NETCONN_H_

#include "global.h"

/** RPC status **/
#define RPC_SUCCESS	0	/* RPC has been processed successfully. */
#define RPC_CONN_FAIL	1	/* Connection is failed. */
#define RPC_FAIL	2 	/* Fail to process the RPC. */
#define RPC_PARSER_ERR	3	/* Fail to parse the request. */

#define RPC_MAGIC_MAX_LEN	8
#define RPC_REQUEST_MAGIC "rpcrqst"
#define RPC_RESPONSE_MAGIC "rpcresp"

/* Following is the special magic number. */
#define	RPC_DROP_TABLE_MAGIC	"drop"
#define RPC_RG2MASTER_REPORT	"rg_rpt"
#define RPC_RBD_MAGIC		"rebalan"



#define CONF_META_IP    "metaserver"
#define CONF_META_PORT  "metaport"
#define	CONF_RG_IP	"rangerserver"
#define CONF_RG_PORT	"rangerport"


/** The Buffer Pool for holding RPCRESP Data  **/
#define CONN_BUF_SIZE (1024 * 1024)

typedef struct rpcreq
{
	char    magic[RPC_MAGIC_MAX_LEN];
	int     data_size;
	char    *data;
}RPCREQ;

typedef struct rpcresp
{
	char	magic[RPC_MAGIC_MAX_LEN];
	int	status_code; /*  */
	int	result_length; /** used in serializing and deserializing**/
	char	*result;
}RPCRESP;


/*
** Copy data from the transmission buffer to the location
** specified by the pointer.
*/ 
#define GET_FROM_BUFFER( buf, idx, ptr, size )		\
{							\
	MEMCPY ( ptr, buf+idx, size );                  \
	idx += size;                                    \
}

#define PUT_TO_BUFFER( buf, idx, ptr, size )		\
{							\
	MEMCPY (buf+idx, ptr, size);                    \
	idx += size;                                    \
}


#define	RPC_REQ_NORMAL_OP	0x0001
#define RPC_REQ_DROP_OP		0x0002
#define RPC_REQ_REBALANCE_OP	0x0004


char *
conn_insert_resp_parser(char *result, char *rg_ip, int rg_port);

RPCRESP*
conn_build_resp(char *resp_bp);

int
conn_get_resp_size(RPCRESP *resp);

char *
conn_build_resp_byte(int status, int result_len, char *result);

void
conn_destroy_resp(RPCRESP* resp);

RPCREQ *
conn_build_req(char *data, int data_len);

int
conn_req_byte_size(RPCREQ  *req);

void
conn_req_byte_buf(char *rpc_buf, RPCREQ  *req);

void
conn_destroy_req(RPCREQ *req);

int 
conn_open(char* ip_address, int port);

int
conn_send_req(RPCREQ* req, int sockfd);

RPCRESP * 
conn_recv_resp(int sockfd);

void
conn_close(int sockfd, RPCREQ* req, RPCRESP* resp);

int
conn_chk_reqmagic(char *str);

int
conn_chk_respmagic(char *str);

void
conn_destroy_resp_byte(char* resp);



void 
start_daemon(int listenfd, char * (*handler_request)(char *req_buf));

void 
startup(int servPort, int opid, char * (*handler_request)(char *req_buf));

#endif