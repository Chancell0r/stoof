/******************************************************************************
file		: touch.c
author		: 
desc		: 
date		: 04-20-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "touch.h"
#include "../string_funcs.h"
#include "../global_defs.h"
#include "../k_t.h"
#include "../mino_t.h"
#include "../proc_t.h"


extern k_t* kr;


int touch_cmd( proc_t* pr, int argc, char** argv )
{
	int i					= 0;
	int inum				= 0; 
	int num_splits			= 0;
	int to_free[MAX_DEPTH]	= { 0 };
	char* split[MAX_DEPTH]	= { 0 };
	mino_t* current			= NULL;

	if(argc < 2)
	{
		printf("Usage: touch [pathname]\n");
		goto fail;
	}

	if( is_abs_path( argv[1] ) )
	{
		current = kr->m_mino_tb[0];
	}
	else
	{
		current = pr->m_cwd;
	}
	
	/*current and dst now point at the starting directories*/
	if( !current)
	{
		printf( "error: couldn't find starting directory\n" );
		goto fail;
	}

	/*split the first path*/
	if( !split_path( argv[1] , MAX_DEPTH, split, &num_splits ) )
	{
		printf( "error splitting path\n" );
		goto fail;
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

	/*if the file/dir can't be found*/
	if(!inum)
		goto fail;

	current->m_ino.i_mtime = time(0L);
	current->m_ino.i_atime = time(0L);

	/*put back all the minos not in use*/
	for( i = 0; i < num_splits; i ++ )
	{
		if( to_free[i] )
			k_t_put_mino( to_free[i] );
	}

	/*free what is left*/
	for( i = 0; i < num_splits; i++ )
	{
		if( split[i] )
		{
			free( split[i] );
			split[i] = NULL;
		}
	}

	return 0;
	

fail:
	/*free what is left*/
	for( i = 0; i < num_splits; i++ )
	{
		if( split[i] )
		{
			free( split[i] );
			split[i] = NULL;
		}
	}

	return -1;

}
