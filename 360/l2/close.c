/******************************************************************************
file		: cd.c
author		: 
desc		: 
date		: 04-01-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "close.h"
#include "../string_funcs.h"
#include "../global_defs.h"
#include "../k_t.h"
#include "../mino_t.h"
#include "../proc_t.h"


extern k_t* kr;

int close_cmd( proc_t* pr, int argc, char** argv )
{
	int i = 0;

	if( argc != 2 )
	{
		printf( "usage: close [fd number]\n" );
		goto fail;
	}

	while( pr->m_fds[i++] != atoi( argv[1] ) && i < NUM_FDS ) {};
	
	if( i >= NUM_FDS )
	{
		printf( "error: file descriptor is not open by this process\n" );
		goto fail;
	}
	
	/*tell the kernel to close it*/
	if( !k_t_close_oft_entry( kr->m_of_tb[ atoi( argv[1] ) ]->m_minoptr ) )
	{
		printf( "error closing kernel kft entry\n" );
		goto fail;
	}

	/*put nothing there*/
	pr->m_fds[ atoi( argv[1] ) ] = 0;

	return 0;

fail:

	return -1;
}



