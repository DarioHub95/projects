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
   int type=1;						// tipo di campo magnetico: 0 uniforme, -1 antiferro, >0 random
   int nstep;             // numero di volte di esecuzione del monte carlo
   int amax=32;
   int n=0, i=0;                         // indice del ciclo while
  // int Sz=-1;                    // magnetizzazione (sempre negativa)

   double Jz=1;						          // interazione
   double eps=1;						        // campo magnetico random
   double beta=10;						      // temperatura inversa
   double tau=0.001;					    	// step tempo immaginario (errore 101 se uguale a beta)
   double alpha=0;					        // dissipazione sul sito centrale
   double *A=dvector(0,amax-1);
   double *B=dvector(0,amax-1);

   double T0 = walltime();          // Registra il tempo di inizio della simulazione
   double DT = 0;                   // Intervallo di tempo tra 2 acquisizioni dei dati


  // getarg_*: legge valore (intero, double) associato a un determinato nome di variabile. 
  // Se l'argomento è presente, restituisce il suo valore intero; altrimenti, restituisce un valore di default.
   L=getarg_i("L",L);
   pbc=getarg_i("pbc",pbc);
   Jz=getarg_d("Jz",Jz);
   eps=getarg_d("eps",eps);
   type=getarg_i("type",type);
   beta=getarg_d("beta",beta);
   tau=getarg_d("tau",tau);
   alpha=getarg_d("alpha",alpha);
   nstep=getarg_i("nstep",50);
  // Sz=getarg_i("Sz",Sz);


   loop *X=new loop();

  printf("step_simulazione\t\t<H_hop>\t\t<H_int>\t\t<H_mag>\t\tTempo_MC(s)\n");

   while (n<nstep)
     {
      X->dinamica(A);
    
    // cpuint: Restituisce booleano che indica se è tempo di effettuare un'interruzione di CPU.
    // cputerm: Controlla se il tempo di CPU massimo è stato raggiunto o se è presente un file "STOP" nella directory del processo, in tal caso termina il job.

      if (cpuint() || cputerm())
        {
         printf("step: %i/%i, <H_hop>=%g, <H_int>=%g, <H_mag>=%g\n",n,nstep,A[3]/A[1],A[10]/A[1],A[12]/A[1]);
         if (cputerm()) break;
        }
      
      double Tn = walltime();          // Registra il tempo al passo n del monte carlo
      DT = Tn-T0;          
      n++;
    
    printf("%6g\t\t%15g%15g%15g%15g\n",A[0],A[3]/A[1],A[10]/A[1],A[12]/A[1],DT);

     }

  //  printf("numero di step: %g\n",A[0]);
  //  printf("<H_hop>:        %g\n",A[3]/A[1]);
  //  printf("<H_int>:        %g\n",A[10]/A[1]);
  //  printf("<H_mag>:        %g\n",A[12]/A[1]);
 

   pvm_end();
   return 0;
  }
