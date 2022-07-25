#ifndef LEGACY_PIC_H
#define LEGACY_PIC_H

#include <stdbool.h>

void pic_eoi(int isr_nr);
bool pic_enabled(void);
void pic_disable(void);
void pic_remap(void);

#endif // LEGACY_PIC_H