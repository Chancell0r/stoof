/******************************************************************************
file		: test_func.c
author		: 
desc		: 
date		: 04-01-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "test_func.h"
#include "../global_defs.h"
#include "../k_t.h"
#include "../mino_t.h"
#include "../proc_t.h"


extern k_t* kr;

int test_cmd( proc_t* pr, int argc, char** argv )
{
	//bool i			= 0;
	//mnt_t* mt		= NULL;
	//inode new_ino	= { 0 };
	//u32 loc			= 0;

	if( !kr )
		goto fail;

	printf( "returned: %i\n", k_t_get_next_imap_loc( pr->m_cwd->m_fd ) );
	printf( "returned: %i\n", k_t_get_next_bmap_loc( pr->m_cwd->m_fd ) );
	k_t_flush_clean_minos();

	if( argc > 3 && !strcmp( argv[1], "add" ) )
	{
		k_t_add_dblocks_to_mino( pr->m_cwd->m_fd, atoi( argv[2] ), atoi( argv[3] ) );
		printf( "ok\n" );
	}

	if( argc > 3 && !strcmp( argv[1], "del" ) )
	{
		k_t_del_dblocks_from_mino( pr->m_cwd->m_fd, atoi( argv[2] ), atoi( argv[3] ) );
		printf( "ok\n" );
	}
		
	return true;

fail:
	return false;
}




