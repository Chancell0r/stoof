/******************************************************************************
file		: cd.c
author		: 
desc		: 
date		: 04-01-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cd.h"
#include "../string_funcs.h"
#include "../global_defs.h"
#include "../k_t.h"
#include "../mino_t.h"
#include "../proc_t.h"


extern k_t* kr;

int cd_cmd( proc_t* pr, int argc, char** argv )
{
	char* split[MAX_DEPTH]	= { '\0' };	//split path elements
	int to_free[MAX_DEPTH]	= { 0 };	//array of minos to put back
	int num_splits			= 0;
	int i					= 0;
	int inum				= 0;
	mino_t* current			= NULL;		//current directory

	if( !kr )
		goto fail;

	if( argc > 1 )
	{

		if( is_abs_path( argv[1] ) )
		{
			current = kr->m_mino_tb[0];
		}
		else
		{
			current = pr->m_cwd;
		}

		if( !current )
		{
			printf( "error: couldn't find starting directory\n" );
			goto fail;
		}


		if(argc == 2 )
		{
			if( !split_path( argv[1], MAX_DEPTH, split, &num_splits ) )
			{
				printf( "error splitting path\n" );
				goto fail;
			}

		}

		/*walk through the directories*/
		for( i = 0; i < num_splits; i++ )
		{
			/*find the inode number for the current directory*/
			inum = k_t_find_dir_num( current, split[i] );
			if( inum != -1 )
			{
				/*get the next inode*/
				to_free[i] = inum;
				k_t_get_mino( current->m_fd, inum, &current );
			}
			else
			{
				printf( "could not find directory %s\n", split[i] );
				goto fail;
			}
		}
	}
	else
	{
		current = kr->m_mino_tb[0];
	}

	pr->m_cwd = current;

	/*put everything back*/
	for( i = 0; i < num_splits; i++ )
	{
		if( to_free[i] )
			k_t_put_mino( to_free[i] );
	}

	/*free the split*/
	for( i = 0; i < num_splits; i++ )
	{
		if( split[i] )
			free( split[i] );
	}

	return 0;

fail:

	/*free the split*/
	for( i = 0; i < num_splits; i++ )
	{
		if( split[i] )
			free( split[i] );
	}


	return -1;
}



