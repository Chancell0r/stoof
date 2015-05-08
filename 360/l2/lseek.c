/******************************************************************************
file		: cd.c
author		: 
desc		: 
date		: 04-01-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lseek.h"
#include "../string_funcs.h"
#include "../global_defs.h"
#include "../k_t.h"
#include "../mino_t.h"
#include "../proc_t.h"


extern k_t* kr;

int lseek_cmd( proc_t* pr, int argc, char** argv )
{
	long o_loc		= 0;

	if( argc != 3 )
	{
		printf( "usage: lseek [fd number] [position]\n" );
		goto fail;
	}

	/*alread wrote this command in the kernel*/
	if( ( o_loc = k_t_lseek( atoi( argv[1] ), atoi( argv[2] ) ) == -1 ) )
		goto fail;

	return o_loc;

fail:
	return -1;
}



