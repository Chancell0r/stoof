/******************************************************************************
file		: cd.c
author		: 
desc		: 
date		: 04-01-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "write.h"
#include "../string_funcs.h"
#include "../global_defs.h"
#include "../k_t.h"
#include "../mino_t.h"
#include "../proc_t.h"


extern k_t* kr;

int write_cmd( proc_t* pr, int argc, char** argv )
{
	int i				= 0;
	int j 				= 0;
	i32 ret				= 0;
	i32 remainder			= 0;
	i32 iteration			= 0;
	char b[8*BLK_SZ]	= { '\0' };

	if( argc < 3 )
	{
		printf( "usage: [fd number] [string to write]\n" );
		goto fail;
	}

	if( atoi( argv[1] ) > 8 * BLK_SZ )
	{
		printf( "error: size too large, max is %i\n", 8 * BLK_SZ );
		goto fail;
	}

	iteration = (strlen(argv[2]) / (8 * BLK_SZ) ) + 1;
	remainder = strlen(argv[2]) % (8 * BLK_SZ);
	
	printf("%d rem %d it\n", remainder, iteration);
	while(iteration != 0)
	{
		memset(b, 0 , 8*BLK_SZ);
		if( iteration == 1)
		{
			for(i = 0; i < remainder; i++)
				b[i] = argv[2][(j * 8 * BLK_SZ) + i];
			if( ( ret += k_t_write( atoi( argv[1] ), remainder , b ) ) == -1 )
			{
				printf( "error writing file\n" );
				goto fail;
			}
			iteration--;
		}
		else
		{
			for(i = 0; i < 8 * BLK_SZ; i++)
				b[i] = argv[2][(j * 8 * BLK_SZ)+i];
			if( ( ret += k_t_write( atoi( argv[1] ), 8*BLK_SZ , b ) ) == -1 )
			{
				printf( "error writing file\n" );
				goto fail;
			}
			j++;	
			iteration--;
		}	
	}

	printf( "bytes written: %i from fd %i\n", ret, atoi( argv[1] ) );


	return 0;

fail:

	return -1;
}



