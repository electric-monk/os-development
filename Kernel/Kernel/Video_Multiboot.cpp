#include "Video_Multiboot.h"
#include "vbe.h"
#include "Console.h"
#include "mem_logical.h"
#include "descript.h"

static bool s_hasVBE = false;
static vbe_controller_t s_controllerInfo;
static vbe_mode_t s_modeInfo;
static UInt16 s_mode;

// For text mode
extern long long virt, phys;
const long long physicalAddress = 0xb8000;
static inline char* Get_Framebuffer(void)
{
    return (char*)(physicalAddress + (long long)&virt - (long long)&phys);
}

class DefaultVGAConsoleDriver : public VGAConsoleDriver
{
private:
    bool set;
    int got;
public:
    void SetCursor(int x, int y)
    {
        int offset = (y * Width()) + x;
		got = offset;
		set = true;
		outb(0x3d4, 14);
		outb(0x3d5, (offset & 0xFF00) >> 8);
		outb(0x3d4, 15);
		outb(0x3d5, offset & 0xFF);
    }
    void GetCursor(int *x, int *y)
    {
		unsigned char low, high;
        
		if (!set) {
            outb(0x3d4, 14);
            high = inb(0x3d5);
            outb(0x3d4, 15);
            low = inb(0x3d5);
            got = (high << 8) | low;
            set = true;
        }
        
        if (x)
            *x = got % Width();
        if (y)
            *y = got / Width();
    }
};

static DefaultVGAConsoleDriver g_vgaConsole;

static void ConfigureTextConsole(int width, int height)
{
    g_vgaConsole.SetMode(Get_Framebuffer(), width, height, width);
    activeConsole = &g_vgaConsole;
}

static GraphicalConsoleDriver g_gfxConsole;

static void ConfigureGraphicsConsole(void *buffer, int width, int height, int lineSpan, int depth)
{
    g_gfxConsole.SetMode(buffer, width, height, depth, lineSpan, depth);
    activeConsole = &g_gfxConsole;
}

void Init_Video_Multiboot(void *vbe_control_info, void *vbe_mode_info, UInt16 vbe_mode)
{
    // Check for values
    if ((vbe_control_info == NULL) || (vbe_mode_info == NULL)) {
        ConfigureTextConsole(80, 25);
        return;
    }
    // Got some values, copy them out
    CopyMemory(&s_controllerInfo, vbe_control_info, sizeof(vbe_controller_t));
    s_mode = vbe_mode;
    CopyMemory(&s_modeInfo, vbe_mode_info, sizeof(vbe_mode_t));
    s_hasVBE = true;
    // Apparently we have values? Fire up the relevant console
    PhysicalPointer base = (PhysicalPointer)(s_modeInfo.phys_base & ~(PAGE_SIZE - 1));
    void *framebuffer = rootAddressSpace.Map(fmWritable, pmKernel, base, ((s_modeInfo.phys_base - UInt32(base)) + (s_modeInfo.bytes_per_scanline * s_modeInfo.y_resolution) + PAGE_SIZE - 1) / PAGE_SIZE);
    ConfigureGraphicsConsole(framebuffer, s_modeInfo.x_resolution, s_modeInfo.y_resolution, s_modeInfo.bytes_per_scanline, s_modeInfo.bits_per_pixel / 8);
}
