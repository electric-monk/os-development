#include "tools.h"
#include "debug.h"
#include "mem_logical.h"
#include "Console.h"
#include "Collections.h"

#define FORMAT_PLAIN		0x07

extern long long virt, phys;
const long long physicalAddress = 0xb8000;

static inline char* Get_Framebuffer(void)
{
    // WARNING: Currently this relies on the fact that we'll be mapped into memory starting at 0x0000000
    return (char*)(physicalAddress + (long long)&virt - (long long)&phys);
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
    if (activeConsole) {
        activeConsole->Set(c, x, y);
    } else {
        CathodeRayTube crt;
        
        crt.FrameBuffer()[crt.XyToOffset(x, y) * 2] = c;
    }
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
    int Width(void)
    {
        if (activeConsole)
            return activeConsole->Width();
        else {
            CathodeRayTube tube;
            return tube.Width();
        }
    }
    int Height(void)
    {
        if (activeConsole)
            return activeConsole->Height();
        else {
            CathodeRayTube tube;
            return tube.Height();
        }
    }
    void GetCursor(int *x, int *y)
    {
        if (activeConsole)
            activeConsole->GetCursor(x, y);
        else {
            CathodeRayTube tube;
            int cursor = tube.GetCursorOffset();
            int width = tube.Width();
            *x = cursor % width;
            *y = cursor / width;
        }
    }
    void SetCursor(int x, int y)
    {
        if (activeConsole)
            activeConsole->SetCursor(x, y);
        else {
            CathodeRayTube tube;
            tube.SetCursorOffset(x + (y * tube.Width()));
        }
    }
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
                    int x, y;
                    GetCursor(&x, &y);
                    x = 0;
                    SetCursor(x, y);
				}
				break;
			case '\f':
				{
                    int x, y;
                    GetCursor(&x, &y);
                    y++;
                    if (y >= Height()) {
                        ShiftLine();
                        y--;
                    }
                    SetCursor(x, y);
				}
				break;
			case '\n':
				{
                    int x, y;
                    GetCursor(&x, &y);
                    y++;
                    x = 0;
                    if (y >= Height()) {
                        ShiftLine();
                        y--;
                    }
                    SetCursor(x, y);
				}
				break;
            case '\0':
                c = ' ';
			default:
				PrintChar(c, FORMAT_PLAIN);
				break;
		}
	}
    void ShiftLine(void)
    {
        if (activeConsole) {
            activeConsole->Copy(0, 1, 0, 0, activeConsole->Width(), activeConsole->Height() - 1);
            activeConsole->Clear(0, activeConsole->Height() - 1, activeConsole->Width(), 1);
        } else {
            CathodeRayTube tube;
            int i;
            int len = tube.Width() * (tube.Height() - 1) * 2;
            char *data = tube.FrameBuffer();
            for (i = 0; i < len; i++)
                data[i] = data[i + (tube.Width() * 2)];
            len += tube.Width() * 2;
            for (; i < len;)
            {
                data[i++] = ' ';
                data[i++] = FORMAT_PLAIN;
            }
        }
    }
	void PrintChar(char c, int f)
	{
        if (activeConsole) {
            int x, y;
            activeConsole->GetCursor(&x, &y);
            if (c == 0x00) {
                // set cursor
                x = f % activeConsole->Width();
                y = f / activeConsole->Width();
            } else {
                // Set character
                activeConsole->Set(c, x, y);
                x++;
                if (x >= activeConsole->Width()) {
                    x = 0;
                    y++;
                    if (y >= activeConsole->Height()) {
                        ShiftLine();
                        y--;
                    }
                }
            }
            activeConsole->SetCursor(x, y);
        } else {
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
                cursor -= tube.Width();
                ShiftLine();
            }
            tube.SetCursorOffset(cursor);
        }
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

static void _report(KernelObject *item, int level)
{
    for (int i = 0; i < level; i++)
        kprintf("    ");
    if (item->IsDerivedFromClass("KernelArray")) {
        kprintf("<%s>\n", item->GetClassName(0));
        ((KernelArray*)item)->Enumerate([=](KernelObject *item){
            _report(item, level + 1);
            return (void*)NULL;
        });
    } else if (item->IsDerivedFromClass("KernelDictionary")) {
        kprintf("<%s>\n", item->GetClassName(0));
        KernelDictionary *dict = (KernelDictionary*)item;
        KernelArray *allKeys = dict->AllKeys();
        allKeys->Enumerate([=](KernelObject *key){
            _report(key, level + 1);
            _report(dict->ObjectFor(key), level + 2);
            return (void*)NULL;
        });
    } else if (item->IsDerivedFromClass("KernelNumber")) {
        kprintf("<%s: %i>\n", item->GetClassName(0), ((KernelNumber*)item)->Value());
    } else if (item->IsDerivedFromClass("KernelString")) {
        kprintf("<%s: \"%s\">\n", item->GetClassName(0), ((KernelString*)item)->CString());
    } else {
        kprintf("<%s: %x>\n", item->GetClassName(0), item);
    }
}

void report(KernelObject *object)
{
    _report(object, 0);
}
