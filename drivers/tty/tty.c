#include "tty.h"
#include "gos.h"
#include "types.h"

#define BUF_SIZE 2048
#define IBUF_SIZE 128
#define MAX_VTERMS 8
#define SYS_TERM 0

typedef struct _tty_dev {
  int nbytes;			/* bytes in the term buffer */
  int start;			/* location of start of buffer  */
  char buf[BUF_SIZE];		/* circular term buffer */
  char ibuf[IBUF_SIZE];		/* circular input buffer */
} tty_dev;

PRIVATE tty_dev *cons_bufs;	/* array of vterms */
PRIVATE int cur_term;
PRIVATE bool initted = FALSE;

PRIVATE void putchar(char c, int term);

PRIVATE int init(void *dev_data){
  
}

PRIVATE int read(void *buf, uint32 num_bytes, void *dev_data){
  
}

PRIVATE int write(void *buf, uint32 num_bytes, void *dev_data){
  int term_no = (int)dev_data;
  int i;
  for (i = 0; i < num_bytes; i++){
    putchar(((char*)buf)[i], term_no);
  }
}

driver * tty_init(){
  int i;
  tty_dev *vt;
  driver *d = (driver*)kmalloc(sizeof(driver));
  d->init = init;
  d->read = read;
  d->write = write;

  /* initialize MAX_VTERMS virtual terminals */
  cons_bufs = (tty_dev*)kmalloc(sizeof(tty_dev) * MAX_VTERMS);
  for (i = 0; i < MAX_VTERMS; i++){
    vt = &cons_bufs[i];
    vt->nbytes = 0;
    vt->start = 0;
  }
  cur_term = SYS_TERM;

  register_device(d, "ttyv0", (void*)0);
  register_device(d, "ttyv1", (void*)1);

  initted = TRUE;
  return d;
}

void kb_int(void){
  putchar(getchar(), cur_term);
}

void kputchar(char c){
  putchar(c, SYS_TERM);
}

PRIVATE void putchar(char c, int term){
  tty_dev *tty;
  if (!initted){
    cons_putchar(c);
    return;
  }

  if (cur_term == term){
    cons_putchar(c);
  }

  tty = &cons_bufs[term];
  if (tty->nbytes < BUF_SIZE){
    tty->buf[tty->nbytes] = c;
    tty->nbytes++;
  } else {
    tty->buf[tty->start] = c;
    tty->start = (tty->start + 1) % BUF_SIZE;
  }
}
