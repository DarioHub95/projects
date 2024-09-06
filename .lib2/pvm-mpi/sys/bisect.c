#ifndef __BISECT_H__
#define __BISECT_H__

int select(int mmax,double *x);
int bisect(int mmax,double *x,double xtmp);

#ifndef __HEADERS__
#include <pvm.h>

// dato un vettore x[0]...x[mmax], dove x[0]=0 e x[i]<x[i+1], e un numero 0<xtmp<x[mmax]
// resituisce un numero 0<=m<=mmax-1 tale che x[m]<xtmp<x[m+1]

int bisect(int mmax,double *x,double xtmp)
  {
   int m1=0;
   int m2=mmax-1;
   while (m2>m1)
     {
      int mm=(m2+m1)/2;			// N.B: se m2=m1+1 allora mm=m1
      if (xtmp>x[mm+1]) {m1=mm+1;} else if (xtmp<=x[mm]) m2=mm; else m2=m1=mm;
     }
   if (m1!=m2 || m1<0 || m1>=mmax) debug(9,"errore 101... m1=%i m2=%i mmax=%i\n",m1,m2,mmax);
   if (xtmp<x[m1] || xtmp>x[m1+1]) debug(9,"errore 102... %.16f %.16f %.16f\n",x[m1],xtmp,x[m1+1]);
   return m1;
  }

// dato un vettore x[0]...x[mmax-1], con x[m]>=0, resituisce un numero m=0...mmax-1 con prob. proporzionale a x[m]

int select(int mmax,double *x)
  {
   double xmax=0;
   for (int m=0;m<mmax;m++)
     {
      if (x[m]<0) debug(9,"errore in select: probabilità negativa...\n");
      xmax+=x[m];
     }
   double xtmp=xmax*Xrandom();
   int m=0;
   while (m<mmax-1 && xtmp>x[m])
     {
      xtmp-=x[m];
      m++;
     }
   return m;
  }
#endif
#endif
