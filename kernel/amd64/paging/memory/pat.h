#ifndef PAGING_MEMORY_PAT_H
#define PAGING_MEMORY_PAT_H

#include <amd64/msr.h>
#include <amd64/moon.h>

// Values extracted from the AMD SDM: table 7-8 pat encodings
enum pat_type_encoding
{
    PAT_UC = 0x0,
    PAT_WC = 0x1,
    PAT_WT = 0x4,
    PAT_WP = 0x5,
    PAT_WB = 0x6,
    PAT_UCM = 0x7
};

enum pat_indices
{
    PA0_UC,
    PA1_WC,
    PA2_WT,
    PA3_WP,
    PA4_WB,
    PA5_UCM,
    PA6_FREE, // Free to use
    PA7_FREE  // Free to use
};

// Set pat index, retrieve it using get_pat().
// This way there is no need for another argument in vmm_map
void set_pat(uint64_t pat);
int get_pat(void);

void configure_pat(void);

#endif // PAGING_MEMORY_PAT_H