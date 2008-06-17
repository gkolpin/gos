#include "gos.h"
#include "kprintf.h"
#include "console.h"
#include "types.h"
#include "stdarg.h"

PRIVATE void kprint_int(int n, int base);

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

PRIVATE void kprint_int(int n, int base){
  if (n <= (base - 1)){
    cons_putchar('0' + n);
    return;
  }

  kprint_int(n / base, base);

  cons_putchar('0' + (n % base));
}

/*int kstrlen(const char* string){
  int i;
  
  for (i = 0; TRUE; i++){
    if (string[i] == '\0'){
      return i;
    }
  }
  }*/
