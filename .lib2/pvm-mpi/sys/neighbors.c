#ifndef __NEIGHBORS_H__
#define __NEIGHBORS_H__

#define BC_PERIODIC	0
#define BC_OPEN		1

int **neighbors(int d,int L,int bc=BC_PERIODIC);

#ifndef __HEADERS__
#include <stdio.h>
#include <stdlib.h>
#include <pvm.h>

int **neighbors(int d,int L,int bc)
  {
   int N=1;
   for (int i=0;i<d;i++) N*=L;
   int h=2*d;
   int **next=imatrix(0,h-1,0,N-1);
   for (int j=0;j<N;j++)
     {
      int m=1;
      for (int i=0;i<d;i++)
        {
         int k=(j/m)%L;
         if (bc==BC_PERIODIC)
           {
            next[i][j]=j+m*(k==0?L-1:-1);
            next[d+i][j]=j+m*(k==L-1?1-L:1);
           }
         else if (bc==BC_OPEN)
           {
            if (k==0) next[i][j]=-1; else next[i][j]=j-m;
            if (k==L-1) next[d+i][j]=-1; else next[d+i][j]=j+m;
           }
         else
           {
            fprintf(stderr,"neighbors: boundary conditions\n");
            exit(1);
           }
         m*=L;
        }
     }
   return next;
  }
#endif
#endif
