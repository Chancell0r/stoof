/******************************************************************************
file		: stat.c
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

#include "stat.h"
#include "../string_funcs.h"
#include "../global_defs.h"
#include "../k_t.h"
#include "../mino_t.h"
#include "../proc_t.h"


extern k_t* kr;


int do_stat(char *pathname, struct stat *stPtr, proc_t* pr)
{
	int i					= 0;
	int inum				= 0; 
	int num_splits			= 0;
	int to_free[MAX_DEPTH]	= { 0 };
	char* split[MAX_DEPTH]	= { 0 };
	mino_t* current			= NULL;

	if( is_abs_path( pathname ) )
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
	if( !split_path( pathname , MAX_DEPTH, split, &num_splits ) )
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
	if( !inum )
		goto fail;

	//apparently the device name isn't put into the kernel :(
	stPtr->st_dev =			current->m_fd;
	stPtr->st_ino =			current->m_ino_num;
	stPtr->st_mode =		current->m_ino.i_mode;
	stPtr->st_nlink =		current->m_ino.i_links_count;
	stPtr->st_uid =			current->m_ino.i_uid;
	stPtr->st_gid =			current->m_ino.i_gid;
	stPtr->st_size =		current->m_ino.i_size;
	stPtr->st_blksize = 	1024;
	stPtr->st_blocks =	 	current->m_ino.i_blocks;
	stPtr->st_atime =		current->m_ino.i_atime;
	stPtr->st_ctime =		current->m_ino.i_ctime;
	stPtr->st_mtime =		current->m_ino.i_mtime;


	printf( "filename:\t%s\t", split[num_splits-1] );
	printf( "device:\t%d\n", (i32)stPtr->st_dev );
	printf( "inode:\t%d\t", (i32)stPtr->st_ino );
	printf( "mode:\t%x\t", stPtr->st_mode );
	printf( "link count:\t%u\t", (u32)stPtr->st_nlink );
	printf( "Uid:\t%u\t", stPtr->st_uid );
	printf( "\ngid:\t%u\t", stPtr->st_gid );
	printf( "size:\t%d\t", (i32)stPtr->st_size );
	printf( "blk size:\t%d\t", (i32)stPtr->st_blksize );
	printf( "blocks:\t%d\t\n", (i32)stPtr->st_blocks );
	printf( "creation time:\t%.24s\t\n", ctime( &(stPtr->st_ctime) ) );
	printf( "accessed time:\t%.24s\t\n", ctime( &(stPtr->st_atime) ) );
	printf( "m time:\t\t%.24s\t\n", ctime( &(stPtr->st_mtime) ) );
	
	/*put back all the minos not in use*/
	for( i = 0; i < num_splits; i ++ )
	{
		if( to_free[i] )
			k_t_put_mino( to_free[i] );
	}

	/*free the split*/
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

int stat_cmd( proc_t* pr, int argc, char** argv )
{
	int r  				= 0;
	struct stat mystat;

	if( !kr || !pr || !argc )
		goto fail;

	if(argc < 2)
	{
		printf("Usage is: stat [pathname]");
		goto fail;
	}

	r = do_stat(argv[1], &mystat, pr);

	if(r == -1)
		goto fail;
	
	return 0;

fail:
	return -1;

}
