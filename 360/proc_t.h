/******************************************************************************
file		: proc_t.h
author		: nc
desc		: proc_t struct
date		: 03-24-13
 ******************************************************************************/

#pragma once

#include "global_defs.h"
#include "of_t.h"
#include "mino_t.h"

/*fw dec*/
typedef struct proc_t proc_t;

/*struct*/
struct proc_t
{
	i32			m_uid;
	i32			m_pid;
	i32			m_gid;
	i32			m_parid;
	i32			m_status;
	proc_t*		m_parent;
	mino_t*		m_cwd;
	i32			m_fds[NUM_FDS];
};

bool	proc_t_init( proc_t** );
void	proc_t_destroy( proc_t** );
bool	proc_t_make(
		proc_t*,
		i16,
		i16,
		i16,
		i16,
		i16,
		proc_t*,
		mino_t* );

i32		proc_t_get_next_fd_loc( proc_t* );
bool	proc_t_add_fd_to_tb( proc_t* pr, i32 fd );
bool	proc_t_del_fd_from_tb( proc_t* pr, i32 fd );
