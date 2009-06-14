#ifndef _CIRC_BUF_H
#define _CIRC_BUF_H

#include "types.h"

typedef struct _circ_buf *circ_buf;

circ_buf circ_buf_init(size_t size);
size_t cbuf_write(circ_buf, void*, size_t);
size_t cbuf_read(circ_buf, void*, size_t);

#endif
