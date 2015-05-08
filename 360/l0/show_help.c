/******************************************************************************
file		: show_help.c
author		: nc
desc		: 
date		: 04-01-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "../global_defs.h"
#include "../proc_t.h"
#include "show_help.h"


int show_help( proc_t* pr, int argc, char** argv )
{
	printf( "currently avalaible commands are:\n\n" );
	printf( "level zero:\n" );
	printf( "\thelp - this menu\n" );
	printf( "\tpminos - show all or specific in-memory m_inode_t*'s\n" );
	printf( "\tpvfs - show all or specific vfs mount points\n" );
	printf( "\ttest - for debugging\n" );
	printf( "\tshow_imap - show the inode bitmap (currently only for root mount)\n" );
	printf( "\tshow_bmap - show the data block bitmap (currently only for root mount)\n" );	
	printf( "\tpino - show information for a list of inodes\n" );
	printf( "\tpprocs - show currently running processes\n" );
	printf( "\tswitch - switch currently running process\n" );
	printf( "\n" );
	printf( "level one:\n" );
	printf( "\tls\n" );
	printf( "\tcd\n" );
	printf( "\tmkdir\n" );
	printf( "\tcreat\n" );
	printf( "\tchgrp\n" );
	printf( "\tchown\n" );
	printf( "\tchmod\n" );
	printf( "\tlink\n" );
	printf( "\tpwd\n" );
	printf( "\trmdir\n" );
	printf( "\tstat\n" );
	printf( "\tsymblink\n" );
	printf( "\ttouch\n" );
	printf( "\tunlink\n" );
	printf( "\n" );
	printf( "level two:\n" );
	printf( "\tcat\n" );
	printf( "\tclose\n" );
	printf( "\tcp\n" );
	printf( "\tlseek\n" );
	printf( "\tmv\n" );
	printf( "\topen\n" );
	printf( "\tpfd\n" );
	printf( "\tread\n" );
	printf( "\twrite\n" );
	printf( "\n" );

ok:
	return true;

fail:
	return false;
}



