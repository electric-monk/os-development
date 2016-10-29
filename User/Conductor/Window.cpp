//
//  Window.cpp
//  TestOSGraphics
//
//  Created by Colin David Munro on 8/10/2016.
//  Copyright © 2016 MICE Software. All rights reserved.
//

#include "Window.hpp"
#include <stdio.h>

namespace Window {
    Window::~Window()
    {
        Library::ForEach(_children, [](Window *child){
            delete child;
            return true;
        });
    }
    
    void Window::SetFrame(Graphics::Frame2D frame)
    {
        SetDirty();
        _frame = frame;
        SetDirty();
    }
    
    void Window::SetBounds(Graphics::Frame2D bounds)
    {
        SetDirty();
        _frame.size = Convert(_transform.Invert().Apply(Convert(bounds))).size;
        SetDirty();
    }

    const Graphics::Frame2D Window::Bounds(void) const
    {
        Graphics::Frame2D bounds = (Graphics::Frame2D){{0, 0}, _frame.size};
        return Convert(_transform.Apply(Convert(bounds)));
    }
    
    void Window::SetTransform(const Graphics::Matrix2D& transform)
    {
        SetDirty();
        _transform = transform;
        SetDirty();
    }
    
    void Window::SetDirty(void)
    {
        SetDirty(Convert(Bounds()));
    }
    
    void Window::SetDirty(const Graphics::Rect2D &area)
    {
        if (_parent) {
            Graphics::Rect2D adjustedBounds = _transform.Invert().Apply(area);
            _parent->SetDirty(adjustedBounds.Offset(_frame.origin));
        }
    }
    
    void Window::DrawAll(Graphics::Context &context, Graphics::Rect2D region)
    {
        context.Apply(_transform);
        Graphics::Rect2D projectedRegion = context.CurrentTransform().Invert().Apply(region);
        if (!Convert(Bounds()).Intersects(projectedRegion))
            return;
        context.SetClipping(Graphics::Path::Rect(projectedRegion));
        Graphics::Context subcontext(context);
        Draw(subcontext, projectedRegion);
        // TODO: Use opaque info to work out what to draw
        Library::ForEach(_children, [&](Window *child){
            subcontext.Push();
            Graphics::Frame2D frame = child->Frame();
            subcontext.Translate(frame.origin.x, frame.origin.y);
            subcontext.SetClipping(Graphics::Path::Rect(Graphics::Convert((Graphics::Frame2D){{0, 0}, frame.size})));
            Graphics::Context subsubcontext(subcontext);
            child->DrawAll(subsubcontext, region);
            subcontext.Pop();
            return true;
        });
    }

    void Window::AddChild(Window *window)
    {
        if (window->_parent == this)
            return;
        if (window->_parent != NULL)
            window->_parent->RemoveChild(window);
        _children.Add(window);
        window->_parent = this;
        // TODO: Insert instead of add+sort
        UpdateLevels();
        window->SetDirty();
    }
    
    void Window::RemoveChild(Window *window)
    {
        window->SetDirty();
        _children.Remove(window);
        window->_parent = NULL;
    }
    
    void Window::SetLevel(SInt32 level)
    {
        if (level == _level)
            return;
        _level = level;
        if (_parent)
            _parent->UpdateLevels();
        // TODO: Make this efficient
        SetDirty();
    }
    
    SInt32 Window::Level(void) const
    {
        return _level;
    }
    
    void Window::UpdateLevels(void)
    {
        Library::Sort(_children.Start(), _children.End(), [](Window *a, Window *b){
            return a->_level < b->_level;
        });
    }
    
    void Window::HitTest(Library::Array<const Window*> &result, Graphics::Point2D location) const
    {
        if (!Frame().Contains(location))
            return;
        result.Add(this);
        location.x -= _frame.origin.x;
        location.y -= _frame.origin.y;
        Graphics::Point2D transformed = _transform.Apply(location);
        Library::ForEach(_children.Reverse(), [&](Window *window){
            window->HitTest(result, transformed);
            return true;
        });
    }
    static void TransformSearch(Graphics::Matrix2D &result, const Window *entry)
    {
        if (!entry)
            return;
        // Seek up to topmost window
        TransformSearch(result, entry->Parent());
        // Translate
        Graphics::Frame2D frame = entry->Frame();
        Graphics::Unit matrix[] = {
            1, 0, frame.origin.x,
            0, 1, frame.origin.y,
            0, 0, 1
        };
        result *= Graphics::Matrix2D(matrix);
        // Apply new transform
        result *= entry->Transform();
    }
    Graphics::Matrix2D Window::FullTransform(void) const
    {
        Graphics::Matrix2D result(true);
        TransformSearch(result, this);
        return result;
    }
    bool Window::TouchDown(Graphics::Point2D position, int button)
    {
        return false;//TODO: "true" to eat touches
    }
    void Window::TouchDrag(Graphics::Point2D position, int button)
    {
        // For subclasses to implement
    }
    void Window::TouchUp(Graphics::Point2D position, int button)
    {
        // For subclasses to implement
    }
    
    BitmapWindow::BitmapWindow(Graphics::Frame2D frame)
    :Window(frame), _bitmap(NULL), _mode(StretchNone)
    {
    }
    
    BitmapWindow::BitmapWindow(const Graphics::FrameBuffer &buffer)
    :Window((Graphics::Frame2D){{0, 0}, {Graphics::Unit(buffer.Width()), Graphics::Unit(buffer.Height())}}), _bitmap(&buffer), _mode(StretchNone)
    {
    }
    
    void BitmapWindow::SetBitmap(const Graphics::FrameBuffer &buffer)
    {
        _bitmap = &buffer;
    }
    
    const Graphics::FrameBuffer& BitmapWindow::Bitmap(void) const
    {
        return *_bitmap;
    }
    
    void BitmapWindow::SetStretchMode(Mode mode)
    {
        if (_mode == mode)
            return;
        _mode = mode;
        SetDirty();
    }
    
    BitmapWindow::Mode BitmapWindow::StretchMode(void)
    {
        return _mode;
    }

    void BitmapWindow::Draw(Graphics::Context &context, Graphics::Rect2D region)
    {
        // TODO: make "region" work for any mode
        context.Push();
        switch(_mode) {
            case StretchFit:
            {
                Graphics::Frame2D bounds = Bounds();
                Graphics::Point2D size = (Graphics::Point2D){Graphics::Unit(_bitmap->Width()), Graphics::Unit(_bitmap->Height())};
                Graphics::Unit scale = bounds.size.x / size.x;
                if ((size.y * scale) > bounds.size.y)
                    scale *= bounds.size.y / (size.y * scale);
                context.Scale(scale, scale);
            }
                break;
            case StretchFill:
            {
                Graphics::Frame2D bounds = Bounds();
                context.Scale(Graphics::Unit(_bitmap->Width()) / bounds.size.x, Graphics::Unit(_bitmap->Height()) / bounds.size.y);
            }
                break;
            default:
                break;
        }
        context.DrawBitmap(Graphics::Path::Rect(Convert(Bounds())/*.Intersection(region)*/), *_bitmap);
        context.Pop();
    }

    ColourWindow::ColourWindow(Graphics::Frame2D frame)
    :Window(frame), _colour(Graphics::Colour::White)
    {
    }
    ColourWindow::ColourWindow(Graphics::Frame2D frame, const Graphics::Colour &colour)
    :Window(frame), _colour(colour)
    {
    }
    
    void ColourWindow::SetColour(const Graphics::Colour &colour)
    {
        _colour = colour;
        SetDirty(Convert(Bounds()));
    }
    const Graphics::Colour& ColourWindow::Colour(void) const
    {
        return _colour;
    }
    
    void ColourWindow::Draw(Graphics::Context &context, Graphics::Rect2D region)
    {
        context.DrawPolygon(Graphics::Path::Rect(Convert(Bounds())/*.Intersection(region)*/), _colour);
    }
    bool ColourWindow::IsOpaque(void)
    {
        return _colour.alpha == 0xFF;
    }
    
}

