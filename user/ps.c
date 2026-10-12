#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/pstat.h"
#include "user/user.h"

//Prints spaces until the specified column is filled.
void
pad(int length, int width){
    for (int j = length; j < width; j++)
        printf(" ");
}

int
main(int argc, char **argv)
{
  struct pstat uproc[NPROC];
  int nprocs;
  int i;
  char *state;
  uint current_time;
  uint age;
  static char *states[] = {
    [SLEEPING]  "sleeping",
    [RUNNABLE]  "runnable",
    [RUNNING]   "running ",
    [ZOMBIE]    "zombie  "
  };

  nprocs = getprocs(uproc);
  if (nprocs < 0)
    exit(-1);

  current_time = uptime();
  printf("PID  STATE       SIZE       PPID   PRIORITY   AGE       NAME\n");
  for (i = 0; i < nprocs; i++) {
    state = states[uproc[i].state];

    printf("%d", uproc[i].pid);
    pad(1, 6);

    printf("%s", state);
    pad(strlen(state), 12);

    printf("%ld", uproc[i].size);
    pad(5, 11);

    printf("%d", uproc[i].ppid);
    pad(1, 7);

    printf("%d", uproc[i].priority);
    pad(1, 11);

    //Calculate age only for RUNNABLE processes (TASK 3)
    if (uproc[i].state == RUNNABLE) {
        age = current_time - uproc[i].readytime;
        printf("%d", age);
        pad(1, 10);

    } else {
        printf("-");
        pad(1, 10);

    }

    printf("%s\n", uproc[i].name);
  }
  exit(0);
}

