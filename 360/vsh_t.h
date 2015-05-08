/******************************************************************************
file		: vsh_t.h
author		: nc
desc		: vsh_t struct
date		: 03-25-13
 ******************************************************************************/

#pragma once

#include "global_defs.h"
#include "proc_t.h"


/*defines*/
#define	MAX_LINE	1024	//maximum number of input chars
#define MAX_ARGS	16		//maximum number of different args
#define TOK_STR		" "		//token for strtok

#define QUIT_STR	"q"		//exit command

/*fw dec*/
typedef struct vsh_t vsh_t;


/*struct*/
struct vsh_t
{	
	char	m_line[MAX_LINE];			//input line
	char*	m_argv[MAX_ARGS];			//arguments passed in
	int		m_argc;						//number of args in m_argv
	proc_t* m_proc;
	int		(*m_ftx)( proc_t*, int, char** );
};

bool	vsh_t_init( vsh_t** );			//initialize
void	vsh_t_destroy( vsh_t** );		//shutdown
int		vsh_t_run( vsh_t*, proc_t* );	//main loop
int		vsh_t_get_cmd( vsh_t* );		//get a command from the user
int		vsh_t_run_cmd( vsh_t* );		//run the command

