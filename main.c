#include <stdio.h>
#include <math.h>
#include <string.h>
#include <pvm.h>
#define __HEADERS__
#include "loop.c"

int main(int argc,char **argv)
  {
   pvm_init();

   int L=6;							// numero di siti della catena di spin
   int pbc=0;							// condizioni al bordo (0: condizioni aperte, 1: condizioni periodiche)
   int type=1;						// tipo di campo magnetico: 0 uniforme, -1 antiferro, >0 random
   int nstep=100;       // numero di esecuzioni del monte carlo
   int amax=32;
   int n=0;
   int Sz=0;             // magnetizzazione della catena (sempre negativa)
   int tw=1;               // tempo di waiting per autocorrelazione
   int Oss=0;

   double Jz=0.2; //0.2						        // interazione spin-spin            // MODIFICATO
   double eps=0;  //5						        //intensità del campo magnetico     // MODIFICATO
   double beta=10;						      // temperatura inversa (parametro fissato)
   double tau=0.001;					    	// step tempo immaginario (errore 101 se uguale a beta)
   double alpha=0.01; //0.01					        // dissipazione sul sito centrale
   double *A=dvector(0,amax-1);
  //  double *E=dvector(0,4);

  // Variabili di input all'eseguibile ./a.out
   if (atoi(argv[1])==5){ L=8; tw=atoi(argv[3]); }
   nstep=atoi(argv[2]);
   Oss=atoi(argv[1]);

  // getarg_*: legge valore (intero, double) associato a un determinato nome di variabile. 
  // Se l'argomento è presente restituisce il suo valore impostato, altrimenti un valore di default.
   L=getarg_i("L",L);
   nstep=getarg_i("nstep",nstep);
   tw=getarg_i("tw",tw);
   Jz=getarg_d("Jz",Jz);
   eps=getarg_d("eps",eps);
   alpha=getarg_d("alpha",alpha);
   Sz=getarg_i("Sz",Sz);
   pbc=getarg_i("pbc",pbc);
   type=getarg_i("type",type);
   beta=getarg_d("beta",beta);
   tau=getarg_d("tau",tau);

   loop *X=new loop();

   while (n<nstep)
     {
      X->dinamica(A);
      // Y->dinamica(B);
    
    // cpuint: Restituisce booleano che indica se è tempo di effettuare un'interruzione di CPU.
    // cputerm: Controlla se il tempo di CPU massimo è stato raggiunto o se è presente un file "STOP" nella directory del processo, in tal caso termina il job.

      if (cpuint() || cputerm())
        {
         printf("step: %i/%i, <H_hop>=%g, <H_int>=%g, <H_mag>=%g\n",n,nstep,A[3]/A[1],A[10]/A[1],A[12]/A[1]);
         if (cputerm()) break;
        }

      // Componenti del vettore (la "Media temporale" va divisa per A[1]):
      // [2]: Media temporale dell'energia totale (H_tot)
      // [3]:  Media temporale numero di hopping diviso per beta (H_hop)
      // [10]: Media temporale dell'interazione spin-spin (H_int)
      // [12]: Media temporale dell'interazione con B (campo magnetico esterno) (H_mag)
      // [13-20]: Correlazione per 8 siti
      // [21-23]: Componenti Sx Sy Sz spin ultimo sito
      // [28]: Numero di bond del bagno termico allo step Monte Carlo corrente
      // [29]: Media temporale del numero di bond del bagno termico
      // [30]: Somma temporale del numero di cluster


    // PRINT OPERATORE SINGOLO
    // if (Oss == 2 || Oss == 3 || Oss == 10 || Oss == 12)
    if (Oss == 2)
        printf("%15g\n",A[Oss]/A[1]);

    // PRINT ENERGIE
    else if (Oss==4)
        printf("%15g%15g%15g\n",A[3]/A[1],A[10]/A[1],A[12]/A[1]);

    // PRINT AUTOCORRELAZIONE PER L SITI
    else if ( Oss == 5 && n>=tw-1 )
        printf("%15g%20.10f%20.10f%20.10f%20.10f%20.10f%20.10f%20.10f%20.10f\n",
              A[0],A[Oss+8],A[Oss+9],A[Oss+10],A[Oss+11],A[Oss+12],A[Oss+13],A[Oss+14],A[Oss+15]);

    // PRINT COMPONENTI SPIN
    else if (Oss==6)
        printf("%15g%15g%15g\n",A[Oss+15]/A[1],A[Oss+16]/A[1],A[Oss+17]/A[1]);


      n++;

     }   

  //  printf("numero di step: %g\n",A[0]);
  //  printf("<H_hop>:        %g\n",A[3]/A[1]);
  //  printf("<H_int>:        %g\n",A[10]/A[1]);
  //  printf("<H_mag>:        %g\n",A[12]/A[1]);
 

   pvm_end();
   return 0;
  }