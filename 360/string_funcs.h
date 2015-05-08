/******************************************************************************
file		: string_funcs.h
author		: nc
desc		: misc string functions
date		: 03-28-13
 ******************************************************************************/

void	strip_nr( char*, unsigned );
bool	split_path( char* path, unsigned ar_size, char** dest, int* num );
char*	loc_dirname( char* );
char*	loc_basename( char* );
bool	is_abs_path( char* );
