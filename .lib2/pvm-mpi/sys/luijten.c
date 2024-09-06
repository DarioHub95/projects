#ifndef __LUIJTEN_H__
#define __LUIJTEN_H__

void luijten1(int N,double *P);
int luijten2(int N,int *stack);

#ifndef __HEADERS__
#include <stdio.h>
#include <math.h>
#include <pvm.h>

static int size;
static double *C;

void luijten1(int N,double *P)
  {
   if (size!=N)
     {
      size=N;
      C=(double*)getmem(C,(1+size)*sizeof(double));
     }
   C[0]=0;
   double prod=1;
   for (int i=0;i<N;i++)
     {
      if (P[i]<0 || P[i]>=1) debug(9,"luijten: probabilità fuori dai margini... N.B: il caso P=1 è possibile ma non ancora implementato\n");
      prod*=1-P[i];
      C[1+i]=1-prod;			// N.B: il vettore C va da C[0]=0 a C[N]=1-\prod_{i=0}^{N-1}P[i]
     }
  }

int luijten2(int N,int *stack)
  {
   if (N<size)
     {
      fprintf(stderr,"luijten2: dimensione di stack insufficiente...\n");
      exit(1);
     }
   int n=0;		// numero siti estratti
   int i=0;		// ultimo sito estratto più uno (i=0 significa che non è stato estratto nessun sito)... è anche il primo sito che possiamo estrarre
   while (i<size)
     {
      double r=C[i]+Xrandom()*(1-C[i]);
      if (r>C[size]) break;			// i siti estratti sono finiti
      // troviamo j tale che C[j]<r<=C[j+1]
      int j1=i;					// minimo valore di j
      int j2=size-1;				// massimo valore di j
      while (j2>j1)
        {
         int jm=(j1+j2)/2;
         if (r>C[jm+1]) {j1=jm+1;} else if (r<=C[jm]) j2=jm; else j2=j1=jm;
        }
      // aggiungiamo j in stack
      stack[n]=j2;
      n++;
      i=j2+1;
     }
   return n;
  }
#endif
#endif
