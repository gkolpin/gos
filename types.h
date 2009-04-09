#ifndef _TYPES_H
#define _TYPES_H

#define BITS_PER_BYTE 8
#define BITS_PER_UINT32 32

typedef unsigned char 	uchar;
typedef unsigned int 	uint;
typedef unsigned short	ushort;
typedef uint		uint32;
typedef ushort		uint16;
typedef uchar		uint8;
typedef uint32		reg_t;
typedef uint16		word_t;
typedef uint32		dword_t;
typedef uint8		byte_t;

typedef int		int32_t;

typedef unsigned long	uintptr_t;
typedef unsigned long	size_t;

#define SIZE_T_MAX (~((size_t)0))

typedef union qword_t {
  word_t words[4];
  dword_t dwords[2];
  byte_t bytes[8];
} qword_t;

typedef enum {FALSE=0, TRUE} bool;

#endif
