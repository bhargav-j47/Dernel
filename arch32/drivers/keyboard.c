#include <stdint.h>
#include <arch32/keyboard.h>
#include <arch32/isr.h>
#include <lib/printk.h>
#include <arch32/io.h>

static const char scancode_ascii_lower[128] = {
    /* 0x00 */ KEY_UNKNOWN, 27 /*ESC*/, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    /* 0x0F */ '\t',
    /* 0x10 */ 'q','w','e','r','t','y','u','i','o','p','[',']','\n',
    /* 0x1D */ KEY_UNKNOWN, // Left Ctrl
    /* 0x1E */ 'a','s','d','f','g','h','j','k','l',';','\'','`',
    /* 0x2A */ KEY_UNKNOWN, // Left Shift
    /* 0x2B */ '\\',
    /* 0x2C */ 'z','x','c','v','b','n','m',',','.','/',
    /* 0x36 */ KEY_UNKNOWN, // Right Shift
    /* 0x37 */ '*',         // Keypad *
    /* 0x38 */ KEY_UNKNOWN, // Left Alt
    /* 0x39 */ ' ',         // Space
    /* 0x3A */ KEY_UNKNOWN, // Caps Lock
    /* 0x3B */ KEY_UNKNOWN, // F1
    /* 0x3C */ KEY_UNKNOWN, // F2
    /* 0x3D */ KEY_UNKNOWN, // F3
    /* 0x3E */ KEY_UNKNOWN, // F4
    /* 0x3F */ KEY_UNKNOWN, // F5
    /* 0x40 */ KEY_UNKNOWN, // F6
    /* 0x41 */ KEY_UNKNOWN, // F7
    /* 0x42 */ KEY_UNKNOWN, // F8
    /* 0x43 */ KEY_UNKNOWN, // F9
    /* 0x44 */ KEY_UNKNOWN, // F10
    /* 0x45 */ KEY_UNKNOWN, // Num Lock
    /* 0x46 */ KEY_UNKNOWN, // Scroll Lock
    /* 0x47 */ '7',         // Keypad 7 / Home
    /* 0x48 */ '8',         // Keypad 8 / Up
    /* 0x49 */ '9',         // Keypad 9 / PgUp
    /* 0x4A */ '-',         // Keypad -
    /* 0x4B */ '4',         // Keypad 4 / Left
    /* 0x4C */ '5',         // Keypad 5
    /* 0x4D */ '6',         // Keypad 6 / Right
    /* 0x4E */ '+',         // Keypad +
    /* 0x4F */ '1',         // Keypad 1 / End
    /* 0x50 */ '2',         // Keypad 2 / Down
    /* 0x51 */ '3',         // Keypad 3 / PgDn
    /* 0x52 */ '0',         // Keypad 0 / Insert
    /* 0x53 */ '.',         // Keypad . / Delete
    /* 0x54 */ KEY_UNKNOWN,
    /* 0x55 */ KEY_UNKNOWN,
    /* 0x56 */ KEY_UNKNOWN,
    /* 0x57 */ KEY_UNKNOWN, // F11
    /* 0x58 */ KEY_UNKNOWN, // F12
    /* 0x59 */ KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,
    /* 0x5E */ KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,
    /* 0x63 */ KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,
    /* 0x68 */ KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,
    /* 0x6D */ KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,
    /* 0x72 */ KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,
    /* 0x77 */ KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,
    /* 0x7C */ KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN
};

static const char scancode_ascii_upper[128] = {
    /* 0x00 */ KEY_UNKNOWN, 27 /*ESC*/, '!','@','#','$','%','^','&','*','(',')','_','+','\b',
    /* 0x0F */ '\t',
    /* 0x10 */ 'Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    /* 0x1D */ KEY_UNKNOWN, // Left Ctrl
    /* 0x1E */ 'A','S','D','F','G','H','J','K','L',':','"','~',
    /* 0x2A */ KEY_UNKNOWN, // Left Shift
    /* 0x2B */ '|',
    /* 0x2C */ 'Z','X','C','V','B','N','M','<','>','?',
    /* 0x36 */ KEY_UNKNOWN, // Right Shift
    /* 0x37 */ '*',         // Keypad *
    /* 0x38 */ KEY_UNKNOWN, // Left Alt
    /* 0x39 */ ' ',         // Space
    /* 0x3A */ KEY_UNKNOWN, // Caps Lock
    /* 0x3B */ KEY_UNKNOWN, // F1
    /* 0x3C */ KEY_UNKNOWN, // F2
    /* 0x3D */ KEY_UNKNOWN, // F3
    /* 0x3E */ KEY_UNKNOWN, // F4
    /* 0x3F */ KEY_UNKNOWN, // F5
    /* 0x40 */ KEY_UNKNOWN, // F6
    /* 0x41 */ KEY_UNKNOWN, // F7
    /* 0x42 */ KEY_UNKNOWN, // F8
    /* 0x43 */ KEY_UNKNOWN, // F9
    /* 0x44 */ KEY_UNKNOWN, // F10
    /* 0x45 */ KEY_UNKNOWN, // Num Lock
    /* 0x46 */ KEY_UNKNOWN, // Scroll Lock
    /* 0x47 */ '7',         // Keypad 7 (numlock ascii same; extended-scan handles Home separately)
    /* 0x48 */ '8',
    /* 0x49 */ '9',
    /* 0x4A */ '-',
    /* 0x4B */ '4',
    /* 0x4C */ '5',
    /* 0x4D */ '6',
    /* 0x4E */ '+',
    /* 0x4F */ '1',
    /* 0x50 */ '2',
    /* 0x51 */ '3',
    /* 0x52 */ '0',
    /* 0x53 */ '.',
    /* 0x54 */ KEY_UNKNOWN,
    /* 0x55 */ KEY_UNKNOWN,
    /* 0x56 */ KEY_UNKNOWN,
    /* 0x57 */ KEY_UNKNOWN, // F11
    /* 0x58 */ KEY_UNKNOWN, // F12
    /* 0x59 */ KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,
    /* 0x5E */ KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,
    /* 0x63 */ KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,
    /* 0x68 */ KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,
    /* 0x6D */ KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,
    /* 0x72 */ KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,
    /* 0x77 */ KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN,
    /* 0x7C */ KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN
};

uint8_t is_shift = 0;
uint8_t is_caps = 0;

inp_buff_t* kbuff;

void keyboard_handler(int_registers_t* regs);

void init_keyboard() {
    printk("initializing keyboard\n");
    install_interrupt_handler(33, keyboard_handler);
    krb_init(kbuff);
    printk("keyboard is up\n");

}

void keyboard_handler(int_registers_t* regs) {
    
    uint8_t rv = inb(0x60);
    krb_push(kbuff, rv);

    print_inp();  //for now
}

void print_inp() {
    uint8_t sc = 0;

    while (!krb_pop(kbuff, &sc)) {
        uint8_t code = GET_MAKE_CODE(sc);
        uint8_t released = IS_KEY_RELEASE(sc);

        if (code == KEY_RSHIFT || code == KEY_LSHIFT) {
            if (released) 
                is_shift = 0;
             else 
                is_shift = 1;
            continue;
        }

        if (code == KEY_CAPSLOCK) {
            if (released == 0) 
                is_caps = 0 ? is_caps : 1;
            continue;
        }

        if ( code < 128 && released == 0) {
            if (is_caps) {
                if (is_shift) 
                    printk("%c", scancode_ascii_lower[code]);
                else 
                    printk("%c", scancode_ascii_upper[code]);
            } else if (is_shift) {
                printk("%c", scancode_ascii_upper[code]);
            } else {
                printk("%c", scancode_ascii_lower[code]);
            }
            continue;
        }

    }
}

