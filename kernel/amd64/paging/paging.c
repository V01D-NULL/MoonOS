#include <amd64/paging/paging.h>
#include <ktypes.h>

inline bool check_flag(int flags, int bit) { return (flags >> bit) & 1; }

struct pte paging_create_entry(uint64_t paddr, int flags)
{
    return (struct pte){
        .present = check_flag(flags, 0),
        .readwrite = check_flag(flags, 1),
        .supervisor = check_flag(flags, 2),
        .writethrough = 0,
        .cache_disabled = 0,
        .accessed = 0,
        .dirty = 0,
        .pagesize = 0, // PS/PAT bit
        .global = 0, // Todo: set this
        .avail = 0,
        .address = paddr >> PAGE_SHIFT // PFN
    };
}

struct pte paging_purge_entry(void)
{
    return paging_create_entry(0, 0);
}

struct Pagefault paging_get_pagefault_flags(int error_code, bool do_panic)
{
    auto present = check_flag(error_code, 0);
    auto write = check_flag(error_code, 1);
    auto user = check_flag(error_code, 2);
    auto ins_fetch = check_flag(error_code, 4);
    auto prot_key = check_flag(error_code, 5);
    auto shadow_stack = check_flag(error_code, 6);

    return (struct Pagefault)
    {
        .present = present,
        .write = write,
        .user = user,
        .reserved = 0,
        .instruction_fetch = ins_fetch,
        .protection_key = prot_key,
        .shadow_stack = shadow_stack,
        .panic_on_unhandled = do_panic && (ins_fetch || prot_key || shadow_stack),
        .flags = (user << 2) | (write << 1) | (present << 0)
    };
}