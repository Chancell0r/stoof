/******************************************************************************
file		: k_t.c
author		: nc
desc		: implementation of the k_t functions
date		: 03-25-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "global_defs.h"
#include "k_t.h"
#include "mnt_t.h"
#include "vsh_t.h"
#include "mino_t.h"
#include "proc_t.h"
#include "string_funcs.h"


extern k_t* kr;

/******************************************************************************
function	: bool k_t_init( k_t** krn )
author		: nc
desc		: create the kernel
date		: 03-24-13
 ******************************************************************************/
bool k_t_init( k_t** krn )
{
	if( kr )
		goto fail;

	*krn = malloc( sizeof(**krn) );

	if( !*krn )
		goto fail;

	memset( *krn, 0, sizeof(**krn) );

	return true;

fail:
	printf( "could not create kernel\n" );
	exit( X_NO_KERNEL );
	return false;
}



/******************************************************************************
function	: void k_t_destroy( k_t** krn )
author		: nc
desc		: destroy the kernel
date		: 03-25-13
 ******************************************************************************/
void k_t_destroy( k_t** krn )
{
	int i = 0;

	k_t_write_fs_to_disk();

	for( i = 0; i < MAX_OFS && (*krn)->m_of_tb[i]; i ++ )
	   of_t_destroy( &(*krn)->m_of_tb[i] );	

	for( i = 0; i < MAX_MNTS && (*krn)->m_mnt_tb[i]; i ++ )
		mnt_t_destroy( &(*krn)->m_mnt_tb[i] );

	for( i = 0; i < MAX_MINOS && (*krn)->m_mino_tb[i]; i ++ )
		mino_t_destroy( &(*krn)->m_mino_tb[i] );

	for( i = 0; i < MAX_PROCS && (*krn)->m_proc_tb[i]; i ++ )
		proc_t_destroy( &(*krn)->m_proc_tb[i] );

	if( *krn )
		free( *krn );

	*krn = NULL;


	return;
}


/******************************************************************************
function	: int k_t_start( void )
author		: nc
desc		: initialize the filesystem
date		: 03-30-13
 ******************************************************************************/
int k_t_start( void )
{
	if( !k_t_mount_root( NULL, &kr->m_mnt_tb[0] ) )
		goto no_root;

	if( !k_t_start_procs() )
		goto no_procs;

	/*add an initial entry to the file table so no one can use it*/
	if( !of_t_init( &kr->m_of_tb[0] ) )
		goto no_ft;

	strcpy( kr->m_of_tb[0]->m_name, "root file descriptor" );

	return X_SUCCESS;
no_ft:
	return X_NO_FTABLE;
no_root:
	return X_NO_ROOT;
no_procs:
	return X_NO_PROCS;

}


/******************************************************************************
function	: int k_t_run( void )
author		: nc
desc		: main loop for the kernel
date		: 03-25-13
 ******************************************************************************/
int k_t_run( void )
{
	vsh_t* sh = NULL;

	vsh_t_init( &sh );

	if( !kr || !sh )
		goto no_krn;

	printf( "type \"help\" for a list of commands\n" );

	/*replace*/
	vsh_t_run( sh, kr->m_proc_tb[1] );

	if( sh )
		vsh_t_destroy( &sh );

	return X_SUCCESS;

no_krn:
	printf( "lost kernel\n" );
	return X_NO_KERNEL;

fail_unk:
	return X_UNKNOWN;

}



/******************************************************************************
function	: bool k_t_get_blk( char* dst, char* src, unsigned num )
author		: nc
desc		: put a block somewhere
date		: 03-30-13
 ******************************************************************************/
bool k_t_get_blk( char* dst, int fd, unsigned num )
{
	if( !dst || !fd )
		goto fail;

	/*read the block in*/
	if( ( lseek( fd, (long)( (BLK_SZ)*num ), 0 ) ) == -1 )
		goto fail;

	read( fd, dst, BLK_SZ );

	return true;

fail:
	printf( "error reading block device: source or destination does not exist\n" );
	return false;
}



/******************************************************************************
function	: bool k_t_put_blk( char* src, in fd, unsigned num )
author		: nc
desc		: put a block back in the fs 
date		: 03-30-13
 ******************************************************************************/
bool k_t_put_blk( char* src, int fd, unsigned num )
{
	if( !src || !fd )
		goto fail;

	/*write the block out*/
	if( ( lseek( fd, (long)( (BLK_SZ)*num ), 0 ) ) == -1 )
		goto fail;

	write( fd, src, BLK_SZ );

	return true;

fail:
	printf( "error writing block device: source or destination does not exist\n" );
	return false;
}



/******************************************************************************
function	: mnt_t* k_t_get_mnt_from_fd( int fd )
author		: nc
desc		: get a mount table entry from its file descriptor
date		: 03-30-13
 ******************************************************************************/
mnt_t* k_t_get_mnt_from_fd( int fd )
{
	int i = 0;
	for( i = 0; i < MAX_MNTS && kr->m_mnt_tb[i]; i ++ )
	{
		if( kr->m_mnt_tb[i]->m_fd == fd )
			return kr->m_mnt_tb[i];
	}

	return NULL;
}



/******************************************************************************
function	: bool k_t_mount_root( char* path )
author		: nc
desc		: mount all the root devices
date		: 03-30-13
 ******************************************************************************/
bool k_t_mount_root( char* path, mnt_t** dst )
{
	char loc[MAX_PATH]	= { '\0' };

	/*use the default path for disk image if one isn't specified*/
	if( !path )
	{
		strcpy( loc, DEF_IMG );
	}
	else
	{
		if( strlen( path ) - 1 > MAX_PATH )
			goto fail;

		strcpy( loc, path );
	}

	/*create the first mount device and the in-memory inode*/
	if( !*dst )
		mnt_t_init( dst );

	if( !*dst )
		goto fail;

	mnt_t_create( *dst, loc, "/", ROOT_INO_NUM );

	printf( "mounted root device successfully\n" );

	return true;

fail:

	if( *dst )
		mnt_t_destroy( dst );

	printf( "failed to mount root device\n" );
	return false;
}


/******************************************************************************
function	: bool k_t_start_procs( void )
author		: nc
desc		: start the initial processes
date		: 04-02-13
 ******************************************************************************/
bool k_t_start_procs( void )
{
	if( !kr )
		goto fail;

	/*make two processes*/
	if( !proc_t_init( &kr->m_proc_tb[0] ) )
		goto fail;

	if( !proc_t_init( &kr->m_proc_tb[1] ) )
		goto fail;

	if( !proc_t_make(
				kr->m_proc_tb[0],
				U_SUPER,
				0,
				0,
				0,
				S_RUNNING,
				NULL,
				kr->m_mino_tb[0] ) )
		goto fail;

	if( !proc_t_make(
				kr->m_proc_tb[1],
				U_REG,
				1,
				0,
				0,
				S_RUNNING,
				kr->m_proc_tb[0],
				kr->m_mino_tb[0] ) )
		goto fail;

	printf( "created root processes sucessfully\n" );
	return true;

fail:
	printf( "unable to start processes\n" );
	proc_t_destroy( &kr->m_proc_tb[0] );
	proc_t_destroy( &kr->m_proc_tb[1] );

	return false;
}



/******************************************************************************
function	: bool k_t_wite_fs_to_disk( void )
author		: nc
desc		: writes everything open in memory out to disk
date		: 04-10-13
 ******************************************************************************/
void k_t_write_fs_to_disk( void )
{
	int i = 0;

	/*write the basic stuff out*/
	for( i = 0; i < MAX_MNTS; i ++ )
	{
		if( kr->m_mnt_tb[i] )
		{
			k_t_flush_superblock( kr->m_mnt_tb[i]->m_fd );
			k_t_flush_gd0( kr->m_mnt_tb[i]->m_fd );
		}
	}

	/*write everything else out*/
	k_t_flush_all_minos();

	return;
}



/******************************************************************************
function	: bool k_t_flush_superblock( int fd )
author		: nc
desc		: write the superblock for a specified mount back to disk
date		: 04-10-13
 ******************************************************************************/
bool k_t_flush_superblock( int fd )
{
	mnt_t* mt		= NULL;
	char sb[BLK_SZ]	= { '\0' };

	if( !fd || sizeof(sb) != BLK_SZ )
		goto fail;

	/*get the mount point*/
	mt = k_t_get_mnt_from_fd( fd );

	if( !mt )
		goto fail;

	/*write it*/
	memcpy( sb, &mt->m_sb, BLK_SZ );
	k_t_put_blk( (char*)&mt->m_sb, fd, SB_BLK ); 

	return true;

fail:
	printf( "uanble to write superblock for fd%i\n", fd );
	return false;
}



/******************************************************************************
function	: bool k_t_flush_gd0( int fd )
author		: nc
desc		: write the group descriptor back to disk
date		: 04-10-13
 ******************************************************************************/
bool k_t_flush_gd0( int fd )
{
	mnt_t* mt			= NULL;
	char gd0[BLK_SZ]	= { '\0' };

	if( !fd )
		goto fail;

	/*get the mount point*/
	mt = k_t_get_mnt_from_fd( fd );

	if( !mt )
		goto fail;

	memcpy( gd0, &mt->m_gd0, sizeof(gd) );

	/*gd0 is a different size so we have to use a different algo here*/	
	if( ( lseek( fd, (long)( (BLK_SZ)*GD0_BLK ), 0 ) ) == -1 )
		goto fail;

	write( fd, gd0, sizeof(gd) );

	return true;

fail:
	printf( "uanble to write gd0 for fd%i\n", fd );
	return false;
}




/******************************************************************************
function	: bool k_t_get_ino( int fd, u32 num, inode* ino )
author		: nc
desc		: read an inode from a file
date		: 03-31-13
 ******************************************************************************/
bool k_t_get_ino( int fd, u32 num, inode* ino )
{
	char b[BLK_SZ]	= { '\0' };
	mnt_t* mnt		= NULL;
	int blk			= 0;
	int off			= 0;

	if( !num || !fd || !ino )
		goto fail;

	/*get a mnt_t* so we can find out where the inode table starts*/
	mnt = k_t_get_mnt_from_fd( fd );

	if( !mnt )
		goto fail;

	/*reset inode*/
	memset( ino, 0, sizeof(*ino) );

	num -= 1;

	/*get the correct offset*/
	blk = ( num / INO_PER_BLK );
	off = ( num % INO_PER_BLK );
	blk += mnt->m_ino_start;

	/*copy the inode info over*/
	k_t_get_blk( b, fd, blk );
	memcpy( ino, b+( off*INO_SIZE ), INO_SIZE );

	return true;

fail:
	printf( "unable to find inode %i\n", num );
	return false;
}




/******************************************************************************
function	: bool k_t_get_mino( int fd, u32 num, mino_t** dst )
author		: nc
desc		: get an in-memory inode
date		: 03-31-13
 ******************************************************************************/
bool k_t_get_mino( int fd, u32 num, mino_t** dst )
{
	int i				= 0;
	inode ino			= { 0 };
	mino_t* new_mino	= NULL;

	if( !fd )
		goto fail;

	/*see if the mino is already open*/
	for( i = 0; i < MAX_MINOS && kr->m_mino_tb[i]; i ++ )
	{
		if( kr->m_mino_tb[i]->m_ino_num == num )
		{
			*dst = kr->m_mino_tb[i];
			kr->m_mino_tb[i]->m_refc++;
			goto success;
		}
	}

	if( i >= MAX_MINOS )
		goto fail;

	/*if not, we need to open it*/
	if( !k_t_get_ino( fd, num, &ino ) )
		goto fail;

	/*now add it to the mino table*/
	mino_t_init( &new_mino );

	/*make it*/
	if( !mino_t_make( new_mino,
				&ino,
				fd,
				num,
				1,
				false,
				false,
				NULL ) )
		goto fail;

	kr->m_mino_tb[i] = new_mino;

	/*point the destination to it*/
	*dst = new_mino;

success:
	return true;

fail:
	mino_t_destroy( &new_mino );
	printf( "unable to get in-memory inode %i\n", num );
	return false;
}



/******************************************************************************
function	: bool k_t_put_ino( int fd, u32 num, inode* ino )
author		: nc
desc		: write a memory to the file
date		: 03-31-13
 ******************************************************************************/
bool k_t_put_ino( int fd, u32 num, inode* ino )
{
	char b[BLK_SZ]	= { '\0' };
	mnt_t* mnt		= NULL;
	int blk			= 0;
	int off			= 0;

	if( !num || !fd || !ino )
		goto fail;

	/*get a mnt_t* so we can find out where the inode table starts*/
	mnt = k_t_get_mnt_from_fd( fd );

	if( !mnt )
		goto fail;

	num -= 1;

	/*get the correct offset*/
	blk = ( num / INO_PER_BLK );
	off = ( num % INO_PER_BLK );
	blk += mnt->m_ino_start;

	/*copy the inode info over*/
	k_t_get_blk( b, fd, blk );
	memcpy( b+( off*INO_SIZE ), ino, INO_SIZE );
	k_t_put_blk( b, fd, blk );

	return true;

fail:
	printf( "unable to find inode %i\n", num );
	return false;
}



/******************************************************************************
function	: bool k_t_put_mino( mino_t** src )
author		: nc
desc		: write out an in-memory inode
date		: 03-31-13
 ******************************************************************************/
bool k_t_put_mino( u32 num )
{
	int i = 0;

	if( !kr )
		goto fail;

	/*see if the mino is already open*/
	for( i = 0; i < MAX_MINOS; i ++ )
	{
		if( kr->m_mino_tb[i] && kr->m_mino_tb[i]->m_ino_num == num )
		{
			kr->m_mino_tb[i]->m_refc--;
			break;
		}
	}

	if( i >= MAX_MINOS )
		goto fail;

	/*check for zero ref count*/
	if( kr->m_mino_tb[i] && kr->m_mino_tb[i]->m_refc <= 0 )
	{
		if( !k_t_put_ino( kr->m_mino_tb[i]->m_fd, num, &kr->m_mino_tb[i]->m_ino ) )
			goto fail;

		mino_t_destroy( &kr->m_mino_tb[i] );
	}

success:
	return true;

fail:
	printf( "unable to put in-memory inode %i\n", num );
	return false;
}



/******************************************************************************
function	: bool k_t_set_mino_as_mounted( int fd, u32 inum, mnt_t* mountpoint )
author		: nc
desc		: flag a mino as a mount point
date		: 04-07-13
 ******************************************************************************/
bool k_t_set_mino_as_mounted( int fd, u32 inum, mnt_t* mountpoint )
{
	mino_t* tmp = NULL;

	if( !fd || !inum || !mountpoint )
		goto fail;

	k_t_get_mino( fd, inum, &tmp );

	if( !tmp )
		goto fail;

	tmp->m_mounted = true;
	tmp->m_mountp = mountpoint;

	k_t_put_mino( inum );

	return true;

fail:
	printf( "unable to mount inode %i\n", inum );
	return false;
}



/******************************************************************************
function	: bool k_t_set_mino_as_unmounted( int fd, u32 inum, mnt_t* mountpoint )
author		: nc
desc		: flag a mino as a mount point
date		: 04-07-13
 ******************************************************************************/
bool k_t_set_mino_as_unmounted( int fd, u32 inum, mnt_t* mountpoint )
{ 
	mino_t* tmp = NULL;

	if( !fd || !inum || !mountpoint )
		goto fail;

	k_t_get_mino( fd, inum, &tmp );

	if( !tmp )
		goto fail;

	tmp->m_mounted = false;
	tmp->m_mountp = NULL;

	k_t_put_mino( inum );

	return true;

fail:
	printf( "unable to unmount inode %i\n", inum );
	return false;
}



/******************************************************************************
function	: bool k_t_flush_clean_minos( void )
author		: nc
desc		: 04-10-13
date		: write all clean minos to disk
 ******************************************************************************/
bool k_t_flush_clean_minos( void )
{
	int i = 0;

	if( !kr || !kr->m_mino_tb )
		goto fail;

	for( i = 0; i < MAX_MINOS; i++ )
	{
		if( kr->m_mino_tb[i] &&
				kr->m_mino_tb[i]->m_dirty == false &&
				kr->m_mino_tb[i]->m_mounted == false )
		{
			//printf( "found mino at location %i\n", i );
			kr->m_mino_tb[i]->m_refc = 1;
			k_t_put_mino( kr->m_mino_tb[i]->m_ino_num );
		}
	}

	return true;

fail:
	printf( "unable to flush clean in-memory inodes\n" );
	return false;
}


/******************************************************************************
function	: bool k_t_flush_dirty_minos( void )
author		: nc
desc		: 04-10-13
date		: write all dirty minos to disk
 ******************************************************************************/
bool k_t_flush_dirty_minos( void )
{
	int i = 0;

	if( !kr || !kr->m_mino_tb )
		goto fail;

	for( i = 0; i < MAX_MINOS; i++ )
	{
		if( kr->m_mino_tb[i] &&
				kr->m_mino_tb[i]->m_dirty == true &&
				kr->m_mino_tb[i]->m_mounted == false )
		{
			//printf( "found mino at location %i\n", i );
			kr->m_mino_tb[i]->m_refc = 1;
			k_t_put_mino( kr->m_mino_tb[i]->m_ino_num );
		}
	}

	return true;

fail:
	printf( "unable to flush dirty in-memory inodes\n" );
	return false;
}



/******************************************************************************
function	: bool k_t_flush_all_not_mounted_minos( void )
author		: nc
desc		: 04-10-13
date		: write all non-mounted minos
 ******************************************************************************/
bool k_t_flush_all_not_mounted_minos( void )
{
	int i = 0;

	if( !kr || !kr->m_mino_tb )
		goto fail;

	for( i = 0; i < MAX_MINOS; i++ )
	{
		if( kr->m_mino_tb[i] &&	kr->m_mino_tb[i]->m_mounted == false )
		{
			//printf( "found mino at location %i\n", i );
			kr->m_mino_tb[i]->m_refc = 1;
			k_t_put_mino( kr->m_mino_tb[i]->m_ino_num );
		}
	}

	return true;

fail:
	printf( "unable to flush all in-memory inodes\n" );
	return false;
}



/******************************************************************************
function	: bool k_t_flush_all_minos( void )
author		: nc
desc		: 04-10-13
date		: write all minos
 ******************************************************************************/
bool k_t_flush_all_minos( void )
{
	int i = 0;

	if( !kr || !kr->m_mino_tb )
		goto fail;

	for( i = 0; i < MAX_MINOS; i++ )
	{
		if( kr->m_mino_tb[i] )
		{
			//printf( "found mino at location %i\n", i );
			kr->m_mino_tb[i]->m_refc = 1;
			k_t_put_mino( kr->m_mino_tb[i]->m_ino_num );
		}
	}

	return true;

fail:
	printf( "unable to flush all in-memory inodes\n" );
	return false;
}



/******************************************************************************
function	: u32 k_t_add_ino_to_fs( inode* new_ino )
author		: nc
desc		: 04-11-13
date		: add an inode to the filesystem. does not add to mino table
 ******************************************************************************/
u32 k_t_add_ino_to_fs( int fd, inode* new_ino )
{
	u32 loc		= 0;
	mnt_t* mt	= NULL;

	if( !fd || !new_ino )
		goto fail;

	/*get next avaliable location*/
	mt = k_t_get_mnt_from_fd( fd );
	loc =  k_t_get_next_imap_loc( fd );

	if( !loc || !mt )
		goto fail;

	if( mt->m_sb.s_free_inodes_count == 0 )
		goto fail;

	/*lock the fs*/
	mnt_t_lock( mt );	

	/*add it to the map*/
	if( !k_t_put_ino( fd, loc, new_ino ) )
		goto fail;

	k_t_set_imap_bit( fd, loc, 1 );

	mt->m_sb.s_free_inodes_count--;

	mnt_t_unlock( mt );	
	return loc;

fail:
	mnt_t_unlock( mt );
	printf( "unable to add inode to filesystem\n" );
	return 0;
}



/******************************************************************************
function	: bool k_t_del_ino_from_fs( int fd, u32 inum )
author		: nc
desc		: delete an inode from the fs
date		: 04-11-13
 ******************************************************************************/
bool k_t_del_ino_from_fs( int fd, u32 inum )
{
	char in[16]	= { 0 };
	mnt_t* mt	= NULL;
	int i		= 0;

	if( !fd || !inum )
		goto fail;

	/*check and see if they are trying to delete the root inode*/
	if( inum == ROOT_INO_NUM )
	{
		printf( "warning: attempting to remove root inode.  continue (y/n)?\n" );
		scanf( "%s", in );

		if( in[0] == 'n' )
			goto fail;
	}

	mt = k_t_get_mnt_from_fd( fd );

	if( !mt )
		goto fail;

	/*check the mino table and remove it if it is open*/
	for( i = 0; i < MAX_MINOS; i++ )
	{
		if( kr->m_mino_tb[i] && kr->m_mino_tb[i]->m_ino_num == inum )
		{
			kr->m_mino_tb[i]->m_refc = 1;
			k_t_put_mino( kr->m_mino_tb[i]->m_ino_num );
		}
	}

	/*lock the fs*/
	mnt_t_lock( mt );	

	/*delete it*/
	k_t_set_imap_bit( fd, inum, 0 );

	mt->m_sb.s_free_inodes_count++;

	/*unlock*/
	mnt_t_unlock( mt );	

	return true;

fail:
	printf( "unable to remove inode %i from fd%i\n", inum, fd );
	mnt_t_unlock( mt );	
	return false;
}


/******************************************************************************
function	: bool k_t_add_dblocks_to_mino( int fd, u32 mino_num, u16 num )
author		: nc
desc		: 
date		: 
 ******************************************************************************/
bool k_t_add_dblocks_to_mino( int fd, u32 mino_num, u32 num )
{
	int i			= 0;
	int j			= 0;
	int loc			= 0;
	unsigned* off	= NULL;
	unsigned* off2	= NULL;
	int blocks_used	= 0;
	mino_t* local	= NULL;
	char b[BLK_SZ]	= { '\0' };
	char b2[BLK_SZ]	= { '\0' };
	mnt_t* mt		= NULL;

	if( !fd || !mino_num || !num )
		goto fail;

	k_t_get_mino( fd, mino_num, &local );
	mt = k_t_get_mnt_from_fd( fd );
	j = num;
	if( !local || !mt )
		goto fail;

	mnt_t_lock( mt );

	/*count the number of data blocks already in use*/
	for( i = 0; i < THREE_INDIR; i++ )
	{
		memset(b, 0, BLK_SZ);
		off = (unsigned*)b;
		/*use <= because we need to count the indirect block as ours*/
		if( local->m_ino.i_block[i] && i < ONE_INDIR - 1 )
		{
			blocks_used++;
			printf( "%i ", local->m_ino.i_block[i] );
		}

		/* first set of indirect blocks*/
		if( i == ONE_INDIR - 1 && local->m_ino.i_block[i] )
		{
			k_t_get_blk( b, fd, local->m_ino.i_block[i] );
			off = (unsigned*)b;

			if( !off )
				goto fail;

			while(*off < DBLOCK && *off > 0 )
			{
				//printf( "%i ", *off );
				off++;
				blocks_used++;
			}
		}

		/*double indirect*/
		if( i == TWO_INDIR - 1 && local->m_ino.i_block[i] )
		{
			/*get the block where the indirect block nums are stored*/
			k_t_get_blk( b, fd, local->m_ino.i_block[i] );
			off = (unsigned*)b;

			/*pointer gymnastics*/
			while( *off < DBLOCK && *off > 0 )
			{
				/*read all the single indirect blocks in the double-indirect loc*/
				k_t_get_blk( b2, fd, *off );
				off2 = (unsigned*)b2;

				/*print what they point at*/
				while( *off2 < DBLOCK && *off2 > 0 )
				{
					printf( "%i ", *off2 );
					off2++;
					blocks_used++;
				}

				printf( "%i ", *off );
				off++;
				blocks_used++;
			}
		}
	}

	/*make sure there is room*/
	if( blocks_used + num > MAX_DBLOCKS )
		goto fail;

	printf( "\nadding direct blocks\n" );

	/*start walking*/
	for( i = 0 ; i < THREE_INDIR && num != 0; i++ )
	{
		if( !local->m_ino.i_block[i] && i < ONE_INDIR - 1 )
		{
			/*get the next open data block*/
			loc = k_t_get_next_bmap_loc( fd );

			/*tell the bmap it is ours*/
			if( !k_t_set_bmap_bit( fd, loc, 1 ) )
				goto fail;

			/*add the new location to the inode*/
			local->m_ino.i_block[i] = loc;

			printf( "added dblock at position %i to ino %i\n", loc, local->m_ino_num );
			num--;
		}

		/* first set of indirect blocks*/
		if( i == ONE_INDIR - 1 )
		{
			memset( b, 0, BLK_SZ );
			off = (unsigned*)b;

			if( !off )
				goto fail;

			/*make an indirect data block*/
			if(local->m_ino.i_block[ONE_INDIR -1] == 0)
			{
				/*get the next open data block - still comes from the bitmap*/
				loc = k_t_get_next_bmap_loc( fd );

				/*tell the bmap it is ours*/
				if( !k_t_set_bmap_bit( fd, loc, 1 ) )
					goto fail;

				local->m_ino.i_block[i] = loc;
							
			}
			k_t_get_blk(b, fd, local->m_ino.i_block[ONE_INDIR-1]);

			off = (unsigned*)b;

			/*traverse used indirect*/	
			while(*off < DBLOCK && *off > 0)
				off++;

			while( num != 0 && ( off - (unsigned*)b ) < BLK_SZ/sizeof(unsigned) )
			{

				/*get the next open data block - still comes from the bitmap*/
				loc = k_t_get_next_bmap_loc( fd );

				/*tell the bmap it is ours*/
				if( !k_t_set_bmap_bit( fd, loc, 1 ) )
					goto fail;

				/*add the new location to the inode*/
				*off = loc;
				off++;

				printf( "added id dblock at position %i to ino %i\n", loc, local->m_ino_num );

				/*we are now adding inode positions to the data block instead of the ino*/
				num--;
			}

			/*write it out*/
			if( !k_t_put_blk( b, fd, local->m_ino.i_block[ONE_INDIR - 1] ) )
				goto fail;
		}

		/*double indirect*/
		if( i == TWO_INDIR - 1 )
		{
			memset( b, 0, BLK_SZ );
			off = (unsigned*)b;

			if(local->m_ino.i_block[TWO_INDIR - 1] == 0)
			{
				/*get the next open data block*/
				loc = k_t_get_next_bmap_loc( fd );

				/*tell the bmap it is ours*/
				if( !k_t_set_bmap_bit( fd, loc, 1 ) )
					goto fail;

				/*add the new location to the inode*/
				local->m_ino.i_block[TWO_INDIR - 1] = loc;
			}

			if( !off )
				goto fail;

			/*get double indirect block*/
			k_t_get_blk(b, fd, local->m_ino.i_block[TWO_INDIR - 1]);

			off = (unsigned*)b;

			while( num != 0 && ( off - (unsigned*)b ) < BLK_SZ/sizeof(unsigned) )
			{
				if(*off > DBLOCK && *off < 0)
				{
					/*get the next open data block - still comes from the bitmap*/
					loc = k_t_get_next_bmap_loc( fd );

					/*tell the bmap it is ours*/
					if( !k_t_set_bmap_bit( fd, loc, 1 ) )
						goto fail;
					
					/*add indirect position to data block*/
					*off = loc;
					
					printf( "added 2id, id dblock at position %i to ino %i\n", loc, local->m_ino_num );
				}

				/*read all the single indirect blocks in the double-indirect loc*/
				memset( b2, 0, BLK_SZ );
				off2 = (unsigned*)b2;

				k_t_get_blk(b2, fd, *off);

				/*traverse used double indirect*/
				off2 = (unsigned*)b2;
				
				/*find open indirect data block*/
				while(*off2 < DBLOCK && *off2 > 0)
					off2++;

				while( num != 0 && ( off2 - (unsigned*)b2 ) < BLK_SZ/sizeof(unsigned) )
				{
					/*get the next open data block - still comes from the bitmap*/
					loc = k_t_get_next_bmap_loc( fd );

					/*tell the bmap it is ours*/
					if( !k_t_set_bmap_bit( fd, loc, 1 ) )
						goto fail;

					/*add the new location to the inode*/
					*off2 = loc;

					printf( "added 2id dblock at position %i to ino %i\n", loc, local->m_ino_num );

					off2++;
					num--;
				}
				k_t_put_blk( b2, fd, *off );
				off++;
			}
			/*write it out*/
			if( !k_t_put_blk( b, fd, local->m_ino.i_block[TWO_INDIR - 1] ) )
				goto fail;
		}
	}

	/*update i_block (indexs 512 bytes so * 2 )*/
	local->m_ino.i_blocks = ( blocks_used + j ) * 2;


	mnt_t_unlock( mt );
	return true;
fail:
	if( mt )
		mnt_t_unlock( mt );
	printf( "unable to add data blocks to mino %i\n", num );
	return false;
}



/******************************************************************************
function	: bool k_t_del_dblocks_from_mino( int fd, u32 mino_num, u32 num )
author		: nc
desc		: delete data blocks from a mino
date		: 
 ******************************************************************************/
bool k_t_del_dblocks_from_mino( int fd, u32 mino_num, u32 num )
{
	int i				= 0;
	int start			= 0;
	int cblock			= 0;
	unsigned* off		= NULL;
	unsigned* off2		= NULL;
	int blocks_used		= 0;
	mino_t* local		= NULL;
	char b[BLK_SZ]		= { '\0' };
	char b2[BLK_SZ]		= { '\0' };
	mnt_t* mt			= NULL;

	if( !fd || !mino_num || !num )
		goto fail;

	k_t_get_mino( fd, mino_num, &local );
	mt = k_t_get_mnt_from_fd( fd );

	if( !local || !mt )
		goto fail;

	/*lock the fs while we are writing to it*/
	mnt_t_lock( mt );

	/*count the number of data blocks in use*/
	for( i = 0; i < THREE_INDIR; i++ )
	{
		memset( b, 0, BLK_SZ );
		off = (unsigned*)b;
		/*use <= because we need to count the indirect block as ours*/
		if( local->m_ino.i_block[i] && i < ONE_INDIR - 1 )
		{
			blocks_used++;
			//printf( "%i ", local->m_ino.i_block[i] );
		}

		/* first set of indirect blocks*/
		if( i == ONE_INDIR - 1 && local->m_ino.i_block[i] != 0 )
		{
			k_t_get_blk( b, fd, local->m_ino.i_block[i] );
			off = (unsigned*)b;

			if( !off )
				goto fail;

			while(*off < DBLOCK && *off > 0 )
			{
				//printf( "%i ", *off );
				off++;
				blocks_used++;
			}
		}

		/*double indirect*/
		if( i == TWO_INDIR - 1 && local->m_ino.i_block[i] )
		{
			/*get the block where the indirect block nums are stored*/
			k_t_get_blk( b, fd, local->m_ino.i_block[i] );
			off = (unsigned*)b;

			/*pointer gymnastics*/
			while( *off < DBLOCK && *off > 0 )
			{
				/*read all the single indirect blocks in the double-indirect loc*/
				k_t_get_blk( b2, fd, *off );
				off2 = (unsigned*)b2;

				/*print what they point at*/
				while( *off2 < DBLOCK && *off2 > 0 )
				{
					printf( "%i ", *off2 );
					off2++;
					blocks_used++;
				}

				printf( "%i ", *off );
				off++;
				blocks_used++;
			}
		}
	}

	/*make sure it is a valid request*/
	if( blocks_used - num < 0 )
		goto fail;

	/*we have to do this one different*/
	start = blocks_used - num;


	printf( "\n" );

	/*sigh*/
	for( i = 0 ; i < THREE_INDIR && num != 0; i++ )
	{
		if( local->m_ino.i_block[i] && i < ONE_INDIR - 1 )
		{
			/*see if we are at the starting position*/
			if( cblock >= start )
			{
				printf( "removing data block %i from inode\n", local->m_ino.i_block[i] );

				/*update the bitmap*/
				if( !local->m_ino.i_block[i] || !k_t_set_bmap_bit( fd, local->m_ino.i_block[i], 0 ) )
					goto fail;

				/*zero the block*/
				local->m_ino.i_block[i] = 0;
				num--;
			}

			/*update block count*/
			cblock++;
		}

		/* first set of indirect blocks*/
		if( i == ONE_INDIR - 1 && local->m_ino.i_block[i] && num != 0 )
		{
			k_t_get_blk( b, fd, local->m_ino.i_block[i] );
			off = (unsigned*)b;
			cblock++;

			if( !off )
				goto fail;

			if( cblock >= start )
			{
				/*free the parent*/
				printf( "removing parent dblock %i\n", local->m_ino.i_block[ONE_INDIR - 1] );
				k_t_set_bmap_bit( fd, local->m_ino.i_block[ONE_INDIR - 1], 0 );
				local->m_ino.i_block[ONE_INDIR - 1] = 0;
				num--;
			}

			while( *off < DBLOCK && *off > 0)
			{
				/*see if we are at the starting position*/
				if( cblock >= start )
				{
					printf( "removing id data block %i from inode\n", *off );

					/*update the bitmap*/
					if( !k_t_set_bmap_bit( fd, *off, 0 ) )
						goto fail;

					/*zero the block*/
					*off = 0;
					num--;
				}

				/*write it out*/
				if( !k_t_put_blk( b, fd, local->m_ino.i_block[ONE_INDIR - 1] ) )
					goto fail;

				off++;
				cblock++;
			}
		}

		/*double indirect*/
		if( i == TWO_INDIR - 1 && local->m_ino.i_block[i] && num != 0 )
		{
			k_t_get_blk( b, fd, local->m_ino.i_block[i] );
			off = (unsigned*)b;
			cblock++;

			if( !off )
				goto fail;

			if( cblock >= start )
			{
				/*free the parent*/
				printf( "removing parent dblock %i\n", local->m_ino.i_block[TWO_INDIR - 1] );
				k_t_set_bmap_bit( fd, local->m_ino.i_block[TWO_INDIR - 1], 0 );
				local->m_ino.i_block[TWO_INDIR - 1] = 0;
				num--;
			}

			while( *off < DBLOCK && *off > 0 )
			{
				k_t_get_blk( b2, fd, *off );
				off2 = (unsigned*)b2;
				cblock++;

				if( !off2 )
					goto fail;

				if( cblock >= start )
				{
					/*free the parent*/
					printf( "removing parent id dblock %i\n", *off );
					k_t_set_bmap_bit( fd, *off, 0 );
					*off = 0;
					num--;
				}

				while( *off2 < DBLOCK && *off2 > 0)
				{
					/*see if we are at the starting position*/
					if( cblock >= start )
					{
						printf( "removing 2id data block %i from inode\n", *off2 );

						/*update the bitmap*/
						if( !k_t_set_bmap_bit( fd, *off2, 0 ) )
							goto fail;

						/*zero the block*/
						*off2 = 0;
						num--;
					}

					/*write it out*/
					if( !k_t_put_blk( b, fd, *off ) )
						goto fail;

					off2++;
					cblock++;
				}

				/*write it out*/
				if( !k_t_put_blk( b, fd, local->m_ino.i_block[TWO_INDIR - 1] ) )
					goto fail;

				off++;
				cblock++;
			}
		}
	}

	/*update i_block (indexs 512 bytes so * 2 )*/
	local->m_ino.i_blocks = ( start ) * 2;

	mnt_t_unlock( mt );
	return true;
fail:
	if( mt )
		mnt_t_unlock( mt );
	printf( "unable to delete blocks from mino %i\n", mino_num );
	return false;
}


/******************************************************************************
function	: bool k_t_truncate_mino( mino_t* mino ) 
author		: nc
desc		: remove all data blocks from a mino and set size to zero
date		: 
 ******************************************************************************/
bool k_t_truncate_mino( mino_t* mino )
{
	int i				= 0;
	int blocks_used		= 0;
	char b[BLK_SZ]		= { '\0' };
	char b2[BLK_SZ]		= { '\0' };
	unsigned* off		= NULL;
	unsigned* off2		= NULL;
	mino_t* local		= NULL;

	if( !mino )
	{
		printf( "error: no m_ino*\n" );
		goto fail;
	}

	/*hax*/
	local = mino;

	if( !local )
	{
		printf( "error: no mount point\n" );
		goto fail;
	}

	/*find out the max block file location*/
	for( i = 0; i < THREE_INDIR; i++ )
	{
		memset( b, 0, BLK_SZ );
		off = (unsigned*)b;
		if( local->m_ino.i_block[i] && i < ONE_INDIR - 1 )
		{
			blocks_used++;
			printf( "%i ", local->m_ino.i_block[i] );
		}


		/* first set of indirect blocks*/
		if( i == ONE_INDIR - 1 && local->m_ino.i_block[i] )
		{
			k_t_get_blk( b, local->m_fd, local->m_ino.i_block[i] );
			off = (unsigned*)b;

			if( !off )
				goto fail;

			while( *off < DBLOCK && *off > 0 )
			{
				printf( "%i ", *off );
				off++;
				blocks_used++;
			}
		}

		/*double indirect*/
		if( i == TWO_INDIR - 1 && local->m_ino.i_block[i] )
		{
			/*get the block where the indirect block nums are stored*/
			k_t_get_blk( b, local->m_fd, local->m_ino.i_block[i] );
			off = (unsigned*)b;
			blocks_used ++;

			/*pointer gymnastics*/
			while( *off < DBLOCK && *off > 0 )
			{
				/*read all the single indirect blocks in the double-indirect loc*/
				k_t_get_blk( b2, local->m_fd, *off );
				off2 = (unsigned*)b2;

				/*print what they point at*/
				while( *off2 < DBLOCK && *off2 > 0)
				{
					printf( "%i ", *off2 );
					off2++;
					blocks_used++;
				}

				printf( "%i ", *off );
				off++;
				blocks_used++;
			}
		}
	}

	/*make sure it is a valid request*/
	if( blocks_used <= 0 )
		goto fail;	

	/*now delete them*/
	if( !k_t_del_dblocks_from_mino( local->m_fd, local->m_ino_num, blocks_used - 1 ) )
	{
		printf( "error removing blocks from inode\n" );
		goto fail;
	}

	/*update the file size*/
	local->m_ino.i_size = 0;

	return true;

fail:
	return false;
}


/******************************************************************************
function	: bool k_t_set_imap_bit( int fd, u32 bnum, bool bval ) 
author		: nc
desc		: set a bit in the bitmap
date		: 
 ******************************************************************************/
bool k_t_set_imap_bit( int fd, u32 bnum, bool bval )
{
	mnt_t* mt				= NULL;
	int bit					= 0;
	int byte				= 0;
	char inode_bmap[BLK_SZ]	= { 0 };

	if( !fd )
		goto fail;

	mt = k_t_get_mnt_from_fd( fd );

	/*cant operate on unlocked fs*/
	if( !mt || mt->m_busy == false )
		goto fail;

	/*get the the inode block*/
	if( !k_t_get_blk( inode_bmap, fd, mt->m_gd0.bg_inode_bitmap ) )
		goto fail;

	bnum -= 1;

	/*get bit/byte loc*/
	byte = bnum / 8;
	bit = bnum % 8;

	if( byte >= BLK_SZ )
		goto fail;

	/*set it*/
	if( bval == 1 )
	{
		inode_bmap[byte] |= ( 1 << bit );
		mt->m_sb.s_free_inodes_count--;
	}
	else
	{
		inode_bmap[byte] &= ~( 1 << bit );
		mt->m_sb.s_free_inodes_count++;
	}

	if( !k_t_put_blk( inode_bmap, fd, mt->m_gd0.bg_inode_bitmap ) )
		goto fail;

	return true;

fail:
	printf( "unable to set bit %i in inode map\n", bnum );
	return false;
}


/******************************************************************************
function	: bool k_t_get_imap_bit( int fd, u32 bnum, bool* retv ) 
author		: nc
desc		: get a bit from the imap
date		: 
 ******************************************************************************/
bool k_t_get_imap_bit( int fd, u32 bnum, bool* retv )
{
	mnt_t* mt				= NULL;
	int bit					= 0;
	int byte				= 0;
	int tmp					= 0;
	char inode_bmap[BLK_SZ]	= { 0 };

	if( !fd || !retv )
		goto fail;

	mt = k_t_get_mnt_from_fd( fd );

	/*cant operate on unlocked fs*/
	if( !mt || mt->m_busy == false )
		goto fail;

	/*get the the inode block*/
	if( !k_t_get_blk( inode_bmap, fd, mt->m_gd0.bg_inode_bitmap ) )
		goto fail;

	/*get bit/byte loc*/
	byte = bnum / 8;
	bit = bnum % 8;

	if( byte >= BLK_SZ )
		goto fail;

	/*get it*/
	tmp = inode_bmap[byte] & ( 1 << bit );

	if( tmp == 0 )
		*retv = false;
	else
		*retv = true;

	return true;

fail:
	printf( "unable to get bit %i in inode map\n", bnum );
	return false;
}



/******************************************************************************
function	: bool k_t_set_bmap_bit( int fd, u32 bnum, bool bval )
author		: nc
desc		: set a bit in the data block bitmap
date		: 
 ******************************************************************************/
bool k_t_set_bmap_bit( int fd, u32 bnum, bool bval )
{
	mnt_t* mt			= NULL;
	int bit				= 0;
	int byte			= 0;
	char bmap[BLK_SZ]	= { 0 };

	if( !fd )
		goto fail;

	mt = k_t_get_mnt_from_fd( fd );

	/*cant operate on unlocked fs*/
	if( !mt || mt->m_busy == false )
		goto fail;

	/*get the the inode block*/
	if( !k_t_get_blk( bmap, fd, mt->m_gd0.bg_block_bitmap ) )
		goto fail;
	
	bnum--;
	
	/*get bit/byte loc*/
	byte = bnum / 8;
	bit = bnum % 8;

	if( byte >= BLK_SZ )
		goto fail;

	/*set it*/
	if( bval == 1 )
	{
		bmap[byte] |= ( 1 << bit );
		mt->m_sb.s_free_blocks_count --;
	}
	else
	{
		bmap[byte] &= ~( 1 << bit );
		mt->m_sb.s_free_blocks_count ++;
	}


	/*hax*/
	if( !k_t_put_blk( bmap, fd, mt->m_gd0.bg_block_bitmap ) )
		goto fail;

	return true;

fail:
	printf( "unable to set bit %i in block bitmap\n", bnum );
	return false;
}


/******************************************************************************
function	: bool k_t_get_bmap_bit( int fd, u32 bnum, bool* retv ) 
author		: nc
desc		: get a bit from the data block bitmap
date		: 
 ******************************************************************************/
bool k_t_get_bmap_bit( int fd, u32 bnum, bool* retv )
{
	mnt_t* mt				= NULL;
	int bit					= 0;
	int byte				= 0;
	int tmp					= 0;
	char inode_bmap[BLK_SZ]	= { 0 };

	if( !fd || !retv )
		goto fail;

	mt = k_t_get_mnt_from_fd( fd );

	/*cant operate on unlocked fs*/
	if( !mt || mt->m_busy == false )
		goto fail;

	/*get the the inode block*/
	if( !k_t_get_blk( inode_bmap, fd, mt->m_gd0.bg_block_bitmap ) )
		goto fail;

	/*get bit/byte loc*/
	byte = bnum / 8;
	bit = bnum % 8;

	if( byte >= BLK_SZ )
		goto fail;

	/*get it*/
	tmp = inode_bmap[byte] & ( 1 << bit );

	if( tmp == 0 )
		*retv = false;
	else
		*retv = true;

	return true;

fail:
	printf( "unable to get bit %i in block bitmap\n", bnum );
	return false;
}


/******************************************************************************
function	: u32 k_t_get_next_imap_loc( int fd )
author		: nc
desc		: get the next open position in the imap
date		: 04-10-13
 ******************************************************************************/
u32 k_t_get_next_imap_loc( int fd )
{
	mnt_t* mt				= NULL;
	int bit					= 0;
	int byte				= 0;
	char inode_bmap[BLK_SZ]	= { 0 };

	if( !fd )
		goto fail;

	mt = k_t_get_mnt_from_fd( fd );

	/*get the the inode block*/
	if( !k_t_get_blk( inode_bmap, fd, mt->m_gd0.bg_inode_bitmap ) )
		goto fail;

	/*start walking*/
	for( byte = 0; byte < BLK_SZ; byte++ )
	{
		for( bit = 0; bit < 8; bit++ )
		{
			/*check for end of imap*/
			if( ( byte * 8 ) + bit > mt->m_sb.s_inodes_count )
			{
				printf( "exceeded max_inodes\n" );
				goto fail;
			}

			if( ( inode_bmap[byte] & ( 1 << bit ) ) == 0 )
			{
				goto ok;
			}
		}
	}

	/*shouldn't get here*/
	goto fail;

ok:
	/*have to add one*/
	return( ( byte * 8 ) + bit  + 1 );

fail:
	printf( "all inode locations are reported as full\n" );
	return 0;
}


/******************************************************************************
function	: u32 k_t_get_next_bmap_loc( int fd )
author		: nc
desc		: get the next open position in the bmap
date		: 04-10-13
 ******************************************************************************/
u32 k_t_get_next_bmap_loc( int fd )
{
	mnt_t* mt				= NULL;
	int bit					= 0;
	int byte				= 0;
	char dbmap[BLK_SZ]	= { 0 };

	if( !fd )
		goto fail;

	mt = k_t_get_mnt_from_fd( fd );

	/*get the the inode block*/
	if( !k_t_get_blk( dbmap, fd, mt->m_gd0.bg_block_bitmap ) )
		goto fail;

	/*start walking*/
	for( byte = 0; byte < BLK_SZ; byte++ )
	{
		for( bit = 0; bit < 8; bit++ )
		{
			/*check for end of bmap*/
			if( ( byte * 8 ) + bit > mt->m_sb.s_blocks_count )
			{
				printf( "exceeded max_dblocks\n" );
				goto fail;
			}

			if( ( dbmap[byte] & ( 1 << bit ) ) == 0 )
			{
				goto ok;
			}
		}
	}

	/*shouldn't get here*/
	goto fail;

ok:
	return( ( byte * 8 ) + bit + 1);

fail:
	printf( "all bmap locations are reported as full\n" );
	return 0;
}



/******************************************************************************
function	: void k_t_p_dir_items( inode* ino, int fd, unsigned num ) 
author		: nc
desc		: forces a listing of all dir items (old)
date		: 
 ******************************************************************************/
void k_t_p_dir_items( inode* ino, int fd, unsigned num )
{
	int i			= 0;
	ext2_dir* cdir	= NULL;
	char* off		= NULL;
	char name[256]	= { '\0' };
	char b[BLK_SZ]	= { '\0' };

	if( !ino || !S_ISDIR( ino->i_mode ) )
		return;

	printf( "\ndirectory entries for inode %i:\n", num ); 

	/*print header*/
	printf( "num\t\trec_len\t\tname_len\tascii name\n" );

	for( i = 0; i < MAX_DIRECT && ino->i_block[i]; i++ )
	{
		/*get the block*/
		k_t_get_blk( b, fd, ino->i_block[i] );
		cdir = (ext2_dir*)b;
		off = (char*)cdir;

		while( cdir->name_len > 0 )
		{
			/*copy the name over*/
			memset( name, 0, 256 );
			memcpy( name, cdir->name, cdir->name_len );

			/*print the name*/
			printf( "%i\t\t", cdir->inode );
			printf( "%i\t\t", cdir->rec_len );
			printf( "%i\t\t", cdir->name_len );
			//printf( ( S_ISDIR( ino->i_mode ) ) ? "d\t\t" : "-\t\t" );
			printf( "%s", name );
			printf( "\n" );

			/*advance the pointers*/
			off += cdir->rec_len;
			cdir = (ext2_dir*)off;
		}
	}

	return;
}



/******************************************************************************
function	: void k_t_get_num_and_fd( u32* num_dst, int* fd_dst, char* path ) 
author		: nc
desc		: get an inode number and from a path
date		: 04-01-13
 ******************************************************************************/
bool k_t_get_num_and_fd( u32* num_dst, int* fd_dst, mino_t* cwd, char* path )
{
	int i					= 0;
	mino_t* mino			= { 0 };	//inode holder
	char p1[MAX_PATH]		= { 0 };	//absolute part of path
	u32 nums[MAX_MINOS]		= { 0 };	//minos opened during traversal
	char* pels[MAX_DEPTH]	= { 0 };	//path elements
	int pels_num			= 0;		//number of path elements
	u32 last_ino			= 0;

	/*TODO fix path truncation*/

	if( !num_dst || !fd_dst || !path || !kr || !kr->m_mnt_tb[0] )
		goto fail;

	/*if we got a relative path with no cwd can't do anything*/
	if( !is_abs_path( path ) && cwd == NULL )
		goto fail;

	/*combine the relative and absolute parts of the path*/
	if( !is_abs_path( path ) )
	{
		/*get minos path name*/
	}

	/*join the paths*/
	strcpy( p1, path );	

	if( !strcmp( p1, "/" ) )
	{
		*num_dst = ROOT_INO_NUM;
		*fd_dst = kr->m_mnt_tb[0]->m_fd;
		goto ok;
	}

	/*split up the path*/
	split_path( p1, MAX_DEPTH, pels, &pels_num );

	if( pels_num == 0 )
		goto fail;

	/*get the start inode*/
	k_t_get_mino( kr->m_mnt_tb[0]->m_fd, ROOT_INO_NUM, &mino );
	last_ino = ROOT_INO_NUM;

	for( i = 0; i < MAX_DEPTH && pels[i]; i ++ )
	{
		nums[i] = last_ino;

		/*get the inode number for the current directory*/
		if( ( last_ino = k_t_find_dir_num( mino, pels[i] ) ) == -1 )
			goto fail;

		if( mino->m_mounted )
		{
			/*check for mount bound crossing here*/
		}

		/*get the inode we just found and search it for the current pel*/
		if( ( k_t_get_mino( mino->m_fd, last_ino, &mino ) ) == -1 )
			goto fail;

		/*is it a dir?*/
		if( ( S_ISDIR( mino->m_ino.i_mode ) ) == 0 )
			break;
	}

	/*done messed up*/
	if( pels[i+1] )
		goto fail;

ok:
	/*release everything we touched*/
	for( i = 0; i < MAX_MINOS; i++ )
	{
		if( nums[i] )
			k_t_put_mino( nums[i] );
	}

	return true;

fail:
	/*release everything we touched*/
	for( i = 0; i < MAX_MINOS; i++ )
	{
		if( nums[i] )
			k_t_put_mino( nums[i] );
	}

	printf( "failed to resolve inode\n" );
	return false;
}


/******************************************************************************
function	: int k_t_find_dir_num( mino_t* mino, char* dir )
author		: nc
desc		: find the inode number for a directory
date		: 03-12-13
 ******************************************************************************/
int k_t_find_dir_num( mino_t* mino, char* dir )
{
	int i			= 0;
	ext2_dir* cdir	= NULL;
	char* off		= NULL;
	char name[256]	= { 0 };
	char b[BLK_SZ]	= { 0 };

	if( !mino || !dir )
		goto fail;


	for( i = 0; i < MAX_DIRECT && mino->m_ino.i_block[i]; i++ )
	{
		/*get the block*/
		k_t_get_blk( b, mino->m_fd, mino->m_ino.i_block[i] );

		/*see if there is a match*/
		cdir = (ext2_dir*)b;
		off = (char*)cdir;

		while( off < (char*)( b + BLK_SZ ) )
		{
			/*get the name*/
			memset( name, 0, 256 );
			memcpy( name, cdir->name, cdir->name_len );

			/*found it*/
			if( !strcmp( name, dir ) )
				goto out;

			/*no match, so advance the pointers*/
			off += cdir->rec_len;
			cdir = (ext2_dir*)off;
		}
	}

	/*got through the loop and couldn't find anything*/
	goto fail;

out:
	return cdir->inode;

fail:
	return -1;
}



/******************************************************************************
function	: i32 k_t_find_child_by_name( mino_t* current, char* cname )
author		: nc
desc		: find a child in some dblocks
date		: 04-15-13
 ******************************************************************************/
i32 k_t_find_child_by_name( mino_t* mino, char* cname )
{
	int i			= 0;
	ext2_dir* cdir	= NULL;
	char* off		= NULL;
	char name[256]	= { 0 };
	char b[BLK_SZ]	= { 0 };

	if( !mino || !cname )
		goto fail;


	for( i = 0; i < MAX_DIRECT && mino->m_ino.i_block[i]; i++ )
	{
		/*get the block*/
		k_t_get_blk( b, mino->m_fd, mino->m_ino.i_block[i] );

		/*see if there is a match*/
		cdir = (ext2_dir*)b;
		off = (char*)cdir;

		while( off < (char*)( b + BLK_SZ ) )
		{
			/*get the name*/
			memset( name, 0, 256 );
			memcpy( name, cdir->name, cdir->name_len );

			/*found it*/
			if( !strcmp( name, cname ) )
				goto out;

			/*no match, so advance the pointers*/
			off += cdir->rec_len;
			cdir = (ext2_dir*)off;
		}
	}

	/*got through the loop and couldn't find anything*/
	goto fail;

out:
	return cdir->inode;

fail:
	return -1;
}



/******************************************************************************
function	: i32 k_t_find_child_by_name( mino_t* current, char* cname )
author		: nc
desc		: find a child in some dblocks
date		: 04-15-13
 ******************************************************************************/
bool k_t_add_child( mino_t* parent, i32 inum, char* cname )
{
	char b[BLK_SZ]			= { '\0' };
	char* cp				= NULL;
	ext2_dir* dp			= NULL;
	i32 tmp_size			= 0;


	/*get the data block*/
	if( !k_t_get_blk( b, parent->m_fd, parent->m_ino.i_block[0] ) )
	{
		printf( "error retrieving data block\n" );
		goto fail;
	}

	/*find the last entry*/
	cp = b;
	dp = (ext2_dir*)cp;

	while( cp + dp->rec_len < ( b + BLK_SZ ) )
	{
		cp += dp->rec_len;
		dp = (ext2_dir*)cp;
	}

	tmp_size = dp->rec_len;

	/*adjust the intermediate file's length*/ 
	dp->rec_len = 4 * ( ( 8 + dp->name_len + 3 ) / 4 );

	/*subtract it from the temoprary*/
	tmp_size -= dp->rec_len;

	/*now add the new entry to the directory*/
	cp += dp->rec_len;
	dp = (ext2_dir*)cp;

	/*TODO check size*/

	dp->inode = inum;
	dp->name_len = strlen( cname );
	dp->rec_len = tmp_size;
	dp->file_type = FILE_TYPE;
	strcpy( dp->name, cname );

	/*write the block*/
	if( !k_t_put_blk( b, parent->m_fd, parent->m_ino.i_block[0] ) )
	{
		printf( "error putting block\n" );
		goto fail;
	}


	return true;
fail:
	return false;
}

/******************************************************************************
function	: bool k_t_remove_child( mino_t* parent, char* cname )
author		: nc
desc		: remove a child from the parent mino's directory
date		: 04-15-13
 ******************************************************************************/
bool k_t_remove_child( mino_t* parent, char* cname )
{
	char b[BLK_SZ]			= { '\0' };
	char name[MAX_NAME]		= { '\0' };
	char* cp				= NULL;
	int tmp_size			= 0;
	ext2_dir* dp			= NULL;

	if( !parent || !cname )
		goto fail;

	/*get the data block*/
	k_t_get_blk( b, parent->m_fd, parent->m_ino.i_block[0] );

	cp = b;
	dp = (ext2_dir*)cp;

	/*try to find the entry*/
	while( cp < ( b + BLK_SZ ) )
	{
		memset( name, 0, MAX_NAME );
		strncpy( name, dp->name, dp->name_len );
		if( !strcmp( name, cname ) )
			break;

		cp += dp->rec_len;
		dp = (ext2_dir*)cp;
	}

	/*did we walk past?*/
	if( cp >= ( b + BLK_SZ ) )
	{
		if( parent->m_ino.i_block[1] == 0 )
			goto fail;
		else
			k_t_del_dblocks_from_mino( parent->m_fd, parent->m_ino_num, 1 );
	}

	/*shift some bits*/
	tmp_size = dp->rec_len;		
	memset( dp, 0, tmp_size );

	while( ( cp + tmp_size ) < ( b + BLK_SZ ) )
	{
		*cp = *(cp + tmp_size);
		cp++;
	}

	/*update the size of the last dir entry*/
	cp = b;
	dp = (ext2_dir*)cp;

	while( ( cp + dp->rec_len + tmp_size ) < ( b + BLK_SZ ) )
	{
		cp += dp->rec_len;
		dp = (ext2_dir*)cp;
	}

	dp->rec_len +=  tmp_size;


	/*put the block back*/
	k_t_put_blk( b, parent->m_fd, parent->m_ino.i_block[0] );

	return true;

fail:
	printf( "error removing child %s\n", cname );
	return false;
}



/******************************************************************************
function	: i32 k_t_find_oft_entry( mino_t* entry )
author		: nc
desc		: find an open entry in the open file table
date		: 
 ******************************************************************************/
i32 k_t_find_oft_entry( mino_t* entry )
{
	int i = 0;

	for( i = 0; i < MAX_OFS; i++ )
	{
		if( kr->m_of_tb[i] && kr->m_of_tb[i]->m_minoptr == entry )
			break;
	}

	if( i >= MAX_OFS )
		goto fail;

	return i;

fail:
	return -1;
}



/******************************************************************************
function	: bool k_t_open_oft_entry( ~ )
author		: nc
desc		: add an entry to the oft
date		: 
 ******************************************************************************/
bool k_t_open_oft_entry( mino_t* entry, E_FILE_MODE mode, i32* fd_loc, char* path )
{
	i32 ret_loc	= 0;

	if( !entry || !fd_loc || !path )
	{
		printf( "error: recieved improper arguemnts\n" );
		goto fail;
	}

	/*see if it already exists*/
	ret_loc = k_t_find_oft_entry( entry );

	/*it is already open so we have to check its mode*/
	if( ret_loc != -1 )
	{
		if( mode != M_READ )
		{
			printf( "error: file open in incompatible mode\n" );
			goto fail;
		}
	}
	else
	{
		/*not open so open it*/
		for( ret_loc = 0; ret_loc < MAX_OFS; ret_loc++ )
		{
			if( kr->m_of_tb[ret_loc] == NULL )
				break;
		}
	}

	/*make sure ret_loc is within the array*/
	if( ret_loc >=  MAX_OFS )
	{
		printf( "error: no open locations in kernel file table\n" );
		goto fail;
	}

	/*alredy exists so modify it*/
	if( kr->m_of_tb[ret_loc] )
	{
		kr->m_of_tb[ret_loc]->m_refc++;
		kr->m_of_tb[ret_loc]->m_off = 0;
	}
	else
	{
		of_t_init( &kr->m_of_tb[ret_loc] );
		kr->m_of_tb[ret_loc]->m_mode = mode;
		kr->m_of_tb[ret_loc]->m_refc = 1;
		kr->m_of_tb[ret_loc]->m_minoptr = entry;

		if( strlen ( path ) < MAX_NAME - 1 )
		{
			memset( kr->m_of_tb[ret_loc]->m_name, 0, MAX_NAME );
			strcpy( kr->m_of_tb[ret_loc]->m_name, path );
		}

		/*different cases for differernt r/w modes*/
		switch( mode )
		{
			case M_READ:
				kr->m_of_tb[ret_loc]->m_off = 0;
				break;
			case M_WRITE:
				k_t_truncate_mino( entry );
				kr->m_of_tb[ret_loc]->m_off = 0;
				break;
			case M_READWRITE:
				kr->m_of_tb[ret_loc]->m_off = 0;
				break;
			case M_APPEND:
				kr->m_of_tb[ret_loc]->m_off = entry->m_ino.i_size;
				break;
			default:
				printf( "error: invalid mode for kft\n" );
				goto fail;
				break;
		}
	}

	/*tell the caller what the file descriptor is*/
	*fd_loc = ret_loc;

	return true;

fail:
	return false;
}



/******************************************************************************
function	: bool k_t_close_oft_entry( mino_t* entry )
author		: nc
desc		: remove an entry from the oft
date		: 
 ******************************************************************************/
bool k_t_close_oft_entry( mino_t* entry )
{
	int loc	= 0;

	if( !entry )
		goto fail;

	loc = k_t_find_oft_entry( entry );

	if( loc == -1 )
	{
		printf( "error: kft entry not open\n" );
		goto fail;
	}

	kr->m_of_tb[loc]->m_refc--;

	/*are we the last user?*/
	if( kr->m_of_tb[loc]->m_refc == 0 )
	{
		/*bleh*/
		k_t_put_mino( kr->m_of_tb[loc]->m_minoptr->m_ino_num );
		of_t_destroy( &kr->m_of_tb[loc] );

		/*learned the hard way*/
		kr->m_of_tb[loc] = NULL;
	}

	return true;

fail:
	return false;
}



/******************************************************************************
function	: i64 k_t_lseek( i32 fd, i32 amount )
author		: nc
desc		: seek into a file descriptor
date		: 
 ******************************************************************************/
i64 k_t_lseek( i32 fd, i32 amount )
{
	i64 o_loc	= 0;

	/*get the table position and make sure it exists*/
	if( !kr->m_of_tb[fd] )
	{
		printf( "error: file desriptor does not exist\n" );
		goto fail;
	}

	/*we are supposed to return the origirnal location*/
	o_loc = kr->m_of_tb[fd]->m_off;

	/*seek the file descriptor*/
	if( amount < 0 )
	{
		printf( "warning: attempted to read before beginning of file\n" );
		kr->m_of_tb[fd]->m_off = 0;
	}
	else if( amount > kr->m_of_tb[fd]->m_minoptr->m_ino.i_size )
	{
		printf( "warning: attempted to read past end of file\n" );
		kr->m_of_tb[fd]->m_off = kr->m_of_tb[fd]->m_minoptr->m_ino.i_size;
	}
	else
	{
		kr->m_of_tb[fd]->m_off = amount;
	}

	return o_loc;

fail:
	return -1;

}


/******************************************************************************
function	: i32 k_t_write( i32 fd, i32 num, char* src)
author		: nc
desc		: write to a file descriptor
date		: 
 ******************************************************************************/
i32 k_t_write( i32 fd, i32 num, char* src)
{
	i32 bytes_writen	= 0;
	i32 blk			= 0;
	i32 byte		= 0;
	i32 add			= 0;
	i32 cblock_loc	= 0;
	i32 cblock_off	= 0;
	of_t* target	= NULL;
	char b[BLK_SZ]	= { '\0' };
	char buf[BLK_SZ] = { '\0' };
	char* cp		= NULL;
	unsigned* off	= 0;

	if( !fd || num <= 0 || !src || !kr->m_of_tb[fd] )
		goto fail;

	target = kr->m_of_tb[fd];
	cp = src;

	if( target->m_mode != M_WRITE &&
			target->m_mode != M_READWRITE )
	{
		printf( "error: file is not open for write mode\n" );
		goto fail;
	}

	target->m_minoptr->m_ino.i_size += num;
	
	/*figure out how many blocks to add*/
	add += ( num / BLK_SZ );

	if(add < 8)
		add += 1;

	printf( "add %d\n", add );
	k_t_add_dblocks_to_mino( target->m_minoptr->m_fd, target->m_minoptr->m_ino_num, add );
	memset( buf, 0 , BLK_SZ );
	memset( b, 0, BLK_SZ );

	while(num != 0)
	{
		/*find out our starting location*/
		blk = target->m_off / BLK_SZ;
		byte = target->m_off % BLK_SZ;

		while( byte < BLK_SZ &&
				num != 0 &&
				target->m_off < target->m_minoptr->m_ino.i_size)
		{
			/*copy the data over*/
			strncat(b, cp, 1);

			/*lots of counters*/
			cp++;
			target->m_off++;
			bytes_writen++;
			byte++;
			num--;
		}

		/*find out indirection level*/
		if( blk <= DIR_RANGE )
		{
			k_t_put_blk(b, target->m_minoptr->m_fd, target->m_minoptr->m_ino.i_block[blk] );
		}
		if( blk > DIR_RANGE && blk <= OID_RANGE )
		{
			/*get indirect block and then get the data block*/
			k_t_get_blk( buf,
					target->m_minoptr->m_fd,
					target->m_minoptr->m_ino.i_block[ONE_INDIR - 1] );

			/*the offset in this block is blk - 12*/
			off = (unsigned*)buf;
			off += ( blk - MAX_DIRECT );

			if( !*off)
				printf( "error: no id (1id) data block\n" );

			/*put the data block*/
			k_t_put_blk( b,
					target->m_minoptr->m_fd,
					*off );

		}
		if( blk > OID_RANGE && blk <= TID_RANGE )
		{
			/*get indirect block and then get the data block*/
			k_t_get_blk( buf,
					target->m_minoptr->m_fd,
					target->m_minoptr->m_ino.i_block[TWO_INDIR - 1] );

			/*now find out which 2id block to get - subtract one for offset*/
			cblock_loc = ( ( blk - MAX_DIRECT ) / ID_PER_BLK ) - 1;
			off = (unsigned*)buf;
			off += cblock_loc;

			if( !*off )
			{
				printf( "error: no id (2id) data block\n" );
				goto fail;
			}

			/*get the two id data block*/
			k_t_get_blk( buf,
					target->m_minoptr->m_fd,
					*off );

			/*find out where the two id offset is*/
			cblock_off = ( ( blk - MAX_DIRECT ) % ID_PER_BLK );
			off = (unsigned*)buf;
			off += cblock_off;

			if( !*off )
			{
				printf( "error: no 2id (2id) data block\n" );
				goto fail;
			}

			/*get the two id data block*/
			k_t_put_blk( b,
					target->m_minoptr->m_fd,
					*off );
		}
		memset(b, 0, BLK_SZ);
	
	}

	return bytes_writen;
fail:

	return -1;

	
}

/******************************************************************************
function	: i32 k_t_read( i32 fd, i32 num, char* dst )
author		: nc
desc		: read bytes from a file
date		: 
 ******************************************************************************/
i32 k_t_read( i32 fd, i32 num, char* dst )
{
	i32 bytes_read	= 0;
	i32 blk			= 0;
	i32 byte		= 0;
	i32 cblock_loc	= 0;
	i32 cblock_off	= 0;
	of_t* target	= NULL;
	char b[BLK_SZ]	= { '\0' };
	char* cp		= NULL;
	unsigned* off	= 0;

	if( !fd || num <= 0 || !dst || !kr->m_of_tb[fd] )
		goto fail;

	/*easier to work with*/
	target = kr->m_of_tb[fd];
	cp = dst;

	if( target->m_mode != M_READ &&
			target->m_mode != M_READWRITE )
	{
		printf( "error: file is not open for read mode\n" );
		goto fail;
	}

	while( num != 0 )
	{
		/*find out our starting location*/
		blk = target->m_off / BLK_SZ;
		byte = target->m_off % BLK_SZ;

		/*find out indirection level*/
		if( blk <= DIR_RANGE )
		{
			k_t_get_blk( b, target->m_minoptr->m_fd, target->m_minoptr->m_ino.i_block[blk] );
		}
		if( blk > DIR_RANGE && blk <= OID_RANGE )
		{
			/*get indirect block and then get the data block*/
			k_t_get_blk( b,
					target->m_minoptr->m_fd,
					target->m_minoptr->m_ino.i_block[ONE_INDIR - 1] );

			/*printf( "got id data block for mino %i at pos %i\n",
					target->m_minoptr->m_ino_num,
					blk );*/

			/*the offset in this block is blk - 12*/
			off = (unsigned*)b;
			off += ( blk - MAX_DIRECT );

			if( !*off )
				printf( "error: no id (1id) data block\n" );

			/*get the data block*/
			k_t_get_blk( b,
					target->m_minoptr->m_fd,
					*off );

		}
		if( blk > OID_RANGE && blk <= TID_RANGE )
		{
			/*get indirect block and then get the data block*/
			k_t_get_blk( b,
					target->m_minoptr->m_fd,
					target->m_minoptr->m_ino.i_block[TWO_INDIR - 1] );

			/*now find out which 2id block to get - subtract one for offset*/
			cblock_loc = ( ( blk - MAX_DIRECT ) / ID_PER_BLK ) - 1;
			off = (unsigned*)b;
			off += cblock_loc;

			if( !*off )
			{
				printf( "error: no id (2id) data block\n" );
				goto fail;
			}

			/*get the two id data block*/
			k_t_get_blk( b,
					target->m_minoptr->m_fd,
					*off );

			/*find out where the two id offset is*/
			cblock_off = ( ( blk - MAX_DIRECT ) % ID_PER_BLK );
			off = (unsigned*)b;
			off += cblock_off;

			if( !*off )
			{
				printf( "error: no 2id (2id) data block\n" );
				goto fail;
			}

			/*get the two id data block*/
			k_t_get_blk( b,
					target->m_minoptr->m_fd,
					*off );
		}

		/*loop while we are in this block*/
		while( byte < BLK_SZ &&
				num != 0 &&
				target->m_off < target->m_minoptr->m_ino.i_size )
		{
			/*copy the data over*/
			*cp = b[byte];

			/*lots of counters*/
			cp++;
			target->m_off++;
			bytes_read++;
			byte++;
			num--;
		}

		/*don't read past the end*/
		if( target->m_off > target->m_minoptr->m_ino.i_size )
			break;

	}

	target->m_minoptr->m_refc = 1;
	return bytes_read;

fail:

	return -1;
}

