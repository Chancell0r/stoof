/******************************************************************************
file		: mnt_t.c
author		: nc
desc		: implementation of the mnt_t functions
date		: 03-25-13
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "global_defs.h"
#include "mnt_t.h"
#include "k_t.h"			//get_blk


/******************************************************************************
function	: bool mnt_t_init( mnt_t** mt )
author		: nc
desc		: create a mount point pointer
date		: 03-24-13
 ******************************************************************************/
bool mnt_t_init( mnt_t** mt )
{
	*mt = malloc( sizeof(**mt) );

	if( !*mt )
		goto fail;

	memset( *mt, 0, sizeof(**mt) );

	return true;

fail:
	return false;
}


/******************************************************************************
function	: void mnt_t_destroy( mnt_t** mt )
author		: nc
desc		: destroy a moint point
date		: 03-24-13
 ******************************************************************************/
void mnt_t_destroy( mnt_t** mt )
{
	if( (*mt)->m_fd )
		close( (*mt)->m_fd );	

	if( *mt )
		free( *mt );

	*mt = NULL;

	return;
}


/******************************************************************************
function	: bool mnt_t_create( mnt_t* mt, char* path_to_img, char* folder_name )
author		: nc
desc		: create a new mount point from arguments
date		: 04-07-13
 ******************************************************************************/
bool mnt_t_create( mnt_t* mt, char* path_to_img, char* folder_name, u32 inum )
{
	int fd			= 0;
	char b[BLK_SZ]	= { 0 };

	if( !mt || !path_to_img || !folder_name )
		goto fail;

	if( inum != ROOT_INO_NUM )
		printf( "warning: attempting to mount non-standard root inode\n" );

	/*open the image to mount*/
	fd = open( path_to_img, O_RDWR );

	if( fd <= 0 )
		goto fail;

	/*assign the basic values*/
	mt->m_fd = fd;
	strcpy( mt->m_dev_name, path_to_img );
	strcpy( mt->m_mnt_name, folder_name );

	/*get the superblock and gd0*/
	k_t_get_blk( (char*)&mt->m_sb, fd, SB_BLK );
	k_t_get_blk( b, fd, GD0_BLK );
	memcpy( &mt->m_gd0, b, sizeof(gd) );

	/*check magic num*/
	if( mt->m_sb.s_magic != EXT2_MAGIC )
		goto fail;

	/*get some basic values*/
	mt->m_num_inos = mt->m_sb.s_inodes_count;
	mt->m_num_blocks = mt->m_sb.s_blocks_count;
	mt->m_first_ino = mt->m_sb.s_first_ino;
	mt->m_data_start = mt->m_sb.s_first_data_block;

	mt->m_ino_start = mt->m_gd0.bg_inode_table;

	/*set the mount point name and device name*/
	strcpy( mt->m_dev_name, path_to_img );
	strcpy( mt->m_mnt_name, folder_name );

	/*set the root mount point*/
	if( mt->m_root_ino )
		goto fail; 

	k_t_get_mino( fd, inum, &mt->m_root_ino );
	k_t_set_mino_as_mounted( fd, inum, mt );

	return true;

fail:
	printf( "unable to create mount point %s\n", path_to_img );
	return false;
}



/******************************************************************************
function	: bool mnt_t_add_root_mino( mnt_t target, mino_t* root )
author		: nc
desc		: add the root inode to a mount point
date		: 04-07-13
 ******************************************************************************/
bool mnt_t_add_root_mino( mnt_t* target, mino_t* root )
{
	if( !target || !root || target->m_root_ino )
		goto fail;

	/*add the inode and set the busy status to false*/
	target->m_root_ino = root;
	target->m_busy = false;

	return true;

fail:
	return false;
}



/******************************************************************************
function	: 
author		: nc
desc		: 
date		: 03-24-13
 ******************************************************************************/
void mnt_t_show( mnt_t* mt )
{
	if( !mt )
		return;

	printf( "mount entry information %p:\n", mt );
	printf( "\tm_num_inos:\t%i\n", mt->m_num_inos );
	printf( "\tm_num_blocks:\t%i\n", mt->m_num_blocks );
	printf( "\tm_first_ino:\t%i\n", mt->m_first_ino );
	printf( "\tm_ino_start:\t%i\n", mt->m_ino_start );
	printf( "\tm_data_start:\t%i\n", mt->m_data_start );
	printf( "\tm_fd:\t\t%i\n", mt->m_fd );
	printf( "\tm_busy:\t\t%i\n", mt->m_busy );
	printf( "\tm_root_ino:\t%p\n", mt->m_root_ino );
	printf( "\tm_dev_name:\t%s\n", mt->m_dev_name );
	printf( "\tm_mnt_name:\t%s\n", mt->m_mnt_name );
	printf( "\n" );

	return;
}


/******************************************************************************
function	: 
author		: nc
desc		: 
date		: 03-24-13
 ******************************************************************************/
void mnt_t_lock( mnt_t* args )
{
	if( args )
		args->m_busy = true;
}


/******************************************************************************
function	: 
author		: nc
desc		: 
date		: 03-24-13
 ******************************************************************************/
void mnt_t_unlock( mnt_t* args )
{
	if( args )
		args->m_busy = false;
}
