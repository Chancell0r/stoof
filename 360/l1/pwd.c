/******************************************************************************
file		: pwd.c
author		: 
desc		: 
date		: 04-01-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pwd.h"
#include "../string_funcs.h"
#include "../global_defs.h"
#include "../k_t.h"
#include "../mino_t.h"
#include "../proc_t.h"


extern k_t* kr;

int pwd_cmd( proc_t* pr, int argc, char** argv )
{
	char b[BLK_SZ]			= { '\0' };
	char* split[MAX_DEPTH]	= { '\0' };	//split path elements
	int to_free[MAX_DEPTH]	= { 0 };	//array of minos to put back
	int num_splits			= 0;
	int i					= 0;
	int inum				= 0;
	mino_t* current			= NULL;		//current directory
	mino_t* parent			= NULL;
	char* cp				= NULL;
	ext2_dir* dp			= NULL;

	if( !kr )
		goto fail;

	/*start at cwd*/
	current = pr->m_cwd;
	parent = kr->m_mino_tb[0];

	while( current &&
			( inum = k_t_find_dir_num( current, ".." ) ) )
	{
		/*find the parent*/
		k_t_get_mino( current->m_fd, inum, &parent );

		if(	current->m_ino_num == parent->m_ino_num )
			break;

		if( !parent )
			goto fail;

		to_free[i] = inum;
		i++;

		/*find our name*/
		k_t_get_blk( b, parent->m_fd, parent->m_ino.i_block[0] );
		cp = b;
		dp = (ext2_dir*)b;

		/*walk through the entries*/
		while( cp < ( b + 1024 ) )
		{
			/*found a match*/
			if( dp->inode == current->m_ino_num )
			{
				/*get memory and copy the name over*/
				split[num_splits] = malloc( dp->name_len + 1 );

				if( !split[num_splits] )
				{
					printf( "error: no memory\n" );
					goto fail;
				}

				memset( split[num_splits], 0, dp->name_len + 1 );
				strncpy( split[num_splits], dp->name, dp->name_len );
				num_splits++;
				break;
			}

			cp += dp->rec_len;
			dp = (ext2_dir*) cp;
		}

		current = parent;
		parent = NULL;
	}

	/*print the dir*/
	printf( "/" );

	for( i = 0; i < num_splits; i++ )
	{
		printf( "%s/", split[num_splits - i - 1] );
	}

	printf( "\n" );


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




