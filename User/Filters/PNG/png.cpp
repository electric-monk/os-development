#include "Library.h"
#include "../ImageBase/ImageBase.h"

// Code based on UPNG but refactored

typedef UInt8 PngByte;
typedef UInt32 PngDWORD;
typedef UInt32 PngBitPointer;
typedef UInt8 PngData;

template<class IntType> PngByte AsByte(IntType amount)
{
    return PngByte(amount & 0xFF);
}

static inline PngDWORD AsDWORD(UInt8 a, UInt8 b, UInt8 c, UInt8 d)
{
    return (AsByte(a) << 24) | (AsByte(b) << 16) | (AsByte(c) << 8) | AsByte(d);
}

static inline PngDWORD AsDWORDFromPointer(const UInt8 *array)
{
    return AsDWORD(array[0], array[1], array[2], array[3]);
}

const PngDWORD ChunkIHDR = AsDWORD('I', 'H', 'D', 'R');
const PngDWORD ChunkIDAT = AsDWORD('I', 'D', 'A', 'T');
const PngDWORD ChunkIEND = AsDWORD('I', 'E', 'N', 'D');

static inline UInt32 ChunkLength(const UInt8 *chunk)
{
    return AsDWORDFromPointer(chunk);
}
static inline UInt32 ChunkType(const UInt8 *chunk)
{
    return AsDWORDFromPointer(chunk + 4);
}
static inline bool ChunkIsCritical(const UInt8 *chunk)
{
    return (chunk[4] & 32) == 0;
}

const UInt32 FirstLengthCodeIndex = 257;
const UInt32 LastLengthCodeIndex = 285;
const UInt32 LengthBase[29] = {
    3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59,
    67, 83, 99, 115, 131, 163, 195, 227, 258
};
const UInt32 LengthExtra[29] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5,
    5, 5, 5, 0
};

const UInt32 DistanceBase[30] = {
    1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513,
    769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577
};
const UInt32 DistanceExtra[30] = {
    0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10,
    11, 11, 12, 12, 13, 13
};

const UInt32 NumDeflateCodeSymbols = 288;
const UInt32 FixedDeflateCodeTree[NumDeflateCodeSymbols * 2] = {
    289, 370, 290, 307, 546, 291, 561, 292, 293, 300, 294, 297, 295, 296, 0, 1,
    2, 3, 298, 299, 4, 5, 6, 7, 301, 304, 302, 303, 8, 9, 10, 11, 305, 306, 12,
    13, 14, 15, 308, 339, 309, 324, 310, 317, 311, 314, 312, 313, 16, 17, 18,
    19, 315, 316, 20, 21, 22, 23, 318, 321, 319, 320, 24, 25, 26, 27, 322, 323,
    28, 29, 30, 31, 325, 332, 326, 329, 327, 328, 32, 33, 34, 35, 330, 331, 36,
    37, 38, 39, 333, 336, 334, 335, 40, 41, 42, 43, 337, 338, 44, 45, 46, 47,
    340, 355, 341, 348, 342, 345, 343, 344, 48, 49, 50, 51, 346, 347, 52, 53,
    54, 55, 349, 352, 350, 351, 56, 57, 58, 59, 353, 354, 60, 61, 62, 63, 356,
    363, 357, 360, 358, 359, 64, 65, 66, 67, 361, 362, 68, 69, 70, 71, 364,
    367, 365, 366, 72, 73, 74, 75, 368, 369, 76, 77, 78, 79, 371, 434, 372,
    403, 373, 388, 374, 381, 375, 378, 376, 377, 80, 81, 82, 83, 379, 380, 84,
    85, 86, 87, 382, 385, 383, 384, 88, 89, 90, 91, 386, 387, 92, 93, 94, 95,
    389, 396, 390, 393, 391, 392, 96, 97, 98, 99, 394, 395, 100, 101, 102, 103,
    397, 400, 398, 399, 104, 105, 106, 107, 401, 402, 108, 109, 110, 111, 404,
    419, 405, 412, 406, 409, 407, 408, 112, 113, 114, 115, 410, 411, 116, 117,
    118, 119, 413, 416, 414, 415, 120, 121, 122, 123, 417, 418, 124, 125, 126,
    127, 420, 427, 421, 424, 422, 423, 128, 129, 130, 131, 425, 426, 132, 133,
    134, 135, 428, 431, 429, 430, 136, 137, 138, 139, 432, 433, 140, 141, 142,
    143, 435, 483, 436, 452, 568, 437, 438, 445, 439, 442, 440, 441, 144, 145,
    146, 147, 443, 444, 148, 149, 150, 151, 446, 449, 447, 448, 152, 153, 154,
    155, 450, 451, 156, 157, 158, 159, 453, 468, 454, 461, 455, 458, 456, 457,
    160, 161, 162, 163, 459, 460, 164, 165, 166, 167, 462, 465, 463, 464, 168,
    169, 170, 171, 466, 467, 172, 173, 174, 175, 469, 476, 470, 473, 471, 472,
    176, 177, 178, 179, 474, 475, 180, 181, 182, 183, 477, 480, 478, 479, 184,
    185, 186, 187, 481, 482, 188, 189, 190, 191, 484, 515, 485, 500, 486, 493,
    487, 490, 488, 489, 192, 193, 194, 195, 491, 492, 196, 197, 198, 199, 494,
    497, 495, 496, 200, 201, 202, 203, 498, 499, 204, 205, 206, 207, 501, 508,
    502, 505, 503, 504, 208, 209, 210, 211, 506, 507, 212, 213, 214, 215, 509,
    512, 510, 511, 216, 217, 218, 219, 513, 514, 220, 221, 222, 223, 516, 531,
    517, 524, 518, 521, 519, 520, 224, 225, 226, 227, 522, 523, 228, 229, 230,
    231, 525, 528, 526, 527, 232, 233, 234, 235, 529, 530, 236, 237, 238, 239,
    532, 539, 533, 536, 534, 535, 240, 241, 242, 243, 537, 538, 244, 245, 246,
    247, 540, 543, 541, 542, 248, 249, 250, 251, 544, 545, 252, 253, 254, 255,
    547, 554, 548, 551, 549, 550, 256, 257, 258, 259, 552, 553, 260, 261, 262,
    263, 555, 558, 556, 557, 264, 265, 266, 267, 559, 560, 268, 269, 270, 271,
    562, 565, 563, 564, 272, 273, 274, 275, 566, 567, 276, 277, 278, 279, 569,
    572, 570, 571, 280, 281, 282, 283, 573, 574, 284, 285, 286, 287, 0, 0
};

const UInt32 NumDistanceSymbols = 32;
const UInt32 FixedDistanceTree[NumDistanceSymbols * 2] = {
    33, 48, 34, 41, 35, 38, 36, 37, 0, 1, 2, 3, 39, 40, 4, 5, 6, 7, 42, 45, 43,
    44, 8, 9, 10, 11, 46, 47, 12, 13, 14, 15, 49, 56, 50, 53, 51, 52, 16, 17,
    18, 19, 54, 55, 20, 21, 22, 23, 57, 60, 58, 59, 24, 25, 26, 27, 61, 62, 28,
    29, 30, 31, 0, 0
};

const UInt32 DeflateCodeBitlen = 15;
const UInt32 DistanceBitlen = 15;
const UInt32 CodeLengthBitlen = 7;
const UInt32 NumCodeLengthCodes = 19;
const UInt32 MaxSymbols = 288;
const UInt32 MaxBitLength = 15;

const UInt32 CLCL[NumCodeLengthCodes] = {
    16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
};

static bool ReadBit(PngBitPointer &bitpointer, const PngData *bitstream)
{
    UInt8 result = UInt8((bitstream[bitpointer >> 3] >> (bitpointer & 0x07)) & 0x01);
    bitpointer++;
    return result != 0;
}

template<class IntType> static IntType ReadBits(PngBitPointer &bitpointer, const PngData *bitstream, UInt32 numberBits)
{
    IntType result = 0;
    for (UInt32 i = 0; i < numberBits; i++) {
        if (ReadBit(bitpointer, bitstream))
            result |= 1 << i;
    }
    return result;
}

class Png
{
public:
    typedef enum {
        NoError = 0,
        Malformed,
        NotPNG,
        NotSupported,
        UnknownFormat,
        NoMemory,
    } PngError;
    
    typedef enum {
        colourInvalid = -1,
        colourLuminance = 0,
        colourRGB = 2,
        colourLuminanceAlpha = 4,
        colourRGBA = 6,
    } PngColourType;
    
    Png()
    {
    }
    
    ~Png()
    {
        Reset();
    }
    
    void SetError(PngError error, int subcode)
    {
        _error = error;
    }
    
    bool IsOkay(void)
    {
        return _error == NoError;
    }
    
    void Reset(void)
    {
        _error = NoError;
        _colourType = colourInvalid;
        _colourDepth = 0;
    }
    
    void Load(const UInt8 *data, UInt32 length)
    {
        Reset();
        // Check header
        if (length < 29) {
            SetError(NotPNG, 0);
            return;
        }
        const UInt8 headerData[] = {137, 80, 78, 71, 13, 10, 26, 10};
        if (CompareMemory(data, headerData, sizeof(headerData)) != 0) {
            SetError(NotPNG, 1);
            return;
        }
        if (AsDWORDFromPointer(data + 12) != ChunkIHDR) {
            SetError(Malformed, 28);
            return;
        }
        _width = AsDWORDFromPointer(data + 16);
        _height = AsDWORDFromPointer(data + 20);
        _colourDepth = data[24];
        _colourType = PngColourType(data[25]);
        if (!CheckFormat()) {
            SetError(UnknownFormat, 0);
            return;
        }
        if (data[26] != 0) {
            SetError(Malformed, 29);
            return;
        }
        if (data[27] != 0) {
            SetError(Malformed, 30);
            return;
        }
        if (data[28] != 0) {
            SetError(NotSupported, 0);
            return;
        }
        // Decode
        UInt32 compressedSize = 0;
        for (const UInt8 *chunk = data + 33; chunk < (data + length); chunk += ChunkLength(chunk) + 12) {
            if ((chunk - data + 12) > length) {
                SetError(Malformed, 30);
                return;
            }
            UInt32 chunkLength = ChunkLength(chunk);
            if ((chunk - data + chunkLength) > length) {
                SetError(Malformed, 31);
                return;
            }
            const UInt8 *data = chunk + 8;
            UInt32 chunkType = ChunkType(chunk);
            if (chunkType == ChunkIDAT)
                compressedSize += length;
            else if (chunkType == ChunkIEND)
                break;
            else if (ChunkIsCritical(chunk)) {
                SetError(NotSupported, 1);
                return;
            }
        }
        UInt8 *compressed = new UInt8[compressedSize];
        if (compressed == NULL) {
            SetError(NoMemory, 0);
            return;
        }
        UInt32 compressedIndex = 0;
        for (const UInt8 *chunk = data + 33; chunk < (data + length); chunk += ChunkLength(chunk) + 12) {
            UInt32 chunkLength = ChunkLength(chunk);
            const UInt8 *chunkData = chunk + 8;
            UInt32 chunkType = ChunkType(chunk);
            if (chunkType == ChunkIDAT) {
                CopyMemory(compressed + compressedIndex, chunkData, chunkLength);
                compressedIndex += chunkLength;
            } else if (chunkType == ChunkIEND) {
                break;
            }
        }
        UInt32 inflatedSize = ((_width * (_height * BitsPerPixel() + 7)) / 8) + _height;
        UInt8 *inflated = new UInt8[inflatedSize];
        if (inflatedSize == NULL) {
            delete[] compressed;
            SetError(NoMemory, 1);
            return;
        }
        uzInflate(compressed, compressedSize, inflated, inflatedSize);
        delete[] compressed;
        if (!IsOkay()) {
            delete[] inflated;
            return;
        }
        _bufferSize = (_height * _width * BitsPerPixel() + 7) / 8;
        _buffer = new UInt8[_bufferSize];
        if (_buffer == NULL) {
            delete[] inflated;
            _bufferSize = 0;
            SetError(NoMemory, 2);
            return;
        }
        PostProcessScanlines(inflated, _buffer);
        delete[] inflated;
        if (!IsOkay()) {
            delete[] _buffer;
            _buffer = NULL;
            _bufferSize = 0;
        }
    }
    
    PngColourType ColourType(void)
    {
        // use ColourTypeComponents() to validate
        return ColourTypeComponents() ? _colourType : colourInvalid;
    }
    
    int ColourTypeComponents(void)
    {
        switch (_colourType) {
            case colourRGB:
                return 3;
            case colourRGBA:
                return 4;
            case colourLuminance:
                return 1;
            case colourLuminanceAlpha:
                return 2;
            default:
                return 0;
        }
    }
    
private:
    void InflateHuffman(const UInt8 *in, UInt32 inLength, UInt8 *out, UInt32 outSize, PngBitPointer &bitPointer, UInt32 &pos, unsigned btype);
    
    void InflateUncompressed(const UInt8 *in, UInt32 inLength, UInt8 *out, UInt32 outSize, PngBitPointer &bitPointer, UInt32 &pos)
    {
        while (bitPointer & 0x07)
            bitPointer++;
        UInt32 p = bitPointer / 8;
        if (p >= (inLength - 4)) {
            SetError(Malformed, 16);
            return;
        }
        unsigned len = in[p] + 256 * in[p + 1];
        p += 2;
        unsigned nlen = in[p] + 256 * in[p + 1];
        p += 2;
        if ((len + nlen) != 65535) {
            SetError(Malformed, 17);
            return;
        }
        if ((pos + len) >= outSize) {
            SetError(Malformed, 18);
            return;
        }
        if ((p + len) > inLength) {
            SetError(Malformed, 19);
            return;
        }
        for (unsigned n = 0; n < len; n++)
            out[pos++] = in[p++];
        bitPointer = p * 8;
    }
    
    void Inflate(const UInt8 *in, UInt32 inLength, UInt8 *out, UInt32 outSize, PngBitPointer &bitPointer, UInt32 &pos, unsigned btype)
    {
        if (btype == 0)
            InflateUncompressed(in, inLength, out, outSize, bitPointer, pos);
        else
            InflateHuffman(in, inLength, out, outSize, bitPointer, pos, btype);
    }
    
    void uzInflateData(const UInt8 *in, UInt32 inLength, UInt32 inPos, UInt8 *out, UInt32 outSize)
    {
        bool done = false;
        UInt32 pos = 0;
        UInt32 bitPointer = 0;
        while (!done) {
            if ((bitPointer >> 3) >= inLength) {
                SetError(Malformed, 20);
                return;
            }
            done = ReadBit(bitPointer, in + inPos);
            UInt32 btype = ReadBit(bitPointer, in + inPos) | (ReadBit(bitPointer, in + inPos) << 1);
            Inflate(in + inPos, inLength, out, outSize, bitPointer, pos, btype);
            if (!IsOkay())
                return;
        }
    }
    
    void uzInflate(const UInt8 *in, UInt32 inLength, UInt8 *out, UInt32 outSize)
    {
        if (inLength < 2) {
            SetError(Malformed, 22);
            return;
        }
        if ((in[0] * 256 + in[1]) % 31 != 0) {
            SetError(Malformed, 23);
            return;
        }
        if ((in[0] & 15) != 8 || ((in[0] >> 4) & 15) > 7) {
            SetError(Malformed, 24);
            return;
        }
        if (((in[1] >> 5) & 1) != 0) {
            SetError(Malformed, 25);
            return;
        }
        uzInflateData(in, inLength, 2, out, outSize);
    }
    
    int Paeth_Predictor(int a, int b, int c)
    {
        int p = a + b - c;
        int pa = p > a ? p - a : a - p;
        int pb = p > b ? p - b : b - p;
        int pc = p > c ? p - c : c - p;
        if (pa <= pb && pa <= pc)
            return a;
        else if (pb <= pc)
            return b;
        else
            return c;
    }

    void UnfilterScanline(UInt8 *recon, const UInt8 *scanline, const UInt8 *precon, UInt32 byteWidth, UInt8 filterType, UInt32 length)
    {
        switch (filterType) {
            case 0:
                for (UInt32 i = 0; i < length; i++)
                    recon[i] = scanline[i];
                break;
            case 1:
                for (UInt32 i = 0; i < byteWidth; i++)
                    recon[i] = scanline[i];
                for (UInt32 i = byteWidth; i < length; i++)
                    recon[i] = scanline[i] + recon[i - byteWidth];
                break;
            case 2:
                if (precon) {
                    for (UInt32 i = 0; i < length; i++)
                        recon[i] = scanline[i] + precon[i];
                } else {
                    for (UInt32 i = 0; i < length; i++)
                        recon[i] = scanline[i];
                }
                break;
            case 3:
                if (precon) {
                    for (UInt32 i = 0; i < byteWidth; i++)
                        recon[i] = scanline[i] + precon[i] / 2;
                    for (UInt32 i = byteWidth; i < length; i++)
                        recon[i] = scanline[i] + ((recon[i - byteWidth] + precon[i]) / 2);
                } else {
                    for (UInt32 i = 0; i < byteWidth; i++)
                        recon[i] = scanline[i];
                    for (UInt32 i = byteWidth; i < length; i++)
                        recon[i] = scanline[i] + recon[i - byteWidth] / 2;
                }
                break;
            case 4:
                if (precon) {
                    for (UInt32 i = 0; i < byteWidth; i++)
                        recon[i] = UInt8(scanline[i] + Paeth_Predictor(0, precon[i], 0));
                    for (UInt32 i = byteWidth; i < length; i++)
                        recon[i] = UInt8(scanline[i] + Paeth_Predictor(recon[i - byteWidth], precon[i], precon[i - byteWidth]));
                } else {
                    for (UInt32 i = 0; i < byteWidth; i++)
                        recon[i] = scanline[i];
                    for (UInt32 i = byteWidth; i < length; i++)
                        recon[i] = UInt8(scanline[i] + Paeth_Predictor(recon[i - byteWidth], 0, 0));
                }
                break;
            default:
                SetError(Malformed, 26);
                break;
        }
    }
    
    void Unfilter(const UInt8 *in, UInt8 *out)
    {
        UInt8 *previousLine = NULL;
        UInt32 bitsPerPixel = BitsPerPixel();
        UInt32 byteWidth = (bitsPerPixel + 7) / 8;
        UInt32 lineBytes = (_width * bitsPerPixel + 7) / 8;
        for (UInt32 y = 0; y < _height; y++) {
            UInt32 outIndex = lineBytes * y;
            UInt32 inIndex = (1 + lineBytes) * y;
            UInt8 filterType = in[inIndex];
            UInt8 *currentLine = out + outIndex;
            UnfilterScanline(currentLine, in + inIndex + 1, previousLine, byteWidth, filterType, lineBytes);
            if (!IsOkay())
                return;
            previousLine = currentLine;
        }
    }
    
    static void RemovePaddingBits(const UInt8 *in, UInt8 *out, UInt32 iLineBits, UInt32 oLineBits, UInt32 height)
    {
        UInt32 diff = iLineBits - oLineBits;
        UInt32 oBitPointer = 0;
        UInt32 iBitPointer = 0;
        for (UInt32 y = 0; y < height; y++) {
            for (UInt32 x = 0; x < oLineBits; x++) {
                UInt8 outBit = UInt8(1 << (7 - (oBitPointer & 0x07)));
                if ((in[iBitPointer >> 3] >> (7 - (iBitPointer & 0x07))) & 0x01)
                    out[oBitPointer >> 3] |= outBit;
                else
                    out[oBitPointer >> 3] &= ~outBit;
                iBitPointer++;
                oBitPointer++;
            }
            iBitPointer += diff;
        }
    }
    
    void PostProcessScanlines(UInt8 *in, UInt8 *out)
    {
        UInt32 bitsPerPixel = BitsPerPixel();
        if (bitsPerPixel == 0) {
            SetError(Malformed, 27);
            return;
        }
        if ((bitsPerPixel < 8) && ((_width * bitsPerPixel) != (((_width * bitsPerPixel) + 7) / 8) * 8)) {
            Unfilter(in, in);
            RemovePaddingBits(in, out, ((_width * bitsPerPixel + 7) / 8) * 8, _width * bitsPerPixel, _height);
        } else {
            Unfilter(in, out);
        }
    }
    
    UInt32 BitsPerPixel(void)
    {
        return ColourTypeComponents() * _colourDepth;
    }
    
    bool CheckFormat(void)
    {
#define IS(num) (_colourDepth == num)
        switch (_colourType) {
            case colourLuminance:
            case colourLuminanceAlpha:
                return IS(1) || IS(2) || IS(4) || IS(8);
            case colourRGB:
            case colourRGBA:
                return IS(8) || IS(16);
            default:
                return false;
        }
    }
    
private:
    PngError _error;
    UInt32 _width, _height;
    PngColourType _colourType;
    UInt16 _colourDepth;
    UInt8 *_buffer;
    UInt32 _bufferSize;
};

class HuffmanTreeBase
{
public:
    virtual void CreateLengths(Png *png, const unsigned *bitLengths) = 0;
};
template<UInt32 _numberCodes, UInt32 _maxBitLength> class HuffmanTree : public HuffmanTreeBase
{
private:
    UInt32 _tree2D[_numberCodes * 2];
public:
    HuffmanTree()
    {
    }
    
    void Load(const UInt32 *data)
    {
        CopyMemory(_tree2D, data, (_numberCodes * 2) * sizeof(UInt32));
    }
    
    void CreateLengths(Png *png, const unsigned *bitLengths)
    {
        UInt32 tree1D[MaxSymbols];
        unsigned blCount[MaxBitLength];
        unsigned nextCode[MaxBitLength + 1];
        unsigned nodeFilled = 0;
        unsigned treePos = 0;
        
        ClearMemory(blCount, sizeof(blCount));
        ClearMemory(nextCode, sizeof(nextCode));
        for (unsigned bits = 0; bits < _numberCodes; bits++)
            blCount[bitLengths[bits]]++;
        for (unsigned bits = 1; bits <= _maxBitLength; bits++)
            nextCode[bits] = (nextCode[bits - 1] + blCount[bits - 1]) << 1;
        for (unsigned n = 0; n < _numberCodes; n++) {
            if (bitLengths[n] != 0)
                tree1D[n] = nextCode[bitLengths[n]]++;
        }
        for (unsigned n = 0, nMax = _numberCodes * 2; n < nMax; n++)
            _tree2D[n] = 32767;
        for (unsigned n = 0; n < _numberCodes; n++) {
            for (unsigned i = 0; i < bitLengths[n]; i++) {
                UInt8 bit = (tree1D[n] >> (bitLengths[n] - i - 1)) & 0x01;
                if (treePos > (_numberCodes - 2)) {
                    png->SetError(Png::Malformed, 0);
                    return;
                }
                if (_tree2D[2 * treePos + bit] == 32767) {
                    if (i + 1 == bitLengths[n]) {
                        _tree2D[2 * treePos + bit] = n;
                        treePos = 0;
                    } else {
                        nodeFilled++;
                        _tree2D[2 * treePos + bit] = nodeFilled + _numberCodes;
                        treePos = nodeFilled;
                    }
                } else {
                    treePos = _tree2D[2 * treePos + bit] - _numberCodes;
                }
            }
        }
        for (unsigned n = 0, nMax = _numberCodes * 2; n < nMax; n++) {
            if (_tree2D[n] == 32767)
                _tree2D[n] = 0;
        }
    }
    
    unsigned DecodeSymbol(Png *png, const UInt8 *in, UInt32 inLength, UInt32 &bitPointer) const
    {
        unsigned treePos = 0;
        while (true) {
            if (((bitPointer & 0x07) == 0) && ((bitPointer >> 3) > inLength)) {
                png->SetError(Png::Malformed, 1);
                return 0;
            }
            UInt8 bit = ReadBit(bitPointer, in);
            unsigned ct = _tree2D[(treePos << 1) | bit];
            if (ct < _numberCodes)
                return ct;
            treePos = ct - _numberCodes;
            if (treePos >= _numberCodes) {
                png->SetError(Png::Malformed, 2);
                return 0;
            }
        }
    }
    
    void InflateDynamic(Png *png, HuffmanTreeBase &codeTree, HuffmanTreeBase &codeTreeD, const UInt8 *in, UInt32 inLength, UInt32 &bitPointer)
    {
        if ((bitPointer >> 3) >= (inLength - 2)) {
            png->SetError(Png::Malformed, 3);
            return;
        }
        unsigned bitLengths[NumDeflateCodeSymbols];
        unsigned bitLengthsD[NumDistanceSymbols];
        unsigned codeLengthCode[NumCodeLengthCodes];
        ClearMemory(bitLengths, sizeof(bitLengths));
        ClearMemory(bitLengthsD, sizeof(bitLengthsD));
        unsigned hLit = ReadBits<unsigned>(bitPointer, in, 5) + 257;
        unsigned hDist = ReadBits<unsigned>(bitPointer, in, 5) + 1;
        unsigned hCLen = ReadBits<unsigned>(bitPointer, in, 4) + 4;
        for (UInt32 i = 0; i < NumCodeLengthCodes; i++)
            codeLengthCode[CLCL[i]] = (i < hCLen) ? ReadBits<unsigned>(bitPointer, in, 3) : 0;
        CreateLengths(png, codeLengthCode);
        if (!png->IsOkay())
            return;
        for (UInt32 i = 0; i < (hLit + hDist);) {
            unsigned code = DecodeSymbol(png, in, inLength, bitPointer);
            if (!png->IsOkay())
                return;
            if (code <= 15) {
                if (i < hLit)
                    bitLengths[i] = code;
                else
                    bitLengthsD[i - hLit] = code;
                i++;
            } else if (code == 16) {
                unsigned repLength = 3;
                unsigned value;
                if ((bitPointer >> 3) >= inLength) {
                    png->SetError(Png::Malformed, 4);
                    break;
                }
                repLength += ReadBits<unsigned>(bitPointer, in, 2);
                value = ((i - 1) < hLit) ? bitLengths[i - 1] : bitLengthsD[i - hLit - 1];
                for (UInt32 n = 0; n < repLength; n++) {
                    if (i >= (hLit + hDist)) {
                        png->SetError(Png::Malformed, 5);
                        break;
                    }
                    if (i < hLit)
                        bitLengths[i] = value;
                    else
                        bitLengthsD[i - hLit] = value;
                    i++;
                }
            } else if (code == 17) {
                unsigned repLength = 3;
                if ((bitPointer >> 3) >= inLength) {
                    png->SetError(Png::Malformed, 6);
                    break;
                }
                repLength += ReadBits<unsigned>(bitPointer, in, 3);
                for (UInt32 n = 0; n < repLength; n++) {
                    if (i < hLit)
                        bitLengths[i] = 0;
                    else
                        bitLengthsD[i - hLit] = 0;
                    i++;
                }
            } else if (code == 18) {
                unsigned repLength = 11;
                if ((bitPointer >> 3) >= inLength) {
                    png->SetError(Png::Malformed, 7);
                    break;
                }
                repLength += ReadBits<unsigned>(bitPointer, in, 7);
                for (UInt32 n = 0; n < repLength; n++) {
                    if (i >= (hLit + hDist)) {
                        png->SetError(Png::Malformed, 9);
                        break;
                    }
                    if (i < hLit)
                        bitLengths[i] = 0;
                    else
                        bitLengthsD[i - hLit] = 0;
                    i++;
                }
            } else {
                png->SetError(Png::Malformed, 8);
                break;
            }
        }
        if (png->IsOkay() && (bitLengths[256] == 0))
            png->SetError(Png::Malformed, 10);
        if (png->IsOkay())
            codeTree.CreateLengths(png, bitLengths);
        if (png->IsOkay())
            codeTreeD.CreateLengths(png, bitLengthsD);
    }
};

void Png::InflateHuffman(const UInt8 *in, UInt32 inLength, UInt8 *out, UInt32 outSize, PngBitPointer &bitPointer, UInt32 &pos, unsigned btype)
{
    HuffmanTree<NumDeflateCodeSymbols, DeflateCodeBitlen> codeTree;
    HuffmanTree<NumDistanceSymbols, DistanceBitlen> codeTreeD;
    
    switch (btype) {
        case 0:
            // Handled above, but for completeness
            InflateUncompressed(in, inLength, out, outSize, bitPointer, pos);
            return;
        case 1:
            codeTree.Load(FixedDeflateCodeTree);
            codeTreeD.Load(FixedDistanceTree);
            break;
        case 2: {
            HuffmanTree<NumCodeLengthCodes, CodeLengthBitlen> codeLengthCodeTree;
            codeLengthCodeTree.InflateDynamic(this, codeTree, codeTreeD, in, inLength, bitPointer);
        }
            break;
        default:
            SetError(Malformed, 21);
            return;
    }
    while (true) {
        unsigned code = codeTree.DecodeSymbol(this, in, inLength, bitPointer);
        if (!IsOkay())
            return;
        if (code == 256) {
            break;
        } else if (code <= 255) {
            if (pos >= outSize) {
                SetError(Malformed, 11);
                return;
            }
            out[pos++] = UInt8(code);
        } else if ((code >= FirstLengthCodeIndex) && (code <= LastLengthCodeIndex)) {
            UInt32 length = LengthBase[code - FirstLengthCodeIndex];
            UInt32 numExtraBits = LengthExtra[code - FirstLengthCodeIndex];
            if ((bitPointer >> 3) >= inLength) {
                SetError(Malformed, 12);
                return;
            }
            length += ReadBits<unsigned>(bitPointer, in, numExtraBits);
            unsigned codeD = codeTreeD.DecodeSymbol(this, in, inLength, bitPointer);
            if (!IsOkay())
                return;
            if (codeD > 29) {
                SetError(Malformed, 13);
                return;
            }
            unsigned distance = DistanceBase[codeD];
            unsigned numExtraBitsD = DistanceExtra[codeD];
            if ((bitPointer >> 3) >= inLength) {
                SetError(Malformed, 14);
                return;
            }
            distance += ReadBits<unsigned>(bitPointer, in, numExtraBitsD);
            UInt32 start = pos;
            UInt32 backward = start - distance;
            if ((pos + length) >= outSize) {
                SetError(Malformed, 15);
                return;
            }
            for (UInt32 forward = 0; forward < length; forward++) {
                out[pos++] = out[backward];
                backward++;
                if (backward >= start)
                    backward = start - distance;
            }
        }
    }
}

class PngContext : public ImageDecoder
{
public:
    PngContext(Png *content)
    :_content(content)
    {
    }
    ~PngContext()
    {
        delete _content;
    }
private:
    Png *_content;
};

extern "C" ImageDecoder* AttemptParseImage(const UInt8 *data, UInt32 length)
{
    Png *png = new Png();
    png->Load(data, length);
    if (!png->IsOkay()) {
        delete png;
        return NULL;
    }
    return new PngContext(png);
}
