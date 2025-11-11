#ifndef FS_H
#define FS_H

#include <stdint.h>

#define MAX_FILES 32
#define MAX_FILENAME 32
#define BLOCK_SIZE 512
#define MAX_FILE_SIZE 4096

typedef struct {
    char name[MAX_FILENAME];
    uint32_t size;
    uint32_t start_block;
    uint8_t used;
} file_entry_t;

typedef struct {
    file_entry_t files[MAX_FILES];
    uint8_t block_bitmap[64]; // 64 * 8 = 512 blocks
} filesystem_t;

// File system functions
void fs_init(void);
int fs_create(const char* filename);
int fs_write(const char* filename, const char* data);
int fs_read(const char* filename, char* buffer);
int fs_delete(const char* filename);
void fs_list(void);
int fs_exists(const char* filename);

#endif