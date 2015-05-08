/******************************************************************************
file		: pfd.c
author		: 
desc		: 
date		: 04-01-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pfd.h"
#include "../string_funcs.h"
#include "../global_defs.h"
#include "../k_t.h"
#include "../mino_t.h"
#include "../proc_t.h"


extern k_t* kr;

int pfd_cmd( proc_t* pr, int argc, char** argv )
{
	int i					= 0;

	if( !kr )
		goto fail;

	printf( "open file descriptor information\n" );
	printf( "\tfd num\t\tmino num\tmino ptr\tref count\toffset\t\tmode\t\tname\n" );

	for( i = 0; i < MAX_OFS; i ++ )
	{
		if( kr->m_of_tb[i] && kr->m_of_tb[i]->m_minoptr )
		{

			printf( "\t%i\t\t%i\t\t%p\t%i\t\t%li\t\t%i\t\t%s\n",
					i,
					kr->m_of_tb[i]->m_minoptr->m_ino_num,
					kr->m_of_tb[i]->m_minoptr,
					kr->m_of_tb[i]->m_refc,
					kr->m_of_tb[i]->m_off,
					kr->m_of_tb[i]->m_mode,
					kr->m_of_tb[i]->m_name );
		}
	}

	printf( "\n" );

	return 0;

fail:
	return -1;
}



