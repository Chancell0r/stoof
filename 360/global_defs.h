/******************************************************************************
file		: global_defs.h
author		: nc
desc		: system-wide
date		: 03-24-13
 ******************************************************************************/

#pragma once

/*includes*/
#include <stdbool.h>
#include <stdint.h>

/*defines*/
#define false			0		//we use these a lot
#define true			1
#define NUM_FDS			128		//max number of open file descriptors
#define MAX_NAME		256		//max number of chars in a name
#define MAX_TMP			1024	//max size of tmp arrays, mostly used in string funcs
#define MAX_PATH		1024	//max size of the path
#define MAX_DEPTH		256		//how deep the file tree can be
#define DBLOCK			1440

#define BLK_SZ			1024	//size of a block in bytes
#define IE_SZ			512		//inode entry size
#define SB_BLK			1		//superblock offset
#define GD0_BLK			2		//group descriptor 0
#define EXT2_MAGIC		0xef53
#define INO_PER_BLK		8		//inodes in a block
#define ID_PER_BLK		256		//number of indirect inodes in a block
#define INO_SIZE		128		//size of inode in bytes
#define FILE_TYPE		1
#define DIR_TYPE		2

#define ROOT_INO_NUM	2
#define MAX_DIRECT		12		//offsets in the inode for block levels
#define	ONE_INDIR		13
#define TWO_INDIR		14
#define THREE_INDIR		15
#define MAX_ENT			15
#define MAX_DBLOCKS		65805	//max number of double indirect
#define DIR_RANGE		12		//some more data block range calulators
#define OID_RANGE		268
#define TID_RANGE		65548

#define DIR_TOK			"/"		

#define DEF_DMODE		0x41ed	//mode types for files and directories
#define DEF_FMODE		0x81a4
#define DEF_LMODE		0xa000
#define DEF_DLINKS		2

/*process status*/
#define S_STOPPED		0
#define S_RUNNING		1
#define	S_KILLED		2

/*user ids*/
#define U_SUPER			0
#define U_ROOT			1
#define U_ADMIN			2
#define U_REG			3

/*exit codes*/
#define X_CRITICAL		-1		//we didn't use these as much as we intended
#define X_SUCCESS		0
#define X_NO_KERNEL		1
#define X_NO_SHELL		2
#define X_NO_MEMORY		3
#define X_DONE			4
#define X_NO_ROOT		5
#define X_NO_PROCS		6
#define X_NO_FTABLE		7
#define X_UNKNOWN		255

/*this is the mode that the file is opened in*/
typedef enum
{
	M_READ,
	M_WRITE,
	M_READWRITE,
	M_APPEND
} E_FILE_MODE;


/*typedefs*/
typedef struct ext2_group_desc		gd;
typedef struct ext2_super_block		sb;
typedef struct ext2_inode			inode;
typedef struct ext2_dir_entry_2		ext2_dir;

typedef int8_t		i8;
typedef int16_t		i16;
typedef int32_t		i32;
typedef int64_t		i64;
typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;



