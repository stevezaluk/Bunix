#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>

// Constants
#define MAX_HISTORY_SIZE 10  // Define the size of the command history buffer

// Command structure
typedef struct {
    const char *name;
    void (*func)(const char *);
    const char *description;
} Command;

// Command function prototypes
void help_command(const char *args);
void clear_command(const char *args);
void echo_command(const char *args);
void cpuinfo_command(const char *args);
void reboot_command(const char *args);
void shutdown_command(const char *args);
void time_command(const char *args);
void date_command(const char *args);
void uptime_command(const char *args);

// Shell functions
int shell_init(void);
void shell_run(void);
void uptime_init(void);

#endif // SHELL_H
