#include "devices.h"
#include "gos.h"

void init_devices(){
  tty_init();
  kprintf("tty_initted\n");
  kbd_init();
  kprintf("kbd_initted\n");
}
