#ifndef FMT_PRINTK_H
#define FMT_PRINTK_H

#include <base/base-types.h>
#include <moon-io/serial.h>
#include <moon-sys/handover.h>

#ifndef PR_MODULE
#define PR_MODULE __FILE__
#endif

#define TODO(x, ...)                    \
    do                                  \
    {                                   \
        printk("TODO", x, __VA_ARGS__); \
        debug(true, x, __VA_ARGS__);    \
    } while (0)

#define trace(bitmask, ...)                 \
    do                                      \
    {                                       \
        if (bitmask & TRACE_MASK)           \
            printk(PR_MODULE, __VA_ARGS__); \
    } while (0)

#define TRACE_MASK (TRACE_ALL)

enum
{
    TRACE_BOOT    = (1 << 0),
    TRACE_INT     = (1 << 1),
    TRACE_DEBUG   = (1 << 2),
    TRACE_SYSCALL = (1 << 3),
    TRACE_MISC    = (1 << 4),  // For logs with no specific category
    TRACE_TASK    = (1 << 5),
    TRACE_SERVICE = (1 << 6),
    TRACE_ELF     = (1 << 7),
    TRACE_SLAB    = (1 << 8),
    TRACE_BUDDY   = (1 << 9),
    TRACE_TLSF    = (1 << 10),
    TRACE_ALLOC   = (1 << 11),
    TRACE_IPC     = (1 << 12),

    TRACE_ALL  = ~0,
    TRACE_NONE = 0,
};

void printk(string_view status, string_view fmt, ...);
void printk_init(bool verbose_boot, HandoverFramebuffer fb);
void tty_write(string_view fmt, ...);

// Note: This should only be called when information
// must be shown, a kernel panic for example
void override_quiet_boot(void);

#endif  // FMT_PRINTK_H