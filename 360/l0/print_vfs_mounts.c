/******************************************************************************
file		: print_vfs_mounts.c
author		: nc
desc		: 
date		: 04-01-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "print_vfs_mounts.h"

#include "../global_defs.h"
#include "../k_t.h"
#include "../mnt_t.h"
#include "../proc_t.h"

extern k_t* kr;

int print_vfs_mounts( proc_t* pr, int argc, char** argv )
{
	int i = 0;

	if( !kr )
		goto fail;

	printf( "vfs mount information:\n" );
	printf( "\tinodes\t\tblocks\t\tbusy\t\troot inode\tdev name\tmount name\n" );

	for( i = 0; i < MAX_MNTS && kr->m_mnt_tb[i]; i++ )
	{
		printf(
				"\t%i\t\t%i\t\t%i\t\t%p\t%s\t\t%s\n",
				kr->m_mnt_tb[i]->m_num_inos,
				kr->m_mnt_tb[i]->m_num_blocks,
				kr->m_mnt_tb[i]->m_busy,
				kr->m_mnt_tb[i]->m_root_ino,
				kr->m_mnt_tb[i]->m_dev_name,
				kr->m_mnt_tb[i]->m_mnt_name
			  );

		printf( "\n" );
	}

ok:
	return true;

fail:
	return false;
}


