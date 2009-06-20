#ifndef _WAIT_H
#define _WAIT_H

#include "list.h"

typedef list waitpoint;
typedef list_node waitlink;

#define waitpoint_create(type, member) ((waitpoint)list_init(type, member))

void waitpoint_wait(waitpoint);
void waitpoint_wake_all(waitpoint);

#endif
