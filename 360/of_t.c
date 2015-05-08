/******************************************************************************
file		: of_t.c
author		: nc
desc		: implementation of the of_t functions
date		: 03-24-13
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "global_defs.h"
#include "of_t.h"


/******************************************************************************
function	: 
author		: nc
desc		: 
date		: 03-24-13
 ******************************************************************************/
bool of_t_init( of_t** ft )
{
	*ft = malloc( sizeof(**ft) );

	if( !*ft )
		goto fail;

	memset( *ft, 0, sizeof(**ft) );

	return true;

fail:
	return false;
}


/******************************************************************************
function	: 
author		: nc
desc		: 
date		: 03-24-13
 ******************************************************************************/
void of_t_destroy( of_t** ft )
{
	if( *ft )
		free( *ft );

	*ft = NULL;

	return;
}



