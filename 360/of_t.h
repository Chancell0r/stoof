/******************************************************************************
file		: of_t.h
author		: nc
desc		: of_t struct
date		: 03-24-13
 ******************************************************************************/

#pragma once

#include "global_defs.h"
#include "mino_t.h"


/*fw dec*/
typedef struct of_t of_t;


/*struct*/
struct of_t
{
	E_FILE_MODE		m_mode;
	i32				m_refc;
	mino_t*			m_minoptr;
	i64				m_off;
	char			m_name[MAX_NAME];
};

bool	of_t_init( of_t** );
void	of_t_destroy( of_t** );
    
