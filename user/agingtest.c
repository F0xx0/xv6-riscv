#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

#define NCHILD 4
#define WORK_TICKS 1200
#define MONITOR_INTERVAL 20
#define MONITOR_SNAPSHOTS 40

int priorities[NCHILD] = {40, 30, 15, 5};

// CPU-intensive workload.
// Uses uptime() to keep each worker active for approximately
// WORK_TICKS of elapsed time.
void
worker(int priority)
{
    volatile unsigned long result = 0;

    pause(10); //allow monitor to begin
    int start = uptime();

    while (uptime() - start < WORK_TICKS) {
        for (int i = 0; i < 10000; i++) {
            result += i;
        }
    }

    exit(0);
}

// Execute the xv6 ps program.
void
run_ps(void)
{
    int pid = fork();

    if (pid < 0) {
        printf("ERROR: Could not fork ps process\n");
        return;
    }

    if (pid == 0) {
        char *args[] = {"ps", 0};

        exec("ps", args);

        printf("ERROR: Could not execute ps\n");
        exit(1);
    }

    wait(0);
}

// Monitor processes periodically.
void
monitor(void)
{
    // Maximum priority available.
    if (setpriority(getpid(), 49) < 0) {
        printf("ERROR: Could not set monitor priority\n");
        exit(1);
    }

    printf("\n");
    printf("\n");
    printf("       PRIORITY AGING MONITOR\n");
    printf("========================================\n");

    printf("Monitor priority: 49\n");
    printf("Snapshot interval: %d ticks\n",
           MONITOR_INTERVAL);

    printf("Total snapshots: %d\n",
           MONITOR_SNAPSHOTS);

    for (int i = 0; i < MONITOR_SNAPSHOTS; i++) {

        printf("\n");
        printf("========================================\n");
        printf("PROCESS SNAPSHOT %d\n", i + 1);
        printf("System uptime: %d ticks\n", uptime());
        printf("========================================\n");

        run_ps();

        if (i < MONITOR_SNAPSHOTS - 1) {
            pause(MONITOR_INTERVAL);
        }
    }

    printf("\n");
    printf("MONITORING COMPLETED\n");
    printf("\n");

    exit(0);
}

int
main(int argc, char *argv[])
{
    int pid;
    int monitor_pid;

    printf("\n");
    printf("========================================\n");
    printf("          PRIORITY AGING TEST\n");
    printf("========================================\n");

    printf("\n");
    printf("Number of worker processes: %d\n", NCHILD);
    printf("Worker duration: %d ticks\n", WORK_TICKS);
    printf("Aging interval: 10 ticks\n");

    printf("\n");
    printf("Worker priorities:\n");

    for (int i = 0; i < NCHILD; i++) {
        printf("Worker %d: Priority %d\n",
               i + 1, priorities[i]);
    }

    printf("\n");
    printf("Creating worker processes...\n");

    // Create CPU-bound worker processes.
    for (int i = 0; i < NCHILD; i++) {

        pid = fork();

        if (pid < 0) {
            printf("ERROR: Fork failed\n");
            exit(1);
        }

        if (pid == 0) {

            if (setpriority(getpid(), priorities[i]) < 0) {
                printf("ERROR: Could not set priority\n");
                exit(1);
            }

            worker(priorities[i]);
        }

        printf("Created PID %d | Priority %d\n",
               pid, priorities[i]);
    }

    printf("\n");
    printf("All worker processes created.\n");

    // Create monitoring process.
    monitor_pid = fork();

    if (monitor_pid < 0) {
        printf("ERROR: Monitor fork failed\n");
        exit(1);
    }

    if (monitor_pid == 0) {
        monitor();
    }

    printf("Created monitor PID %d\n", monitor_pid);

    printf("\n");
    printf("          STARTING BENCHMARK\n");
    printf("========================================\n");
    printf("\n");

    // Wait for all four workers and the monitor.
    for (int i = 0; i < NCHILD + 1; i++) {
        wait(0);
    }

    printf("\n");
    printf("TEST COMPLETED\n");
    printf("\n");

    printf("All worker processes finished.\n");
    printf("Monitoring completed.\n");

    exit(0);
}
