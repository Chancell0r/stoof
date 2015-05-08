/******************************************************************************
file		: mino_t.h
author		: nc
desc		: mino_t struct
date		: 03-25-13
 ******************************************************************************/

#pragma once

#include <ext2fs/ext2_fs.h>
#include "global_defs.h"
#include "mnt_t.h"


/*fw dec*/
typedef struct mino_t mino_t;
typedef struct mnt_t mnt_t;


/*struct*/
struct mino_t
{
	inode	m_ino;
	u16		m_fd;
	u32		m_ino_num;
	u16		m_refc;
	bool	m_dirty;
	bool	m_mounted;
	mnt_t*	m_mountp;
};

bool	mino_t_init( mino_t** );
void	mino_t_destroy( mino_t** );

bool	mino_t_make(
		mino_t*,
		inode*,
		u16,
		u32,
		u16,
		bool,
		bool,
		mnt_t* );

bool	mino_t_set_ino(
		inode* dst,
		u16 uid,
		u16 gid,
		u32 size,
		u16 mode,
		u16 links );

void	mino_t_show( mino_t* );
void	mino_t_show_ino( inode*, unsigned );
     

