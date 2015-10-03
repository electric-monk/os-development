#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "KernelObject.h"

// Generic interface, whether it's actually a graphics mode or just VGA text

class ConsoleDriver
{
public:
    virtual void Set(char c, int x, int y) = 0;
    virtual void SetCursor(int x, int y) = 0;
    virtual void GetCursor(int *x, int *y) = 0;
    virtual int Width(void) = 0;
    virtual int Height(void) = 0;
    virtual void Clear(int x, int y, int w, int h) = 0;
    virtual void Copy(int xFrom, int yFrom, int xTo, int yTo, int w, int h) = 0;
};

extern ConsoleDriver *activeConsole;    // Set this - it can be VGA console for text only mode, graphical mode for the kernel to debug, or a custom ConsoleDriver subclass to capture kernel output.

// Text mode console driver

class VGAConsoleDriver : public ConsoleDriver
{
public:
    void SetMode(void *address, int width, int height, int widthSpan);

    void Set(char c, int x, int y);
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
    
    void Set(char c, int x, int y);
    void SetCursor(int x, int y);
    void GetCursor(int *x, int *y);
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
};

#endif // __CONSOLE_H__
