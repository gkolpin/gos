#include "gos.h"
#include "kprintf.h"
#include "console.h"
#include "types.h"
#include "stdarg.h"

PRIVATE void kprint_int(uint32 n, int base);
PRIVATE char get_char_for_base(uint32, int base);

void kprintf(const char* fmt, ...){
  int i;
  va_list ap;
  

  va_start(ap, fmt);

  for (i = 0; fmt[i] != '\0'; i++){
    if (fmt[i] == '%'){

      switch (fmt[++i]){
      case 'd':
	kprint_int(va_arg(ap, int), 10);
	break;
      case 'u':
	kprint_int(va_arg(ap, uint32), 10);
	break;
      case 'x':
	kprintf("0x");
	kprint_int(va_arg(ap, int), 16);
	break;
      default:
	cons_putchar(fmt[i]);
	break;
      }
    }
    else {
      cons_putchar(fmt[i]);
    }
  }
}

PRIVATE void kprint_int(uint32 n, int base){
  if (n <= (base - 1)){
    cons_putchar(get_char_for_base(n, base));
    return;
  }

  kprint_int(n / base, base);

  cons_putchar(get_char_for_base(n % base, base));
}

PRIVATE char get_char_for_base(uint32 n, int base){
  if (n <= 9){
    return '0' + n;
  } else {
    return 'A' + n - 10;
  }
}

/*int kstrlen(const char* string){
  int i;
  
  for (i = 0; TRUE; i++){
    if (string[i] == '\0'){
      return i;
    }
  }
  }*/
