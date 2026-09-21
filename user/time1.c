#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid;
  int start;
  int end;
  int elapsed;

  // A command to time must be provided.
  if(argc < 2){
    printf("usage: time1 command [args...]\n");
    exit(1);
  }

  // Record the time before creating the child.
  start = uptime();

  pid = fork();

  if(pid < 0){
    printf("time1: fork failed\n");
    exit(1);
  }

  if(pid == 0){
    // Child process:
    // argv[1] is the command, and &argv[1] gives the command
    // and all of its arguments to exec().
    exec(argv[1], &argv[1]);

    // exec() only returns if it failed.
    printf("time1: exec %s failed\n", argv[1]);
    exit(1);
  }

  // Parent process waits for the command to finish.
  wait(0);

  // Record the time after the child has terminated.
  end = uptime();

  elapsed = end - start;

  printf("elapsed time: %d ticks\n", elapsed);

  exit(0);
}
