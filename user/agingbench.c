#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NCHILD 4
#define WORK_TICKS 100

int priorities[NCHILD] = {40, 30, 15, 5};

struct result {
    int pid;
    int priority;
    int first_run;
    int finish;
};

int
main(void)
{
    int pipes[NCHILD][2];
    int pids[NCHILD];
    int created[NCHILD];
    struct result results[NCHILD];

    int total_response = 0;
    int total_turnaround = 0;

    printf("\n");
    printf("PRIORITY SCHEDULER BENCHMARK\n");
    printf("\n");

    for (int i = 0; i < NCHILD; i++) {

        if (pipe(pipes[i]) < 0) {
            printf("ERROR: pipe failed\n");
            exit(1);
        }

        created[i] = uptime();

        int pid = fork();

        if (pid < 0) {
            printf("ERROR: fork failed\n");
            exit(1);
        }

        if (pid == 0) {

            close(pipes[i][0]);

            setpriority(getpid(), priorities[i]);

            struct result r;

            r.pid = getpid();
            r.priority = priorities[i];
            r.first_run = uptime();

            volatile unsigned long result = 0;
            int start = uptime();

            while (uptime() - start < WORK_TICKS) {
                for (int j = 0; j < 10000; j++) {
                    result += j;
                }
            }

            r.finish = uptime();

            write(pipes[i][1], &r, sizeof(r));
            close(pipes[i][1]);

            exit(0);
        }

        pids[i] = pid;

        close(pipes[i][1]);

        printf("Created PID %d | Priority %d\n",
               pid, priorities[i]);
    }

    printf("\nWaiting for workers...\n");

    for (int i = 0; i < NCHILD; i++) {

        if (read(pipes[i][0], &results[i],
                 sizeof(struct result)) != sizeof(struct result)) {
            printf("ERROR: Could not read result\n");
            exit(1);
        }

        close(pipes[i][0]);
    }

    for (int i = 0; i < NCHILD; i++) {
        wait(0);
    }

    printf("\n");
    printf("RESULTS:\n");
    printf("\n");

    for (int i = 0; i < NCHILD; i++) {

        int response =
            results[i].first_run - created[i];

        int turnaround =
            results[i].finish - created[i];

        total_response += response;
        total_turnaround += turnaround;

        printf("\nPID %d | Priority %d\n",
               pids[i], results[i].priority);

        printf("Response time: %d ticks\n", response);
        printf("Turnaround time: %d ticks\n", turnaround);
    }

    printf("\n");
    printf("AVERAGES\n");
    printf("\n");

    printf("Average response time: %d ticks\n",
           total_response / NCHILD);

    printf("Average turnaround time: %d ticks\n",
           total_turnaround / NCHILD);

    exit(0);
}
