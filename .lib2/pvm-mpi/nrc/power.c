/*********************************************************************
N.B.: dividere lo spettro per 2\sigma^2(x), e moltiplicarlo per
      l'intervallo temporale \Delta
      in maniera che il limite \omega\to 0 dia esattamente
      il tempo integrale di autocorrelazione (nelle stesse unita' di \Delta)

      nel caso di decadimento esponenziale ad esempio lo spettro
      (una volta diviso per 2\sigma^2(x) e moltiplicato per \Delta)
      vale esattamente \tau/(1+\omega^2\tau^2) 
      dove \tau e' il tempo caratteristico e \omega e' la pulsazione

      per ottenere la pulsazione, moltiplicare l'indice del vettore p
      (che va da 0 a n/2) per 2\pi/n\Delta 
      in questa maniera la pulsazione critica 
      (ovvero quella con indice n/2) e' data da \pi/\Delta
      (frequenza critica 1/2\Delta)

                         SPETTRO DI POTENZA
            -------------------------------------------

  void power(double *x,int n,double *p)
  ------------------------------------------
  calcola lo spettro di potenza del segnale reale x_0...x_(n-1) contenuto
  in x[0..n-1] (che viene distrutto)
  n deve essere una potenza di 2
  p[0..n/2] contiene P_0,...,P_n/2 in uscita:
  P_0 = modulo quadro della componente (reale) di frequenza zero
  P_1,...,P_(n/2-1) = moduli quadri delle componenti di frequenza positiva
  e minore della frequenza critica
  P_n/2 = modulo quadro della componente (reale) di frequenza critica 
  
  la frequenza critica e' f_c = 1/2\Delta dove \Delta e' l'intervallo
  temporale di sampling del segnale
  per ottenere la pulsazione \omega moltiplicare l'indice dello spettro
  m=0,...,n/2 per 2\pi/n\Delta
  lo spettro e' la trasformata di Fourier (pari e reale) della funzione
  di correlazione non normalizzata (pari e reale) del segnale x_0,...,x_(n-1), 
  (non normalizzata vuol dire che vale <x^2>-<x>^2 in t=0),
  piu' una delta (convoluta con la trasformata della finestra) in \omega=0,
  proporzionale a <x>^2
  notare che se si sottrae il contributo della delta, 
  il limite \omega\to 0 dello spettro vale 2(<x^2>-<x>^2)\tau, 
  dove \tau e' il tempo integrale di correlazione
  esempio: se la funzione di correlazione vale e^{-t/\tau} 
  allora lo spettro, (una volta sottratto il contributo della delta) vale
  2\tau/(1+\omega^2\tau^2)
  tenere conto inoltre dell'aliasing, ovvero del fatto che lo spettro è
  riflesso periodicamente con periodo 2f_c=1/\Delta, per cui per esempio
  alla frequenza f_c vale circa 2 volte il suo valore reale

****************************************************************************/
#ifndef __POWER_H__
#define __POWER_H__

void power(double *x,int n,double *p);

#ifndef __HEADERS__
#include <stdlib.h>
#include <math.h>
#include <pvm.h>

#define WINDOW(j,m)  (1.0-fabs(((j)-(m))/(double)(m)))  /* Bartlett window */

void power(double *x,int n,double *p)
  {
   int m,i,i1,i2;
   double wtmp,sum;
 
   m=n>>1;
   sum=0.0;
   for (i=0;i<n;i++)
     {
      x[i]*=(wtmp=WINDOW(i,m));
      sum+=wtmp*wtmp;
     }
   realft(x,n);
   p[0]+=x[0]*x[0]/sum;
   for (i=1;i<m;i++)
     {
      i1=i<<1;
      i2=i1+1;
      p[i]+=(x[i1]*x[i1]+x[i2]*x[i2])/sum;
     }
   p[m]+=x[1]*x[1]/sum;
  }
#endif
#endif
