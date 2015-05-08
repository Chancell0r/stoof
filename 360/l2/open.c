/******************************************************************************
file		: cd.c
author		: 
desc		: 
date		: 04-01-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "open.h"
#include "../string_funcs.h"
#include "../global_defs.h"
#include "../k_t.h"
#include "../mino_t.h"
#include "../proc_t.h"


extern k_t* kr;

int open_cmd( proc_t* pr, int argc, char** argv )
{
	char dn[MAX_NAME]		= { '\0' };
	char bn[MAX_NAME]		= { '\0' };
	char* split[MAX_DEPTH]	= { '\0' };
	int to_free[MAX_DEPTH]	= { 0 };
	int num_splits			= 0;
	int i					= 0;
	int inum				= 0;
	int fd_returned			= 0;
	int proc_loc			= 0;
	E_FILE_MODE mode		= 0;
	mino_t* current			= NULL;
	mino_t* target			= NULL;


	if( !pr || !argc || !argv )
		goto fail;

	if( argc != 3 )
	{
		printf( "usage: open [file] [r|w|rw|a]\n" );
		goto fail;
	}

	/*figure out the mode*/
	if( !strcmp( argv[2], "r" ) )
	{
		mode = M_READ;
	}
	else if( !strcmp( argv[2], "w" ) )
	{
		mode = M_WRITE;
	}
	else if( !strcmp( argv[2], "rw" ) )
	{
		mode = M_READWRITE;
	}
	else if( !strcmp( argv[2], "a" ) )
	{
		mode = M_APPEND;
	}
	else
	{
		printf( "invalid mode\n" );
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
		printf( "error: file to open does not exist\n" );
		goto fail;
	}

	/*now open the inode num*/
	k_t_get_mino( current->m_fd, inum, &target );

	if( !target )
	{
		printf( "error retrieveing m_inode %i\n", inum );
		goto fail;
	}

	/*add it to the kft*/
	if( !k_t_open_oft_entry( target, mode, &fd_returned, argv[1] ) )
	{
		printf( "error adding inode to oft\n" );
		goto fail;
	}

	/*add it to the process*/
	if( ( proc_loc = proc_t_get_next_fd_loc( pr ) ) == -1 )
	{
		printf( "error adding inode to oft\n" );
		goto fail;
	}

	pr->m_fds[proc_loc] = fd_returned;

	return 0;

fail:
	return -1;
}



