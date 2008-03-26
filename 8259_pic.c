#include "8259_pic.h"
#include "portio.h"

void pic_init(){
  /* initialize both sets of irqs for remapping */
  outb(0x20, 0x11); 		/* init command 1 - cascade, edge triggered */
  outb(0x21, 0x20); 		/* init command 2 - redirect irqs 0-7 to vector 32 */
  outb(0x21, 0x4);		/* init command 3 - slave on irq 2 */
  outb(0x21, 0x1);		/* init command 4 - nonbuffered - 80x86 mode */

  outb(0xA0, 0x11);		/* see above */
  outb(0xA1, 0x28);		/* init command 2 - redirect irqs 8 - 15 to vector 40 */
  outb(0xA1, 0x2);		/* init command 3 */
  outb(0xA1, 0x1);		/* init command 4 - 80x86 mode */
}
