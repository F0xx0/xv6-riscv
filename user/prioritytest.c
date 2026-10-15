#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int pid;
    int priority;
    int result;

    printf("Priority Test:\n");

   //Forking testing
    pid = fork();
    if (pid < 0) {
        printf("ERROR: fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        // Child process
        int mypid = getpid();

        printf("\nChild PID: %d\n", mypid);

        // Check initial priority
        priority = getpriority(mypid);
        printf("Initial priority: %d\n", priority);

        // Set priority to 20
        result = setpriority(mypid, 20);
        printf("setpriority(%d, 20) returned: %d\n", mypid, result);

        // Verify priority changed
        priority = getpriority(mypid);
        printf("New priority: %d\n", priority);
        printf("\nChild sleeping for 10 seconds...\n");
        printf("Run 'ps' now to check priority\n");

        // Keep the child alive temporarily for testing
        // Keep the child alive for approximately 10 seconds
        pause(100);
        printf("\nChild finished.\n");
        exit(0);
    }

    // Parent waits for child
    wait(0);
    printf("\nPriority Test Complete!\n");
    exit(0);
}
