#ifndef __CYCLO_H__
#define __CYCLO_H__
#include <stdlib.h>

class cyclo
  {
   public:
   cyclo(long cmax,long tmax,int ppdec);
   void pop(void);                       // trova cnext e next successivi
   int push(void);             // se restituisce un intero >=0, fai partire una nuova serie

   long cmax;
   long tmax;
   long *delta;
   int imax;
   int nmax;		// numero massimo serie
   int ntmp;		// numero successivo serie
   long *W;
   int *z;
   long cnext;		// prossimo step di correlazione
   int next;		// serie corrispondente a cnext
   int inx;		// indice della correlazione
   long count;
  };

#ifndef __HEADERS__
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pvm.h>

#define COUNT_MAX    4611686018427387904L

cyclo::cyclo(long _cmax,long _tmax,int ppdec)
  {
   // parametri
   cmax=_cmax;							// ogni quanti step fa partire una serie
   tmax=_tmax;							// massimo tempo di decorrelazione
   // crea vettore delta
   if (ppdec>0) 		// scala logaritmica
     {
      delta=(long*)getmem(NULL,3*sizeof(long));
      delta[0]=0;
      double ftmp=1;
      delta[1]=(long)rint(ftmp);
      imax=1;
      while (delta[imax]<tmax)
        {
         long dtmp=delta[imax];
         while (dtmp==delta[imax])
           {
            ftmp*=pow(10.,1./ppdec);
            dtmp=(long)rint(ftmp);
           }
         imax++;
         delta=(long*)getmem(delta,(2+imax)*sizeof(long));
         delta[imax]=dtmp;
        }
      delta[imax+1]=COUNT_MAX;
     }
   else				// scala lineare
     {
      delta=(long*)getmem(NULL,2*sizeof(long));
      delta[0]=0;
      delta[1]=-ppdec;
      imax=1;
      while (delta[imax]<tmax)
        {
         imax++;
         delta=(long*)getmem(delta,(2+imax)*sizeof(long));
         delta[imax]=delta[imax-1]-ppdec;
        }
      delta[imax+1]=COUNT_MAX;
     }
   nmax=tmax/cmax;
   W=(long*)getmem(NULL,nmax*sizeof(long));
   z=(int*)getmem(NULL,nmax*sizeof(int));
   for (int n=0;n<nmax;n++)
     {
      W[n]=0;
      z[n]=imax;
     }
   cnext=COUNT_MAX;
   next=-1;
   count=0;
   ntmp=0;
  }

// trova cnext e next successivi, chiamare dopo ogni correlazione
void cyclo::pop(void)
  {
   if (next>=0) z[next]++;
   cnext=COUNT_MAX;
   for (int n=0;n<nmax;n++) if (delta[z[n]]<=tmax)
     {
      long ctmp=W[n]+delta[z[n]];
      if (ctmp<cnext)
        {
         cnext=ctmp;
         next=n;
        }
     }
   if (next>=0) inx=z[next];
  }

// chiamare a ogni step (dopo il controllo di cnext)
// fa partire una nuova serie se sono passati cmax step dall'ultima serie
int cyclo::push(void)
  {
   int n=-1;
   if (count%cmax==0)
     {
      n=ntmp%nmax;
      if (ntmp>=nmax && z[n]<imax)
        {
         fprintf(stderr,"count=%li conflitto... n=%i W[%i]=%li z[%i]=%i/%i delta[%i]=%li\n",count,n,n,W[n],n,z[n],imax,z[n],delta[z[n]]);
         exit(1);
        }
      W[n]=count;
      z[n]=1;
      next=-1;		// in questo modo pop() non aumenta nessun indicatore z[n], come deve fare invece dopo la correlazione
      pop();
      ntmp++;
     }
   count++;
   return n;
  }
#endif
#endif
