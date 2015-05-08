/******************************************************************************
file		: print_ino.c
author		: nc
desc		: prints out the in-memory inodes
date		: 04-01-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "../global_defs.h"
#include "../k_t.h"
#include "../mino_t.h"
#include "../proc_t.h"
#include "print_ino.h"

extern k_t* kr;

int print_ino( proc_t* pr, int argc, char** argv )
{
	int i		= 0;
	int j		= 1;
	mino_t* tmp	= NULL;

	if( !kr )
		goto fail;

	if( argc < 2 )
	{
		printf( "usage: pino [list of inodes]\n" );
		goto fail;
	}

	printf( "inode info:\n" );
	printf( "\tmode\tuid\tsize\tgid\tlinks\tblocks\tblock (direct)\n" );

	for( i = 1; i < argc; i ++ )
	{
		k_t_get_mino( pr->m_cwd->m_fd, atoi( argv[i] ), &tmp );

		if( !tmp )
		{
			printf( "\terror retrieveing inode\n" );
			goto fail;
		}

		printf( "\t%x\t%i\t%i\t%i\t%i\t%i\t",
				tmp->m_ino.i_mode,
				tmp->m_ino.i_uid,
				tmp->m_ino.i_size,
				tmp->m_ino.i_gid,
				tmp->m_ino.i_links_count,
				tmp->m_ino.i_blocks	);

		for( j = 0; j < MAX_ENT; j ++ )
		{
			printf( "%i ", tmp->m_ino.i_block[j] );
		}

		printf( "\n" );

		k_t_put_mino( atoi( argv[i] ) );

	}


ok:
	return true;

fail:
	return false;
}

