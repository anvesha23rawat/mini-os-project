#ifndef SHELL_H
#define SHELL_H

void shell_start(void);
void execute_command(char* command);

// Shell command functions
void shell_help(void);
void shell_ls(void);
void shell_cat(char* filename);
void shell_create(char* filename);
void shell_write(char* filename, char* text);
void shell_delete(char* filename);
void shell_run(char* type);
void shell_ps(void);
void shell_sched(void);

#endif