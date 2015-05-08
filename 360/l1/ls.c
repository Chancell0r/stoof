/******************************************************************************
file		: ls.c
author		: 
desc		: 
date		: 04-01-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "ls.h"
#include "../string_funcs.h"
#include "../global_defs.h"
#include "../k_t.h"
#include "../mino_t.h"
#include "../proc_t.h"


extern k_t* kr;


/******************************************************************************
function	: int ls_cmd( proc_t* pr, int argc, char** argv )
author		: jk
desc		: ls command
date		: 
 ******************************************************************************/
int ls_cmd( proc_t* pr, int argc, char** argv )
{
	mino_t* tmp				= NULL;
	int i					= 0;
	int inum				= 0; 
	int num_splits			= 0;
	int to_free[MAX_DEPTH]	= { 0 };
	char buf[BLK_SZ]		= { 0 };
	char lnk[BLK_SZ]		= { 0 };
	char name[256]			= { 0 };
	char* split[MAX_DEPTH]	= { 0 };
	char* cp				= NULL;
	ext2_dir* dp			= NULL;
	mino_t* current			= NULL;
	time_t temp			= 0;

	if( !kr || !pr || !argc )
		goto fail;

	/*path was provided*/
	if( argc > 1 )
	{
		if( is_abs_path( argv[1] ) )
		{
			current = kr->m_mino_tb[0];
		}
		else
		{
			current = pr->m_cwd;
		}
		if( !current )
		{
			printf( "error: couldn't find starting directory\n" );
			goto fail;
		}

		if( !split_path( argv[1], MAX_DEPTH, split, &num_splits ) )
		{
			printf( "error splitting path\n" );
			goto fail;
		}

		/*walk through the directories*/
		for( i = 0; i < num_splits; i++ )
		{
			/*find the inode number for the current directory*/
			inum = k_t_find_dir_num( current, split[i] );
			if( inum != -1 )
			{
				/*get the next inode*/
				to_free[i] = inum;
				k_t_get_mino( current->m_fd, inum, &current );
			}
			else
			{
				printf( "could not find directory %s\n", split[i] );
				goto fail;
			}
		}

		/*free what we don't need*/
		for( i = 0; i < num_splits; i ++ )
		{
			if( to_free[i] )
				k_t_put_mino( to_free[i] );
		}

	}
	else
	{
		current = pr->m_cwd;
	}


	/*don't need to lock since we aren't writing to the fs*/
	k_t_get_blk( buf, current->m_fd, current->m_ino.i_block[0] );
	cp = buf;
	dp = (ext2_dir*) buf;

	i = 0;
	/*read the contents*/
	while( cp && cp < ( buf + BLK_SZ ) )
	{	
		k_t_get_mino( current->m_fd, dp->inode, &tmp );//make new m inode

		/*print ino number*/
		printf( "%i\t", dp->inode );

		/*print mode and other information of file/directory*/
		printf( (S_ISDIR(tmp->m_ino.i_mode)) ? "d" : 
				(S_ISLNK(tmp->m_ino.i_mode) ? "l" : "-"));

		/*check r/w/x bits*/
		for( i = 0; i < 7; i+= 3 )
		{
			printf( ( ( tmp->m_ino.i_mode & 0x0100 >> i ) == 0 ) ? 
					"-" : "r");
			printf( ( ( tmp->m_ino.i_mode & 0x0100 >> ( i + 1 ) ) == 0 ) ? 
					"-" : "w");
			printf( ( ( tmp->m_ino.i_mode & 0x0100 >> ( i + 2 ) ) == 0 ) ? 
					"-" : "x");
		}
		
		printf( "  " );

		/*show the number of links*/
		printf( "%i  ", tmp->m_ino.i_links_count );

		/*show uid*/
		printf( "%i  ", tmp->m_ino.i_uid );

		/*show gid*/
		printf( "%i  ", tmp->m_ino.i_gid );

		/*show the ctime*/
		temp = tmp->m_ino.i_ctime;
		printf( "%.24s\t", ctime( &(temp) ) );

		printf( "%d %d \t", tmp->m_ino.i_uid, 
				tmp->m_ino.i_size);



		/*print name of file/directory*/
		memset( name, 0, 256);
		memcpy( name, dp->name, dp->name_len);
		printf( "%s", name);
		
		/*print link if found*/
		if(S_ISLNK(tmp->m_ino.i_mode))
		{
			k_t_get_blk(lnk, tmp->m_fd, tmp->m_ino.i_block[0]);
			printf(" -> %s", lnk);
		}
		printf("\n");
		cp += dp->rec_len;
		dp = (ext2_dir*)cp;

	}


fail:

	/*free the split*/
	for( i = 0; i < num_splits; i++ )
	{
		if( split[i] )
		{
			free( split[i] );
			split[i] = NULL;
		}
	}


	return -1;
}

