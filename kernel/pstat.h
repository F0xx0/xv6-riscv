#ifndef _PSTAT_H_
#define _PSTAT_H_

enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE }; //So that it works (PA2)

struct rusage {
  uint cputime;
};

struct pstat {
  int pid;              // Process ID
  enum procstate state; // Process state
  uint64 size;          // Size of process memory (bytes)
  int ppid;             // Parent process ID
  char name[16];        // Parent command name
  int priority;         // Process priority (PA2)
  uint readytime;       // Time process entered RUNNABLE (PA2)
};

#endif
