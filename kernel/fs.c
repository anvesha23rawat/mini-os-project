#include "fs.h"
#include "kernel.h"
#include "string.h"

static filesystem_t fs;
static uint8_t data_blocks[512][BLOCK_SIZE]; // Simple storage

// Simple delay function
static void delay(int cycles) {
    for(int i = 0; i < cycles; i++) { 
        asm volatile ("nop"); 
    }
}

void fs_init(void) {
    print_string("[FS] Initializing File System...\n");
    delay(500000000); // 5 second delay
    
    // Clear file system
    memset(&fs, 0, sizeof(fs));
    
    // Mark first few blocks as used (for metadata)
    for(int i = 0; i < 4; i++) {
        fs.block_bitmap[i / 8] |= (1 << (i % 8));
    }
    
    // Create some demo files
    fs_create("readme.txt");
    delay(5000000000); // 5 second delay
    fs_write("readme.txt", "Welcome to Mini OS with ML Scheduler!");
    delay(50000000000); // 5 second delay
    
    fs_create("ml_info.txt");
    delay(5000000000); // 5 second delay
    fs_write("ml_info.txt", "ML Scheduler: CPU=8, IO=4, ML=6 bursts");
    delay(5000000000); // 5 second delay
    
    print_string("[FS] File System Ready\n");
    delay(5000000000); // 5 second delay
}

int fs_create(const char* filename) {
    // Find free file entry
    int i;
    for(i = 0; i < MAX_FILES; i++) {
        if(!fs.files[i].used) {
            break;
        }
    }
    
    if(i >= MAX_FILES) {
        print_string("[FS] Error: File table full\n");
        delay(500000000); // 5 second delay
        return -1;
    }
    
    // Find free block
    int block = -1;
    for(int j = 0; j < 512; j++) {
        if(!(fs.block_bitmap[j / 8] & (1 << (j % 8)))) {
            block = j;
            fs.block_bitmap[j / 8] |= (1 << (j % 8));
            break;
        }
    }
    
    if(block == -1) {
        print_string("[FS] Error: No free blocks\n");
        delay(50000000000); // 5 second delay
        return -1;
    }
    
    // Create file entry
    strcpy(fs.files[i].name, filename);
    fs.files[i].size = 0;
    fs.files[i].start_block = block;
    fs.files[i].used = 1;
    
    // Clear block
    memset(data_blocks[block], 0, BLOCK_SIZE);
    
    print_string("[FS] Created file: ");
    delay(25000000000); // 2.5 second delay
    print_string(filename);
    delay(25000000000); // 2.5 second delay
    print_string("\n");
    delay(50000000000); // 5 second delay
    
    return 0;
}

int fs_write(const char* filename, const char* data) {
    // Find file
    int i;
    for(i = 0; i < MAX_FILES; i++) {
        if(fs.files[i].used && strcmp(fs.files[i].name, filename) == 0) {
            break;
        }
    }
    
    if(i >= MAX_FILES) {
        print_string("[FS] Error: File not found - ");
        delay(2500000000); // 2.5 second delay
        print_string(filename);
        delay(2500000000); // 2.5 second delay
        print_string("\n");
        delay(5000000000); // 5 second delay
        return -1;
    }
    
    // Write data
    int len = strlen(data);
    if(len >= BLOCK_SIZE) {
        len = BLOCK_SIZE - 1;
    }
    
    memcpy(data_blocks[fs.files[i].start_block], data, len);
    data_blocks[fs.files[i].start_block][len] = '\0';
    fs.files[i].size = len;
    
    print_string("[FS] Written to ");
    delay(2500000000); // 2.5 second delay
    print_string(filename);
    delay(25000000000); // 2.5 second delay
    print_string(": ");
    delay(25000000000); // 2.5 second delay
    print_string(data);
    delay(25000000000); // 2.5 second delay
    print_string("\n");
    delay(50000000000); // 5 second delay
    
    return 0;
}

int fs_read(const char* filename, char* buffer) {
    // Find file
    int i;
    for(i = 0; i < MAX_FILES; i++) {
        if(fs.files[i].used && strcmp(fs.files[i].name, filename) == 0) {
            break;
        }
    }
    
    if(i >= MAX_FILES) {
        print_string("[FS] Error: File not found - ");
        delay(25000000000); // 2.5 second delay
        print_string(filename);
        delay(25000000000); // 2.5 second delay
        print_string("\n");
        delay(50000000000); // 5 second delay
        return -1;
    }
    
    // Read data
    memcpy(buffer, data_blocks[fs.files[i].start_block], fs.files[i].size);
    buffer[fs.files[i].size] = '\0';
    
    return fs.files[i].size;
}

int fs_delete(const char* filename) {
    // Find file
    int i;
    for(i = 0; i < MAX_FILES; i++) {
        if(fs.files[i].used && strcmp(fs.files[i].name, filename) == 0) {
            break;
        }
    }
    
    if(i >= MAX_FILES) {
        print_string("[FS] Error: File not found - ");
        delay(2500000000); // 2.5 second delay
        print_string(filename);
        delay(2500000000); // 2.5 second delay
        print_string("\n");
        delay(5000000000); // 5 second delay
        return -1;
    }
    
    // Free block
    fs.block_bitmap[fs.files[i].start_block / 8] &= ~(1 << (fs.files[i].start_block % 8));
    
    // Free file entry
    fs.files[i].used = 0;
    
    print_string("[FS] Deleted file: ");
    delay(2500000000); // 2.5 second delay
    print_string(filename);
    delay(25000000000); // 2.5 second delay
    print_string("\n");
    delay(50000000000); // 5 second delay
    
    return 0;
}

void fs_list(void) {
    print_string("\n=== File System Contents ===\n");
    delay(50000000000); // 5 second delay
    print_string("Filename        Size\n");
    delay(500000000000); // 5 second delay
    print_string("--------        ----\n");
    delay(500000000000); // 5 second delay
    
    int count = 0;
    for(int i = 0; i < MAX_FILES; i++) {
        if(fs.files[i].used) {
            print_string(fs.files[i].name);
            delay(25000000000); // 2.5 second delay
            
            // Padding for alignment
            int padding = 16 - strlen(fs.files[i].name);
            for(int j = 0; j < padding; j++) {
                print_string(" ");
            }
            delay(25000000000); // 2.5 second delay
            
            print_int(fs.files[i].size);
            delay(25000000000); // 2.5 second delay
            print_string(" bytes\n");
            delay(50000000000); // 5 second delay
            count++;
        }
    }
    
    if(count == 0) {
        print_string("No files found\n");
        delay(50000000000); // 5 second delay
    }
    
    print_string("============================\n");
    delay(500000000000); // 5 second delay
}

int fs_exists(const char* filename) {
    for(int i = 0; i < MAX_FILES; i++) {
        if(fs.files[i].used && strcmp(fs.files[i].name, filename) == 0) {
            return 1;
        }
    }
    return 0;
}