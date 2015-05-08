/******************************************************************************
file		: mkdir.c
author		: 
desc		: 
date		: 04-01-13
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

int mkdir_cmd( proc_t* pr, int argc, char** argv )
{
	char dn[MAX_NAME]		= { '\0' };	//path to folder to create
	char bn[MAX_NAME]		= { '\0' };	//name of folder to create
	char b[BLK_SZ]			= { '\0' };
	char* split[MAX_DEPTH]	= { '\0' };	//split path elements
	char* cp				= NULL;
	int to_free[MAX_DEPTH]	= { 0 };	//array of minos to put back
	int num_splits			= 0;
	int i					= 0;
	int inum				= 0;
	int newdir_size			= 0;
	int int_size			= 0;
	int sub_val				= 0;
	mino_t* current			= NULL;		//current directory
	mino_t* new_mino		= NULL;
	inode new_ino			= { 0 };	//inode we are adding
	mnt_t* mt				= NULL;		//mount
	ext2_dir* dp			= NULL;

	if( !kr )
		goto fail;

	if( argc != 2 )
	{
		printf( "usage: mkdir directory\n" );
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

	/*start of mkdir code*/
	if( k_t_find_dir_num( current, bn ) != -1 )
	{
		printf( "error, directory already exists\n" );
		goto fail;
	}

	/*make the inode*/
	mino_t_set_ino( &new_ino,
			pr->m_uid,
			pr->m_gid,
			BLK_SZ,
			DEF_DMODE,
			DEF_DLINKS );

	/*write it to disk*/
	inum = k_t_add_ino_to_fs( current->m_fd, &new_ino );
	
	if( inum == 0 )
	{
		printf( "error adding new inode to the filesystem\n" );
		goto fail;
	}
	
	/*re-open it as a mino to do work*/
	k_t_get_mino( current->m_fd, inum, &new_mino );

	if( !new_mino )
	{
		printf( "error retrieveing mino\n" );
		goto fail;
	}

	/*add a data block*/
	k_t_add_dblocks_to_mino( new_mino->m_fd, inum, 1 );

	/*lock the fs*/
	mt = k_t_get_mnt_from_fd( new_mino->m_fd );
	while( mt->m_busy ) {};
	mnt_t_lock( mt );

	k_t_get_blk( b, new_mino->m_fd, new_mino->m_ino.i_block[0] );

	/*add parents*/
	dp = (ext2_dir*)b;
	dp->inode = inum;
	strncpy( dp->name, ".", 1 );
	dp->name_len = 1;
	dp->rec_len = 12;

	cp = b;
	cp += dp->rec_len;
	dp = (ext2_dir*)cp;
	
	dp->inode = current->m_ino_num;
	dp->name_len = 2;
	strncpy(dp->name, "..",2);
	dp->rec_len = BLK_SZ - 12;

	k_t_put_blk( b, new_mino->m_fd, new_mino->m_ino.i_block[0] );
	mnt_t_unlock( mt );

	/*add ourselves to the parent*/
	mt = k_t_get_mnt_from_fd( current->m_fd );
	while( mt->m_busy ) {};
	mnt_t_lock( mt );

	k_t_get_blk( b, current->m_fd, current->m_ino.i_block[0] );

	dp = (ext2_dir*)b;
	cp = b;
	
	/*make it a multiple of four*/
	newdir_size = 4 * ( ( 8 + strlen( bn ) + 3 ) / 4 );

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
		
		dp->name_len = strlen( bn );
		
		strncpy( dp->name, bn, strlen( bn ) );
		dp->rec_len = sub_val;
		
		current->m_ino.i_links_count++;
		current->m_dirty = true;
		current->m_ino.i_atime = time(0L);
	}

	memcpy( b + ( 1024 - dp->rec_len ), cp, dp->rec_len );		
	dp = (ext2_dir*)b;
	cp = b;

	k_t_put_blk( b, current->m_fd, current->m_ino.i_block[0]);//write new data back to system
	

	/*put everything back*/
	for( i = 0; i < num_splits; i++ )
	{
		if( to_free[i] )
			k_t_put_mino( to_free[i] );
	}

	/*de-reference new inode*/
	new_mino->m_refc--;
	k_t_put_mino( new_mino->m_ino_num );


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



