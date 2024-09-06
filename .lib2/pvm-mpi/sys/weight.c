#ifndef __WEIGHT_H__
#define __WEIGHT_H__

class weight
  {
   public:
   weight(int N);
   void push(int i,double wtmp);
   int pop(void);
   void restart(void);

   int M;
   int K;
   double *W;
   double *B;
   int mmax;
};

#ifndef __HEADERS__
#include <stdio.h>
#include <stdlib.h>
#include <pvm.h>

weight::weight(int N)
  {
   M=1;
   while (M<N) M*=2;
   K=M/2;
   W=(double*)calloc(M,sizeof(double));
   B=(double*)calloc(K,sizeof(double));
   mmax=M;
  }

// N.B: se il bit più significativo di x è l'n-mo, allora B[x] è la somma dei pesi che hanno i primi n bit uguali
// B[0]					somma di tutti i pesi
// B[1]			   1		somma dei dispari			(i pari saranno B[0]-B[1])
// B[2]			  10		somma di quelli uguali a 2 modulo 4	(quelli uguali a 0 modulo 4 saranno B[0]-B[1]-B[2])
// B[3]			  11		somma di quelli uguali a 3 modulo 4	(quelli uguali a 1 modulo 4 saranno B[1]-B[3])
// B[4]			 100		somma di quelli uguali a 4 modulo 8	(quelli uguali a 0 modulo 8 saranno B[0]-B[1]-B[2]-B[4])
// B[5]			 101		somma di quelli uguali a 5 modulo 8	...
// B[6]			 110		somma di quelli uguali a 6 modulo 8	...
// B[7]			 111		somma di quelli uguali a 7 modulo 8	...
// B[8]			1000		somma di quelli uguali a 8 modulo 16	...
// B[9]			1001		somma di quelli uguali a 9 modulo 16	...
// etc...

void weight::restart(void)
  {
   for (int l=0;l<K;l++) B[l]=0;
   for (int j=0;j<M;j++)
     {
      double wtmp=W[j];
      B[0]+=wtmp;
      int b=0;
      for (int m=1;m<K;m<<=1)
        {
         b+=m;
         if (j&m) B[j&b]+=wtmp;
        }
     }
  }

void weight::push(int i,double wtmp)
  {
   if (wtmp<0)
     {
      fprintf(stderr,"weight: peso negativo...\n");
      exit(1);
     }
   double wdel=wtmp-W[i];
   W[i]=wtmp;
   B[0]+=wdel;
   int b=0;
   for (int m=1;m<K;m<<=1)
     {
      b+=m;
      if (i&m) B[i&b]+=wdel;
     }
  }

int weight::pop(void)
  {
   double xtmp=Xrandom();
   for (int k=0;k<2;k++)
     {
      double r=B[0]*xtmp;
      int i=0;
      for (int m=1;m<K;m<<=1) if (r<B[i+m]) i+=m; else r-=B[i+m];
      if (r<W[i+K]) i+=K;
      if (W[i]>0) return i;
      restart();
     }
   for (int j=0;j<M;j++) fprintf(stderr,"W[%i]=%g\n",j,W[j]);
   fprintf(stderr,"weight_pop...\n");
   exit(1);
  }
#endif
#endif
