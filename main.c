#include <stdio.h>
#include <math.h>
#include <time.h>
#include <pvm.h>
#define __HEADERS__
#include "loop.c"

int main(int argc,char **argv)
  {
   pvm_init();

   int L=8;							// numero di siti della catena di spin
   int pbc=0;							// condizioni al bordo (0: condizioni aperte, 1: condizioni periodiche)
   int type=1;						// tipo di campo magnetico: 0 uniforme, -1 antiferro, >0 random
   int nstep=10100;       // numero di esecuzioni del monte carlo
   int amax=32;
   int n=0;
   int Sz=0;             // magnetizzazione della catena (sempre negativa)
   int tw=100;               // tempo di waiting per autocorrelazione
   int Oss=5;
   int mype=pvm_mype();

   double Jz=0.2; //0.2						        // interazione spin-spin            // MODIFICATO
   double eps=0;  //5						        //intensità del campo magnetico     // MODIFICATO
   double beta=10;						      // temperatura inversa (parametro fissato)
   double tau=0.001;					    	// step tempo immaginario (errore 101 se uguale a beta) 0.001
   double alpha=0; //0.01					        // dissipazione sul sito centrale
   int N=4*(int)rint(0.5*beta/tau);
   int M=L*N;
   double *A=dvector(0,amax+M-1);
   double *Stmp=dvector(0,M-1);       // contiene tutti gli spin a fissato i, ovvero per j=1,2,...,N
   double *Corr=dvector(0,L-1);          // vettore che salva gli spin della catena al tempo t_w
  //  double *E=dvector(0,4);
   unsigned long seed=(int)time(NULL)+mype;
   
  // Variabili extra--------------------------------------------------------

    if (Oss==5) L=8; // se scelgo la correlazione, la lunghezza deve essere fissata
    for(int m=0;m<M;m++) A[amax+m]=0;  // salva config spin al tempo t=t_w

    int P = 4;
    switch (P) {
        case 1:
            Jz = 0;
            eps = 0;
            alpha = 0;
            break;
        case 2:
            Jz = 0.2;
            eps = 0;
            alpha = 0;
            break;
        case 3:
            Jz = 0;
            eps = 5;
            alpha = 0;
            break;
        case 4:
            Jz = 0.2;
            eps = 5;
            alpha = 0;
            break;
        case 5:
            Jz = 0.2;
            eps = 5;
            alpha = 0.01;
            break;
    }
  //---------------------------------------------------------------------------------


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
    
    // cpuint: Restituisce booleano che indica se è tempo di effettuare un'interruzione di CPU.
    // cputerm: Controlla se il tempo di CPU massimo è stato raggiunto o se è presente un file "STOP" nella directory del processo, in tal caso termina il job.

      if (cpuint() || cputerm())
        {
        //  printf("step: %i/%i, <H_hop>=%g, <H_int>=%g, <H_mag>=%g\n",n,nstep,A[3]/A[1],A[10]/A[1],A[12]/A[1]);
         if (cputerm()) break;
        }

      // Componenti del vettore (la "Media temporale" va divisa per A[1]):
      // [1]: numero di configurazioni
      // [2]: Media temporale dell'energia totale (H_tot)
      // [3]:  Media temporale numero di hopping diviso per beta (H_hop)
      // [10]: Media temporale dell'interazione spin-spin (H_int)
      // [12]: Media temporale dell'interazione con B (campo magnetico esterno) (H_mag)
      // [21->23]: Componenti Sx Sy Sz spin ultimo sito
      // [28]: Numero di bond del bagno termico allo step Monte Carlo corrente
      // [29]: Media temporale del numero di bond del bagno termico
      // [30]: Somma temporale del numero di cluster
      // [32->(32+M)]: configurazione degli spin


    // PRINT OPERATORE SINGOLO
    // if (Oss == 2 || Oss == 3 || Oss == 10 || Oss == 12)
    if (Oss == 2)
        printf("%15g%15g\n",A[0],A[Oss]/A[1]);

    // PRINT ENERGIE
    else if (Oss==4)
        printf("%15g%15g%15g%15g\n",A[0],A[3]/A[1],A[10]/A[1],A[12]/A[1]);

    // PRINT AUTOCORRELAZIONE SPIN-SPIN PER L SITI <Sij(tw)*Sij(t)>_tau
    else if (Oss == 5 && n>=tw-1)
    {

        for (int i=0;i<L;i++) Corr[i]=0;

        if(A[0]==(double)tw) for(int m=0;m<M;m++) Stmp[m]=A[amax+m];  // salva config spin al tempo t=t_w  
        if(A[0]>=(double)tw)
          for (int i=0;i<L;i++)
          {
              for (int j=0;j<N;j++)
                {
                int m=i+j*L;
                Corr[i]+=Stmp[m]*A[amax+m];        // somma tutti gli spin immaginari
                }    
              Corr[i]=Corr[i]/N;        // media su tempo immaginario      
          }

        // for(int j=0;j<N;j++) printf("%4g%4g%4g%4g%4g%4g%4g%4g\n",Stmp[0+j*L],Stmp[1+j*L],Stmp[2+j*L],Stmp[3+j*L],Stmp[4+j*L],Stmp[5+j*L],Stmp[6+j*L],Stmp[7+j*L]);
        // printf("\n");
        // for(int j=0;j<N;j++) printf("%4g%4g%4g%4g%4g%4g%4g%4g\n",A[amax+j*L],A[amax+1+j*L],A[amax+2+j*L],A[amax+3+j*L],A[amax+4+j*L],A[amax+5+j*L],A[amax+6+j*L],A[amax+7+j*L]);
        // printf("\n");
        // for(int j=0;j<N;j++) printf("%4g%4g%4g%4g%4g%4g%4g%4g\n",Stmp[j*L]*A[amax+j*L],Stmp[1+j*L]*A[amax+1+j*L],Stmp[2+j*L]*A[amax+2+j*L],Stmp[3+j*L]*A[amax+3+j*L],Stmp[4+j*L]*A[amax+4+j*L],Stmp[5+j*L]*A[amax+5+j*L],Stmp[6+j*L]*A[amax+6+j*L],Stmp[7+j*L]*A[amax+7+j*L]);
        // printf("\n");

        printf("%15g%20.10g%20.10g%20.10g%20.10g%20.10g%20.10g%20.10g%20.10g\n",
              A[0],Corr[0],Corr[1],Corr[2],Corr[3],Corr[4],Corr[5],Corr[6],Corr[7]);

    }


    // PRINT COMPONENTI SPIN
    else if (Oss==6)
        printf("%15g%15g%15g%15g\n",A[0],A[Oss+15],A[Oss+16],A[Oss+17]);


      n++;

     }   

  //  printf("numero di step: %g\n",A[0]);
  //  printf("<H_hop>:        %g\n",A[3]/A[1]);
  //  printf("<H_int>:        %g\n",A[10]/A[1]);
  //  printf("<H_mag>:        %g\n",A[12]/A[1]);
 

   pvm_end();
   return 0;
  }
