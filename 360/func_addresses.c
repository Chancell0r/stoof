/******************************************************************************
file		: func_addresses.c
author		: nc
desc		: adrresses for function pointers
date		: 04-01-13
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "global_defs.h"
#include "func_addresses.h"


fp_tb get_fp( char* string )
{
	/*level 0*/
	if( !strcmp( string, "help" ) )
		return show_help;
	if( !strcmp( string, "test" ) )
		return test_cmd;
	if( !strcmp( string, "pminos" ) )
		return print_minos;
	if( !strcmp( string, "pvfs" ) )
		return print_vfs_mounts;
	if( !strcmp( string, "show_imap" ) )
		return show_imap;
	if( !strcmp( string, "show_bmap" ) )
		return show_bmap;
	if( !strcmp( string, "pino" ) )
		return print_ino;
	if( !strcmp( string, "pprocs" ) )
		return print_procs;
	if( !strcmp( string, "switch" ) )
		return switch_cmd;

	/*level 1*/
	if( !strcmp( string, "ls" ) )
		return ls_cmd;
	if( !strcmp( string, "cd" ) )
		return cd_cmd;
	if( !strcmp( string, "creat" ) )
		return creat_cmd;
	if( !strcmp( string, "mkdir" ) )
		return mkdir_cmd;
	if( !strcmp( string, "pwd" ) )
		return pwd_cmd;
	if( !strcmp( string, "link" ) )
		return link_cmd;
	if( !strcmp( string, "unlink" ) )
		return unlink_cmd;
	if( !strcmp( string, "rmdir" ) )
		return rmdir_cmd;
	if( !strcmp( string, "symlink") )
		return symblink_cmd;
	if( !strcmp( string, "stat") )
		return stat_cmd;
	if( !strcmp( string, "touch") )
		return touch_cmd;
	if( !strcmp( string, "chown") )
		return chown_cmd;
	if( !strcmp( string, "chmod") )
		return chmod_cmd;
	if( !strcmp( string, "chgrp") )
		return chgrp_cmd;

	/*level 2*/
	if( !strcmp( string, "open" ) )
		return open_cmd;
	if( !strcmp( string, "close" ) )
		return close_cmd;
	if( !strcmp( string, "lseek" ) )
		return lseek_cmd;
	if( !strcmp( string, "pfd" ) )
		return pfd_cmd;
	if( !strcmp( string, "read" ) )
		return read_cmd;
	if( !strcmp( string, "cat" ) )
		return cat_cmd;
	if( !strcmp( string, "write") )
		return write_cmd;
	if( !strcmp( string, "cp") )
		return cp_cmd;

	return NULL;
}

