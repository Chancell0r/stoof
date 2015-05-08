/******************************************************************************
file		: string_funcs.h
author		: nc
desc		: k_t struct
date		: 03-28-13
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "global_defs.h"
#include "string_funcs.h"


/******************************************************************************
function	: void strip_nr( char* string, unsigned len )
author		: nc
desc		: strip \n\r chars from a line
date		: 03-28-13
 ******************************************************************************/
void strip_nr( char* string, unsigned len )
{
	unsigned i			= 0;
	unsigned j			= 0;
	char local[MAX_TMP] = { 0 };

	/*not gonna have n/r anyway*/
	if( !string || len > MAX_TMP || len == 0 )
		return;

	for( i = 0; i < len; i ++ )
	{
		while( string[i] == '\n' || string[i] == '\r' )
			i++;

		if( i > len || !string[i] )
			break;

		local[i] = string[j];

		j++;
	}

	/*copy it back over*/
	memset( string, 0, len );
	strcpy( string, local );
	string[strlen(string)] = '\0';

	return;

}


/******************************************************************************
function	: void split_path( char* path, char** ar, int* num )
author		: nc
desc		: if you call this you need to free the destination array
date		: 
 ******************************************************************************/
bool split_path( char* path, unsigned ar_size, char** dst, int* num )
{
	int i				= 0;
	char tmp[MAX_PATH]	= { '\0' };
	char* tok			= NULL;

	if( !path || !dst || !num || strlen( path ) > MAX_PATH - 1 || ar_size > MAX_DEPTH )
		goto fail;

	strcpy( tmp, path );

	/*clear everything in the destination array*/
	for( i = 0; i < ar_size; i ++ )
	{
		if( dst[i] )
		{
			free( dst[i] );
			dst[i] = NULL;
		}
	}

	i = 0;

	/*tokenize the path*/
	tok = strtok( tmp, DIR_TOK );

	while( tok )
	{
		dst[i] = malloc( strlen( tok ) + 1 );

		if( !dst[i] )
			goto fail;

		memset( dst[i], 0, strlen( tok ) + 1 );
		strcpy( dst[i], tok );

		i++;

		tok = strtok( NULL, DIR_TOK );
	}

	/*set the number of path elements*/
	*num = i;

	return true;;

fail:

	/*try to clean up*/
	for( i = 0; i < ar_size; i ++ )
	{
		if( dst[i] )
		{
			free( dst[i] );
			dst[i] = NULL;
		}
	}

	return false;
}


/******************************************************************************
function	: char* loc_dirname( char* path )
author		: nc
desc		: 
date		: modified version of bsd dirname()
 ******************************************************************************/
char* loc_dirname( char* path_in )
{
	static char result[MAX_PATH] = { '\0' };
	char path[MAX_PATH] = { '\0' };
	char* end		= NULL;
	unsigned len	= 0;

	/*don't want to modify input*/
	if( strlen( path_in ) + 1 > MAX_PATH )
		return NULL;

	memcpy( path, path_in, strlen( path_in ) );

	/*check for cwd*/
	if( ( path == NULL ) || ( *path == '\0' ) )
		goto cwd;

	/*remove trailing slashes*/
	end = path + strlen( path ) - 1;
	while( end != path && *end == '/' )
		end--;

	while( --end >= path )
	{
		if( *end == '/' )
		{
			/*remove trailing slashes again*/			
			while( end != path && *end == '/' )
				end--;

			/*copy result*/
			len = (end - path) + 1;
			if( len > ( MAX_PATH - 1 ) )
				len = MAX_PATH - 1;

			memcpy( result, path, len );
			result[len] = '\0';

			return result;
		}
	}

cwd:
	result[0] = '.';
	result[1] = '\0';

	return result;
}


/******************************************************************************
function	: char* loc_dirname( char* path )
author		: nc
desc		: 
date		: 03-30-13
 ******************************************************************************/
char* loc_basename( char* path_in )
{
	static char bname[MAX_PATH] = { '\0' };
	char path[MAX_PATH]			= { '\0' };
	char* end		= NULL;
	char* start		= NULL;

	/*don't want to modify input*/
	if( strlen( path_in ) + 1 > MAX_PATH )
		return NULL;

	memcpy( path, path_in, strlen( path_in ) );

	/*check for cwd*/
	if( !path_in || *path == '\0')
	{
		strcpy( bname, "." );
		return bname;
	}

	/*remove trailing slashes*/
	end = path + strlen( path ) - 1;

	while( end > path && *end == '/' )
		end--;

	/*root dir*/
	if( end == path && *end == '/' )
	{
		strcpy( bname, "/" );
		return bname;
	}

	/*find the start of the base*/ 
	start = end;
	while( start > path && *( start - 1 ) != '/')
		start--;

	if( end - start + 2 > MAX_PATH )
		return NULL;

	strncpy( bname, start, end - start + 1 );
	bname[end - start + 1] = '\0';
	return bname;
}



/******************************************************************************
function	: bool is_abs_path( char* path )
author		: nc
desc		: check and see if a path is absolute or not
date		: 03-30-13
 ******************************************************************************/
bool is_abs_path( char* path )
{
	if( !path || path[0] != '/' )
		return false;

	if( path[0] == '/' )
		return true;

	/*???*/
	return false;
}

