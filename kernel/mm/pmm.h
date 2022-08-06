#ifndef PMM_H
#define PMM_H

#include <base/base-types.h>
#include <moon-extra/range.h>

void *pmm_alloc(void);
void pmm_free(void *page);
Range pmm_alloc_range(size_t pages);

#endif // PMM_H