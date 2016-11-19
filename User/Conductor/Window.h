//
//  Window.h
//  TestOSGraphics
//
//  Created by Colin David Munro on 8/10/2016.
//  Copyright © 2016 MICE Software. All rights reserved.
//

#ifndef Window_hpp
#define Window_hpp

#include "Context.h"

namespace Window {

    class Window
    {
    public:
        Window(Graphics::Frame2D frame):_transform(true),_frame(frame),_parent(NULL),_level(0),_hidden(false){}
        virtual ~Window(); // any windows left in vector will be deleted
        
        // Frame is parent coordinates
        void SetFrame(Graphics::Frame2D frame);
        const Graphics::Frame2D& Frame(void) const { return _frame; }
        
        // Bounds is own coordinates
        void SetBounds(Graphics::Frame2D bounds);
        const Graphics::Frame2D Bounds(void) const;
        
        void SetTransform(const Graphics::Matrix2D& transform);
        const Graphics::Matrix2D& Transform(void) const { return _transform; }
        
        void SetDirty(void);
        virtual void SetDirty(const Graphics::Rect2D &area);
        void DrawAll(Graphics::Context &context, Graphics::Rect2D region);
        
        Window* Parent(void) const { return _parent; }
        void AddChild(Window *window);
        void RemoveChild(Window *window);
        
        void SetHidden(bool hidden);
        bool Hidden(void) const { return _hidden; }
        
        void SetLevel(SInt32 level);
        SInt32 Level(void) const;
        
        virtual void SetFocused(bool focus) {}
        
        const Library::Array<Window*>& Children(void) const { return _children; }
        
        void HitTest(Library::Array<const Window*> &result, Graphics::Point2D location) const;
        Graphics::Matrix2D FullTransform(void) const;
        
        virtual bool TouchDown(Graphics::Point2D position, int button); // Return true if you want to handle this touch - false will fall through
        virtual void TouchDrag(Graphics::Point2D position, int button);
        virtual void TouchUp(Graphics::Point2D position, int button);
        
    protected:
        virtual void Draw(Graphics::Context &context, Graphics::Rect2D region) {/*for subclasses to implement*/}
        virtual bool IsOpaque(void) {return false;}
        virtual void Resized(void) {}
        virtual bool WantTouch(Graphics::Point2D location) const { return true; }
        
        void SetLevelWithoutDirty(SInt32 level) { _level = level; }
        
    private:
        void UpdateLevels();
        
        Graphics::Frame2D _frame;
        Window *_parent;
        SInt32 _level;
        Library::Array<Window*> _children;
        Graphics::Matrix2D _transform;
        bool _hidden;
    };
    
    class BitmapWindow : public Window
    {
    public:
        typedef enum {
            StretchNone,
            StretchFill,
            StretchFit,
        } Mode;
        
        BitmapWindow(Graphics::Frame2D frame);
        BitmapWindow(const Graphics::FrameBuffer &buffer);
        
        void SetBitmap(const Graphics::FrameBuffer &buffer);
        const Graphics::FrameBuffer& Bitmap(void) const;
        
        void SetStretchMode(Mode mode);
        Mode StretchMode(void);
        
        void SetTint(const Graphics::Colour &tint);
        const Graphics::Colour& Tint(void) const { return _tint; }
        
    protected:
        void Draw(Graphics::Context &context, Graphics::Rect2D region);
        
    private:
        const Graphics::FrameBuffer *_bitmap;
        Mode _mode;
        Graphics::Colour _tint;
    };
    
    class ColourWindow : public Window
    {
    public:
        ColourWindow(Graphics::Frame2D frame);
        ColourWindow(Graphics::Frame2D frame, const Graphics::Colour &colour);
        
        void SetColour(const Graphics::Colour &colour);
        const Graphics::Colour& Colour(void) const;
        
    protected:
        void Draw(Graphics::Context &context, Graphics::Rect2D region);
        bool IsOpaque(void);
        
    private:
        Graphics::Colour _colour;
    };
    
    template<class Background> class Desktop : public Background
    {
    public:
        Desktop(Graphics::FrameBuffer &output)
        :Background((Graphics::Frame2D){{0, 0}, {Graphics::Unit(output.Width()), Graphics::Unit(output.Height())}}), _output(output), _dirty(false)
        {
            Background::SetDirty();
        }
        
        void SetDirty(const Graphics::Rect2D &area)
        {
            if (_dirty) {
                _dirtyRect = _dirtyRect.Union(area);
            } else {
                _dirty = true;
                _dirtyRect = area;
            }
        }
        
        void Update(void)
        {
            if (!_dirty)
                return;
            Graphics::Context context(_output);
            this->DrawAll(context, _dirtyRect);
//            context.DrawPolygon(Graphics::Path::Rect(_dirtyRect), (Graphics::Colour){0xFF,0x00,0x00,0xa0});
            _dirty = false;
        }
        
        void TouchDown(int button, Graphics::Point2D location)
        {
            Library::Array<const Window*> result;
            this->HitTest(result, location);
            const Window *newTop = NULL;
            int rootCount = 0;
            Library::ForEach(result, [&](const Window *window){
                if (window == _mousePointer)
                    return true;
                // Yuck - de-const :(
                if (((Window*)window)->TouchDown(window->FullTransform().Invert().Apply(location), button)) {
                    _touches[button] = (Window*)window;
                    return false;
                }
                if (window->Parent() == this) {
                    rootCount++;
                    if (rootCount == 1)
                        newTop = window;
                    else if (rootCount == 2)
                        return false;
                }
                return true;
            });
#define LEVEL_MIN   0
#define LEVEL_MAX   1000000
            if (newTop) {
                SInt32 oldLevel = newTop->Level();
                if ((oldLevel >= LEVEL_MIN) && (oldLevel <= LEVEL_MAX)) {
                    Library::Array<const Window*> adjustableChildren;
                    Library::ForEach(this->Children(), [&](const Window *window){
                        SInt32 level = window->Level();
                        if ((window != newTop) && ((level >= LEVEL_MIN) && (level <= LEVEL_MAX)))
                            adjustableChildren.Add(window);
                        return true;
                    });
                    SInt32 levelGap = (LEVEL_MAX - LEVEL_MIN) / (adjustableChildren.Count() + 2);
                    SInt32 count = levelGap;
                    Library::ForEach(adjustableChildren, [&](const Window *window){
                        ((Window*)window)->SetFocused(false);
                        ((Window*)window)->SetLevelWithoutDirty(count);
                        count += levelGap;
                        return true;
                    });
                    ((Window*)newTop)->SetLevel(count);
                    ((Window*)newTop)->SetFocused(true);
                }
            }
        }
        void TouchMove(int button, Graphics::Point2D location)
        {
            if (_mousePointer) {
                Graphics::Frame2D frame = _mousePointer->Frame();
                frame.origin.x = location.x - _mouseHotspot.x;
                frame.origin.y = location.y - _mouseHotspot.y;
                _mousePointer->SetFrame(frame);
            }
            Window *window = _touches[button];
            if (window)
                window->TouchDrag(window->FullTransform().Invert().Apply(location), button);
        }
        void TouchUp(int button, Graphics::Point2D location)
        {
            Window *window = _touches[button];
            if (window) {
                window->TouchUp(window->FullTransform().Invert().Apply(location), button);
                _touches.Reset(button);
            }
        }
        
        void SetMouseCursor(Window *cursor, Graphics::Point2D hotspot)
        {
            _mousePointer = cursor;
            _mouseHotspot = hotspot;
        }
        
    private:
        Graphics::FrameBuffer &_output;
        bool _dirty;
        Graphics::Rect2D _dirtyRect;
        Library::Dictionary<int, Window*> _touches;
        // TODO: Remove windows from _touches when they're removed from the desktop
        Window *_mousePointer;
        Graphics::Point2D _mouseHotspot;
    };
}

#endif /* Window_hpp */