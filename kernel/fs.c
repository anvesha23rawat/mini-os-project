#include "fs.h"
#include "kernel.h"
#include "string.h"

static filesystem_t fs;
static uint8_t data_blocks[512][BLOCK_SIZE];

static void delay(int cycles) {
    for(int i = 0; i < cycles; i++) { 
        asm volatile ("nop"); 
    }
}

void fs_init(void) {
    print_string("[FS] Initializing File System...\n");
    delay(5000000);
    
    memset(&fs, 0, sizeof(fs));
    
    for(int i = 0; i < 4; i++) {
        fs.block_bitmap[i / 8] |= (1 << (i % 8));
    }
    
    fs_create("readme.txt");
    delay(5000000);
    fs_write("readme.txt", "Welcome to Mini OS with ML Scheduler!");
    delay(5000000);
    
    fs_create("ml_info.txt");
    delay(5000000);
    fs_write("ml_info.txt", "ML Scheduler: CPU=8, IO=4, ML=6 bursts");
    delay(5000000);
    
    print_string("[FS] File System Ready\n");
    delay(5000000);
}

int fs_create(const char* filename) {
    int i;
    for(i = 0; i < MAX_FILES; i++) {
        if(!fs.files[i].used) {
            break;
        }
    }
    
    if(i >= MAX_FILES) {
        print_string("[FS] Error: File table full\n");
        delay(5000000);
        return -1;
    }
    
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
        delay(5000000);
        return -1;
    }
    
    strcpy(fs.files[i].name, filename);
    fs.files[i].size = 0;
    fs.files[i].start_block = block;
    fs.files[i].used = 1;
    
    memset(data_blocks[block], 0, BLOCK_SIZE);
    
    print_string("[FS] Created file: ");
    delay(5000000);
    print_string(filename);
    delay(5000000);
    print_string("\n");
    delay(5000000);
    
    return 0;
}

int fs_write(const char* filename, const char* data) {
    int i;
    for(i = 0; i < MAX_FILES; i++) {
        if(fs.files[i].used && strcmp(fs.files[i].name, filename) == 0) {
            break;
        }
    }
    
    if(i >= MAX_FILES) {
        print_string("[FS] Error: File not found - ");
        delay(5000000);
        print_string(filename);
        delay(5000000);
        print_string("\n");
        delay(5000000);
        return -1;
    }
    
    int len = strlen(data);
    if(len >= BLOCK_SIZE) {
        len = BLOCK_SIZE - 1;
    }
    
    memcpy(data_blocks[fs.files[i].start_block], data, len);
    data_blocks[fs.files[i].start_block][len] = '\0';
    fs.files[i].size = len;
    
    print_string("[FS] Written to ");
    delay(5000000);
    print_string(filename);
    delay(5000000);
    print_string(": ");
    delay(5000000);
    print_string(data);
    delay(5000000);
    print_string("\n");
    delay(5000000);
    
    return 0;
}

int fs_read(const char* filename, char* buffer) {
    int i;
    for(i = 0; i < MAX_FILES; i++) {
        if(fs.files[i].used && strcmp(fs.files[i].name, filename) == 0) {
            break;
        }
    }
    
    if(i >= MAX_FILES) {
        print_string("[FS] Error: File not found - ");
        delay(5000000);
        print_string(filename);
        delay(5000000);
        print_string("\n");
        delay(5000000);
        return -1;
    }
    
    memcpy(buffer, data_blocks[fs.files[i].start_block], fs.files[i].size);
    buffer[fs.files[i].size] = '\0';
    
    return fs.files[i].size;
}

int fs_delete(const char* filename) {
    int i;
    for(i = 0; i < MAX_FILES; i++) {
        if(fs.files[i].used && strcmp(fs.files[i].name, filename) == 0) {
            break;
        }
    }
    
    if(i >= MAX_FILES) {
        print_string("[FS] Error: File not found - ");
        delay(5000000);
        print_string(filename);
        delay(5000000);
        print_string("\n");
        delay(5000000);
        return -1;
    }
    
    fs.block_bitmap[fs.files[i].start_block / 8] &= ~(1 << (fs.files[i].start_block % 8));
    
    fs.files[i].used = 0;
    
    print_string("[FS] Deleted file: ");
    delay(5000000);
    print_string(filename);
    delay(5000000);
    print_string("\n");
    delay(5000000);
    
    return 0;
}

void fs_list(void) {
    print_string("\n=== File System Contents ===\n");
    delay(5000000);
    print_string("Filename        Size\n");
    delay(5000000);
    print_string("--------        ----\n");
    delay(5000000);
    
    int count = 0;
    for(int i = 0; i < MAX_FILES; i++) {
        if(fs.files[i].used) {
            print_string(fs.files[i].name);
            delay(5000000);
            
            int padding = 16 - strlen(fs.files[i].name);
            for(int j = 0; j < padding; j++) {
                print_string(" ");
            }
            delay(5000000);
            
            print_int(fs.files[i].size);
            delay(5000000);
            print_string(" bytes\n");
            delay(5000000);
            count++;
        }
    }
    
    if(count == 0) {
        print_string("No files found\n");
        delay(5000000);
    }
    
    print_string("============================\n");
    delay(5000000);
}

int fs_exists(const char* filename) {
    for(int i = 0; i < MAX_FILES; i++) {
        if(fs.files[i].used && strcmp(fs.files[i].name, filename) == 0) {
            return 1;
        }
    }
    return 0;
}