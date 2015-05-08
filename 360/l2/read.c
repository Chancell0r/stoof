/******************************************************************************
file		: cd.c
author		: 
desc		: 
date		: 04-01-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "read.h"
#include "../string_funcs.h"
#include "../global_defs.h"
#include "../k_t.h"
#include "../mino_t.h"
#include "../proc_t.h"


extern k_t* kr;

int read_cmd( proc_t* pr, int argc, char** argv )
{
	i32 ret				= 0;
	char b[8*BLK_SZ]	= { '\0' };

	if( argc != 3 )
	{
		printf( "usage: [fd number] [bytes to read]\n" );
		goto fail;
	}

	if( atoi( argv[1] ) > 8 * BLK_SZ )
	{
		printf( "error: size too large, max is %i\n", 8 * BLK_SZ );
		goto fail;
	}

	if( ( ret = k_t_read( atoi( argv[1] ), atoi( argv[2] ), b ) ) == -1 )
	{
		printf( "error reading file\n" );
		goto fail;
	}

	printf( "bytes read: %i from fd %i\n", ret, atoi( argv[1] ) );

	/*for( i = 0; i < ret; i ++ )
	{
		printf( "%c", b[i] );
	}

	printf( "\n\n" );*/

	return 0;

fail:

	return -1;
}



