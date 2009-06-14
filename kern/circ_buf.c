#include "circ_buf.h"
#include "gos.h"
#include "types.h"
#include "kmalloc.h"

struct _circ_buf {
  void *buf;
  void *start, *end;
  size_t fill_count;
  size_t buf_size;
};

circ_buf circ_buf_init(size_t size){
  circ_buf retVal = (circ_buf)kmalloc(sizeof(struct _circ_buf));
  retVal->buf = kmalloc(size);
  retVal->start = retVal->end = retVal->buf;
  retVal->fill_count = 0;
  retVal->buf_size = size;
  return retVal;
}

PRIVATE void write_bytes(circ_buf cbuf, void *buf, size_t size){
  size_t write_size;
  if ((uintptr_t)cbuf->end + size < (uintptr_t)cbuf->start + cbuf->buf_size){
    /* enough room to do one memcpy */
    kmemcpy(cbuf->end, buf, size);
    cbuf->end = (void*)((uintptr_t)cbuf->end + size);
  } else {
    /* need 2 copies */
    write_size = (uintptr_t)cbuf->buf + cbuf->fill_count - (uintptr_t)cbuf->end;
    kmemcpy(cbuf->end, buf, write_size);
    cbuf->end = cbuf->buf;
    buf = (void*)((uintptr_t)buf + write_size);
    kmemcpy(cbuf->end, buf, size - write_size);
    cbuf->end = (void*)((uintptr_t)cbuf->end + size - write_size);
  }
  cbuf->fill_count += size;
}

size_t cbuf_write(circ_buf cbuf, void *buf, size_t size){
  if (cbuf->buf_size - cbuf->fill_count > size){
    /* all bytes can be written */
    write_bytes(cbuf, buf, size);
  } else {
    size = cbuf->buf_size - cbuf->fill_count;
    write_bytes(cbuf, buf, size);
  }
  return size;
}

PRIVATE void read_bytes(circ_buf cbuf, void *buf, size_t size){
  size_t read_size;
  if ((cbuf->end > cbuf->start && cbuf->fill_count > 0) || cbuf->fill_count == 0){
    /* one memcpy required */
    kmemcpy(buf, cbuf->start, size);
    cbuf->start = (void*)((uintptr_t)cbuf->start + size);
  } else {
    read_size = (uintptr_t)cbuf->buf + (uintptr_t)cbuf->buf_size - 
      (uintptr_t)cbuf->start;
    kmemcpy(buf, cbuf->start, read_size);
    cbuf->start = cbuf->buf;
    kmemcpy(buf, cbuf->start, size - read_size);
    cbuf->start = (void*)((uintptr_t)cbuf->buf + size - read_size);
  }
  cbuf->fill_count -= size;
}

size_t cbuf_read(circ_buf cbuf, void *buf, size_t size){
  if (cbuf->fill_count >= size){
    /* all of size bytes can be read */
    read_bytes(cbuf, buf, size);
  } else {
    size = cbuf->fill_count;
    read_bytes(cbuf, buf, size);
  }
  return size;
}
