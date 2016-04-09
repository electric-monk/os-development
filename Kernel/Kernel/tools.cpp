#include "tools.h"
#include "debug.h"

#define HEXSTRING		"0123456789ABCDEF"

CStringPrint::CStringPrint()
{
	// Do nothing
}

CStringPrint::~CStringPrint()
{
	// Do nothing
}

void CStringPrint::Print(const char *format, ...)
{
	va_list vl;

	va_start(vl, format);
	PrintList(format, vl);
	va_end(vl);
}

void CStringPrint::PrintList(const char *format, va_list &params)
{
	int i;

	ASSERT(format != NULL);

	for (i = 0; format[i] != '\0';)
	{
		if (format[i] == '%')
		{
			i += ParseFormat(params, format + i + 1) + 1;
		}
		else
		{
			PrintOut(format + (i++), 1);
		}
	}
}

int CStringPrint::ParseFormat(va_list &params, const char *format)
{
	int index;
	bool added;
	// Flags
	int padding = 0;

	index = 0;
	added = false;
	do {
		switch (format[index])
		{
			// Modifiers
			case '.':
				index++;
				while (IsDigit(format[index]))
				{
					padding *= 10;
					padding += format[index] - '0';
					index++;
				}
				break;

			// Types
			case 'i':	// Decimal integer
				{
					int x = va_arg(params, int);
					char buf[32];
					int i;

					i = 32;
					if (x == 0)
						buf[--i] = '0';
					while (x != 0)
					{
						buf[--i] = HEXSTRING[x % 10];
						x /= 10;
					}
					padding -= 32 - i;
					while (padding > 0)
					{
						PrintOut("0", 1);
						padding--;
					}
					PrintOut(buf + i, 32 - i);
				}
				index++;
				added = true;
				break;
			case 'x':	// Hex integer
				{
					unsigned int x = va_arg(params, unsigned int);
					char buf[32];
					int i;

					i = 32;
					if (x == 0)
						buf[--i] = '0';
					while (x != 0)
					{
						buf[--i] = HEXSTRING[x & 0x0F];
						x >>= 4;
					}
					padding -= 32 - i;
					while (padding > 0)
					{
						PrintOut("0", 1);
						padding--;
					}
					PrintOut(buf + i, 32 - i);
				}
				index++;
				added = true;
				break;
			case 's':	// String
				{
					char *s = va_arg(params, char*);
                    if (s == NULL) {
                        s = "(null)";
                    } else {
                        int len = LengthOf(s);
                        padding -= len;
                        while (padding > 0)
                        {
                            PrintOut(" ", 1);
                            padding--;
                        }
                    }
					PrintOut(s, LengthOf(s));
				}
				index++;
				added = true;
				break;
			case '%':	// Plain '%'
				PrintOut("%", 1);
				index++;
				added = true;
				break;
            case 'c':   // Character
            {
                char c = (char)va_arg(params, int);
                PrintOut(&c, 1);
            }
                index++;
                added = true;
                break;

			// ..else
			default:
				PrintOut("%", 1);
				added = true;
				break;
		}
	} while (!added);
	return index;
}

bool CStringPrint::IsDigit(char c)
{
	return ((c >= '0') && (c <= '9'));
}

int CStringPrint::LengthOf(const char *s)
{
	int i;
	for (i = 0; s[i] != '\0'; i++);
	return i;
}

void ClearMemory(void *pointer, UInt32 length)
{
    char *buf = (char*)pointer;
    while (length) {
        buf[0] = 0;
        buf++;
        length--;
    }
}

void CopyMemory(void *output, const void *input, UInt32 length)
{
    const char *inBuf = (const char*)input;
    char *outBuf = (char*)output;
    while (length) {
        outBuf[0] = inBuf[0];
        inBuf++;
        outBuf++;
        length--;
    }
}

UInt32 StringLength(const char *input)
{
    UInt32 count = 0;
    while (*input) {
        count++;
        input++;
    }
    return count;
}

void CrashTest(CRASH_TYPE crash)
{
    switch (crash) {
        case ctStackOverflow:
            CrashTest(crash);
            break;
        case ctNullPointer:
        {
            char *bob = NULL;
            bob[0] = ((char*)CrashTest)[0];
        }
            break;
        case ctInvalidOpcode:
            asm("ud2");
            break;
        default:
            // what?
            break;
    }
}
