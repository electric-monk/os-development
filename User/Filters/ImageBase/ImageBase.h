#ifndef __IMAGEBASE_H__
#define __IMAGEBASE_H__

class ImageDecoder
{
public:
    virtual ~ImageDecoder() {}
};

extern "C" ImageDecoder* AttemptParseImage(const UInt8 *data, UInt32 length);

#endif // __IMAGEBASE_H__
