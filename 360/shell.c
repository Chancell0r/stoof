/******************************************************************************
file		: shell.c
author		: nc
desc		: entry point
date		: 03-24-13
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "shell.h"
#include "proc_t.h"
#include "of_t.h"
#include "k_t.h"

k_t* kr = NULL;

int main( int argc, char** argv )
{
	int ret = 0;

	k_t_init( &kr );
	k_t_start();

	if( ret = k_t_run() )
	{
		/*got an error code*/
		exit( ret );
	}

	k_t_destroy( &kr );

	return 0;
}
	

	
