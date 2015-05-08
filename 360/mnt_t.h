/******************************************************************************
file		: mnt_t.h
author		: nc
desc		: mnt_t struct
date		: 03-25-13
 ******************************************************************************/

#pragma once

#include "global_defs.h"
#include "mino_t.h"


/*fw dec*/
typedef struct mnt_t mnt_t;
typedef struct mino_t mino_t;


/*struct*/
struct mnt_t
{
	int			m_num_inos;
	int			m_num_blocks;
	int			m_first_ino;
	int			m_ino_start;
	int			m_data_start;
	int			m_fd;
	bool		m_busy;
	mino_t*		m_root_ino;
	char		m_dev_name[MAX_NAME];
	char		m_mnt_name[MAX_NAME];
	sb			m_sb;
	gd			m_gd0;
};

bool	mnt_t_init( mnt_t** );
void	mnt_t_destroy( mnt_t** );
void	mnt_t_show( mnt_t* );
bool	mnt_t_create( mnt_t* mt, char* path_to_img, char* folder_name, u32 inum );
bool	mnt_t_add_root_mino( mnt_t* target, mino_t* root );
void	mnt_t_lock( mnt_t* args );
void	mnt_t_unlock( mnt_t* args );

