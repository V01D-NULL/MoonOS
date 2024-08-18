#include "limine.h"
#include <base/assert.h>
#include <base/mem.h>
#include <moon-io/serial.h>

LIMINE_REQUEST static volatile LIMINE_BASE_REVISION(2);

LIMINE_REQUEST static volatile struct limine_framebuffer_request
    framebuffer_request = {.id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0};

LIMINE_REQUEST static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST, .revision = 0};

LIMINE_REQUEST static volatile struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST, .revision = 0};

// command line?

LIMINE_REQUEST static volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST, .revision = 0};

LIMINE_REQUEST static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST, .revision = 0};

LIMINE_REQUEST static volatile struct limine_kernel_address_request
    kernel_address_request = {
        .id = LIMINE_KERNEL_ADDRESS_REQUEST, .revision = 0};

LIMINE_REQUEST static volatile struct limine_kernel_file_request
    kernel_file_request = {.id = LIMINE_KERNEL_FILE_REQUEST, .revision = 0};

USED_SECTION(".requests_start_marker")
static volatile LIMINE_REQUESTS_START_MARKER;

USED_SECTION(".requests_end_marker")
static volatile LIMINE_REQUESTS_END_MARKER;

static BootHandover handover = {};

BootHandover limine_initialize(void)
{
    auto fb   = framebuffer_request.response->framebuffers[0];
    auto mmap = memmap_request.response;
    auto rsdp = rsdp_request.response;
    // auto cmdline = cmdline_request.response;
    auto modules = module_request.response;
    auto hhdm    = hhdm_request.response;

    assert(fb != NULL &&
           "MoonOS was not provided with a framebuffer."
           "Refusing to boot.");

    // Could probably handle this better but I'm lazy and not too worried about
    // it
    assert(rsdp != NULL &&
           "MoonOS was not provided with an RSDP."
           "Refusing to boot.");

    assert(mmap != NULL &&
           "MoonOS was not provided with a memory map."
           "Refusing to boot.");

    assert(modules != NULL &&
           "MoonOS was not provided with a module list."
           "Refusing to boot.");

    assert(hhdm != NULL &&
           "MoonOS was not provided with a HHDM."
           "Refusing to boot.");

    handover.kernel_phys_start = kernel_address_request.response->physical_base;
    handover.kernel_virt_start = kernel_address_request.response->virtual_base;
    handover.kernel_size = kernel_file_request.response->kernel_file->size;

    handover.rsdp        = rsdp->address;
    handover.hhdm_offset = hhdm->offset;

    handover.memory_map.entry_count = mmap->entry_count;
    handover.memory_map.entries     = mmap->entries;

    handover.framebuffer = (HandoverFramebuffer){fb->address,
                                                 fb->width,
                                                 fb->height,
                                                 fb->pitch,
                                                 fb->bpp,
                                                 fb->memory_model,
                                                 fb->red_mask_size,
                                                 fb->red_mask_shift,
                                                 fb->green_mask_size,
                                                 fb->green_mask_shift,
                                                 fb->blue_mask_size,
                                                 fb->blue_mask_shift,
                                                 {0},
                                                 fb->edid_size,
                                                 fb->edid};

    // We only support 16 modules. The rest should be loaded via an initrd
    // anyways so 16 should be plenty
    handover.modules.count =
        modules->module_count > 16 ? 16 : modules->module_count;

    for (size_t i = 0; i < handover.modules.count; i++)
    {
        handover.modules.modules[i] =
            (HandoverModuleEntry){.address = modules->modules[i]->address,
                                  .size    = modules->modules[i]->size,
                                  .cmdline = modules->modules[i]->cmdline};
    }

    return handover;
}
