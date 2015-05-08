/******************************************************************************
file		: func_addresses.h
author		: nc
desc		: adrresses for function pointers
date		: 04-07-13
 ******************************************************************************/

#pragma once

#include "l0/show_help.h"
#include "l0/test_func.h"
#include "l0/print_minos.h"
#include "l0/print_vfs_mounts.h"
#include "l0/show_imap.h"
#include "l0/show_bmap.h"
#include "l0/print_ino.h"
#include "l0/print_procs.h"
#include "l0/switch.h"

#include "l1/ls.h"
#include "l1/cd.h"
#include "l1/creat.h"
#include "l1/mkdir.h"
#include "l1/pwd.h"
#include "l1/link.h"
#include "l1/unlink.h"
#include "l1/rmdir.h"
#include "l1/symblink.h"
#include "l1/stat.h"
#include "l1/touch.h"
#include "l1/chown.h"
#include "l1/chmod.h"
#include "l1/chgrp.h"

#include "l2/open.h"
#include "l2/close.h"
#include "l2/lseek.h"
#include "l2/pfd.h"
#include "l2/read.h"
#include "l2/cat.h"
#include "l2/write.h"
#include "l2/cp.h"
#include "l2/mv.h"

#include "global_defs.h"


typedef int (*fp_tb)( proc_t*, int, char** );

fp_tb get_fp( char* );


