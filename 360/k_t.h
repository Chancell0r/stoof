/******************************************************************************
file		: k_t.h
author		: nc
desc		: k_t struct
date		: 03-25-13
 ******************************************************************************/

#pragma once

#include "global_defs.h"
#include "proc_t.h"
#include "mnt_t.h"
#include "mino_t.h"
#include "of_t.h"

/*fw dec*/
typedef struct k_t k_t;


/*defines*/
#define MAX_PROCS	64		//maximum number of running processes
#define MAX_MNTS	256		//maximum number of mount table entires
#define MAX_MINOS	256		//maximum number of in-memory inodes
#define MAX_OFS		256		//maximum number of open files
#define DEF_IMG		"img/img"


/*struct*/
struct k_t
{
	proc_t*		m_proc_tb[MAX_PROCS];	//array of processes
	mnt_t*		m_mnt_tb[MAX_MNTS];		//array of mount points
	mino_t*		m_mino_tb[MAX_MINOS];	//array of memory inodes
	of_t*		m_of_tb[MAX_OFS];
	proc_t*		m_cproc;				//current process
	mnt_t*		m_cmnt;					//current mount

};

/*prototypes*/
bool	k_t_init( k_t** );				//initialize
void	k_t_destroy( k_t** );			//shutdown
int		k_t_start( void );				//startup stuff
int		k_t_run( void );				//main loop
bool	k_t_mount_root( char*, mnt_t** );
bool	k_t_start_procs( void );
void	k_t_write_fs_to_disk( void );	//flushes everything

/*blk/mnt related*/
bool	k_t_get_blk( char* dst, int fd, unsigned off );
bool	k_t_put_blk( char* src, int fd, unsigned off );
mnt_t*	k_t_get_mnt_from_fd( int );
bool	k_t_flush_superblock( int fd );
bool	k_t_flush_gd0( int fd );

/*ino/mino related*/
bool	k_t_get_ino( int, u32, inode* );
bool	k_t_get_mino( int fd, u32 inum, mino_t** dst );
bool	k_t_put_ino( int fd, u32 num, inode* ino );
bool	k_t_put_mino( u32 );
bool	k_t_set_mino_as_mounted( int fd, u32 inum, mnt_t* mountpoint );
bool	k_t_set_mino_as_unmounted( int fd, u32 inum, mnt_t* mountpoint );
bool	k_t_flush_clean_minos( void );
bool	k_t_flush_dirty_minos( void );
bool	k_t_flush_all_not_mounted_minos( void );
bool	k_t_flush_all_minos( void );
u32		k_t_add_ino_to_fs( int fd, inode* new_ino );
bool	k_t_del_ino_from_fs( int fd, u32 inum );
bool	k_t_add_dblocks_to_mino( int fd, u32 mino_num, u32 num );
bool	k_t_del_dblocks_from_mino( int fd, u32 mino_num, u32 num );
bool	k_t_truncate_mino( mino_t* mino );

/*bitmap related*/
bool	k_t_set_imap_bit( int fd, u32 bnum, bool bval );
bool	k_t_get_imap_bit( int fd, u32 bnum, bool* retv );
bool	k_t_set_bmap_bit( int fd, u32 bnum, bool bval );
bool	k_t_get_bmap_bit( int fd, u32 bnum, bool* retv );
u32		k_t_get_next_imap_loc( int fd );
u32		k_t_get_next_bmap_loc( int fd );

/*misc*/
void	k_t_p_dir_items( inode*, int, unsigned );
int		k_t_find_dir_num( mino_t* mino, char* dir );
i32		k_t_find_child_by_name( mino_t* current, char* cname );
bool	k_t_add_child( mino_t* parent, i32 inum, char* cname );
bool	k_t_remove_child( mino_t* parent, char* cname );

/*of_tb related*/
i32		k_t_find_oft_entry( mino_t* entry );
bool	k_t_open_oft_entry( mino_t* entry, E_FILE_MODE mode, i32* fd_loc, char* path );
bool	k_t_close_oft_entry( mino_t* entry );
i64		k_t_lseek( i32 fd, i32 amount );
i32		k_t_read( i32 fd, i32 num, char* dst );
i32		k_t_write( i32 fd, i32 num, char* src);


