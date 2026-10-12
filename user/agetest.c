#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
    printf("Starting AGE test...\n");

    for (int i = 0; i < 3; i++) {
        int pid = fork();

        if (pid == 0) {
            printf("Child %d started\n", getpid());

            // Keep the child busy.
            volatile unsigned long counter = 0;

            while (1) {
                counter++;
            }

            exit(0);
        }
    }

    printf("Three CPU-intensive children created.\n");
    printf("Run 'ps' to check their AGE values.\n");

    exit(0);
}
