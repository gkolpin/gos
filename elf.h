#ifndef _ELF_H
#define _ELF_H

#include "types.h"

typedef addr32 	Elf32_Addr;
typedef uint16 	Elf32_Half;
typedef uint32 	Elf32_Off;
typedef int32_t	Elf32_Sword;
typedef uint32	Elf32_Word;

#define EI_NIDENT	16

typedef struct {
  uchar 	e_ident[EI_NIDENT];
  Elf32_Half	e_type;
  Elf32_Half	e_machine;
  Elf32_Word	e_version;
  Elf32_Addr	e_entry;
  Elf32_Off	e_phoff;
  Elf32_Off	e_shoff;
  Elf32_Word	e_flags;
  Elf32_Half	e_ehsize;
  Elf32_Half	e_phentsize;
  Elf32_Half	e_phnum;
  Elf32_Half	e_shentsize;
  Elf32_Half	e_shnum;
  Elf32_Half 	e_shstrndx;
} Elf32_Ehdr;

/* object file types */
enum {
  ET_NONE = 0,
  ET_REL,
  ET_EXEC,
  ET_DYN,
  ET_CORE,
  ET_LOPROC = 0xff00,
  ET_HIPROC = 0xffff
};

/* file architecture */
enum {
  EM_NONE, 			/* No machine */
  EM_M32,			/* AT&t WE 32100 */
  EM_SPARC,			/* SPARC */
  EM_386,			/* Intel 80386 */
  EM_68K,			/* Motorola 68000 */
  EM_88K,			/* Motorola 88000 */
  EM_860,			/* Intel 80860 */
  EM_MIPS			/* MIPS RS3000 */
};

/* object file version */
enum {
  EV_NONE,			/* Invalid version */
  EV_CURRENT			/* current version */
};

/* identification indexes */
enum {
  EI_MAG0,			/* File identification */
  EI_MAG1,
  EI_MAG2,
  EI_MAG3,
  EI_CLASS,			/* File class */
  EI_DATA,			/* Data encoding */
  EI_VERSION,			/* File version */
  EI_PAD			/* Start of padding bytes */
};

/* file class */
enum {
  ELFCLASSNONE,			/* invalid class */
  ELFCLASS32,			/* 32-bit objects */
  ELFCLASS64			/* 64-bit objects */
};

/* data encodings */
enum {
  ELFDATANONE,			/* Invalid data encoding */
  ELFDATA2LSB,			/* little endian */
  ELFDATA2MSB			/* big endian */
};

typedef struct {
  Elf32_Word	sh_name;
  Elf32_Word	sh_type;
  Elf32_Word	sh_flags;
  Elf32_Addr	sh_addr;
  Elf32_Off	sh_offset;
  Elf32_Word	sh_size;
  Elf32_Word	sh_link;
  Elf32_Word	sh_info;
  Elf32_Word	sh_addralign;
  Elf32_Word	sh_entsize;
} Elf32_Shdr;

/* section types - sh_type */
enum {
  SHT_NULL,
  SHT_PROGBITS,
  SHT_SYMTAB,
  SHT_STRTAB,
  SHT_RELA,
  SHT_HASH,
  SHT_DYNAMIC,
  SHT_NOTE,
  SHT_NOBITS,
  SHT_REL,
  SHT_SHLIB,
  SHT_DYNSYM,
  SHT_LOPROC = 0x70000000,
  SHT_HIPROC = 0x7fffffff,
  SHT_LOUSER = 0x80000000,
  SHT_HIUSER = 0xffffffff
};

/* section attribute flags - sh_flags */
enum {
  SHF_WRITE = 0x1,
  SHF_ALLOC = 0x2,
  SHF_EXECINSTR = 0x4,
  SHF_MASKPROC = 0xf0000000
};

/* symbol table entry */
typedef struct {
  Elf32_Word 	st_name;
  Elf32_Addr	st_value;
  Elf32_Word	st_size;
  uchar		st_info;
  uchar		st_other;
  Elf32_Half	st_shndx;
} Elf32_Sym;

#define ELF32_ST_BIND(i)	((i)>>4)
#define ELF32_ST_TYPE(i)	((i) & 0xf)
#define ELF32_ST_INFO(b, t)	(((b)<<4) + ((t) & 0xf))

/* symbol binding */
#define STB_LOCAL	0
#define STB_GLOBAL	1
#define STB_WEAK	2
#define STB_LOPROC	13
#define STB_HIPROC	15

/* symbol types */
#define STT_NOTYPE	0
#define STT_OBJECT	1
#define STT_FUNC	2
#define STT_SECTION	3
#define STT_FILE	4
#define STT_LOPROC	13
#define STT_HIPROC	15

typedef struct {
  Elf32_Addr	r_offset;
  Elf32_Word	r_info;
} Elf32_Rel;

typedef struct {
  Elf32_Addr	r_offset;
  Elf32_Word	r_info;
  Elf32_Sword	r_addend;
} Elf32_Rela;

#define ELF32_R_SYM(i)	((i)>>8)
#define ELF32_R_TYPE(i)	((uchar)(i))
#define ELF32_R_INFO(s, t)	(((s)<<8)+(uchar)(t))

/* relocation types */
enum {
  R_386_NONE,
  R_386_32,
  R_386_PC32,
  R_386_GOT32,
  R_386_PLT32,
  R_386_COPY,
  R_386_GLOB_DAT,
  R_386_JMP_SLOT,
  R_386_RELATIVE,
  R_386_GOTOFF,
  R_386_GOTPC
};

/* Program header */
typedef struct {
  Elf32_Word	p_type;
  Elf32_Off	p_offset;
  Elf32_Addr	p_vaddr;
  Elf32_Addr	p_paddr;
  Elf32_Word	p_filesz;
  Elf32_Word	p_memsz;
  Elf32_Word	p_flags;
  Elf32_Word	p_align;
} Elf32_Phdr;

/* Segment types - p_type */
enum {
  PT_NULL,
  PT_LOAD,
  PT_DYNAMIC,
  PT_INTERP,
  PT_NOTE,
  PT_SHLIB,
  PT_PHDR,
  PT_LOPROC = 0x70000000,
  PT_HIPROC = 0x7fffffff
};

#endif
