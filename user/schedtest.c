#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NCHILDREN 3
#define WORK 30000000

// CPU-intensive workload
void
work(int pid, int priority)
{
    volatile unsigned long counter = 0;

    printf("PID %d (priority %d): STARTED\n",
           pid, priority);

    for (unsigned long i = 0; i < WORK; i++) {
        counter += i;
    }

    printf("PID %d (priority %d): FINISHED\n",
           pid, priority);
}

// Run one scheduling experiment
void
run_test(int priorities[], char *description)
{
    int pids[NCHILDREN];
    int startpipe[2];

    printf("\n================================\n");
    printf("%s\n", description);
    printf("================================\n");

    if (pipe(startpipe) < 0) {
        printf("Pipe creation failed\n");
        return;
    }

    // Create children
    for (int i = 0; i < NCHILDREN; i++) {

        int pid = fork();

        if (pid < 0) {
            printf("Fork failed\n");
            exit(1);
        }

        if (pid == 0) {
            char signal;

            close(startpipe[1]);

            // Wait until parent assigns priorities
            if (read(startpipe[0], &signal, 1) != 1)
                exit(1);

            close(startpipe[0]);

            work(getpid(), getpriority(getpid()));

            exit(0);
        }

        pids[i] = pid;
    }

    close(startpipe[0]);

    // Assign priorities before releasing children
    for (int i = 0; i < NCHILDREN; i++) {

        int result = setpriority(pids[i], priorities[i]);

        printf("Child PID %d assigned priority %d (result %d)\n",
               pids[i], priorities[i], result);
    }

    printf("\nReleasing children...\n");

    // Release all children
    for (int i = 0; i < NCHILDREN; i++) {
        write(startpipe[1], "X", 1);
    }

    close(startpipe[1]);

    // Wait for all children
    for (int i = 0; i < NCHILDREN; i++) {
        wait(0);
    }

    printf("\nTest completed.\n");
}

int
main(void)
{
    int different_priorities[NCHILDREN] = {10, 30, 49};
    int equal_priorities[NCHILDREN] = {20, 20, 20};

    printf("\n");
    printf("================================\n");
    printf("       SCHEDULER TEST\n");
    printf("================================\n");

    run_test(different_priorities,
             "TEST 1: DIFFERENT PRIORITIES");

    run_test(equal_priorities,
             "TEST 2: EQUAL PRIORITIES");

    printf("\nAll scheduler tests completed.\n");

    exit(0);
}
