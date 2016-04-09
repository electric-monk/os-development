#include "Video_Multiboot.h"
#include "vbe.h"
#include "Console.h"
#include "mem_logical.h"
#include "descript.h"
#include "Collections.h"
#include "debug.h"

#define GENERIC_IDENTIFIER          0x20151013
#define GENERIC_VIDEO_TEXT          (void*)(GENERIC_IDENTIFIER + 0)
#define GENERIC_VIDEO_GRAPHICS      (void*)(GENERIC_IDENTIFIER + 1)

static bool s_hasVBE = false;
static vbe_controller_t s_controllerInfo;
static vbe_mode_t s_modeInfo;
static UInt16 s_mode;

class MultibootVideo : public GenericVideo
{
public:
    CLASSNAME(GenericVideo, MultibootVideo);
    
    MultibootVideo();
    
    bool Start(Driver *parent);
    void Stop(void);
    
protected:
    ~MultibootVideo();
    
    // For GenericVideo
    int GetPortCount(void);
    int GetModes(int port, MODE *mode);
    void SetMode(int port, MODE_IDENTIFIER mode);
    MODE_IDENTIFIER GetMode(int port);
    TYPE GetPortMode(int index);
    void GetFramebuffer(int port, FRAMEBUFFER *info);
    void GetConsole(int index, int *width, int *height, bool *colour, bool *cursor);
    void ShowCursor(int index, bool visible);
    void SetCursor(int index, int x, int y);
    void SetColour(int index, bool foreground, unsigned char r, unsigned char g, unsigned char b);
    void SetCharacter(int index, int x, int y, char c);
    void MoveCharacters(int index, int fromX, int fromY, int toX, int toY, int width, int height);
    
private:
    bool _graphical;
};

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
    void SetCursorEnabled(bool enabled)
    {
        // TODO
    }
};

static DefaultVGAConsoleDriver g_vgaConsole;

static void ConfigureTextConsole(int width, int height)
{
    g_vgaConsole.SetMode(Get_Framebuffer(), width, height, width);
    activeConsole = panicConsole = &g_vgaConsole;
}

class GraphicalConsoleDriverColour
{
public:
    unsigned char red, green, blue;
    
    SInt64 Value(void) const
    {
        return (red * red) + (green * green) + (blue * blue);
    }
    SInt64 Distance(GraphicalConsoleDriverColour const &other)
    {
        SInt64 result = Value() - other.Value();
        if (result < 0)
            return -result;
        else
            return result;
    }
};

static GraphicalConsoleDriverColour g_gfxColourForeground, g_gfxColourBackground;
static GraphicalConsoleDriver g_gfxConsole;

static void ConfigureGraphicsConsole(void *buffer, int width, int height, int lineSpan, int depth)
{
    g_gfxConsole.SetMode(buffer, width, height, depth / 8, lineSpan, depth);
    activeConsole = panicConsole = &g_gfxConsole;
    g_gfxColourBackground.red = 0;
    g_gfxColourBackground.green = 0;
    g_gfxColourBackground.blue = 0;
    g_gfxColourForeground.red = 255;
    g_gfxColourForeground.green = 255;
    g_gfxColourForeground.blue = 255;
}

class MultibootVideo_Factory : public DriverFactory
{
private:
    class Match : public DriverFactory::Match
    {
    public:
        CLASSNAME(DriverFactory::Match, MultibootVideo_Factory::Match);
        
        int MatchValue(void)
        {
            return GenericVideo::Match_Default;
        }
        Driver* Instantiate(void)
        {
            return new MultibootVideo();
        }
    };
public:
    CLASSNAME(DriverFactory, MultibootVideo_Factory);
    
    KernelArray* MatchForParent(Driver *parent)
    {
        // s_hasVBE should be true here
        KernelString *propertyBus = (KernelString*)parent->PropertyFor(kDriver_Property_Bus);
        if (propertyBus == NULL)
            return NULL;
        if (!propertyBus->IsEqualTo(kDriver_Bus_PCI))
            return NULL;
        KernelNumber *propertyClass = (KernelNumber*)parent->PropertyFor(kDriver_Property_PCI_Class);
        if (propertyClass == NULL)
            return NULL;
        UInt32 mainclass = propertyClass->Value();
        KernelNumber *propertySubclass = (KernelNumber*)parent->PropertyFor(kDriver_Property_PCI_Subclass);
        if (propertySubclass == NULL)
            return NULL;
        UInt32 subclass = propertySubclass->Value();
        if (((mainclass != 0x00) || (subclass != 0x01)) && (mainclass != 0x03))
            return NULL;
        KernelArray *output = new KernelArray();
        Match *primary = new Match();
        output->Add(primary);
        primary->Release();
        output->Autorelease();
        return output;
    }
};

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
    ConfigureGraphicsConsole(framebuffer, s_modeInfo.x_resolution, s_modeInfo.y_resolution, s_modeInfo.bytes_per_scanline, s_modeInfo.bits_per_pixel);
    // Register a PCI video adapter
    MultibootVideo_Factory *mvf = new MultibootVideo_Factory();
    Driver::RegisterFactory(mvf);
    mvf->Release();
}

MultibootVideo::MultibootVideo()
:GenericVideo("Multiboot Video Device")
{
}

MultibootVideo::~MultibootVideo()
{
    
}

bool MultibootVideo::Start(Driver *parent)
{
    if (GenericVideo::Start(parent)) {
        _graphical = false;
        UpdatePort(0);
        return true;
    }
    return false;
}

void MultibootVideo::Stop(void)
{
    
}

int MultibootVideo::GetPortCount(void)
{
    return 1;
}

int MultibootVideo::GetModes(int port, MODE *mode)
{
    if (port != 0)
        return 0;
    if (mode != NULL) {
        mode[0].identifier = GENERIC_VIDEO_GRAPHICS;
        mode[0].mode = tGraphical;
        mode[0].width = s_modeInfo.x_resolution;
        mode[0].height = s_modeInfo.y_resolution;
        mode[0].depth = s_modeInfo.bits_per_pixel;
        
        mode[1].identifier = GENERIC_VIDEO_TEXT;
        mode[1].mode = tText;
        mode[1].width = g_gfxConsole.Width();
        mode[1].height = g_gfxConsole.Height();
        mode[1].depth = 24; // Guess?
    }
    return 2;
}

void MultibootVideo::SetMode(int port, MODE_IDENTIFIER mode)
{
    if (port != 0)
        return;
    _graphical = mode == GENERIC_VIDEO_GRAPHICS;
    // TODO: Turn on/off console?
    UpdatePort(0);
}

GenericVideo::MODE_IDENTIFIER MultibootVideo::GetMode(int port)
{
    if (port != 0)
        return NULL;
    return _graphical ? GENERIC_VIDEO_GRAPHICS : GENERIC_VIDEO_TEXT;
}

GenericVideo::TYPE MultibootVideo::GetPortMode(int index)
{
    if (index != 0)
        return tInvalid;
    return _graphical ? tGraphical : tText;
}

void MultibootVideo::GetFramebuffer(int port, FRAMEBUFFER *info)
{
    if (port != 0) {
        info->address = CPhysicalMemory::Invalid;
        return;
    }
    info->address = (PhysicalPointer)s_modeInfo.phys_base;
    info->width = s_modeInfo.x_resolution;
    info->height = s_modeInfo.y_resolution;
    info->lineSpan = s_modeInfo.bytes_per_scanline;
    info->pixelSpan = s_modeInfo.bits_per_pixel / 8;
}

void MultibootVideo::GetConsole(int index, int *width, int *height, bool *colour, bool *cursor)
{
    if (width)
        *width = 80;
    if (height)
        *height = 25;
    if (colour)
        *colour = true;
    if (cursor)
        *cursor = true;
}

void MultibootVideo::ShowCursor(int index, bool visible)
{
    g_gfxConsole.SetCursorEnabled(visible);
}

void MultibootVideo::SetCursor(int index, int x, int y)
{
    g_gfxConsole.SetCursor(x, y);
}

void MultibootVideo::SetColour(int index, bool foreground, unsigned char r, unsigned char g, unsigned char b)
{
    if (foreground) {
        g_gfxColourForeground.red = r;
        g_gfxColourForeground.green = g;
        g_gfxColourForeground.blue = b;
    } else {
        g_gfxColourBackground.red = r;
        g_gfxColourBackground.green = g;
        g_gfxColourBackground.blue = b;
    }
}

void MultibootVideo::SetCharacter(int index, int x, int y, char c)
{
    // TODO: colours
    g_gfxConsole.Set(c, x, y);
}

void MultibootVideo::MoveCharacters(int index, int fromX, int fromY, int toX, int toY, int width, int height)
{
    g_gfxConsole.Copy(fromX, fromY, toX, toY, width, height);
}
