/******************************************************************************
file		: vsh_t.c
author		: nc
desc		: implementation of the vsh_t functions
date		: 03-30-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "global_defs.h"
#include "vsh_t.h"
#include "string_funcs.h"
#include "func_addresses.h"
#include "k_t.h"

/*globals (remove*/
extern k_t* kr;



/******************************************************************************
function	: bool vsh_t_init( vsh_t** sh )
author		: nc
desc		: create the shell
date		: 03-24-13
 ******************************************************************************/
bool vsh_t_init( vsh_t** sh )
{
	*sh = malloc( sizeof(**sh) );

	if( !*sh )
		goto fail;

	memset( *sh, 0, sizeof(**sh) );

	return true;

fail:
	exit( X_NO_SHELL );
	return false;
}



/******************************************************************************
function	: void vsh_t_destroy( vsh_t** sh )
author		: nc
desc		: destroy the shell
date		: 03-30-13
 ******************************************************************************/
void vsh_t_destroy( vsh_t** sh )
{
	int i = 0;
	if( !*sh )
		return;

	/*free eveything in the arg array*/
	for( i = 0; i < MAX_ARGS; i++ )
	{
		if( (*sh)->m_argv[i] )
		{
			free( (*sh)->m_argv[i] );
			(*sh)->m_argv[i] = NULL;
		}
	}

	if( *sh )
		free( *sh );

	*sh = NULL;

	return;
}



/******************************************************************************
function	: int vsh_t_run( void )
author		: nc
desc		: main loop for the shell
date		: 03-30-13
 ******************************************************************************/
int vsh_t_run( vsh_t* sh, proc_t* pr )
{
	if( !sh || !pr )
		goto no_sh;
	
	sh->m_proc = pr;
	
	while( true )
	{
		vsh_t_get_cmd( sh );

		if( vsh_t_run_cmd( sh ) == X_DONE )
			break;

	}

	return X_SUCCESS;

no_sh:
	printf( "lost shell\n" );
	return X_NO_SHELL;

fail_unk:
	return X_UNKNOWN;

}



/******************************************************************************
function	: int vsh_t_get_cmd( void )
author		: nc
desc		: split a user command up
date		: 03-30-13
 ******************************************************************************/
int vsh_t_get_cmd( vsh_t* sh )
{
	int i				= 0;
	char tmp[MAX_LINE]	= { '\0' };
	char* tok			= NULL;

	if( !sh )
		goto no_sh;

	/*delete some stuff*/
	memset( sh->m_line, 0, MAX_LINE );

	for( i = 0; i < MAX_ARGS; i++ )
	{
		if( sh->m_argv[i] )
		{
			free( sh->m_argv[i] );
			sh->m_argv[i] = NULL;
		}
	}

	sh->m_ftx = NULL;

	sh->m_argc = 0;

	/*get some stuff*/
	if( kr->m_cproc )
	{
	   printf( "proc %i$ ", kr->m_cproc->m_pid );	
	}
	else
	{
		printf( "np$ " );
	}
	fgets( sh->m_line, MAX_LINE, stdin );

	strip_nr( sh->m_line, strlen( sh->m_line ) );
	memcpy( tmp, sh->m_line, MAX_LINE );

	/*find out the arg count*/
	i = 0;
	tok = strtok( tmp, TOK_STR );

	/*split up the line*/
	while( tok )
	{
		/*set up argv*/
		sh->m_argv[i] = malloc( strlen( tok ) + 1 );

		if( !sh->m_argv[i] )
			goto no_mem;

		memset( sh->m_argv[i], 0, strlen( tok ) + 1 );
		strcpy( sh->m_argv[i], tok );

		tok = strtok( NULL, TOK_STR );
		i++;
	}

	sh->m_argc = i;

	return X_SUCCESS;
no_sh:
	return X_NO_SHELL;
no_mem:
	return X_NO_MEMORY;
fail:
	return X_CRITICAL;

};



/******************************************************************************
function	: int vsh_t_run( vsh_t* sh )
author		: nc
desc		: run the command we got
date		: 03-30-13
 ******************************************************************************/
int vsh_t_run_cmd( vsh_t* sh )
{
	if( !sh )
		goto fail;

	/*first check for no command*/
	if( !sh->m_argc || !sh->m_argv[0] )
		goto succeed;

	/*check for exit*/
	if( !strcmp( sh->m_argv[0], QUIT_STR ) )
		goto done;

	/*otherwise try to execute a command*/
	sh->m_ftx = get_fp( sh->m_argv[0] );

	if( sh->m_ftx == NULL )
		goto not_found;

	sh->m_ftx( sh->m_proc, sh->m_argc, sh->m_argv );
	goto succeed;

not_found:
	printf( "command not found\n" );
	return X_SUCCESS;
succeed:
	return X_SUCCESS;
done:
	return X_DONE;
fail:
	return X_CRITICAL;
}






