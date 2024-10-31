typedef void (*MainFn)(int argc, char **argv);

void libc_entry(MainFn main, int argc, char **argv)
{
    main(argc, argv);
    // TODO: exit()
}