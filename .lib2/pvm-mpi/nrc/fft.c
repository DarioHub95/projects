/**************************************************************************
                     FAST FOURIER TRANSFORM
                     ----------------------

  void fft(double *x,int nn)
  -----------------------------   
  calcola la trasformata di Fourier veloce del vettore complesso x
  x[0..2*nn-1] contiene nn numeri complessi  h_0,...,h_(nn-1)
  dove nn deve essere una potenza di 2
  x[0]+i*x[1] = h_0 , ... , x[2*nn-2]+i*x[2*nn-1] = h_(nn-1)
  in uscita, x contiene nn numeri complessi H_0,...,H_(nn-1)
  impacchettati esattamente come h_0,...,h_(nn-1)
  H_0 rappresenta la componente di frequenza zero
  H_1,...,H_(nn/2-1) le frequenze positive < f_c = frequenza critica
  H_(nn/2) le componenti di frequenza f_c e -f_c (coincidono)
  H_(nn/2+1),...,H_(nn-1) le frequenze negative da quella subito sopra -f_c
    a quella subito sotto lo zero

****************************************************************************/
#ifndef __FFT_H__
#define __FFT_H__

void fft(double *x,int nn);

#ifndef __HEADERS__
#include <math.h>
 
void fft(double *x,int nn)
  {
   int n,i,j,m,istep,mmax;
   double xtmp,theta,wtmp,wr,wi,wpr,wpi,tmpr,tmpi;

   n=nn<<1;
   j=0;				/* bit-reversal section of the routine */
   for (i=0;i<n-1;i+=2)
     {
      if (j>i)
        {
         xtmp=x[j];
         x[j]=x[i];
         x[i]=xtmp;
         xtmp=x[j+1];
         x[j+1]=x[i+1];
         x[i+1]=xtmp;
        }
      m=n>>1;
      while (m>=2 && j>=m)
        {
         j-=m;
         m>>=1;
        }
      j+=m;
     }
   mmax=2;                   /* Danielson-Lanczos section of the routine */
   while (n>mmax)
     {
      istep=mmax<<1;
      theta=2*M_PI/mmax;
      wtmp=sin(0.5*theta);
      wpr=-2.0*wtmp*wtmp;
      wpi=sin(theta);
      wr=1.0;
      wi=0.0;
      for (m=0;m<mmax-1;m+=2)
        {
         for (i=m;i<n;i+=istep)
           {
            j=i+mmax;
            tmpr=wr*x[j]-wi*x[j+1];
            tmpi=wr*x[j+1]+wi*x[j];
            x[j]=x[i]-tmpr;
            x[j+1]=x[i+1]-tmpi;
            x[i]+=tmpr;
            x[i+1]+=tmpi;
           }
         wr=(wtmp=wr)*wpr-wi*wpi+wr;
         wi=wi*wpr+wtmp*wpi+wi;
        }
      mmax=istep;
     }
  }
#endif
#endif
