#ifndef __OOM_H__
#define __OOM_H__

void oomscore(int score);

#ifndef __HEADERS__
#include <stdio.h>
#include <unistd.h>
#include <pvm.h>

void oomscore(int score)
  {
   pid_t pid=getpid();
   FILE *stream=fopenf("/proc/%i/oom_score_adj","w",pid);
   fprintf(stream,"%i\n",score);
   fclose(stream);
  }
#endif
#endif
