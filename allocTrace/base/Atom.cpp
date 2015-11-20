#include "stdafx.h"
#include "Platform.h"

int lock_set(volatile int &a, int value)
{
	return InterlockedExchange((long*)&a, value);
}

int lock_comp_set(volatile int &a, int oldvalue, int value)
{
	if (InterlockedCompareExchange((long *)&a, value, oldvalue) == oldvalue)
		return value;
	else return oldvalue;
}

int lock_inc(volatile int &n)
{
	return InterlockedIncrement((long*)&n);
}

int lock_dec(volatile int &n)
{
	return InterlockedDecrement((long*)&n);
}

int lock_add(volatile int &n, int value)
{
	return InterlockedExchangeAdd((long*)&n, value) + value;
}

int lock_sub(volatile int &n, int value)
{
	return InterlockedExchangeAdd((long*)&n, -value) - value;
}

int lock_or(volatile int &n, int value)
{
	return _InterlockedOr((long*)&n, value);
}

int lock_and(volatile int &n, int value)
{
	return _InterlockedAnd((long*)&n, value);
}

int lock_xor(volatile int &n, int value)
{
	return _InterlockedXor((long*)&n, value);
}


unsigned int lock_set(volatile unsigned int &a, unsigned int value)
{
	return InterlockedExchange(&a, value);
}

unsigned int lock_comp_set(volatile unsigned int &a, unsigned int oldvalue, unsigned int value)
{
	if (InterlockedCompareExchange((long *)&a, value, oldvalue) == oldvalue)
		return value;
	else return oldvalue;
}

unsigned int lock_inc(volatile unsigned int &n)
{
	return InterlockedIncrement(&n);
}

unsigned int lock_dec(volatile unsigned int &n)
{
	return InterlockedDecrement(&n);
}

unsigned int lock_add(volatile unsigned int &n, unsigned int value)
{
	return InterlockedExchangeAdd(&n, value) + value;
}

unsigned int lock_sub(volatile unsigned int &n, unsigned int value)
{
	return InterlockedExchangeAdd(&n, -(int)value) - value;
}

unsigned int lock_or(volatile unsigned int &n, unsigned int value)
{
	return _InterlockedOr((long*)&n, value);
}

unsigned int lock_and(volatile unsigned int &n, unsigned int value)
{
	return _InterlockedAnd((long*)&n, value);
}

unsigned int lock_xor(volatile unsigned int &n, unsigned int value)
{
	return _InterlockedXor((long*)&n, value);
}
