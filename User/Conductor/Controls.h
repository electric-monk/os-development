//
//  Controls.h
//  TestOSGraphics
//
//  Created by Colin David Munro on 2/11/2016.
//  Copyright © 2016 MICE Software. All rights reserved.
//

#ifndef Controls_h
#define Controls_h

#include "Window.h"

namespace Controls {
    
    class StretchBitmap : public Window::Window
    {
    public:
        StretchBitmap(Graphics::Frame2D frame, Graphics::Rect2D bitmapInners, Graphics::FrameBuffer *bitmap);
        ~StretchBitmap() { delete _bitmap; }
        
        void SetTint(const Graphics::Colour &colour);
        const Graphics::Colour& Tint(void) const { return _tint; }
        
    protected:
        void Draw(Graphics::Context &context, Graphics::Rect2D region);
        
    private:
        Graphics::Rect2D _bitmapInner;
        Graphics::FrameBuffer *_bitmap;
        Graphics::Colour _tint;
    };
    
    class Button : public Window::Window
    {
    public:
        typedef enum {
            StateOff,
            StatePressed,
            StateDepressed,
            StateOn,
        } ButtonState;
        
        Button(Graphics::Frame2D frame);
        
        Library::Function<ButtonState(ButtonState)> handleChange;
        ButtonState State(void) const { return _state; }
        
        bool TouchDown(Graphics::Point2D position, int button);
        void TouchDrag(Graphics::Point2D position, int button);
        void TouchUp(Graphics::Point2D position, int button);
        
    private:
        ButtonState _state;
    };
    
    class Label : public Window::Window
    {
    public:
        Label(Graphics::Frame2D frame);
        ~Label();

        const char* Text(void) const { return _text; }
        void SetText(const char *text);
        
        Graphics::Colour TextColour(void) const { return _colour; }
        void SetColour(const Graphics::Colour &colour);

        Graphics::Point2D RequiredSize(void);
        void AutoSize(void);
        
        void Draw(Graphics::Context &context, Graphics::Rect2D region);
        
    private:
        const char *_text;
        Graphics::Colour _colour;
    };
    
    class MainWindow : public Window::Window
    {
    public:
        typedef enum {
            FlagClosable    = 1 << 0,
            FlagSizable     = 1 << 1,
        } WindowFlags;

        MainWindow(Graphics::Frame2D frame);
        ~MainWindow();
        
        WindowFlags Flags(void) const { return _flags; }
        void SetFlags(WindowFlags newFlags);
        
        const char* Title(void) const { return _title; }
        void SetTitle(const char *title);
        
        void* AddPage(const Graphics::Bitmap &icon, ::Window::Window *content);
        void RemovePage(void *page);
        
        void SetFocused(bool focus);
        
    protected:
        void Resized(void);
        
    private:
        const char *_title;
        WindowFlags _flags;
        ::Window::Window *_titleBar;
        ::Window::Window *_tabBar;
        ::Window::Window *_content;
        ::Window::Window *_sizeBit;
    };
    
}

#endif /* Controls_h */
