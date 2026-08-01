#include <lib/math.h>
#include <stdint.h>

int16_t max(int16_t a, int16_t b) {

    if (a >= b) {
        return a;
    }

    return b;
}

int16_t min(int16_t a, int16_t b) {

    if (a > b) {
        return b;
    }

    return a;
}
