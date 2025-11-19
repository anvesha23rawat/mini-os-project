#include "process.h"
#include "kernel.h"
#include "string.h"
#include <stddef.h>

typedef struct {
    int pid;
    int process_type;
    int predicted_burst;
    float priority_score;
} ml_process_info_t;

#define MAX_ML_PROCESSES 16

static ml_process_info_t ml_process_data[MAX_ML_PROCESSES];
static int ml_scheduler_active = 0;

static void delay(int cycles) {
    for(int i = 0; i < cycles; i++) { 
        asm volatile ("nop"); 
    }
}

int ml_predict_time_slice(int process_type) {
    if (process_type < 0 || process_type > 2) {
        print_string("[ML WARNING] Invalid process type: ");
        print_int(process_type);
        print_string("\n");
        delay(5000000);
        return 5;
    }
    
    int burst;
    switch(process_type) {
        case 0: burst = 8; break;
        case 1: burst = 4; break;  
        case 2: burst = 6; break;
        default: burst = 5; break;
    }
    
    print_string("[ML DEBUG] Predict - Type: ");
    delay(5000000);
    print_int(process_type);
    delay(5000000);
    print_string(" -> Burst: ");
    delay(5000000);
    print_int(burst);
    delay(5000000);
    print_string("\n");
    delay(5000000);
    
    return burst;
}

void ml_scheduler_init(void) {
    print_string("[ML] Initializing Random Forest Scheduler\n");
    delay(5000000);
    for(int i = 0; i < MAX_ML_PROCESSES; i++) {
        ml_process_data[i].pid = 0;
        ml_process_data[i].process_type = -1;
        ml_process_data[i].predicted_burst = 0;
        ml_process_data[i].priority_score = 0.0f;
    }
    ml_scheduler_active = 1;
}

void ml_update_process_features(int pid, int process_type) {
    if (pid <= 0) {
        print_string("[ML ERROR] Invalid PID: ");
        delay(5000000);
        print_int(pid);
        delay(5000000);
        print_string("\n");
        delay(5000000);
        return;
    }
    
    if (process_type < 0 || process_type > 2) {
        print_string("[ML ERROR] Invalid process type for PID ");
        delay(5000000);
        print_int(pid);
        delay(5000000);
        print_string(": ");
        delay(5000000);
        print_int(process_type);
        delay(5000000);
        print_string("\n");
        delay(5000000);
        return;
    }
    
    for (int i = 0; i < MAX_ML_PROCESSES; i++) {
        if (ml_process_data[i].pid == 0) {
            ml_process_data[i].pid = pid;
            ml_process_data[i].process_type = process_type;
            ml_process_data[i].predicted_burst = ml_predict_time_slice(process_type);
            
            if (ml_process_data[i].predicted_burst == 0) {
                print_string("[ML WARNING] Zero burst detected for PID ");
                delay(5000000);
                print_int(pid);
                delay(5000000);
                print_string(", using default priority\n");
                delay(5000000);
                ml_process_data[i].priority_score = 0.1f;
            } else {
                ml_process_data[i].priority_score = 1.0f / ml_process_data[i].predicted_burst;
            }
            
            print_string("[ML] Process ");
            delay(5000000);
            print_int(pid);
            delay(5000000);
            print_string(": Type=");
            delay(5000000);
            print_int(process_type);
            delay(5000000);
            print_string(", Predicted burst=");
            delay(5000000);
            print_int(ml_process_data[i].predicted_burst);
            delay(5000000);
            print_string(", Priority=");
            delay(5000000);
            print_float(ml_process_data[i].priority_score);
            delay(5000000);
            print_string("\n");
            delay(5000000);
            break;
        }
    }
}

void ml_schedule(void) {
    if (!ml_scheduler_active || ready_queue == NULL) return;
    
    float highest_priority = -1.0f;
    pcb_t* next_process = NULL;
    
    pcb_t* current = ready_queue;
    do {
        for (int i = 0; i < MAX_ML_PROCESSES; i++) {
            if (ml_process_data[i].pid == (int)current->pid && current->state == PROCESS_READY) {
                if (ml_process_data[i].priority_score > highest_priority) {
                    highest_priority = ml_process_data[i].priority_score;
                    next_process = current;
                }
                break;
            }
        }
        current = current->next;
    } while (current != ready_queue);
    
    if (next_process != NULL && next_process != current_process) {
        current_process->state = PROCESS_READY;
        next_process->state = PROCESS_RUNNING;
        current_process = next_process;
        
        for (int i = 0; i < MAX_ML_PROCESSES; i++) {
            if (ml_process_data[i].pid == (int)current_process->pid) {
                print_string("[ML] Selected: ");
                delay(5000000);
                print_string(current_process->name);
                delay(5000000);
                print_string(" (Burst=");
                delay(5000000);
                print_int(ml_process_data[i].predicted_burst);
                delay(5000000);
                print_string(", Priority=");
                delay(5000000);
                print_float(ml_process_data[i].priority_score);
                delay(5000000);
                print_string(")\n");
                delay(5000000);
                break;
            }
        }
    }
}

void print_ml_scheduler_stats(void) {
    print_string("\n=== ML Scheduler Stats ===\n");
    delay(5000000);
    print_string("PID Type Prediction Priority\n");
    delay(5000000);
    
    for (int i = 0; i < MAX_ML_PROCESSES; i++) {
        if (ml_process_data[i].pid != 0) {
            print_int(ml_process_data[i].pid);
            delay(5000000);
            print_string(" ");
            delay(5000000);
            
            switch(ml_process_data[i].process_type) {
                case 0: print_string("CPU "); break;
                case 1: print_string("IO  "); break;
                case 2: print_string("ML  "); break;
                default: print_string("UNK "); break;
            }
            delay(5000000);
            print_string(" ");
            delay(5000000);
            
            print_int(ml_process_data[i].predicted_burst);
            delay(5000000);
            print_string(" ");
            delay(5000000);
            
            print_float(ml_process_data[i].priority_score);
            delay(5000000);
            print_string("\n");
            delay(5000000);
        }
    }
    print_string("==========================\n");
    delay(5000000);
}