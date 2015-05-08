/******************************************************************************
file		: proc_t.c
author		: nc
desc		: implementation of the proc_t functions
date		: 03-24-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "global_defs.h"
#include "proc_t.h"


/******************************************************************************
function	: 
author		: nc
desc		: 
date		: 03-24-13
 ******************************************************************************/
bool proc_t_init( proc_t** pr )
{
	*pr = malloc( sizeof(**pr) );

	if( !*pr )
		goto fail;

	memset( *pr, 0, sizeof(**pr) );

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
void proc_t_destroy( proc_t** pr )
{
	if( *pr )
		free( *pr );

	*pr = NULL;

	return;
}


/******************************************************************************
function	: 
author		: nc
desc		: make a new process
date		: 03-24-13
 ******************************************************************************/
bool proc_t_make(
		proc_t* p,
		i16 uid,
		i16 pid,
		i16 gid,
		i16 parid,
		i16 status,
		proc_t* parent,
		mino_t* cwd )
{

	if( !p || !cwd )
		goto fail;

	p->m_uid = uid;
	p->m_pid = pid;
	p->m_gid = gid;
	p->m_parid = parid;
	p->m_status = status;

	if( parent == NULL )
		p->m_parent = p;
	else
		p->m_parent = parent;

	p->m_cwd = cwd;	

	return true;
fail:
	printf( "unable to create process %i\n", pid );
	return false;
}



/******************************************************************************
function	: 
author		: nc
desc		: 
date		: 03-24-13
 ******************************************************************************/
i32 proc_t_get_next_fd_loc( proc_t* pr )
{
	int i = 0;
	for( i = 0; i < NUM_FDS; i++ )
	{
		if( pr->m_fds[i] == 0 )
			break;
	}

	if( i >= NUM_FDS )
	{
		printf( "error: no open file descriptors avaliable\n" );
		goto fail;
	}

	return i;

fail:
	return -1;
}


/******************************************************************************
function	: bool proc_t_add_fd_to_tb( proc_t* pr, i32 fd ) 
author		: nc
desc		: 
date		: 03-24-13
 ******************************************************************************/
bool proc_t_add_fd_to_tb( proc_t* pr, i32 fd )
{
	int i = 0;

	i = proc_t_get_next_fd_loc( pr );

	if( i == -1 )
	{
		goto fail;
	}

	pr->m_fds[i] = fd;

	return true;

fail:
	return false;
}



/******************************************************************************
function	: bool proc_t_del_fd_from_tb( proc_t* pr, i32 fd )
author		: nc
desc		: 
date		: 03-24-13
 ******************************************************************************/
bool proc_t_del_fd_from_tb( proc_t* pr, i32 fd )
{
	if( !pr )
		goto fail;

	int i = 0;
	
	for( i = 0; i < NUM_FDS; i++ )
	{
		if( pr->m_fds[i] == fd )
			break;
	}

	if( i >= NUM_FDS )
	{
		printf( "error: can't remove fd, doesn't exist in proct oft\n" );
		goto fail;
	}


	return true;

fail:
	return false;

}
