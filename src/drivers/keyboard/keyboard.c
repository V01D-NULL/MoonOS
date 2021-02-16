#include "keyboard.h"
#include "../screen/monitor.h"
#include "../../arch/x86/cpu/interrupts/interrupts.h"
#include "layout/EN-US.h"

static bool shift_is_pressed = false;
static bool caps_lock_is_pressed = false;
static bool ctrl_occupied = false;

/*
*   Key combos:
*       Ctrl + r or ctrl + R = Reload screen (clear)
*/
static void kb_handler_callback(registers_t reg)
{
    set_color(VGA_BLACK, VGA_LIGHT_GREEN);
    unsigned char scancode;
    scancode = inb(0x60);

    //Key has been released
    if (scancode & 0x80)
    {    
        //Left and right shift
        if (scancode == 0xAA || scancode == 0xB6)
        {
            shift_is_pressed = false;
        }
    }
    //key has been pressed
    else {
        switch (scancode) {
            //Left shift
            case 0x2A:
                shift_is_pressed = true;
                break;
            //Right shift
            case 0x36:
                shift_is_pressed = true;
                break;
            //Capslock
            case 0x3A:
                if (caps_lock_is_pressed) { caps_lock_is_pressed = false; }
                else { caps_lock_is_pressed = true; }
                break;
            
            case 0x1D:
                debug("CTRL Pressed. Awaiting second key combo\n");
                ctrl_occupied = true;
                return;

            default:
                break;
        }
        
        // debug("%x\n", scancode); //0x1d, 0x13
        //Is this ugly? Yes. Will I optimize? Maybe.
        if (!shift_is_pressed && !caps_lock_is_pressed && !ctrl_occupied)
        {
            if (keyboard_us_lower[scancode] == '\b')
                monitor_put('\b');
            else
                monitor_put(keyboard_us_lower[scancode]);
        }
        else if (shift_is_pressed)
        {
            if (keyboard_us_upper[scancode] == '\b')
                monitor_put('\b');
            else
                monitor_put(keyboard_us_upper[scancode]);
        }
        else if (caps_lock_is_pressed) {
            if (keyboard_us_upper_caps[scancode] == '\b')
                monitor_put('\b');
            else
                monitor_put(keyboard_us_upper_caps[scancode]);
        }
        else {
            if (keyboard_us_lower[scancode] == 'r' || keyboard_us_upper[scancode] == 'R')
            {
                monitor_clear();
                ctrl_occupied = false;
            }
            else { 
                debug("Invalid key combo! -- Aborting.\n"); 
                ctrl_occupied = false;
            }
        }
    }
    set_color(VGA_BLACK, VGA_WHITE);
}

void install_keyboard()
{
    monitor_write("Installing keyboard...\n", true, false);
    register_interrupt_handler(IRQ1, &kb_handler_callback);
}
