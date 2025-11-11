#include "stats.h"
#include "kernel.h"

static int schedule_count[3] = {0, 0, 0}; // CPU, IO, ML
static int total_schedules = 0;

void update_schedule_stats(int process_type) {
    if(process_type >= 0 && process_type <= 2) {
        schedule_count[process_type]++;
        total_schedules++;
    }
}

void print_scheduling_stats(void) {
    print_string("\n=== ML Scheduling Statistics ===\n");
    print_string("Process Type  Schedules  Percentage\n");
    print_string("------------  ---------  ----------\n");
    
    print_string("CPU           ");
    print_int(schedule_count[0]);
    print_string("         ");
    if(total_schedules > 0) {
        print_int((schedule_count[0] * 100) / total_schedules);
        print_string("%\n");
    } else {
        print_string("0%\n");
    }
    
    print_string("IO            ");
    print_int(schedule_count[1]);
    print_string("         ");
    if(total_schedules > 0) {
        print_int((schedule_count[1] * 100) / total_schedules);
        print_string("%\n");
    } else {
        print_string("0%\n");
    }
    
    print_string("ML            ");
    print_int(schedule_count[2]);
    print_string("         ");
    if(total_schedules > 0) {
        print_int((schedule_count[2] * 100) / total_schedules);
        print_string("%\n");
    } else {
        print_string("0%\n");
    }
    
    print_string("Total: ");
    print_int(total_schedules);
    print_string(" schedules\n");
    print_string("===============================\n");
}

void reset_stats(void) {
    for(int i = 0; i < 3; i++) {
        schedule_count[i] = 0;
    }
    total_schedules = 0;
}