#include "process.h"
#include "kernel.h"
#include "fs.h" 

// Demo process functions
void cpu_process(void) {
    while(1) {
        print_string("[CPU] Process running\n");
        for(int i = 0; i < 1000000; i++);
        process_yield();
    }
}

void io_process(void) {
    while(1) {
        print_string("[IO] Process running\n");
        for(int i = 0; i < 500000; i++);
        process_yield();
    }
}

void ml_process(void) {
    while(1) {
        print_string("[ML] Process running\n");
        for(int i = 0; i < 800000; i++);
        process_yield();
    }
}
void demo_file_system(void) {
    print_string("\n=== File System Demo ===\n");
    
    print_string("[FS] Listing all files:\n");
    fs_list(); 
    
    char buffer[100];
    int bytes_read = fs_read("readme.txt", buffer); 
    if(bytes_read > 0) {
        print_string("[FS] Read from readme.txt: ");
        print_string(buffer);
        print_string("\n");
    }
    
    bytes_read = fs_read("ml_info.txt", buffer);  // CORRECT - 2 arguments
    if(bytes_read > 0) {
        print_string("[FS] Read from ml_info.txt: ");
        print_string(buffer);
        print_string("\n");
    }
    
    // Step 4: Create and write to a new file
    print_string("[FS] Creating new file: test.txt\n");
    fs_create("test.txt");
    fs_write("test.txt", "This is a test file from demo!");
    
    // Step 5: List files again to show new file - FIXED: fs_list_files -> fs_list
    print_string("[FS] Files after creating test.txt:\n");
    fs_list(); 
    
    print_string("==========================\n\n");
}

// Initialize demo processes
void init_demo_processes(void) {
    print_string("\n=== Creating Demo Processes ===\n");
    
    // Step 1: First run file system demo
    demo_file_system();
    
    // Step 2:create processes
    int pid1 = process_create(cpu_process, "CPU_Process", 0);
    ml_update_process_features(pid1, 0);
    
    int pid2 = process_create(io_process, "IO_Process", 1);
    ml_update_process_features(pid2, 1);
    
    int pid3 = process_create(ml_process, "ML_Process", 2);
    ml_update_process_features(pid3, 2);
    
    print_string("=== Demo Processes Created ===\n\n");
}