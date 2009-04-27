#include "devices.h"
#include "gos.h"

void init_devices(){
  cons_init();
  kprintf("cons_initted\n");
  kbd_init();
  kprintf("kbd_initted\n");
}
