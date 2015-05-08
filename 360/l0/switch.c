/******************************************************************************
file		: print_proc.c
author		: nc
desc		: prints out the in-memory inodes
date		: 04-01-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "../global_defs.h"
#include "../k_t.h"
#include "../mino_t.h"
#include "../proc_t.h"
#include "switch.h"

extern k_t* kr;

int switch_cmd( proc_t* pr, int argc, char** argv )
{
	int i = 0;

	if( argc != 2 )
	{
		printf( "usage: switch [proc number]\n" );
		goto fail;
	}

	for( i = 0; i < MAX_PROCS; i++ )
	{
		if( kr->m_proc_tb[i] && kr->m_proc_tb[i]->m_pid == atoi(argv[1]) )
		{
			kr->m_cproc = kr->m_proc_tb[i];
			break;
		}
	}

	if( i >= MAX_PROCS )
	{
		printf( "error: process does not exist\n" );
		goto fail;
	}

	return 0;

fail:
	return -1;
}

