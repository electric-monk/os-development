typedef struct { unsigned char b, g, r, a; } Pixel; typedef struct { int w, h; Pixel *data; } Bitmap;
Pixel nat299_data[] = {
    {0x00, 0x00, 0x00, 0xff}, {0x00, 0x00, 0x00, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x44, 0x44, 0x44, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x44, 0x44, 0x44, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x44, 0x44, 0x44, 0xff}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0x00, 0x00, 0x00, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x44, 0x44, 0x44, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0x00, 0x00, 0x00, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0x00, 0x00, 0x00, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x44, 0x44, 0x44, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0x00, 0x00, 0x00, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0x00, 0x00, 0x00, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x44, 0x44, 0x44, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0x00, 0x00, 0x00, 0xff}, {0x00, 0x00, 0x00, 0xff}, {0x00, 0x00, 0x00, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0x77, 0x77, 0x77, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff},
    {0x00, 0x00, 0x00, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0x00, 0x00, 0x00, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0x00, 0x00, 0x00, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x44, 0x44, 0x44, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0x77, 0x77, 0x77, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x11, 0x11, 0x11, 0xff}, {0x00, 0x00, 0x00, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x22, 0x22, 0x22, 0xff},
    {0x00, 0x00, 0x00, 0xff}, {0x00, 0x00, 0x00, 0xff}, {0x00, 0x00, 0x00, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0x00, 0x00, 0x00, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x77, 0x77, 0x77, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0x00, 0x00, 0x00, 0xff}, {0xff, 0xff, 0xff, 0xff}, {0x00, 0x00, 0x00, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x22, 0x22, 0x22, 0xff},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0xff}, {0x00, 0x00, 0x00, 0xff}, {0x00, 0x00, 0x00, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0x11, 0x11, 0x11, 0xff}, {0x00, 0x00, 0x00, 0xff}, {0x11, 0x11, 0x11, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0x22, 0x22, 0x22, 0xff},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0x44, 0x44, 0x44, 0xff},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0x44, 0x44, 0x44, 0xff},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x44, 0x44, 0x44, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x44, 0x44, 0x44, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xff, 0xfb, 0xf0, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xff, 0x66, 0x33, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xdd, 0xdd, 0xdd, 0xff}, {0xc0, 0xc0, 0xc0, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x55, 0x55, 0x55, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0xa0, 0xa0, 0xa4, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x44, 0x44, 0x44, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x44, 0x44, 0x44, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0xcc, 0x99, 0x66, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0xff, 0xff, 0xcc, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0xff, 0xcc, 0x99, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x44, 0x44, 0x44, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x22, 0x22, 0x22, 0xff}, {0x44, 0x44, 0x44, 0xff}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
};
Bitmap nat299 = {32, 32, nat299_data};
