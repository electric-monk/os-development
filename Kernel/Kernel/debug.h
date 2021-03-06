#ifndef __DEBUG_H__
#define __DEBUG_H__

#define DEBUG

class KernelObject;

void kcls(void);
void kprintf(const char *format, ...);
void test(int x, int y, char c);
void report(KernelObject *object);

#ifdef DEBUG
#define ASSERT(x)			if (!(x)) kprintf("ASSERTION FAILURE (%s:%i): %s\n", __FILE__, __LINE__, #x)
#define FAILURE(x)			kprintf("FAILURE (%s:%i): %s\n", __FILE__, __LINE__, x)
#else // DEBUG
#define ASSERT(x)
#define FAILURE(x)
#endif // DEBUG

#endif // __DEBUG_H__
