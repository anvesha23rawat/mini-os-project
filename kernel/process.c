#include "process.h"
#include "kernel.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

static pcb_t process_table[MAX_PROCESSES];

pcb_t* current_process = NULL;
pcb_t* ready_queue = NULL;

static int next_pid = 1;
static scheduler_type_t current_scheduler = SCHEDULER_ROUND_ROBIN;

static void delay(int cycles) {
    for(int i = 0; i < cycles; i++) { 
        asm volatile ("nop"); 
    }
}

void process_init(void) {
    print_string("[PROCESS] Initializing Process Manager...\n");
    delay(5000000);
    
    for(int i = 0; i < MAX_PROCESSES; i++) {
        process_table[i].state = PROCESS_NEW;
        process_table[i].pid = 0;
        process_table[i].next = NULL;
    }
    
    process_table[0].pid = 0;
    process_table[0].state = PROCESS_READY;
    process_table[0].priority = 0;
    
    const char* idle_name = "idle";
    int i = 0;
    while (idle_name[i] != '\0' && i < 31) {
        process_table[0].name[i] = idle_name[i];
        i++;
    }
    process_table[0].name[i] = '\0';
    
    current_process = &process_table[0];
    ready_queue = &process_table[0];
    ready_queue->next = ready_queue;
    
    print_string("[PROCESS] Process Manager Ready\n");
    delay(5000000);
}

int process_create(void (*entry_point)(void), const char* name, int process_type) {
    int i;
    for (i = 1; i < MAX_PROCESSES; i++) {
        if (process_table[i].state == PROCESS_NEW || 
            process_table[i].state == PROCESS_TERMINATED) {
            break;
        }
    }
    
    if (i >= MAX_PROCESSES) {
        print_string("[PROCESS] Error: Process table full\n");
        delay(5000000);
        return -1;
    }
    
    pcb_t* pcb = &process_table[i];
    pcb->pid = next_pid++;
    pcb->state = PROCESS_READY;
    pcb->priority = 1;
    pcb->time_slice = 10;
    
    int j = 0;
    while (name[j] != '\0' && j < 31) {
        pcb->name[j] = name[j];
        j++;
    }
    pcb->name[j] = '\0';
    
    pcb->eip = (uint32_t)entry_point;
    
    pcb_t* last = ready_queue;
    while (last->next != ready_queue) {
        last = last->next;
    }
    pcb->next = ready_queue;
    last->next = pcb;
    
    ml_update_process_features(pcb->pid, process_type);
    
    print_string("[PROCESS] Created process: ");
    delay(5000000);
    print_string(name);
    delay(5000000);
    print_string(" (PID: ");
    delay(5000000);
    print_int(pcb->pid);
    delay(5000000);
    print_string(")\n");
    delay(5000000);
    
    return pcb->pid;
}

void process_schedule(void) {
    if (ready_queue == NULL) return;
    
    pcb_t* next = current_process->next;
    while (next != current_process && next->state != PROCESS_READY) {
        next = next->next;
    }
    
    if (next->state == PROCESS_READY && next != current_process) {
        current_process->state = PROCESS_READY;
        next->state = PROCESS_RUNNING;
        current_process = next;
        
        print_string("[RR] Switched to: ");
        delay(5000000);
        print_string(current_process->name);
        delay(5000000);
        print_string(" (PID: ");
        delay(5000000);
        print_int(current_process->pid);
        delay(5000000);
        print_string(")\n");
        delay(5000000);
    }
}

pcb_t* get_current_process(void) {
    return current_process;
}

void process_yield(void) {
    if(current_scheduler == SCHEDULER_ML_BASED) {
        ml_schedule();
    } else {
        process_schedule();
    }
}

void process_exit(void) {
    current_process->state = PROCESS_TERMINATED;
    
    pcb_t* prev = ready_queue;
    while (prev->next != current_process) {
        prev = prev->next;
    }
    prev->next = current_process->next;
    
    if (current_process == ready_queue) {
        ready_queue = current_process->next;
    }
    
    print_string("[PROCESS] Process terminated: ");
    delay(5000000);
    print_string(current_process->name);
    delay(5000000);
    print_string("\n");
    delay(5000000);
    
    process_yield();
}

void set_scheduler_type(scheduler_type_t type) {
    current_scheduler = type;
    print_string("[PROCESS] Scheduler set to: ");
    delay(5000000);
    print_string(type == SCHEDULER_ROUND_ROBIN ? "Round Robin" : "ML Based");
    delay(5000000);
    print_string("\n");
    delay(5000000);
}

void print_process_table(void) {
    print_string("\n=== Process Table ===\n");
    delay(5000000);
    print_string("Slot PID State Name\n");
    delay(5000000);
    
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].state != PROCESS_NEW) {
            print_int(i);
            delay(5000000);
            print_string(" ");
            delay(5000000);
            print_int(process_table[i].pid);
            delay(5000000);
            print_string(" ");
            delay(5000000);
            print_int(process_table[i].state);
            delay(5000000);
            print_string(" ");
            delay(5000000);
            print_string(process_table[i].name);
            delay(5000000);
            print_string("\n");
            delay(5000000);
        }
    }
    print_string("====================\n");
    delay(5000000);
}