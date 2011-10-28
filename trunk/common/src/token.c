/*
** token.c 2010-12-17 xueyingfei
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


#include "global.h"
#include "utils.h"
#include "token.h"

TOKENS  MT_Tokens[] =
{
	{TABCREAT,	"create table"},
	{INSERT,	"insert into"},
	{CRTINDEX,	"create index"},
	{SELECT,	"select"},
	{DELETE,	"delete"},
	{ADDSERVER,	"add server"},
	{ADDSSTAB,	"addsstab into"},
	{DROP,		"drop"},
	{REMOVE,	"remove"},
	{REBALANCE,	"rebalance"},
	{SELECTRANGE,	"selectrange"},
	{MCC,		"mcc"},
};

int
token_validate(char *token)
{
	int i;
	int tok_num;

	tok_num = 0;

	for (i = 0;  i < MAXSI_FIXED_TOKENS; i++)
	{
		if (!strcmp(token, MT_Tokens[i].tokstring))
		{
			tok_num = MT_Tokens[i].toknum;
			break;
		}
	}

	return tok_num;
}

