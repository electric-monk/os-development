//
//  Controls.cpp
//  TestOSGraphics
//
//  Created by Colin David Munro on 2/11/2016.
//  Copyright © 2016 MICE Software. All rights reserved.
//

#include "Controls.h"
#include "Bitmaps.h"
#include "TestFont.hpp"

Controls::StretchBitmap::StretchBitmap(Graphics::Frame2D frame, Graphics::Rect2D bitmapInners, Graphics::FrameBuffer *bitmap)
:Window(frame), _bitmapInner(bitmapInners), _bitmap(bitmap), _tint(Graphics::Colour::White)
{
}

void Controls::StretchBitmap::SetTint(const Graphics::Colour &colour)
{
    _tint = colour;
    SetDirty();
}

void Controls::StretchBitmap::Draw(Graphics::Context &context, Graphics::Rect2D region)
{
    context.Push();
    Graphics::Point2D bitmapSize = (Graphics::Point2D){Graphics::Unit(_bitmap->Width()), Graphics::Unit(_bitmap->Height())};
    Graphics::Point2D windowSize = Bounds().size;
    // Top left
    context.DrawBitmapTinted(Graphics::Path::Rect((Graphics::Rect2D){{0, 0}, {bitmapSize.x * _bitmapInner.topLeft.x, bitmapSize.y * _bitmapInner.topLeft.y}}), *_bitmap, _tint);
    // Top right
    context.Push();
    context.Translate(windowSize.x - bitmapSize.x, 0);
    context.DrawBitmapTinted(Graphics::Path::Rect((Graphics::Rect2D){{bitmapSize.x * _bitmapInner.bottomRight.x, 0}, {bitmapSize.x, bitmapSize.y * _bitmapInner.topLeft.y}}), *_bitmap, _tint);
    context.Pop();
    // Bottom left
    context.Push();
    context.Translate(0, windowSize.y - bitmapSize.y);
    context.DrawBitmapTinted(Graphics::Path::Rect((Graphics::Rect2D){{0, bitmapSize.y * _bitmapInner.bottomRight.y}, {bitmapSize.y * _bitmapInner.topLeft.x, bitmapSize.y}}), *_bitmap, _tint);
    context.Pop();
    // Bottom right
    context.Push();
    context.Translate(windowSize.x - bitmapSize.x, windowSize.y - bitmapSize.y);
    context.DrawBitmapTinted(Graphics::Path::Rect((Graphics::Rect2D){{bitmapSize.x * _bitmapInner.bottomRight.x, bitmapSize.y * _bitmapInner.bottomRight.y}, {bitmapSize.x, bitmapSize.y}}), *_bitmap, _tint);
    context.Pop();
    // Top centre
    context.Push();
    context.Translate(bitmapSize.x * _bitmapInner.topLeft.x, 0);
    Graphics::Unit bitmapChunkX = bitmapSize.x * (_bitmapInner.bottomRight.x - _bitmapInner.topLeft.x);
    Graphics::Unit usedChunkX = bitmapSize.x - bitmapChunkX;
    Graphics::Unit scaleX = (windowSize.x - usedChunkX) / bitmapChunkX;
    context.Scale(scaleX, 1);
    context.Translate(-bitmapSize.x * _bitmapInner.topLeft.x, 0);
    context.DrawBitmapTinted(Graphics::Path::Rect((Graphics::Rect2D){{bitmapSize.x * _bitmapInner.topLeft.x, 0}, {bitmapSize.x * _bitmapInner.bottomRight.x, bitmapSize.y * _bitmapInner.topLeft.y}}), *_bitmap, _tint);
    // Bottom centre
    context.Translate(0, windowSize.y - bitmapSize.y);
    context.DrawBitmapTinted(Graphics::Path::Rect((Graphics::Rect2D){{bitmapSize.x * _bitmapInner.topLeft.x, bitmapSize.y * _bitmapInner.bottomRight.y}, {bitmapSize.x * _bitmapInner.bottomRight.x, bitmapSize.y}}), *_bitmap, _tint);
    context.Pop();
    // Left side
    context.Push();
    context.Translate(0, bitmapSize.y * _bitmapInner.topLeft.y);
    Graphics::Unit bitmapChunkY = bitmapSize.y * (_bitmapInner.bottomRight.y - _bitmapInner.topLeft.y);
    Graphics::Unit usedChunkY = bitmapSize.y - bitmapChunkY;
    Graphics::Unit scaleY = (windowSize.y - usedChunkY) / bitmapChunkY;
    context.Scale(1, scaleY);
    context.Translate(0, -bitmapSize.y * _bitmapInner.topLeft.y);
    context.DrawBitmapTinted(Graphics::Path::Rect((Graphics::Rect2D){{0, bitmapSize.y * _bitmapInner.topLeft.y}, {bitmapSize.x * _bitmapInner.topLeft.x, bitmapSize.y * _bitmapInner.bottomRight.y}}), *_bitmap, _tint);
    // Right side
    context.Translate(windowSize.x - bitmapSize.x, 0);
    context.DrawBitmapTinted(Graphics::Path::Rect((Graphics::Rect2D){{bitmapSize.x * _bitmapInner.bottomRight.x, bitmapSize.y * _bitmapInner.topLeft.y}, {bitmapSize.x, bitmapSize.y * _bitmapInner.bottomRight.y}}), *_bitmap, _tint);
    context.Pop();
    // Centre
    context.Translate(bitmapSize.x * _bitmapInner.topLeft.x, bitmapSize.y * _bitmapInner.topLeft.y);
    context.Scale(scaleX, scaleY);
    context.Translate(-bitmapSize.x * _bitmapInner.topLeft.x, -bitmapSize.y * _bitmapInner.topLeft.y);
    context.DrawBitmapTinted(Graphics::Path::Rect((Graphics::Rect2D){{bitmapSize.x * _bitmapInner.topLeft.x, bitmapSize.y * _bitmapInner.topLeft.y}, {bitmapSize.x * _bitmapInner.bottomRight.x, bitmapSize.y * _bitmapInner.bottomRight.y}}), *_bitmap, _tint);
    // Done
    context.Pop();
}

Controls::Button::Button(Graphics::Frame2D frame)
:Window(frame)
{
    _state = StateOff;
    handleChange = [](ButtonState state){return state;};
}

bool Controls::Button::TouchDown(Graphics::Point2D position, int button)
{
    handleChange(StatePressed);
    _state = StatePressed;
    return true;
}

void Controls::Button::TouchDrag(Graphics::Point2D position, int button)
{
    bool within = Bounds().Contains(position);
    if (_state == StatePressed) {
        if (!within) {
            handleChange(StateDepressed);
            _state = StateDepressed;
        }
    } else {
        if (within) {
            handleChange(StatePressed);
            _state = StatePressed;
        }
    }
}

void Controls::Button::TouchUp(Graphics::Point2D position, int button)
{
    _state = handleChange(StateOff);
}

#define ICON_SIZE       16
#define PADDING_X       5

namespace Controls {
    namespace Internal {
        class TitleBar : public StretchBitmap
        {
        public:
            TitleBar()
            :StretchBitmap((Graphics::Frame2D){{0, 0}, {0, 0}},
                           (Graphics::Rect2D){{0.2, 0.1}, {0.8, 0.9}},
                           Bitmaps::Get(Bitmaps::Title::titlebar))
            ,_closeIcon(Bitmaps::Get(Bitmaps::Close::close))
            ,_title(NULL), _close(false)
            {
                
            }
            
            ~TitleBar()
            {
                delete _closeIcon;
            }
            
            void SetFocus(bool focus)
            {
                Graphics::Colour colour = {0xff, 0xff, 0xff, (UInt8)(focus ? 0xff : 0xa0)};
                SetTint(colour);
            }
            
            void SetState(bool showClose, const char *title)
            {
                _close = showClose;
                _title = title;
            }
            
            Graphics::Point2D RequiredSize(void)
            {
                TestFont font;
                Graphics::Point2D size = (Graphics::Point2D){0, 0};
                if (_title) {
                    size = font.Size(_title);
                    size.x += PADDING_X;
                }
                size.y = 16;
                if (_close)
                    size.x += 16;
                size.x += PADDING_X * 2;
                size.y += 6;
                return size;
            }

            bool TouchDown(Graphics::Point2D position, int button)
            {
                _dragging = true;
                _referencePoint = position;
                return true;
            }
            void TouchDrag(Graphics::Point2D position, int button)
            {
                Graphics::Frame2D frame = Parent()->Frame();
                frame.origin.x += (int)(position.x - _referencePoint.x);
                frame.origin.y += (int)(position.y - _referencePoint.y);
                Parent()->SetFrame(frame);
            }
            void TouchUp(Graphics::Point2D position, int button)
            {
                TouchDrag(position, button);
                _dragging = false;
            }

            void Draw(Graphics::Context &context, Graphics::Rect2D region)
            {
                TestFont font;
                StretchBitmap::Draw(context, region);
                Graphics::Point2D windowSize = Bounds().size;
                context.Translate(PADDING_X, 0);
                if (_close) {
                    context.Push();
                    context.Translate(0, (windowSize.y - ICON_SIZE) / 2);
                    context.Scale(ICON_SIZE / _closeIcon->Width(), ICON_SIZE / _closeIcon->Height());
                    context.DrawBitmapTinted(*_closeIcon, Tint());
                    context.Pop();
                    context.Translate(ICON_SIZE + PADDING_X, 0);
                }
                if (_title) {
                    Graphics::Point2D size = font.Size(_title);
                    context.Push();
                    context.Translate(0, (windowSize.y - size.y) / 2);
                    context.Push();
                    context.Translate(1, 1);
                    font.Render(context, (Graphics::Colour){0xFF, 0xFF, 0xFF, 0xFF}, _title);
                    context.Pop();
                    font.Render(context, (Graphics::Colour){0x00, 0x00, 0x00, 0xFF}, _title);
                    context.Pop();
                    context.Translate(size.x + PADDING_X, 0);
                }
            }

        private:
            bool _close;
            const char *_title;
            bool _dragging;
            Graphics::Point2D _referencePoint;
            Graphics::FrameBuffer *_closeIcon;
        };
        
        class TabBar : public Window::Window
        {
        public:
            TabBar()
            :Window((Graphics::Frame2D){{0, 0}, {0, 0}})
            {
                
            }
            
            int Width(void)
            {
                return 24;
            }
        };
        
        class Content : public StretchBitmap
        {
        public:
            Content()
            :StretchBitmap((Graphics::Frame2D){{0, 0}, {0, 0}},
                           (Graphics::Rect2D){{0.1, 0.1}, {0.9, 0.9}},
                           Bitmaps::Get(Bitmaps::Content::window))
            {
            }
        };
        
        class SizeArea : public Window::BitmapWindow
        {
        public:
            SizeArea()
            :BitmapWindow(*Bitmaps::Get(Bitmaps::Resize::resize)), _dragging(false)
            {
            }
            
            bool TouchDown(Graphics::Point2D position, int button)
            {
                _dragging = true;
                _referencePoint = position;
                return true;
            }
            void TouchDrag(Graphics::Point2D position, int button)
            {
                Graphics::Frame2D frame = Parent()->Frame();
                frame.size.x += (int)(position.x - _referencePoint.x);
                if (frame.size.x < 30)
                    frame.size.x = 30;
                frame.size.y += (int)(position.y - _referencePoint.y);
                if (frame.size.y < 30)
                    frame.size.y = 30;
                Parent()->SetFrame(frame);
            }
            void TouchUp(Graphics::Point2D position, int button)
            {
                TouchDrag(position, button);
                _dragging = false;
            }
            
        private:
            bool _dragging;
            Graphics::Point2D _referencePoint;
        };
    }
    
    static int tempcount(const char *s)
    {
        int total = 0;
        while(*s) {
            s++;
            total++;
        }
        return total;
    }
    
    Label::Label(Graphics::Frame2D frame)
    :Window(frame), _text(NULL)
    {
        _colour = (Graphics::Colour){0x00, 0x00, 0x00, 0xFF};
    }
    
    Label::~Label()
    {
        if (_text)
            delete[] _text;
    }
    
    void Label::SetText(const char *text)
    {
        if (_text == text)
            return;
        if (_text)
            delete[] _text;
        int amount = tempcount(text) + 1;
        _text = new char[amount];
        CopyMemory((char*)_text, text, amount);
        SetDirty();
    }
    
    void Label::SetColour(const Graphics::Colour &colour)
    {
        _colour = colour;
        SetDirty();
    }
    
    Graphics::Point2D Label::RequiredSize(void)
    {
        if (_text) {
            TestFont font;
            return font.Size(_text);
        }
        return (Graphics::Point2D){0, 0};
    }
    
    void Label::AutoSize(void)
    {
        Graphics::Frame2D frame = Frame();
        frame.size = RequiredSize();
        SetFrame(frame);
    }
    
    void Label::Draw(Graphics::Context &context, Graphics::Rect2D region)
    {
        if (_text) {
            TestFont font;
            Graphics::Point2D windowSize = Bounds().size;
            Graphics::Point2D size = font.Size(_text);
            context.Push();
            context.Translate(0, (windowSize.y - size.y) / 2);
            font.Render(context, _colour, _text);
            context.Pop();
        }
    }

    MainWindow::MainWindow(Graphics::Frame2D frame)
    :Window(frame), _title(NULL), _flags(WindowFlags(0))
    {
        _titleBar = new Internal::TitleBar();
        _titleBar->SetHidden(true);
        _titleBar->SetLevel(1);
        AddChild(_titleBar);
        _content = new Internal::Content();
        _content->SetLevel(0);
        AddChild(_content);
        _tabBar = new Internal::TabBar();
        _tabBar->SetHidden(true);
        _tabBar->SetLevel(1);
        AddChild(_tabBar);
        _sizeBit = new Internal::SizeArea();
        _sizeBit->SetHidden(true);
        _sizeBit->SetLevel(2);
        AddChild(_sizeBit);
    }

    MainWindow::~MainWindow()
    {
        if (_title)
            delete[] _title;
    }
    
    void MainWindow::SetFlags(WindowFlags newFlags)
    {
        _flags = newFlags;
        Resized();
    }
    
    void MainWindow::SetTitle(const char *title)
    {
        if (_title == title)
            return;
        delete[] _title;
        int amount = tempcount(title) + 1;
        _title = new char[amount];
        CopyMemory((char*)_title, title, amount);
        Resized();
    }
    
    void* MainWindow::AddPage(const Graphics::Bitmap &icon, ::Window::Window *content)
    {
        return NULL;
    }
    
    void MainWindow::RemovePage(void *page)
    {
        
    }
    
    void MainWindow::SetFocused(bool focus)
    {
        ((Internal::TitleBar*)_titleBar)->SetFocus(focus);
    }
    
    void MainWindow::Resized(void)
    {
        bool wantsTitle = _title || (_flags & FlagClosable);
        bool wantsTabs = false;
        Graphics::Frame2D tabFrame = {{0, 0}, {0, 0}};
        Graphics::Frame2D titleFrame = {{0, 0}, {0, 0}};
        Graphics::Frame2D fullFrame = Bounds();
        Graphics::Frame2D contentFrame = fullFrame;
        _tabBar->SetHidden(!wantsTabs);
        if (wantsTabs) {
            int width = ((Internal::TabBar*)_tabBar)->Width();
            contentFrame.origin.x += width;
            contentFrame.size.x -= width;
            tabFrame.size.x = width;
            tabFrame.size.y = contentFrame.size.y;
        }
        _titleBar->SetHidden(!wantsTitle);
        if (wantsTitle) {
            Internal::TitleBar *titleBar = (Internal::TitleBar*)_titleBar;
            titleBar->SetState(_flags & FlagClosable, _title);
            titleFrame.origin.x = contentFrame.origin.x;
            titleFrame.size = titleBar->RequiredSize();
            contentFrame.origin.y += titleFrame.size.y;
            contentFrame.size.y -= titleFrame.size.y;
            tabFrame.origin.y += titleFrame.size.y;
            tabFrame.size.y -= titleFrame.size.y;
        }
        _sizeBit->SetHidden(!(_flags & FlagSizable));
        Graphics::Frame2D sizeFrame = _sizeBit->Frame();
        sizeFrame.origin.x = fullFrame.size.x - (sizeFrame.size.x + 2);
        sizeFrame.origin.y = fullFrame.size.y - (sizeFrame.size.y + 2);
        _titleBar->SetFrame(titleFrame);
        _tabBar->SetFrame(tabFrame);
        _content->SetFrame(contentFrame);
        _sizeBit->SetFrame(sizeFrame);
    }
}
