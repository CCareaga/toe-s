#ifndef __SYSTEM_H
#define __SYSTEM_H

typedef unsigned int   u32int;
typedef          int   s32int;
typedef unsigned short u16int;
typedef          short s16int;
typedef unsigned char  u8int;
typedef          char  s8int;

extern u8int inport(u16int _port);
extern void outport(u16int _port, u8int _data);
extern void bochs_break();

char convert_hex(u8int);

#endif