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
#include "print_procs.h"

extern k_t* kr;

int print_procs( proc_t* pr, int argc, char** argv )
{
	int i		= 0;

	if( !kr || !argc )
		goto fail;

	printf( "process information\n" );
	printf( "\tuid\tpid\tgid\tparid\tstatus\n" );

	for( i = 0; i < MAX_PROCS; i++ )
	{
		if( kr->m_proc_tb[i] )
		{
			printf( "\t%i\t%i\t%i\t%i\t%i\n",
					kr->m_proc_tb[i]->m_uid,
					kr->m_proc_tb[i]->m_pid,
					kr->m_proc_tb[i]->m_gid,
					kr->m_proc_tb[i]->m_parid,
					kr->m_proc_tb[i]->m_status );
		}
	}

	printf( "\n" );


ok:
	return true;

fail:
	return false;
}

