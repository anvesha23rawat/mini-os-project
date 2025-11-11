#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

// Define size_t if we're in freestanding environment
typedef unsigned int size_t;

// VGA text mode constants
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// Color constants
enum vga_color {
    COLOR_BLACK = 0,
    COLOR_BLUE = 1,
    COLOR_GREEN = 2,
    COLOR_CYAN = 3,
    COLOR_RED = 4,
    COLOR_MAGENTA = 5,
    COLOR_BROWN = 6,
    COLOR_LIGHT_GREY = 7,
    COLOR_DARK_GREY = 8,
    COLOR_LIGHT_BLUE = 9,
    COLOR_LIGHT_GREEN = 10,
    COLOR_LIGHT_CYAN = 11,
    COLOR_LIGHT_RED = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_LIGHT_BROWN = 14,
    COLOR_WHITE = 15,
};

// Process management
#include "process.h"

// Function declarations
void kernel_main(void);
void print_string(const char* str);
void print_char(char c);
void print_int(int num);
void print_float(float num);
void clear_screen(void);
void terminal_initialize(void);
void terminal_setcolor(uint8_t color);
//size_t strlen(const char* str);
void memcpy(void* dest, const void* src, size_t n);
void memset(void* dest, int val, size_t n);
//void strcpy(char* dest, const char* src);
//int strcmp(const char* s1, const char* s2);

// Demo processes
void init_demo_processes(void);

// ML Scheduler functions
void ml_scheduler_init(void);
void ml_schedule(void);
void ml_update_process_features(int pid, int process_type);
void print_ml_scheduler_stats(void);

// Statistics functions are now in stats.h - REMOVED from here

#endif