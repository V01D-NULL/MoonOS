#include "common.h"
#include "drivers/io/serial.h"
#include "drivers/screen/monitor.h"

//Obsolete. Will be removed soon
// The boolean is_caller_debug is used sothat a debug() function call with the %b symbol will redirect the output to the serial and not the screen
void to_bin(unsigned int n, bool is_caller_debug)
{
    int a[10], i;
    for(i=0;n>0;i++)    
    {    
        a[i]=n%2;    
        n=n/2;    
    }    
    
    for(i=i-1;i>=0;i--)    
    {
        if (!is_caller_debug)
            kprintf("%d",a[i]);
        else
            debug("%d", a[i]);
	}
}