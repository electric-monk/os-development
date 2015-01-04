#ifndef __TOOLS_H__
#define __TOOLS_H__

#ifndef NULL
#define NULL 0L
#endif

#include <stdarg.h>
#include "runtime.h"

// sprintf-like functionality
class CStringPrint
{
public:
	CStringPrint();
	virtual ~CStringPrint();

	void Print(const char *format, ...);
	void PrintList(const char *format, va_list &params);

	static bool IsDigit(char c);
	static int LengthOf(const char *s);

protected:
	virtual void PrintOut(const char *data, int length) = 0;

private:
	int ParseFormat(va_list &params, const char *format);
};

// stuff for IO

static __inline__ void outb(unsigned short port, unsigned char val)
{
   asm volatile("outb %0,%1"::"a"(val), "Nd" (port));
}

static __inline__ void outw(unsigned short port, unsigned short val)
{
   asm volatile("outw %0,%1"::"a"(val), "Nd" (port));
}

static __inline__ void outl(unsigned short port, unsigned int val)
{
   asm volatile("outl %0,%1"::"a"(val), "Nd" (port));
}

static __inline__ unsigned char inb(unsigned short port)
{
   unsigned char ret;
   asm volatile ("inb %1,%0":"=a"(ret):"Nd"(port));
   return ret;
}

static __inline__ unsigned short inw(unsigned short port)
{
   unsigned short ret;
   asm volatile ("inw %1,%0":"=a"(ret):"Nd"(port));
   return ret;
}

static __inline__ unsigned int inl(unsigned short port)
{
   unsigned int ret;
   asm volatile ("inl %1,%0":"=a"(ret):"Nd"(port));
   return ret;
}

/*
static __inline__ void io_wait(void)
{
   asm volatile("jmp 1f;1:jmp 1f;1:");
}
*/

static __inline__ void io_wait(void)
{
   asm volatile("outb %%al, $0x80" : : "a"(0));
   // port 0x80 is used for 'checkpoints' during POST.
   // linux kernel seems to think it's free for use :-/
}

extern "C" {
    bool AtomicIncrement(UInt32 *value);    // Returns true if result is non-zero
    bool AtomicDecrement(UInt32 *value);    // Returns true if result is non-zero
};

#endif // __TOOLS_H__
