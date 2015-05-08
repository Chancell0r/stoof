/******************************************************************************
file		: stat.h
author		: jk
desc		: 
date		: 04-20-13
 ******************************************************************************/
#pragma once

#include <sys/stat.h>
#include "../global_defs.h"
#include "../proc_t.h"

int do_stat( char* pathname, struct stat* stPtr, proc_t* pr );
int stat_cmd( proc_t*, int argc, char** argv );

