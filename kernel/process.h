#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#define MAX_PROCESSES 16
#define STACK_SIZE 4096

// Scheduler types
typedef enum {
    SCHEDULER_ROUND_ROBIN,
    SCHEDULER_ML_BASED
} scheduler_type_t;

// Process states
typedef enum {
    PROCESS_NEW,
    PROCESS_READY,
    PROCESS_RUNNING, 
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
} process_state_t;

// Process Control Block
typedef struct process_control_block {
    uint32_t pid;
    process_state_t state;
    uint32_t esp;
    uint32_t ebp;
    uint32_t eip;
    uint32_t page_dir;
    uint32_t stack_top;
    int priority;
    int time_slice;
    char name[32];
    struct process_control_block *next;
} pcb_t;

// Global variables
extern pcb_t* ready_queue;
extern pcb_t* current_process;

// Function declarations
void process_init(void);
int process_create(void (*entry_point)(void), const char* name, int process_type);
void process_schedule(void);
void ml_schedule(void);
pcb_t* get_current_process(void);
void process_yield(void);
void process_exit(void);
void set_scheduler_type(scheduler_type_t type);
void print_process_table(void);
void ml_scheduler_init(void);
void ml_update_process_features(int pid, int process_type);
void print_ml_scheduler_stats(void);
void cpu_process(void);
void io_process(void); 
void ml_process(void);
void init_demo_processes(void);

#endif