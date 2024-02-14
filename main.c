#include <stdio.h>
#include <math.h>
#include <pvm.h>
#define __HEADERS__
#include "loop.c"

int main(int argc,char **argv)
  {
   pvm_init();

   int L=11;							// numero di siti
   int pbc=0;							// condizioni al bordo (0: condizioni aperte, 1: condizioni periodiche)
   int type=1;						// tipo di campo magnetico: 0 uniforme, -1 antiferro, >0 random
   int nstep;      // numero di volte di esecuzione del monte carlo
   int amax=32;
   int n=0;                           // indice del ciclo while

   double Jz=1;						          // interazione
   double eps=1;						        // campo magnetico random
   double beta=10;						      // temperatura inversa
   double tau=0.001;					    	// step tempo immaginario
   double alpha=1;					        // dissipazione sul sito centrale
   double *A=dvector(0,amax-1);

   L=getarg_i("L",L);
   pbc=getarg_i("pbc",pbc);
   Jz=getarg_d("Jz",Jz);
   eps=getarg_d("eps",eps);
   type=getarg_i("type",type);
   beta=getarg_d("beta",beta);
   tau=getarg_d("tau",tau);
   alpha=getarg_d("alpha",alpha);
   nstep=getarg_i("nstep",1000);

   loop *X=new loop();

  printf("numero di step\t\t<H_hop>\t\t<H_int>\t\t<H_mag>\n");

   while (n<nstep)
     {
      X->dinamica(A);
      n++;
      if (cpuint() || cputerm())
        {
         printf("step: %i/%i, <H_hop>=%g, <H_int>=%g, <H_mag>=%g\n",n,nstep,A[3]/A[1],A[10]/A[1],A[12]/A[1]);
         if (cputerm()) break;
        }
    
    printf("%6g\t\t%15g%15g%15g\n",A[0],A[3]/A[1],A[10]/A[1],A[12]/A[1]);

     }

  //  printf("numero di step: %g\n",A[0]);
  //  printf("<H_hop>:        %g\n",A[3]/A[1]);
  //  printf("<H_int>:        %g\n",A[10]/A[1]);
  //  printf("<H_mag>:        %g\n",A[12]/A[1]);
 

   pvm_end();
   return 0;
  }
