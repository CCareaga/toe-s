#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "types.h"

extern u8int inport(u16int _port);
extern void outport(u16int _port, u8int _data);

#endif