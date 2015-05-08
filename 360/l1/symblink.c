/******************************************************************************
file		: symblink.c
author		: 
desc		: 
date		: 04-20-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "symblink.h"
#include "../string_funcs.h"
#include "../global_defs.h"
#include "../k_t.h"
#include "../mino_t.h"
#include "../proc_t.h"


extern k_t* kr;

int symblink_cmd( proc_t* pr, int argc, char** argv )
{
	char b[BLK_SZ]			= { '\0' };		//get higher
	char* split[MAX_DEPTH]	= { '\0' };		//split path elements
	char bn_2[MAX_PATH]		= { '\0' };		//second basename
	char dn_2[MAX_PATH]		= { '\0' };		//second dirname
	int to_free[MAX_DEPTH]	= { 0 };		//array of minos to put back
	int num_splits			= 0;
	int i					= 0;
	int inum				= 0;
	int newdir_size			= 0;
	int int_size			= 0;
	int sub_val			= 0;

	mino_t* current			= NULL;			//current directory
	mino_t* new_mino		= NULL;
	mino_t* dst				= NULL;			//destination dir
	char* cp				= NULL;			//	"
	ext2_dir* dp			= NULL;			//	"
	mnt_t* mt				= NULL;		//mount
	inode new_ino			= { 0 };	//inode we are adding

	if( !kr )
		goto fail;

	if( argc != 3 )
	{
		printf( "usage: symlink [file 1] [file 2]\n" );
		goto fail;
	}

	/*get first and second basenames*/
	strcpy( bn_2, loc_basename( argv[2] ) );
	strcpy( dn_2, loc_dirname( argv[2] ) );	

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
		dst = kr->m_mino_tb[0];
	}
	else
	{
		dst = pr->m_cwd;
	}

	/*current and dst now point at the starting directories*/
	if( !current || !dst )
	{
		printf( "error: couldn't find starting directory\n" );
		goto fail;
	}

	/*split the first path*/
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

	/*put back all the minos not in use*/
	for( i = 0; i < num_splits; i ++ )
	{
		if( to_free[i] )
			k_t_put_mino( to_free[i] );
	}

	/*if the file/dir can't be found*/
	if(!inum)
		goto fail;

	/*don't get the node yet - find where we are putting it*/
	memset( b, 0, BLK_SZ );

	/*free the first split*/
	for( i = 0; i < num_splits; i++ )
	{
		if( split[i] )
		{
			free( split[i] );
			split[i] = NULL;
		}
	}

	/*split path two*/
	if( !split_path( dn_2, MAX_DEPTH, split, &num_splits ) )
	{
		printf( "error splitting path\n" );
		goto fail;
	}

	/*walk through the directories*/
	memset( to_free, 0, MAX_PATH );

	for( i = 0; i < num_splits; i++ )
	{
		/*find the inode number for the current directory*/
		inum = k_t_find_dir_num( dst, split[i] );
		if( inum != -1 )
		{
			/*get the next inode*/
			to_free[i] = inum;
			k_t_get_mino( dst->m_fd, inum, &dst );
		}
		else
		{
			printf( "could not find directory %s\n", split[i] );
			goto fail;
		}
	}

	/*start of symbolic link code*/
	if( k_t_find_dir_num( dst , bn_2 ) != -1 )
	{
		printf( "error, directory already exists\n" );
		goto fail;
	}

	/*make the inode*/
	mino_t_set_ino( &new_ino,
			pr->m_uid,
			pr->m_gid,
			BLK_SZ,
			DEF_LMODE,
			1 );

	/*write it to disk*/
	inum = k_t_add_ino_to_fs( current->m_fd, &new_ino );
	
	if( inum == 0 )
	{
		printf( "error adding new inode to the filesystem\n" );
		goto fail;
	}
	
	/*re-open it as a mino to do work*/
	k_t_get_mino( current->m_fd, inum, &new_mino );
	to_free[i+1] = inum;
	
	if( !new_mino )
	{
		printf( "error retrieving mino\n" );
		goto fail;
	}

	/*add a data block*/
	k_t_add_dblocks_to_mino( new_mino->m_fd, inum, 1 );

	/*lock the fs*/
	mt = k_t_get_mnt_from_fd( new_mino->m_fd );
	while( mt->m_busy ) {};
	mnt_t_lock( mt );

	/*write pathname to data block*/
	k_t_get_blk( b, new_mino->m_fd, new_mino->m_ino.i_block[0] );

	strcpy(b,argv[1]);

	k_t_put_blk( b, new_mino->m_fd, new_mino->m_ino.i_block[0] );

	mnt_t_unlock( mt );

	/*add ourselves to the parent*/
	mt = k_t_get_mnt_from_fd( dst->m_fd );
	while( mt->m_busy ) {};
	mnt_t_lock( mt );
	
	k_t_get_blk( b, dst->m_fd, dst->m_ino.i_block[0] );

	dp = (ext2_dir*)b;
	cp = b;
	
	/*make it a multiple of four*/
	newdir_size = 4 * ( ( 8 + strlen( bn_2 ) + 3 ) / 4 );

	/*find the last directory entry*/
	while( cp < ( b + 1024 ) )
	{
		if( cp + dp->rec_len >= b + 1024 )
			break;

		cp += dp->rec_len;
		dp = (ext2_dir*) cp;
	}

	int_size = 4 * ( ( 8 + dp->name_len + 3 ) / 4 );

	/*add it*/
	if( dp->rec_len - int_size >= newdir_size ) //if there is enough room for the new directory
	{
		sub_val = dp->rec_len - int_size;
		dp->rec_len = int_size;

		cp -= sub_val;
		dp = (ext2_dir*)cp;
		dp->inode = inum;
		
		dp->name_len = strlen( bn_2 );
		
		strncpy( dp->name, bn_2, strlen( bn_2 ) );
		dp->rec_len = sub_val;
		
		dst->m_ino.i_links_count++;
		dst->m_dirty = true;
		dst->m_ino.i_atime = time(0L);
	}

	memcpy( b + ( 1024 - dp->rec_len ), cp, dp->rec_len );		
	dp = (ext2_dir*)b;
	cp = b;

	k_t_put_blk( b, dst->m_fd, dst->m_ino.i_block[0]);//write new data back to system

	/*put everything back*/
	for( i = 0; i < num_splits; i++ )
	{
		if( to_free[i] )
			k_t_put_mino( to_free[i] );
	}

	new_mino->m_refc--;
	k_t_put_mino( new_mino->m_ino_num );

	/*unlock the fs*/
	mnt_t_unlock( mt );
	
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



