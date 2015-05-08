/******************************************************************************
file		: mino_t.c
author		: nc
desc		: implementation of the mino_t functions
date		: 03-25-13
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ext2fs/ext2_fs.h>
#include <time.h>

#include "global_defs.h"
#include "mino_t.h"
#include "mnt_t.h"


/******************************************************************************
function	: 
author		: nc
desc		: 
date		: 03-24-13
 ******************************************************************************/
bool mino_t_init( mino_t** ino )
{
	*ino = malloc( sizeof(**ino) );

	if( !*ino )
		goto fail;

	memset( *ino, 0, sizeof(**ino) );

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
void mino_t_destroy( mino_t** ino )
{
	if( *ino )
		free( *ino );

	*ino = NULL;

	return;
}



/******************************************************************************
function	: bool mino_t_create( ~ ) 
author		: nc
desc		: create an mino_t from args
date		: 03-31-13
 ******************************************************************************/
bool mino_t_make( mino_t* dst,
		inode* ino,
		u16 fd,
		u32 num,
		u16 refs,
		bool dirty,
		bool mounted,
		mnt_t* point )
{
	if( !dst || !ino || fd <= 0 || !num || ( mounted && !point ) )
		goto fail;

	memcpy( &dst->m_ino, ino, sizeof(*ino) );
	dst->m_fd = fd;
	dst->m_ino_num = num;
	dst->m_refc = refs;
	dst->m_dirty = dirty;
	dst->m_mounted = mounted;
	dst->m_mountp = point;

	return true;

fail:
	return false;
}


/******************************************************************************
function	: 
author		: nc
desc		: 
date		: 03-31-13
 ******************************************************************************/
void mino_t_show( mino_t* mi )
{
	if( !mi )
		return;


	printf( "mino_t %p:\n", mi );
	mino_t_show_ino( &mi->m_ino, mi->m_ino_num );

	printf( "\n" );
	printf( "\tm_fd:\t\t%i\n", mi->m_fd );
	printf( "\tm_ino_num:\t%i\n", mi->m_ino_num );
	printf( "\tm_refc:\t\t%i\n", mi->m_refc );
	printf( "\tm_dirty:\t%i\n", mi->m_dirty );
	printf( "\tm_mounted:\t%i\n", mi->m_mounted );
	printf( "\tm_mountp:\t%p\n", mi->m_mountp );

	printf( "\n" );

	return;
}


/******************************************************************************
function	: 
author		: nc
desc		: 
date		: 03-12-13
 ******************************************************************************/
void mino_t_show_ino( inode* ino, unsigned loc )
{
	printf( "\tinode %d info:\n", loc );
	printf( "\tuid:\t\t%d\n", ino->i_uid );
	printf( "\tsize:\t\t%d\n", ino->i_size );
	printf( "\tlinks:\t\t%d\n", ino->i_links_count );
	return;
}


/******************************************************************************
function	: 
author		: nc
desc		: 
date		: 03-12-13
 ******************************************************************************/
bool mino_t_set_ino(
		inode* dst,
		u16 uid,
		u16 gid,
		u32 size,
		u16 mode,
		u16 links )
{
	if( !dst || !mode )
		goto fail;

	dst->i_mode			= mode;
	dst->i_uid			= uid;
	dst->i_gid			= gid;
	dst->i_size			= size;
	dst->i_links_count	= links;
	dst->i_atime		= time(0L);
	dst->i_ctime		= time(0L);
	dst->i_mtime		= time(0L);
	dst->i_blocks		= size / IE_SZ;

	return true;

fail:
	printf( "could not create new inode\n" );

	return false;
}
