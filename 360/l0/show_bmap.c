/******************************************************************************
file		: how_bmap.c
author		: nc
desc		: print out the data block bitmap
date		: 04-07-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "../global_defs.h"
#include "../k_t.h"
#include "../mino_t.h"
#include "../proc_t.h"
#include "../mnt_t.h"
#include "show_imap.h"

extern k_t* kr;

int show_bmap( proc_t* pr, int argc, char** argv )
{
	u32 cnt_dblocks			= 0;
	int i					= 0;
	int j					= 0;
	bool ret				= 0;
	mnt_t* current_mount	= NULL;

	/*get the current mount point*/
	current_mount = k_t_get_mnt_from_fd( pr->m_cwd->m_fd );

	if( !current_mount )
		goto fail;

	mnt_t_lock( current_mount );

	if( argc < 2 )
	{

		printf( "data block bitmap for mount point %s (%s):\n\n",
				current_mount->m_mnt_name,
				current_mount->m_dev_name );

		printf( "\t[0-7]\t\t\t  [8-15]\t\t    [16-23]\t\t      [24-31]" );

		for( i = 0; i < current_mount->m_sb.s_blocks_count; i ++ )
		{
			if( i % 32 == 0 )
			{
				j = 0;
				printf( "\n%i:\t", i );
			}

			if( j > 1 && j % 8 == 0 )
			{
				printf( "  " );
			}

			k_t_get_bmap_bit( pr->m_cwd->m_fd, i, &ret );
			printf( "%d  ", ret );

			j++;

			/*increment our count*/
			if( ret )
				cnt_dblocks++;
		}

		printf( "\n\n" );

		printf( "superblock reports:\t%i dblocks, %i free and %i used\n",
				current_mount->m_sb.s_blocks_count,
				current_mount->m_sb.s_free_blocks_count,	
				current_mount->m_sb.s_blocks_count - current_mount->m_sb.s_free_blocks_count );

		printf( "counted:\t\t%i dblocks, %i free and %i used\n",
				current_mount->m_sb.s_blocks_count,
				current_mount->m_sb.s_blocks_count - cnt_dblocks,
				cnt_dblocks );
	}
	else
	{
		if( atoi( argv[1] ) > current_mount->m_sb.s_blocks_count )
		{
			printf( "error, value too large - maximum dblock value is %i\n",
					current_mount->m_sb.s_blocks_count );
			goto ok;
		}

		k_t_get_bmap_bit( pr->m_cwd->m_fd, atoi( argv[1] ), &ret );

		printf( "data block %i is set to %i\n",
				atoi( argv[1] ),
				ret );
	}

ok:
	mnt_t_unlock( current_mount );
	return true;

fail:
	printf( "error retrieving dmap\n") ;
	mnt_t_unlock( current_mount );
	return false;
}


