#include "../ipc/ipc.h"

typedef void (*MainFn)(int argc, char **argv);

void libc_entry(MainFn main, int argc, char **argv)
{
    ipc_init();
    main(argc, argv);
    // TODO: exit()
}