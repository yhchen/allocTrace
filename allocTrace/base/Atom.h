#ifndef __ALLOC_TRACE_ATOM_H__
#define __ALLOC_TRACE_ATOM_H__

//值原子锁定更改函数
int lock_set(volatile int &a, int value);
int lock_inc(volatile int &a);
int lock_dec(volatile int &n);
int lock_add(volatile int &n, int value);
int lock_sub(volatile int &n, int value);
int lock_or(volatile int &n, int value);
int lock_and(volatile int &n, int value);
int lock_xor(volatile int &n, int value);
unsigned int lock_set(volatile unsigned int &a, unsigned int value);
unsigned int lock_inc(volatile unsigned int &n);
unsigned int lock_dec(volatile unsigned int &n);
unsigned int lock_add(volatile unsigned int &n, unsigned int value);
unsigned int lock_sub(volatile unsigned int &n, unsigned int value);
unsigned int lock_or(volatile unsigned int &n, unsigned int value);
unsigned int lock_and(volatile unsigned int &n, unsigned int value);
unsigned int lock_xor(volatile unsigned int &n, unsigned int value);


#endif // __ALLOC_TRACE_ATOM_H__