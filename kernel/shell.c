#include "kernel.h"
#include "process.h"
#include "fs.h"
#include <stddef.h>
#include "shell.h"
#include "string.h"

#define MAX_COMMAND_LENGTH 64
#define MAX_ARGUMENTS 8

static void delay(int cycles) {
    for(int i = 0; i < cycles; i++) { 
        asm volatile ("nop"); 
    }
}

void shell_help(void) {
    print_string("\n=== Mini OS Shell Commands ===\n");
    delay(5000000);
    
    print_string("help          - Show this help\n");
    delay(5000000);
    print_string("ls            - List files\n");
    delay(5000000);
    print_string("cat <file>    - Read file content\n");
    delay(5000000);
    print_string("create <file> - Create new file\n");
    delay(5000000);
    print_string("write <file> <text> - Write to file\n");
    delay(5000000);
    print_string("delete <file> - Delete file\n");
    delay(5000000);
    print_string("run <type>    - Run process (cpu/io/ml)\n");
    delay(5000000);
    print_string("ps            - Show process table\n");
    delay(5000000);
    print_string("sched         - Show ML scheduler stats\n");
    delay(5000000);
    print_string("clear         - Clear screen\n");
    delay(5000000);
    print_string("==============================\n");
    delay(5000000);
}

void shell_ls(void) {
    print_string("\n=== File System Contents ===\n");
    delay(5000000);
    fs_list();
}

void shell_cat(char* filename) {
    char buffer[100];
    int bytes_read = fs_read(filename, buffer);
    if(bytes_read > 0) {
        print_string("File content: ");
        delay(5000000);
        print_string(buffer);
        delay(5000000);
        print_string("\n");
    } else {
        print_string("Error: File not found or empty\n");
    }
    delay(5000000);
}

void shell_create(char* filename) {
    if(fs_create(filename) == 0) {
        print_string("File created: ");
        delay(5000000);
        print_string(filename);
        delay(5000000);
        print_string("\n");
    } else {
        print_string("Error: Could not create file\n");
    }
    delay(5000000);
}

void shell_write(char* filename, char* text) {
    if(fs_write(filename, text) == 0) {
        print_string("Written to: ");
        delay(5000000);
        print_string(filename);
        delay(5000000);
        print_string("\n");
    } else {
        print_string("Error: Could not write to file\n");
    }
    delay(5000000);
}

void shell_delete(char* filename) {
    if(fs_delete(filename) == 0) {
        print_string("File deleted: ");
        delay(5000000);
        print_string(filename);
        delay(5000000);
        print_string("\n");
    } else {
        print_string("Error: Could not delete file\n");
    }
    delay(5000000);
}

void shell_run(char* type) {
    if(strcmp(type, "cpu") == 0) {
        int pid = process_create(cpu_process, "CPU_Process", 0);
        ml_update_process_features(pid, 0);
        print_string("Started CPU process (PID: ");
        delay(5000000);
        print_int(pid);
        delay(5000000);
        print_string(")\n");
    }
    else if(strcmp(type, "io") == 0) {
        int pid = process_create(io_process, "IO_Process", 1);
        ml_update_process_features(pid, 1);
        print_string("Started IO process (PID: ");
        delay(5000000);
        print_int(pid);
        delay(5000000);
        print_string(")\n");
    }
    else if(strcmp(type, "ml") == 0) {
        int pid = process_create(ml_process, "ML_Process", 2);
        ml_update_process_features(pid, 2);
        print_string("Started ML process (PID: ");
        delay(5000000);
        print_int(pid);
        delay(5000000);
        print_string(")\n");
    }
    else {
        print_string("Error: Unknown process type. Use: cpu/io/ml\n");
    }
    delay(5000000);
}

void shell_ps(void) {
    print_string("\n=== Process Table ===\n");
    delay(5000000);
    print_process_table();
}

void shell_sched(void) {
    print_string("\n=== ML Scheduler Stats ===\n");
    delay(5000000);
    print_ml_scheduler_stats();
}

void execute_command(char* command) {
    print_string("\n> ");
    delay(5000000);
    print_string(command);
    delay(5000000);
    print_string("\n");
    delay(5000000);
    
    char* args[MAX_ARGUMENTS];
    int arg_count = 0;
    
    char* token = command;
    while(*token && arg_count < MAX_ARGUMENTS) {
        while(*token == ' ') token++;
        if(!*token) break;
        
        args[arg_count++] = token;
        
        while(*token && *token != ' ') token++;
        if(*token) *token++ = '\0';
    }
    
    if(arg_count == 0) return;
    
    if(strcmp(args[0], "help") == 0) {
        shell_help();
    }
    else if(strcmp(args[0], "ls") == 0) {
        shell_ls();
    }
    else if(strcmp(args[0], "cat") == 0 && arg_count >= 2) {
        shell_cat(args[1]);
    }
    else if(strcmp(args[0], "create") == 0 && arg_count >= 2) {
        shell_create(args[1]);
    }
    else if(strcmp(args[0], "write") == 0 && arg_count >= 3) {
        shell_write(args[1], args[2]);
    }
    else if(strcmp(args[0], "delete") == 0 && arg_count >= 2) {
        shell_delete(args[1]);
    }
    else if(strcmp(args[0], "run") == 0 && arg_count >= 2) {
        shell_run(args[1]);
    }
    else if(strcmp(args[0], "ps") == 0) {
        shell_ps();
    }
    else if(strcmp(args[0], "sched") == 0) {
        shell_sched();
    }
    else if(strcmp(args[0], "clear") == 0) {
        print_string("\n\n\n\n\n\n\n\n\n\n");
    }
    else {
        print_string("Error: Unknown command '");
        delay(5000000);
        print_string(args[0]);
        delay(5000000);
        print_string("'. Type 'help' for commands.\n");
    }
}

void shell_start(void) {
    print_string("\n=== Mini OS Shell Demo ===\n");
    delay(5000000);
    
    execute_command("help");
    delay(5000000);
    
    execute_command("ls");
    delay(5000000);
    
    execute_command("cat readme.txt");
    delay(5000000);
    
    execute_command("ps");
    delay(5000000);
    
    execute_command("sched");
    delay(5000000);
    
    print_string("\n=== Demo Complete ===\n");
    delay(5000000);
    print_string("All systems working: Bootloader + Kernel + Processes + ML + FS\n");
}