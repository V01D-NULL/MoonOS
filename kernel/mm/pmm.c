#include "pmm.h"

struct bitmap bitmap;

void init_pmm()
{
    debug("Bitmap size: %lld\n", BITMAP_SIZE);
    bitmap.bitmap = BITMAP_SIZE;
}