#include <stdint.h>
#include <stddef.h>

void* kmemset(void* ptr, int value, size_t num) {
    uint8_t* p = (uint8_t*)ptr;
    for (size_t i = 0; i < num; i++) {
        p[i] = (uint8_t)value;
    }
    return ptr;
}

//std implementation for fast memcpy using inline asm
void* kmemcpy(void* dest, const void* src, size_t num) {
    void* d = dest;
    const void* s = src;
    size_t dwords = num >> 2; // num / 4 (number of 32-bit chunks)
    size_t bytes  = num & 3;  // num % 4 (remaining 0-3 bytes)
    
    asm volatile (
        "cld\n\t"
        "rep movsd\n\t"            // Copy 4 bytes at a time 
        "mov ecx, %[rem]\n\t"
        "rep movsb"               // Copy remaining 0-3 bytes
        : "+D"(d), "+S"(s), "+c"(dwords)
        : [rem] "r"(bytes)
        : "memory"
    );

    return dest;
}


