#include "Console.h"
#include "CPU_intr.h"

ConsoleDriver *activeConsole;

static void SimpleBlit(unsigned char *buffer, int pixelSpan, int lineSpan, int sourceX, int sourceY, int destX, int destY, int width, int height)
{
    if ((sourceX == destX) && (sourceY == destY))
        return;
    if ((width == 0) || (height == 0))
        return;
    // Work out directions
    int xDirection, yDirection;
    int startLineSource, startLineDest;
    int startPixelSource, startPixelDest;
    if (destY < sourceY) {
        // Copy from bottom to top, start at top
        yDirection = 1;
        startLineSource = sourceY;
        startLineDest = destY;
    } else {
        // Copy from top to bottom, start at bottom
        yDirection = -1;
        startLineSource = sourceY + height - 1;
        startLineDest = destY + height - 1;
    }
    if (destX < sourceX) {
        // Copy from right to left, start at left
        xDirection = 1;
        startPixelSource = sourceX;
        startPixelDest = destX;
    } else {
        // Copy from left to right, start at right
        xDirection = -1;
        startPixelSource = sourceX + width - 1;
        startPixelDest = destX + width - 1;
    }
    // Compute offsets
    int startByteSource = startPixelSource * pixelSpan;
    int startByteDest = startPixelDest * pixelSpan;
    int copyBytes = width * pixelSpan;
    int directionalLineSpan = yDirection * lineSpan;
    unsigned char *source = buffer + (startLineSource * lineSpan);
    unsigned char *dest = buffer + (startLineDest * lineSpan);
    // Actual copy
    for (int lineCount = height; lineCount != 0; lineCount--, source += directionalLineSpan, dest += directionalLineSpan) {
        unsigned char *sourceByte = source + startByteSource;
        unsigned char *destByte = dest + startByteDest;
        for (int byteCount = copyBytes; byteCount != 0; byteCount--, sourceByte += xDirection, destByte += xDirection)
            *destByte = *sourceByte;
    }
}

static void SimpleClear(unsigned char *buffer, int pixelSpan, int lineSpan, int x, int y, int w, int h)
{
    if ((w == 0) || (h == 0))
        return;
    buffer += lineSpan * y;
    buffer += pixelSpan * x;
    for (int line = 0; line < h; line++, buffer += lineSpan) {
        unsigned char *bufferLine = buffer;
        for (int pixelCount = pixelSpan * w; pixelCount != 0; pixelCount--, bufferLine++)
            *bufferLine = 0;
    }
}

static void SimpleInvert(unsigned char *buffer, int pixelSpan, int lineSpan, int x, int y, int w, int h)
{
    if ((w == 0) || (h == 0))
        return;
    buffer += lineSpan * y;
    buffer += pixelSpan * x;
    for (int line = 0; line < h; line++, buffer += lineSpan) {
        unsigned char *bufferLine = buffer;
        for (int pixelCount = pixelSpan * w; pixelCount != 0; pixelCount--, bufferLine++)
            *bufferLine = ~*bufferLine;
    }
}

void VGAConsoleDriver::SetMode(void *address, int width, int height, int widthSpan)
{
    _buffer = (unsigned char*)address;
    _width = width;
    _height = height;
    _widthSpan = widthSpan;
}

void VGAConsoleDriver::Set(char c, int x, int y)
{
    int offset = ((y * _widthSpan) + x) * 2;
    _buffer[offset + 0] = c;
    _buffer[offset + 1] = 0x07;
}

int VGAConsoleDriver::Width(void)
{
    return _width;
}

int VGAConsoleDriver::Height(void)
{
    return _height;
}

void VGAConsoleDriver::Clear(int x, int y, int w, int h)
{
    SimpleClear(_buffer, 2, _widthSpan, x, y, w, h);
}

void VGAConsoleDriver::Copy(int xFrom, int yFrom, int xTo, int yTo, int w, int h)
{
    SimpleBlit(_buffer, 2, _widthSpan, xFrom, yFrom, xTo, yTo, w, h);
}
    
namespace ZeppFont {
#include "kernel.font.h"
}

#define PIXEL_HEIGHT        2
static void ToggleCursor(unsigned char *buffer, int pixelSpan, int widthSpan, int x, int y)
{
    SimpleInvert(buffer, pixelSpan, widthSpan, x * ZeppFont::width, ((y + 1) * ZeppFont::height) - (PIXEL_HEIGHT + 1), ZeppFont::width, PIXEL_HEIGHT);
}

void GraphicalConsoleDriver::SetMode(void *address, int width, int height, int pixelSpan, int widthSpan, int depth)
{
    _buffer = (unsigned char*)address;
    _width = width;
    _height = height;
    _pixelSpan = pixelSpan;
    _widthSpan = widthSpan;
    _depth = depth;
    _cursorX = 0;
    _cursorY = 0;
    
    SimpleClear(_buffer, _pixelSpan, _widthSpan, 0, 0, _width, _height);
    ToggleCursor(_buffer, _pixelSpan, _widthSpan, 0, 0);
}

static void SetPixel24(unsigned char *buffer, int widthSpan, int x, int y, UInt32 colour)
{
    buffer += (y * widthSpan) + (3 * x);
    buffer[0] = colour & 0xFF;
    buffer[1] = (colour >> 8) & 0xFF;
    buffer[2] = (colour >> 16) & 0xFF;
}

static void SetPixel32(unsigned char *buffer, int widthSpan, int x, int y, UInt32 colour)
{
    UInt32 *data = (UInt32*)buffer;
    data[(y * widthSpan) + x] = colour;
}

void GraphicalConsoleDriver::Set(char c, int x, int y)
{
    InterruptDisabler disabler;
    ToggleCursor(_buffer, _pixelSpan, _widthSpan, _cursorX, _cursorY);
    int actualX = x * ZeppFont::width;
    int actualY = y * ZeppFont::height;
    int charWidth = ZeppFont::font[c].width;
    unsigned char *bitmap = ZeppFont::bitmap + (ZeppFont::font[c].offset * ZeppFont::height);
    for (int charY = 0; charY < ZeppFont::height; charY++) {
        unsigned char byte;
        // Prime first read
        int actualXcursor = actualX;
        for (int charX = 0; charX < charWidth; charX++, byte <<= 1) {
            if ((charX % 8) == 0) {
                byte = *bitmap;
                bitmap++;
            }
            UInt32 colour;
            if (byte & 0x80)
                colour = 0xFFFFFFFF;
            else
                colour = 0x00000000;
            SetPixel24(_buffer, _widthSpan, actualXcursor, actualY, colour);
            actualXcursor++;
        }
        actualY++;
    }
    ToggleCursor(_buffer, _pixelSpan, _widthSpan, _cursorX, _cursorY);
}

void GraphicalConsoleDriver::SetCursor(int x, int y)
{
    InterruptDisabler disabler;
    ToggleCursor(_buffer, _pixelSpan, _widthSpan, _cursorX, _cursorY);
    _cursorX = x;
    _cursorY = y;
    ToggleCursor(_buffer, _pixelSpan, _widthSpan, _cursorX, _cursorY);
}

void GraphicalConsoleDriver::GetCursor(int *x, int *y)
{
    if (x)
        *x = _cursorX;
    if (y)
        *y = _cursorY;
}

int GraphicalConsoleDriver::Width(void)
{
    return _width / ZeppFont::width;
}

int GraphicalConsoleDriver::Height(void)
{
    return _height / ZeppFont::height;
}

void GraphicalConsoleDriver::Clear(int x, int y, int w, int h)
{
    InterruptDisabler disabler;
    SimpleClear(_buffer, _pixelSpan, _widthSpan, x * ZeppFont::width, y * ZeppFont::height, w * ZeppFont::width, h * ZeppFont::height);
}

void GraphicalConsoleDriver::Copy(int xFrom, int yFrom, int xTo, int yTo, int w, int h)
{
    InterruptDisabler disabler;
    SimpleBlit(_buffer, _pixelSpan, _widthSpan, xFrom * ZeppFont::width, yFrom * ZeppFont::height, xTo * ZeppFont::width, yTo * ZeppFont::height, w * ZeppFont::width, h * ZeppFont::height);
}
