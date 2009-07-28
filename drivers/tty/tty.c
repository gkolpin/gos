#include "tty.h"
#include "gos.h"
#include "types.h"
#include "circ_buf.h"
#include "wait.h"
#include "kmalloc.h"
#include "devfs.h"
#include "keyboard.h"
#include "console.h"
#include "rs232.h"

#define BUF_SIZE 2048
#define IBUF_SIZE 128
#define MAX_VTERMS 8
#define SYS_TERM 0

typedef struct _tty_dev {
  int nbytes;			/* bytes in the term buffer */
  int start;			/* location of start of buffer  */
  char buf[BUF_SIZE];		/* circular term buffer */
  
  circ_buf ibuf;		/* circular input buffer */

  /* terminal options */
  bool echo;
  /* waitpoint for blocked reads  */
  waitpoint wp_read_block;
} tty_dev;

PRIVATE tty_dev *cons_bufs;	/* array of vterms */
PRIVATE int cur_term;
PRIVATE bool initted = FALSE;

PRIVATE void putchar(char c, int term);

PRIVATE int init(void *dev_data){
  return 0;
}

PRIVATE int read(void *buf, uint32 num_bytes, void *dev_data){
  int term_no = (int)dev_data;
  tty_dev *t = &cons_bufs[term_no];
  size_t bytes;
  while (!(bytes = cbuf_read(t->ibuf, buf, num_bytes))){
    waitpoint_wait(t->wp_read_block);
  }
  return bytes;
}

PRIVATE int write(void *buf, uint32 num_bytes, void *dev_data){
  int term_no = (int)dev_data;
  int i;
  for (i = 0; i < num_bytes; i++){
    putchar(((char*)buf)[i], term_no);
  }
  return 0;
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
    vt->ibuf = circ_buf_init(IBUF_SIZE);
    vt->echo = TRUE;
    vt->wp_read_block = waitpoint_create(tty_dev, wp_read_block);
  }
  cur_term = SYS_TERM;

  register_device(d, "ttyv0", (void*)0);
  register_device(d, "ttyv1", (void*)1);

  initted = TRUE;
  return d;
}

void kb_int(void){
  char c = getchar();
  if (cons_bufs[cur_term].echo)
    putchar(c, cur_term);
  tty_dev *t = &cons_bufs[cur_term];
  cbuf_write(t->ibuf, &c, sizeof(char));
  waitpoint_wake_all(t->wp_read_block);
}

void kputchar(char c){
  putchar(c, SYS_TERM);
  serial_putchar(c);
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
