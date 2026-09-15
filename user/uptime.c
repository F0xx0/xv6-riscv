#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  uint ticks;

  ticks = uptime();

  printf("up %d clock ticks\n", ticks);

  exit(0);
}
