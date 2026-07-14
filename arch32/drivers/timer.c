#include <arch32/io.h>
#include <arch32/isr.h>
#include <arch32/timer.h>
#include <stdint.h>

#define PIT_CHANNEL0 0x40
#define PIT_COMMAND 0x43

/* info related to pit configuration taken from oswiki
 * Bits         Usage
 * 7 and 6      Select channel :
 *               0 0 = Channel 0
 *               0 1 = Channel 1
 *               1 0 = Channel 2
 *               1 1 = Read-back command (8254 only)
 * 5 and 4      Access mode :
 *               0 0 = Latch count value command
 *               0 1 = Access mode: lobyte only
 *               1 0 = Access mode: hibyte only
 *               1 1 = Access mode: lobyte/hibyte
 * 3 to 1       Operating mode :
 *              0 0 0 = Mode 0 (interrupt on terminal count)
 *               0 0 1 = Mode 1 (hardware re-triggerable one-shot)
 *               0 1 0 = Mode 2 (rate generator)
 *               0 1 1 = Mode 3 (square wave generator)
 *               1 0 0 = Mode 4 (software triggered strobe)
 *               1 0 1 = Mode 5 (hardware triggered strobe)
 *               1 1 0 = Mode 2 (rate generator, same as 010b)
 *               1 1 1 = Mode 3 (square wave generator, same as 011b)
 * 0            BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD

*/

uint32_t pit_frequency;

volatile uint32_t tick = 0;

void timer_callback(int_registers_t* regs) {
    tick++;
}

void init_pit(uint32_t freq) {
    pit_frequency = freq;
    install_interrupt_handler(32, timer_callback);  // IRQ0 -> int 32

    uint32_t divisor = 1193182 / freq;

    outb(PIT_COMMAND, 0x36);  // channel 0, lobyte/hibyte, mode 3 (square wave)
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}

void ksleep(uint32_t t){
    uint32_t end_tick = tick + (t * pit_frequency);
    
    while (tick < end_tick) {
        asm volatile("hlt");
    }
}
