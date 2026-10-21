#pragma once
#include <stdint.h>

// --- KERNEL LOG BUFFER ---
#define KERNEL_LOG_SIZE 4096
static char kernel_log[KERNEL_LOG_SIZE];
static uint32_t log_write_ptr = 0;
static uint32_t log_read_ptr = 0;

static inline void buff_putchar(const char c){
    kernel_log[log_write_ptr] = c;
    log_write_ptr = (log_write_ptr + 1) % KERNEL_LOG_SIZE;
    
    // If buffer is full, overwrite oldest data by moving read pointer
    if (log_write_ptr == log_read_ptr) {
        log_read_ptr = (log_read_ptr + 1) % KERNEL_LOG_SIZE;
    }
}

void printk(const char *fmt, ...);

void buff_flush(); 
