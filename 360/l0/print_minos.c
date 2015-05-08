/******************************************************************************
file		: print_minos.c
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
#include "print_minos.h"

extern k_t* kr;

int print_minos( proc_t* pr, int argc, char** argv )
{
	int i = 0;
	int j = 1;

	if( !kr )
		goto fail;

	/*print all the loaded in-memory inodes*/
	printf( "memory inode information:\n" );
	printf( "\toffset\t\tinode num\tfd\t\trefs\t\tdirty\t\tmounted\t\tmount point\n" );

	/*if we were given a list*/
	if( argc > 1 )
	{
		for( j = 1; j < argc; j++ )
		{
			i = atoi( argv[j] );
			if( kr->m_mino_tb[i] )
			{
				printf(
						"\t%i\t\t%i\t\t%i\t\t%i\t\t%i\t\t%i\t\t%p\n",
						i,
						kr->m_mino_tb[i]->m_ino_num,
						kr->m_mino_tb[i]->m_fd,
						kr->m_mino_tb[i]->m_refc,
						kr->m_mino_tb[i]->m_dirty,
						kr->m_mino_tb[i]->m_mounted,
						kr->m_mino_tb[i]->m_mountp
					  );
			}
			else
			{
				printf( "\tinvalid entry %i\n", i );
			}
		}
		goto ok;
	}

	/*otherwise print them all*/
	for( i = 0; i < MAX_MINOS && kr->m_mino_tb[i]; i++ )
	{
		printf(
				"\t%i\t\t%i\t\t%i\t\t%i\t\t%i\t\t%i\t\t%p\n",
				i,
				kr->m_mino_tb[i]->m_ino_num,
				kr->m_mino_tb[i]->m_fd,
				kr->m_mino_tb[i]->m_refc,
				kr->m_mino_tb[i]->m_dirty,
				kr->m_mino_tb[i]->m_mounted,
				kr->m_mino_tb[i]->m_mountp
			  );
	}

	printf( "\n" );

ok:
	return true;

fail:
	return false;
}

