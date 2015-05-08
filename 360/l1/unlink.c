/******************************************************************************
file		: link.c
author		: 
desc		: 
date		: 04-01-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "unlink.h"
#include "../string_funcs.h"
#include "../global_defs.h"
#include "../k_t.h"
#include "../mino_t.h"
#include "../proc_t.h"


extern k_t* kr;

int unlink_cmd( proc_t* pr, int argc, char** argv )
{
	char* split[MAX_DEPTH]	= { '\0' };		//split path elements
	char bn[MAX_PATH]		= { '\0' };		//fist basename
	char dn[MAX_PATH]		= { '\0' };		//first dirname
	int to_free[MAX_DEPTH]	= { 0 };		//array of minos to put back
	int num_splits			= 0;
	int i					= 0;
	u32 inum				= 0;
	mino_t* current			= NULL;			//current directory
	mino_t* target			= NULL;

	if( !kr )
		goto fail;

	if( argc != 2 )
	{
		printf( "usage: unlink [file]\n" );
		goto fail;
	}

	/*get first and second basenames*/
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


	/*current and dst now point at the starting directories*/
	if( !current )
	{
		printf( "error: couldn't find starting directory\n" );
		goto fail;
	}

	/*split the first path*/
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

	/*put back all the minos not in use*/
	for( i = 0; i < num_splits; i ++ )
	{
		if( to_free[i] )
			k_t_put_mino( to_free[i] );
	}

	/*get the inum of the child before it is deleted*/
	inum = k_t_find_dir_num( current, bn );

	/*remove the child from the data blocks*/
	if( !k_t_remove_child( current, bn ) )
	{
		printf( "error: could not remove child %s from inode\n", bn );
		goto fail;
	}
	
	/*decrease the link count of the inode*/
	if( !k_t_get_mino( current->m_fd, inum, &target ) )
	{
		printf( "error retrieveing target inode\n" );
		goto fail;
	}

	target->m_ino.i_links_count--;

	/*this is on purpose*/
	if( target->m_ino.i_links_count == 0 )
	{
		k_t_put_mino( inum );
		k_t_del_ino_from_fs( current->m_fd, inum );
	}
	else
	{
		k_t_put_mino( inum );
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

	/*free the split*/
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





