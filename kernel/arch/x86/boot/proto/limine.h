#ifndef PROTO_LIMINE_H
#define PROTO_LIMINE_H

#include <boot/limine.h>
#include <moon-sys/handover.h>
#include <moon.h>

#define LIMINE_REQUEST USED_SECTION(".requests")

BootHandover limine_initialize(void);

#endif  // PROTO_LIMINE_H