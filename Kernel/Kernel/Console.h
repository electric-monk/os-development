#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "KernelObject.h"

// Generic interface, whether it's actually a graphics mode or just VGA text

class ConsoleDriver
{
public:
    class Colour
    {
    public:
        Colour(){red = green = blue = 0;}
        Colour(UInt8 r, UInt8 g, UInt8 b):red(r), green(g), blue(b){}
        UInt8 red, green, blue;
    };

public:
    virtual void Set(char c, int x, int y, Colour foreground = Colour(255, 255, 255), Colour background = Colour(0, 0, 0)) = 0;
    virtual void SetCursor(int x, int y) = 0;
    virtual void GetCursor(int *x, int *y) = 0;
    virtual void SetCursorEnabled(bool enabled) = 0;
    virtual int Width(void) = 0;
    virtual int Height(void) = 0;
    virtual void Clear(int x, int y, int w, int h) = 0;
    virtual void Copy(int xFrom, int yFrom, int xTo, int yTo, int w, int h) = 0;
};

extern ConsoleDriver *activeConsole;    // Set this - it can be VGA console for text only mode, graphical mode for the kernel to debug, or a custom ConsoleDriver subclass to capture kernel output.
extern ConsoleDriver *panicConsole;     // Set this - it should always end up on the screen

void Console_Panic(void);

// TODO: debug ConsoleDriver objects which can wrap a 'real' one like VGA or text capture, 

// Text mode console driver

class VGAConsoleDriver : public ConsoleDriver
{
public:
    void SetMode(void *address, int width, int height, int widthSpan);

    void Set(char c, int x, int y, Colour foreground = Colour(255, 255, 255), Colour background = Colour(0, 0, 0));
    int Width(void);
    int Height(void);
    void Clear(int x, int y, int w, int h);
    void Copy(int xFrom, int yFrom, int xTo, int yTo, int w, int h);
    
private:
    unsigned char *_buffer;
    int _width, _height;
    int _widthSpan;
};

// Graphical simulated console

class GraphicalConsoleDriver : public ConsoleDriver
{
public:
    void SetMode(void *address, int width, int height, int pixelSpan, int widthSpan, int depth);
    
    void Set(char c, int x, int y, Colour foreground = Colour(255, 255, 255), Colour background = Colour(0, 0, 0));
    void SetCursor(int x, int y);
    void GetCursor(int *x, int *y);
    void SetCursorEnabled(bool enabled);
    int Width(void);
    int Height(void);
    void Clear(int x, int y, int w, int h);
    void Copy(int xFrom, int yFrom, int xTo, int yTo, int w, int h);
    
private:
    unsigned char *_buffer;
    int _width, _height;
    int _pixelSpan, _widthSpan;
    int _depth;
    int _cursorX, _cursorY;
    bool _cursorEnabled;

    void(*_pixelSet)(unsigned char *buffer, int widthSpan, int x, int y, Colour colour);
};

#endif // __CONSOLE_H__
