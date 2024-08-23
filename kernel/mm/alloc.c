#define PR_MODULE "alloc"

#include "alloc.h"
#include <base/mem.h>
#include <mm/addr.h>
#include <mm/page.h>
#include <mm/phys.h>
#include <panic.h>
#include <printk.h>
#include "tlsf.h"

/**
 * TODO: Add a system to grow the TLSF pool when it runs out of memory / handle
 * (impending) memory exhaustion in general.
 */

#define TLSF_POOL_SIZE MiB(512)

static tlsf_t tlsf = NULL;

void alloc_init(void)
{
    if (tlsf)
    {
        trace(TRACE_ALLOC, "\e[31malloc_init() called twice\e[39m");
        return;
    }

    tlsf = tlsf_create_with_pool(
        arch_alloc_page_sz(TLSF_POOL_SIZE), TLSF_POOL_SIZE);

    panic_if(!tlsf, "Early OOM: Failed to create TLSF pool");
}

void *alloc(int sz)
{
    return tlsf_malloc(tlsf, sz);
}

void *alloc_aligned(int sz, int align)
{
    return tlsf_memalign(tlsf, align, sz);
}

void *realloc(void *ptr, int sz)
{
    return tlsf_realloc(tlsf, ptr, sz);
}

void *calloc(int sz)
{
    void *ptr = tlsf_malloc(tlsf, sz);
    if (likely(ptr))
        memset(ptr, 0, sz);
    return ptr;
}

void free(void *ptr, int sz)
{
    tlsf_free(tlsf, ptr);
}
