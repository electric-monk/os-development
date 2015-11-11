#ifndef __GENERICVIDEO_H__
#define __GENERICVIDEO_H__

#include "Driver.h"
#include "mem_physical.h"

class GenericVideo : public ProviderDriver
{
public:
    CLASSNAME(ProviderDriver, GenericVideo);
    
    typedef enum {
        Match_Specialised = 10000,    // A device-specific driver, like an accelerated driver for some ATI card
        Match_Default = 1000,         // A driver like the multiboot one, where the system has given us a video mode
        Match_Backup = 100,           // Something like a VGA driver, that'll work but look terrible
    } MatchLevel;
    
public:
    GenericVideo(const char *name);
    
    bool Start(Driver *parent);
    void Stop(void);
    
protected:
    typedef void* MODE_IDENTIFIER;
    typedef enum {
        tInvalid,
        tGraphical,
        tText,
    } TYPE;
    typedef struct {
        MODE_IDENTIFIER identifier;
        TYPE mode;
        int width;
        int height;
        int depth;
    } MODE;
    typedef struct {
        // Basic data
        int width, height;
        TYPE mode;
        // Actual logistical data
        PhysicalPointer address;
        int lineSpan, pixelSpan;
    } FRAMEBUFFER;

    // Convenient hooks
    virtual int GetPortCount(void) = 0;
    virtual int GetModes(int port, MODE *mode) = 0;
    virtual void SetMode(int port, MODE_IDENTIFIER mode) = 0;
    virtual MODE_IDENTIFIER GetMode(int port) = 0;
    
    void UpdatePort(int index);   // Call this to configure a port, NULL for off or a mode for a valid mode. It will cause an IPC service to appear describing the requested mode, which will be accessed via the hooks below
    virtual TYPE GetPortMode(int index) = 0;
    // For tGraphical
    virtual void GetFramebuffer(int index, FRAMEBUFFER *info) = 0;
    // For tText
    virtual void GetConsole(int index, int *width, int *height, bool *colour, bool *cursor) = 0;
    virtual void ShowCursor(int index, bool visible) = 0;
    virtual void SetCursor(int index, int x, int y) = 0;
    virtual void SetColour(int index, bool foreground, unsigned char r, unsigned char g, unsigned char b) = 0;
    virtual void SetCharacter(int index, int x, int y, char c) = 0;
    virtual void MoveCharacters(int index, int fromX, int fromY, int toX, int toY, int width, int height) = 0;
    
private:
};

#endif // __GENERICVIDEO_H__
