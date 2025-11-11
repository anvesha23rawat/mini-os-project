// kernel/kernel.c - Main Kernel
#include "kernel.h"
#include "process.h"
#include "fs.h"
#include "shell.h"
#include "string.h"

// VGA Text Buffer
volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;

// Terminal state
int terminal_row;
int terminal_col;
uint8_t terminal_color;

// REMOVED: Duplicate process functions (they're in demo_processes.c)

void terminal_initialize(void) {
    terminal_row = 0;
    terminal_col = 0;
    terminal_color = ((COLOR_BLACK << 4) | COLOR_WHITE);
    
    // Clear screen
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            vga_buffer[index] = (uint16_t)' ' | (uint16_t)(terminal_color << 8);
        }
    }
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

void print_char(char c) {
    if (c == '\n') {
        terminal_col = 0;
        terminal_row++;
    } else {
        const size_t index = terminal_row * VGA_WIDTH + terminal_col;
        vga_buffer[index] = (uint16_t)c | (uint16_t)(terminal_color << 8);
        terminal_col++;
    }

    if (terminal_col >= VGA_WIDTH) {
        terminal_col = 0;
        terminal_row++;
    }
    
    // Proper scroll when at bottom
    if (terminal_row >= VGA_HEIGHT) {
        // Move all lines up by one
        for (int y = 0; y < VGA_HEIGHT-1; y++) {
            for (int x = 0; x < VGA_WIDTH; x++) {
                vga_buffer[y * VGA_WIDTH + x] = vga_buffer[(y+1) * VGA_WIDTH + x];
            }
        }
        // Clear the last line
        for (int x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[(VGA_HEIGHT-1) * VGA_WIDTH + x] = (uint16_t)' ' | (uint16_t)(terminal_color << 8);
        }
        terminal_row = VGA_HEIGHT - 1;
    }
}

void print_string(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        print_char(str[i]);
    }
}

void print_int(int num) {
    if (num == 0) {
        print_char('0');
        return;
    }
    
    if (num < 0) {
        print_char('-');
        num = -num;
    }
    
    char buffer[32];
    int i = 0;
    
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    for (int j = i - 1; j >= 0; j--) {
        print_char(buffer[j]);
    }
}

// Add print_float function for ML scheduler
void print_float(float num) {
    int integer_part = (int)num;
    print_int(integer_part);
    print_char('.');
    
    // Print first 2 decimal places
    float fractional = num - integer_part;
    for(int i = 0; i < 2; i++) {
        fractional *= 10;
        int digit = (int)fractional;
        print_char('0' + digit);
        fractional -= digit;
    }
}

void clear_screen(void) {
    terminal_initialize();
}

// Utility Functions

void memcpy(void* dest, const void* src, size_t n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
}

void memset(void* dest, int val, size_t n) {
    char* d = (char*)dest;
    for (size_t i = 0; i < n; i++) {
        d[i] = (char)val;
    }
}

/*Kernel Main Function - ML SCHEDULER + FILE SYSTEM DEMO */
/*
void kernel_main(void) {
    terminal_initialize();

    print_string("Mini OS with ML Scheduler & File System\n");
    print_string("========================================\n\n");

    // Initialize systems
    process_init();
    ml_scheduler_init();
    fs_init();  // Initialize file system
    
    // Create demo processes
    init_demo_processes();

    // Print initial predictions
    print_process_table();
    print_ml_scheduler_stats();

    // Show file system contents
    fs_list();

    // Set to ML scheduler
    set_scheduler_type(SCHEDULER_ML_BASED);
    
    print_string("\n=== ML Scheduler Active ===\n");
    print_string("ML Predictions:\n");
    print_string("- CPU Process: Burst=8, Priority=0.12\n");
    print_string("- IO Process:  Burst=4, Priority=0.25 (HIGHEST)\n");
    print_string("- ML Process:  Burst=6, Priority=0.16\n\n");

    // Demo file operations
    print_string("=== File System Demo ===\n");
    
    // Read and display demo files
    char buffer[256];
    if(fs_read("readme.txt", buffer) > 0) {
        print_string("readme.txt: ");
        print_string(buffer);
        print_string("\n");
    }
    
    if(fs_read("ml_info.txt", buffer) > 0) {
        print_string("ml_info.txt: ");
        print_string(buffer);
        print_string("\n");
    }
    
    // Create a new file
    fs_create("demo.txt");
    fs_write("demo.txt", "This file was created by ML OS!");
    
    // Show updated file list
    fs_list();

    // Show a few scheduling decisions
    print_string("\n=== ML Scheduling Demo ===\n");
    for(int i = 0; i < 3; i++) {
        ml_schedule();
        print_string("Running: ");
        print_string(current_process->name);
        print_string("\n");
        
        for(int j = 0; j < 2000000; j++);
    }
    
    print_string("\n✅ SYSTEM READY: ML Scheduler + File System\n");

    // MOVE shell_start() HERE - BEFORE infinite loop
    shell_start();  // NOW IT WILL BE CALLED

    // Infinite loop at the end
    while(1) {
        asm volatile ("hlt");
    }
}
*/

void kernel_main(void) {
    terminal_initialize();

    print_string("Mini OS: Bootloader+Kernel+Process+ML+FS\n");
    print_string("=========================================\n\n");

    // Initialize quietly
    process_init();
    ml_scheduler_init();
    fs_init();
    
    // Create demo processes
    init_demo_processes();

    print_string("System Ready. Starting Shell Demo...\n\n");
    
    // Start shell
    shell_start();

    while(1) {
        asm volatile ("hlt");
    }
}