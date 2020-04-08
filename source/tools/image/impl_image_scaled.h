#ifndef IMPL_IMAGE_SCALED_H
#define IMPL_IMAGE_SCALED_H

#include "ky_image.h"

struct implImageScaled
{
    eImageScaleds mode;
    bool (*is_valid)(eImagePixels pix);
    void* (*scaled)(void *src, int w, int h, int d, eImagePixels pix,
                    void *dst, int dw, int dh, int dd, eImagePixels dpix);
};

extern implImageScaled ImageScaled[Scaled_Count];

#endif // IMPL_IMAGE_SCALED_H
