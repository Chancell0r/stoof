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
#include "../l1/creat.h"

extern k_t* kr;

int cp_cmd( proc_t* pr, int argc, char** argv )
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
	char* dst[MAX_DEPTH] = {'\0'};
	E_FILE_MODE mode		= 0;
	mino_t* current			= NULL;
	mino_t* target			= NULL;
	char b[8 * BLK_SZ] 		= { '\0'};
	int remainder			= 0;
	int iteration			= 0;
	int fd_to_rd			= 0;
	int fd_to_wr			= 0;
	int ret				= 0;


	if( !pr || !argc || !argv )
		goto fail;


	if( argc < 3 )
	{
		printf( "usage: cp [source pathname] [destination pathname]\n" );
		goto fail;
	}

	/*split into dirname and basename*/
	strcpy( bn, loc_basename( argv[2] ) );
	strcpy( dn, loc_dirname( argv[2] ) );
	

	if( is_abs_path( argv[1]) )
	{
		current = kr->m_mino_tb[0];
	}
	else
	{
		current = pr->m_cwd;
	}

	if( is_abs_path( argv[2] ) )
	{
		target = kr->m_mino_tb[0];
	}
	else
	{
		target = pr->m_cwd;
	}

	if( !current )
	{
		printf( "error: couldn't find starting directory\n" );
		goto fail;
	}

	if( !split_path( argv[1], MAX_DEPTH, split, &num_splits ) )
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

	mode = M_READ;

	/*add it to the kft*/
	if( !k_t_open_oft_entry( current, mode, &fd_returned, argv[1] ) )
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
	
	/*make use of creat command*/
	dst[0] = malloc(strlen(argv[0] +1));
	memset( dst[0], 0, strlen( argv[0]) + 1 );
	strcpy( dst[0], argv[0] );
	dst[1] = malloc(strlen(argv[2] +1));
	memset( dst[1], 0, strlen( argv[2]) + 1 );
	strcpy( dst[1], argv[2] );

	
	creat_cmd(pr, argc-1, dst);
	free(dst[0]);
	free(dst[1]);


	if( !target )
	{
		printf( "error: couldn't find starting directory\n" );
		goto fail;
	}

	if( !split_path( argv[2], MAX_DEPTH, split, &num_splits ) )
	{
		printf( "error splitting path\n" );
		goto fail;
	}

	/*walk through the directories*/
	for( i = 0; i < num_splits; i++ )
	{
		/*find the inode number for the current directory*/
		inum = k_t_find_dir_num( target, split[i] );
		if( inum != -1 )
		{
			/*get the next inode*/
			to_free[i] = inum;
			k_t_get_mino( target->m_fd, inum, &target );
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

	mode = M_WRITE;

	/*add it to the kft*/
	if( !k_t_open_oft_entry( target, mode, &fd_returned, argv[2] ) )
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

	iteration = (current->m_ino.i_size / (8 * BLK_SZ)) + 1;
	remainder = current->m_ino.i_size % (8 * BLK_SZ);

	/*go to beginning of each file*/
	fd_to_rd = k_t_find_oft_entry(current);
	k_t_lseek(fd_to_rd, 0);

	fd_to_wr = k_t_find_oft_entry(target);
	k_t_lseek(fd_to_wr, 0);

	/*copy data over*/
	while(iteration != 0)
	{
		memset(b, 0 , 8*BLK_SZ);
		if( iteration == 1)
		{
			k_t_read(fd_to_rd, remainder, b);
			ret += k_t_write(fd_to_wr, remainder, b);
			iteration--;
		}
		else
		{
			k_t_read(fd_to_rd, (8 * BLK_SZ), b);
			ret += k_t_write(fd_to_wr, (8 * BLK_SZ), b);
			iteration--;
		}	
	}

	printf("%d bytes copied\n", ret);

	/*put minodes and get rid of of_t variables*/
	k_t_close_oft_entry(current);
	k_t_close_oft_entry(target);

	
	return 0;
fail:
	return -1;
	



}
