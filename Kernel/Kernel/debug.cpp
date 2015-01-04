#include "tools.h"
#include "debug.h"
#include "mem_logical.h"

#define FORMAT_PLAIN		0x07

extern long long virt, phys;
const long long physicalAddress = 0xb8000;

//static char* s_framebuffer = 0;

static inline char* Get_Framebuffer(void)
{
    // WARNING: Currently this relies on the fact that we'll be mapped into memory starting at 0x0000000
//    if (s_framebuffer == NULL)
//        s_framebuffer = (char*)(physicalAddress + (long long)&virt - (long long)&phys);
//    return s_framebuffer;
    return (char*)(physicalAddress + (long long)&virt - (long long)&phys);
}

//void InitDebug(void)
//{
//    kprintf("Framebuffer address: 0x%.8x\n", Get_Framebuffer());
//    s_framebuffer = (char*)rootAddressSpace.Map(fmWritable, pmKernel, (PhysicalPointer)physicalAddress, 1);
//    kprintf("Framebuffer address: 0x%.8x\n", Get_Framebuffer());
//}

void test(char c)
{
	char *output = Get_Framebuffer() + (((80 * 24) + 79) * 2);
	output[0] = c;
	output[1] = FORMAT_PLAIN;
}

class CathodeRayTube
{
private:
	bool set;
	int got;
public:
	CathodeRayTube()
	{
		set = false;
	}
	~CathodeRayTube()
	{
	}

	// Size
	int Width(void)
	{
		return 80;
	}
	int Height(void)
	{
		return 25;
	}

	// Get pointer to the character buffer in video RAM
	char* FrameBuffer(void)
	{
        return Get_Framebuffer();
	}

	// Get cursor offset in character buffer
	int GetCursorOffset(void)
	{
		unsigned char low, high;

		if (set)
			return got;
		outb(0x3d4, 14);
		high = inb(0x3d5);
		outb(0x3d4, 15);
		low = inb(0x3d5);
		got = (high << 8) | low;
		set = true;
		return got;
	}

	// Set cursor offset in character buffer
	void SetCursorOffset(int offset)
	{
		got = offset;
		set = true;
		outb(0x3d4, 14);
		outb(0x3d5, (offset & 0xFF00) >> 8);
		outb(0x3d4, 15);
		outb(0x3d5, offset & 0xFF);
	}

	// Convert offset into coordinates
	void OffsetToXy(int offset, int *x, int *y)
	{
		*x = offset % Width();
		*y = offset / Width();
	}

	// Convert coordinates to offset
	int XyToOffset(int x, int y)
	{
		return x + (y * Width());
	}

	// Clear screen
	void Clear(void)
	{
		unsigned short *data;
		int i;

		data = (unsigned short*)FrameBuffer();
		for (i = 0; i < (80 * 25); i++)
			data[i] = ' ' | (0x07 << 8);
	}
};

void test(int x, int y, char c)
{
    CathodeRayTube crt;
    
    crt.FrameBuffer()[crt.XyToOffset(x, y) * 2] = c;
}

class ScreenPrinter : public CStringPrint
{
protected:
	void PrintOut(const char *data, int length)
	{
		for (int i = 0; i < length; i++)
		{
			HandleChar(data[i]);
			// Bochs debug
			outb(0xe9, data[i]);
		}
	}
private:
	void HandleChar(char c)
	{
		switch (c)
		{
			case '\t':
				for (int i = 0; i < 4; i++)
					PrintChar(' ', FORMAT_PLAIN);
				break;
			case '\r':
				{
					CathodeRayTube tube;
					int cursor = tube.GetCursorOffset();
					cursor -= cursor % tube.Width();
					PrintChar(0, cursor);
				}
				break;
			case '\f':
				{
					CathodeRayTube tube;
					int cursor = tube.GetCursorOffset();
					cursor += tube.Width();
					PrintChar(0, cursor);
				}
				break;
			case '\n':
				{
					CathodeRayTube tube;
					int cursor = tube.GetCursorOffset();
					cursor += tube.Width();
					cursor -= cursor % tube.Width();
					PrintChar(0, cursor);
				}
				break;
            case '\0':
                c = ' ';
			default:
				PrintChar(c, FORMAT_PLAIN);
				break;
		}
	}
	void PrintChar(char c, int f)
	{
		CathodeRayTube tube;
		char *data;
		int cursor;

		data = tube.FrameBuffer();
		if (c != 0x00)
		{
			cursor = tube.GetCursorOffset();
			data[(cursor * 2) + 0] = c;
			data[(cursor * 2) + 1] = f;
			cursor++;
		}
		else
		{
			cursor = f;
		}
		while (cursor >= (tube.Width() * tube.Height()))
		{
			int i, len;

			cursor -= tube.Width();
			len = tube.Width() * (tube.Height() - 1) * 2;
			for (i = 0; i < len; i++)
				data[i] = data[i + (tube.Width() * 2)];
			len += tube.Width() * 2;
			for (; i < len;)
			{
				data[i++] = ' ';
				data[i++] = FORMAT_PLAIN;
			}
		}
		tube.SetCursorOffset(cursor);
	}
};

void kcls(void)
{
	CathodeRayTube crt;

	crt.Clear();
}

void kprintf(const char *format, ...)
{
	ScreenPrinter printer;
	va_list vl;

	va_start(vl, format);
	printer.PrintList(format, vl);
	va_end(vl);
}
