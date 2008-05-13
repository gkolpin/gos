#ifndef _TYPES_H
#define _TYPES_H

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

typedef union qword_t {
  word_t words[4];
  dword_t dwords[2];
  byte_t bytes[8];
} qword_t;

typedef enum {FALSE=0, TRUE} bool;

#endif
