#include <stdio.h>
#include <math.h>
#include <pvm.h>
#define __HEADERS__
#include "loop.c"

int main(int argc,char **argv)
  {
   pvm_init();

   int L=5;							  // numero di siti della catena di spin
   int pbc=0;							// condizioni al bordo (0: condizioni aperte, 1: condizioni periodiche)
   int type=1;						// tipo di campo magnetico: 0 uniforme, -1 antiferro, >0 random
   int nstep=10000;         // numero di esecuzioni del monte carlo
   int amax=32;
   int n=0;
   int Sz=-1;                    // magnetizzazione della catena (sempre negativa)
   int i=0, Oss=0;      // coordinate spaziotemporali dello spin per autocorrelazione

   double Jz=1;						          // interazione spin-spin
   double eps=1;						        //intensità del campo magnetico
   double beta=10;						      // temperatura inversa (parametro fissato)
   double tau=0.001;					    	// step tempo immaginario (errore 101 se uguale a beta)
   double alpha=0;					        // dissipazione sul sito centrale
   double *A=dvector(0,amax-1);
   double *B=dvector(0,amax-1);
  //  double *C=dvector(0,amax-1);
  //  double *E=dvector(0,4);


  // getarg_*: legge valore (intero, double) associato a un determinato nome di variabile. 
  // Se l'argomento è presente restituisce il suo valore impostato, altrimenti un valore di default.
   L=getarg_i("L",L);
   eps=getarg_d("eps",eps);
   Jz=getarg_d("Jz",Jz);
   pbc=getarg_i("pbc",pbc);
   type=getarg_i("type",type);
   beta=getarg_d("beta",beta);
   tau=getarg_d("tau",tau);
   alpha=getarg_d("alpha",alpha);
   nstep=getarg_i("nstep",nstep);
   Sz=getarg_i("Sz",Sz);


   loop *X=new loop();
   loop *Y=new loop();

  // printf("n_simulazione\tC_i\t<H_tot>\t\t<H_hop>\n");

   while (n<nstep)
     {
      X->dinamica(A);
      Y->dinamica(B);
    
    // cpuint: Restituisce booleano che indica se è tempo di effettuare un'interruzione di CPU.
    // cputerm: Controlla se il tempo di CPU massimo è stato raggiunto o se è presente un file "STOP" nella directory del processo, in tal caso termina il job.

      if (cpuint() || cputerm())
        {
         printf("step: %i/%i, <H_hop>=%g, <H_int>=%g, <H_mag>=%g\n",n,nstep,A[3]/A[1],A[10]/A[1],A[12]/A[1]);
         if (cputerm()) break;
        }

      Oss=2;
      // Componenti del vettore:
      // [2]: Media temporale dell'energia interna (H_tot)
      // [3]: Numero di hopping diviso per beta (H_hop)
      // [10]: Media temporale dell'interazione spin-spin (H_int)
      // [12]: Media temporale dell'interazione con B (campo magnetico esterno) (H_mag)
      // [28]: Numero di bond del bagno termico allo step Monte Carlo corrente
      // [29]: Media temporale del numero di bond del bagno termico
      // [30]: Somma temporale del numero di configurazioni

      n++;          
    
      printf("%10g%15g%15g\n",A[0],A[Oss]/A[1],B[Oss]/B[1]);//,A[3]/A[1]);



     }


  //  printf("numero di step: %g\n",A[0]);
  //  printf("<H_hop>:        %g\n",A[3]/A[1]);
  //  printf("<H_int>:        %g\n",A[10]/A[1]);
  //  printf("<H_mag>:        %g\n",A[12]/A[1]);
 

   pvm_end();
   return 0;
  }
