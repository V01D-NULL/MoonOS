void _start(void)
{
	__asm__ volatile("int $0x0");
    for (;;) {}
}