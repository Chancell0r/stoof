/******************************************************************************
file		: mkdir.c
author		: 
desc		: 
date		: 04-17-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "mkdir.h"
#include "../global_defs.h"
#include "../k_t.h"
#include "../mino_t.h"
#include "../proc_t.h"
#include "../string_funcs.h"

extern k_t* kr;

int rmdir_cmd( proc_t* pr, int argc, char** argv )
{
	char dn[MAX_NAME]		= { '\0' };	//path to folder to remove
	char bn[MAX_NAME]		= { '\0' };	//name of folder to remove
	char b[BLK_SZ]			= { '\0' };
	char* split[MAX_DEPTH]	= { '\0' };	//split path elements
	char* cp				= NULL;
	int to_free[MAX_DEPTH]	= { 0 };	//array of minos to put back
	int num_splits			= 0;
	int i					= 0;
	int inum				= 0;
	int par_ino				= 0;
	mino_t* current			= NULL;		//current directory
	mnt_t* mt				= NULL;		//mount
	ext2_dir* dp			= NULL;

	if( !kr )
		goto fail;

	if( argc != 2 )
	{
		printf( "usage: rmdir directory\n" );
		goto fail;
	}

	strcpy( bn, loc_basename( argv[1] ) );
	strcpy( dn, loc_dirname( argv[1] ) );

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

	/*save parent location and enter directory*/
	par_ino = current->m_ino_num;
	inum = k_t_find_dir_num(current, bn);
	i++;
	to_free[i] = inum;

	if(inum == -1) //if not directory
		goto fail;

	k_t_get_mino( current->m_fd, inum, &current);
	k_t_get_blk( b, current->m_fd, current->m_ino.i_block[0] );

	if(current->m_ino.i_links_count > 2)
		goto fail;

	dp = (ext2_dir*)b;
	cp = b;
	
	/*find the last directory entry*/
	while( cp < ( b + 1024 ) )
	{
		if( cp + dp->rec_len >= b + 1024 )
			break;
		cp += dp->rec_len;
		dp = (ext2_dir*) cp;
	}

	/*if their aren't any file names besides . and .. then deletion is easy*/
	if(strncmp(dp->name, ".", 1) != 0)
		goto fail;

	/*delete blocks held by directory*/
	k_t_del_dblocks_from_mino( current->m_fd, 
		current->m_ino_num, 1 );

	/*delete inode*/
	k_t_del_ino_from_fs( current->m_fd, current->m_ino_num);

	/*get parent inode*/
	k_t_get_mino( current->m_fd, par_ino, &current );

	if( !k_t_remove_child( current, bn ) )
	{
		printf( "error: could not remove child %s from inode\n", bn );
		goto fail;
	}

	current->m_ino.i_links_count--;

	/*this is on purpose*/
	if( current->m_ino.i_links_count == 0 )
	{
		k_t_put_mino( par_ino );
		k_t_del_ino_from_fs( current->m_fd, par_ino );
	}
	else
	{
		k_t_put_mino( par_ino );
	}

	/*put everything back*/
	for( i = 0; i < num_splits; i++ )
	{
		if( to_free[i] )
			k_t_put_mino( to_free[i] );
	}

	/*unlock the fs*/
	mnt_t_unlock( mt );

	/*free the split*/
	for( i = 0; i < num_splits; i++ )
	{
		if( split[i] )
			free( split[i] );
	}
	return 0;

fail:
	if( mt )
		mnt_t_unlock( mt );

	/*free the split*/
	for( i = 0; i < num_splits; i++ )
	{
		if( split[i] )
			free( split[i] );
	}

	return -1;
}
