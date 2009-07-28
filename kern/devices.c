#include "gos.h"
#include "devices.h"
#include "tty.h"
#include "kprintf.h"
#include "keyboard.h"

void init_devices(){
  tty_init();
  kprintf("tty_initted\n");
  kbd_init();
  kprintf("kbd_initted\n");
}
