/******************************************************************************
file		: cd.c
author		: 
desc		: 
date		: 04-01-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cat.h"
#include "../string_funcs.h"
#include "../global_defs.h"
#include "../k_t.h"
#include "../mino_t.h"
#include "../proc_t.h"


extern k_t* kr;

int cat_cmd( proc_t* pr, int argc, char** argv )
{
	char dn[MAX_NAME]		= { '\0' };
	char bn[MAX_NAME]		= { '\0' };
	char* split[MAX_DEPTH]	= { '\0' };
	int to_free[MAX_DEPTH]	= { 0 };
	int i					= 0;
	long iterations			= 0;
	long remainder			= 0;
	i32 ret					= 0;
	int inum				= 0;
	int num_splits			= 0;
	i32 fd_to_cat			= 0;
	char b[8*BLK_SZ]		= { '\0' };
	mino_t* current			= NULL;
	mino_t* target			= NULL;

	if( argc != 2 )
	{
		printf( "usage: cat [file]\n" );
		goto fail;
	}

	/*split into dirname and basename*/
	strcpy( bn, loc_basename( argv[1] ) );
	strcpy( dn, loc_dirname( argv[1] ) );

	if( is_abs_path( bn ) )
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

	if( !split_path( dn, MAX_DEPTH, split, &num_splits ) )
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

	/*free the split*/
	for( i = 0; i < num_splits; i++ )
	{
		if( split[i] )
		{
			k_t_put_mino( to_free[i] );
			free( split[i] );
			split[i] = NULL;
		}
	}

	/*current now points at the target directory*/
	inum = k_t_find_child_by_name( current, bn );

	if( inum == -1 )
	{
		printf( "error: file to cat does not exist\n" );
		goto fail;
	}

	/*now open the inode num*/
	k_t_get_mino( current->m_fd, inum, &target );

	if( !target )
	{
		printf( "error retrieveing m_inode %i\n", inum );
		goto fail;
	}

	/*get the fd*/
	fd_to_cat = k_t_find_oft_entry( target );

	if( fd_to_cat == -1 )
	{
		printf( "error: file does not exist in kft\n" );
		goto fail;
	}

	/*seek to the beginning*/
	k_t_lseek( fd_to_cat, 0 );

	/*calculate number of bytes to read*/
	iterations = ( target->m_ino.i_size / ( 8 * BLK_SZ ) ) + 1;
	remainder =  target->m_ino.i_size % ( 8 * BLK_SZ );

	/*loop*/
	while( iterations != 0 )
	{	
		/*reset the block*/
		memset( b, 0, 8 * BLK_SZ );

		/*if the size is less than the total buffer size*/
		if( iterations == 1 )
		{
			if( ( ret = k_t_read( fd_to_cat, remainder, b ) ) == -1 )
			{
				printf( "error reading file\n" );
				goto fail;
			}
		}
		else
		{
			if( ( ret = k_t_read( fd_to_cat, 8 * BLK_SZ, b ) ) == -1 )
			{
				printf( "error reading file\n" );
				goto fail;
			}
		}

		for( i = 0; i < ret; i ++ )
		{
			printf( "%c", b[i] );
		}

		iterations --;

	}

	/*put it back*/
	k_t_put_mino( inum );


	printf( "\n\n" );

	return 0;

fail:

	return -1;
}




