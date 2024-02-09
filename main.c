#include <stdio.h>
#include <math.h>
#include <pvm.h>
#define __HEADERS__
#include "loop.c"

int main(int argc,char **argv)
  {
   pvm_init();

   int L=10;							// numero di siti
   int pbc=0;							// condizioni al bordo (0: condizioni aperte, 1: condizioni periodiche)
   double Jz=0.5;						// interazione
   double eps=1;						// campo magnetico random
   double beta=10;						// temperatura inversa
   double tau=0.001;						// step tempo immaginario
   int nstep=10000;

   double *h=dvector(0,L-1);
   for (int i=0;i<L;i++) h[i]=eps*(2*Xrandom()-1);		// estrai campo magnetico random
   //for (int i=0;i<L;i++) h[i]=eps*(i%2?1:-1);

   loop *X=new loop(L,pbc,Jz,h,beta,tau);

   int amax=32;
   double *A=dvector(0,amax-1);
   for (int i=0;i<nstep;i++)
     {
      X->dinamica(A);
      if (cpuint() || cputerm())
        {
         printf("step: %i/%i\n",1+i,nstep);
         if (cputerm()) break;
        }
     }

   printf("numero di step: %g\n",A[0]);
   printf("<H_hop>:        %g\n",A[3]/A[1]);
   printf("<H_int>:        %g\n",A[10]/A[1]);
   printf("<H_mag>:        %g\n",A[12]/A[1]);  

   pvm_end();
   return 0;
  }
