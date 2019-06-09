#ifndef __IMAGEBASE_H__
#define __IMAGEBASE_H__

class ImageDecoder
{
public:
    typedef struct {
        int width, height;
    } Format;

    class Handle
    {
    public:
        virtual ~Handle() {}
        
        virtual Format GetFormat(void) = 0;
        
        virtual void SetScale(double scale) = 0;
        virtual double Scale(void) = 0;
        
        virtual void GetData(int x, int y, int w, int h, void *output) = 0;
    };
    
    virtual ~ImageDecoder() {}
    
    virtual Handle* CreateHandle(void) = 0;
};

extern "C" ImageDecoder* AttemptParseImage(const UInt8 *data, UInt32 length);

#endif // __IMAGEBASE_H__
