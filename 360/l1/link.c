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

#include "link.h"
#include "../string_funcs.h"
#include "../global_defs.h"
#include "../k_t.h"
#include "../mino_t.h"
#include "../proc_t.h"


extern k_t* kr;

int link_cmd( proc_t* pr, int argc, char** argv )
{
	char b[BLK_SZ]			= { '\0' };		//get higher
	char* split[MAX_DEPTH]	= { '\0' };		//split path elements
	char bn_1[MAX_PATH]		= { '\0' };		//fist basename
	char dn_1[MAX_PATH]		= { '\0' };		//first dirname
	char bn_2[MAX_PATH]		= { '\0' };		//second basename
	char dn_2[MAX_PATH]		= { '\0' };		//second dirname
	char name[MAX_NAME]		= { '\0' };
	int to_free[MAX_DEPTH]	= { 0 };		//array of minos to put back
	int num_splits			= 0;
	int i					= 0;
	int inum				= 0;
	u32 target_inum			= 0;
	int tmp_size			= 0;
	mino_t* current			= NULL;			//current directory
	mino_t* target			= NULL;
	mino_t* dst				= NULL;			//destination dir
	char* cp				= NULL;			//	"
	ext2_dir* dp			= NULL;			//	"

	if( !kr )
		goto fail;

	if( argc != 3 )
	{
		printf( "usage: link [file 1] [file 2]\n" );
		goto fail;
	}

	/*get first and second basenames*/
	strcpy( bn_1, loc_basename( argv[1] ) );
	strcpy( dn_1, loc_dirname( argv[1] ) );
	strcpy( bn_2, loc_basename( argv[2] ) );
	strcpy( dn_2, loc_dirname( argv[2] ) );	

	/*watch me as i navigate ha ha ha ha haaaaaaaaa >.<*/
	if( is_abs_path( bn_1 ) )
	{
		current = kr->m_mino_tb[0];
	}
	else
	{
		current = pr->m_cwd;
	}

	if( is_abs_path( bn_2 ) )
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
	if( !split_path( dn_1, MAX_DEPTH, split, &num_splits ) )
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

	/*current is now our relative dir so find the inode we want*/
	k_t_get_blk( b, pr->m_cwd->m_fd, current->m_ino.i_block[0] );

	dp = (ext2_dir*)b;
	cp = b;

	/*walk*/
	while( true )
	{
		/*have to do this in case of names that contain substrings*/
		memset( name, 0, MAX_NAME );
		memcpy( name, dp->name, dp->name_len );

		/*found it*/
		if( !strcmp( name, bn_1 ) )
			break;

		if( cp >= ( b + 1024 ) )
		{
			printf( "could not find file\n" );
			goto fail;
		}

		cp += dp->rec_len;
		dp = (ext2_dir*)cp;
	}

	/*get dis inode*/
	target_inum = dp->inode;

	if( !inum )
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
		inum = k_t_find_dir_num( current, split[i] );
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

	/*put back all the minos not in use*/
	for( i = 0; i < num_splits; i ++ )
	{
		if( to_free[i] )
			k_t_put_mino( to_free[i] );
	}

	/*dst now points at the target directory*/
	memset( b, 0, BLK_SZ );
	k_t_get_blk( b, pr->m_cwd->m_fd, dst->m_ino.i_block[0] );

	/*show some stuff*/
	printf( "current inode is %i\n", current->m_ino_num );
	printf( "dst inode is %i\n", dst->m_ino_num );

	dp = (ext2_dir*)b;
	cp = b;

	/*walk*/
	while( cp + dp->rec_len < ( b + 1024 ) )
	{
		/*have to do this in case of names that contain substrings*/
		memset( name, 0, MAX_NAME );
		memcpy( name, dp->name, dp->name_len );

		/*found it*/
		if( !strcmp( name, bn_2 ) )
		{
			printf( "file already exists\n" );
			goto fail;
		}

		cp += dp->rec_len;
		dp = (ext2_dir*)cp;
	}

	/*now do work - dp points at the last entry so re-adjust its size*/
	tmp_size = dp->rec_len;
	dp->rec_len = 4 * ( ( 8 + dp->name_len + 3 ) / 4 );
	tmp_size -= dp->rec_len;

	cp += dp->rec_len;
	dp = (ext2_dir*)cp;

	/*zero out the rest of the block*/
	memset( cp, 0, tmp_size );

	/*add an entry - target contains our inode*/
	dp->inode = target_inum;
	memcpy( dp->name, bn_2, strlen( bn_2 ) );
	dp->file_type = FILE_TYPE;
	dp->name_len = strlen( bn_2 );
	
	dp->rec_len = tmp_size;

	/*put the block back*/
	k_t_put_blk( b, pr->m_cwd->m_fd, dst->m_ino.i_block[0] );

	/*get the target inode*/
	if( !k_t_get_mino( current->m_fd, target_inum, &target ) || 
			S_ISDIR( target->m_ino.i_mode ) )
	{
		printf( "error retrieving target inode\n" );
		goto fail;
	}
	
	printf( "got inode %i to link\n", target_inum );

	/*update the link count*/
	target->m_ino.i_links_count++;
	target->m_dirty = true;

	/*put the mino back*/
	k_t_put_mino( target_inum );
	
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





