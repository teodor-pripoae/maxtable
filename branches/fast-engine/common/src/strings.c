/*
** strings.c 2010-12-06 xueyingfei
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


/*
** return the start of 
**
**	        0 1 2 3 4 5 6 7 89
**	buf : a b c d c c d x s d
**	sub:          dc
**	rtn: 5
** you can use buf[5-9]
*/
int
str1nstr (char *buf, char *sub, int len)
{
	char	*bp;
	char	*sp;
	int	i;
	int	j;


	i = 0;
	
	if (!*sub)
	{
		return i;
	}
	
	while (*buf && (i < len))
	{
		bp = buf;
		sp = sub;
		j = 0;
		do 
		{
			if (!*sp)
			{
				return (i + j);
			}

			j++;

		} while (*bp++ == *sp++);
		buf++;
		i++;
	}
	
	return i;
}


/*
** return the start of 
**
**	        0 1 2 3 4 5 6 7 89
**	buf : a b c d c c d x s d
**	sub:          dc
**	rtn: 2
** you can use buf[0-2]
*/

int
str01str (char *buf, char *sub, int len)
{
	char	*bp;
	char	*sp;
	int	i;
	
	i = 0;
	if (!*sub)
	{
		return (i - 1);
	}
	
	while (*buf &&  (i < len))
	{
		bp = buf;
		sp = sub;

		do 
		{
			if (!*sp)
			{
				return (i - 1);
			}
	
		} while (*bp++ == *sp++);
		buf++;
		i++;
	}
	
	return (i - 1);
}

/*
**	"./rg_table/yxue/yxuetablet0sstable1313042021"     &  "/",    strlen()
**
**			To
**
**	"yxuetablet0sstable1313042021"
*/

int
strmnstr (char *buf, char *sub, int len)
{
	char	*bp;
	char	*sp;
	int	i;
	int	j;
	int	k;


	i = 0;
	
	if (!*sub)
	{
		return i;
	}
	
	while (*buf && (i < len))
	{
		bp = buf;
		sp = sub;
		j = 0;
		do 
		{
			if (!*sp)
			{
				k = (i + j);
				break;
			}

			j++;

		} while (*bp++ == *sp++);
		buf++;
		i++;
	}
	
	return k;

}



int
str0n_trunc_0t(char *buf, int len, int *start, int *end)
{
	char	*str;

	
	*start = 0;
	*end = len;
	str = buf;
	
	/* Normalize this command. It's just a simple op. */
	while (((*str == ' ') || (*str == '\t')) && ((*start) < len))
	{
		str++;
		(*start)++;		
	}

	str = &(buf[len - 1]);
	
	while (((*str == ' ') || (*str == '\t')) && ((*end) > (*start)))
	{
		str--;
		(*end)--;		
	}

	return TRUE;
}

