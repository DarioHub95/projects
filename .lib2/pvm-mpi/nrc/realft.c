/**************************************************************************
                     REAL FOURIER TRANSFORM
                     ----------------------

  void realft(double *x,int n)
  -------------------------------  
  calcola la trasformata di Fourier veloce del vettore reale x
  x[0..n-1] contiene n numeri reali  x_0,...,x_(n-1)
  dove n deve essere una potenza di 2
  in uscita:
  x[0] contiene la componente (sempre reale) di frequenza zero
  x[1] contiene la componente (sempre reale) di frequenza critica f_c
  x[2]+i*x[3] e' la prima componente di frequenza positiva
  x[n-2]+i*x[n-1] e' l'ultima componente di frequenza positiva
    (subito sotto la frequenza critica f_c)
  le frequenze negative sono complesse coniugate di quelle positive
  la frequenza critica e' data da 1/2\Delta, dove \Delta e' l'intervallo
  temporale di campionamento
  
****************************************************************************/
#ifndef __REALFT_H__
#define __REALFT_H__

void realft(double *x,int n);

#ifndef __HEADERS__
#include <math.h>
#include <pvm.h>

void realft(double *x,int n)
  {
   int i,i1,i2,i3,i4,np1;
   double h1r,h1i,h2r,h2i,wr,wi,wpr,wpi,wtmp,theta;

   fft(x,n>>1);
   theta=M_PI/(double)(n>>1);
   wtmp=sin(0.5*theta);
   wpr=-2.0*wtmp*wtmp;
   wpi=sin(theta);
   wr=1.0+wpr;
   wi=wpi;
   np1=n+1;
   for (i=1;i<(n>>2);i++)
     {
      i4=1+(i3=np1-(i2=1+(i1=i<<1)));
      h1r=0.5*(x[i1]+x[i3]);
      h1i=0.5*(x[i2]-x[i4]);   
      h2r=0.5*(x[i2]+x[i4]);
      h2i=-0.5*(x[i1]-x[i3]);
      x[i1]=h1r+wr*h2r-wi*h2i;
      x[i2]=h1i+wr*h2i+wi*h2r;
      x[i3]=h1r-wr*h2r+wi*h2i;
      x[i4]=-h1i+wr*h2i+wi*h2r;
      wr=(wtmp=wr)*wpr-wi*wpi+wr;
      wi=wi*wpr+wtmp*wpi+wi;
     }
   x[0]=(h1r=x[0])+x[1];
   x[1]=h1r-x[1];
  }
#endif
#endif
