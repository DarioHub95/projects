#ifndef __POLYFIT_H__
#define __POLYFIT_H__

void polyfit(int n,double *x,double *y,double *s,int m,double *a);

#ifndef __HEADERS__
#include <pvm.h>
#include <math.h>

// fai un fit dei punti x[i],y[i] con errore s[i], i=0...n-1
// con un polinomio di grado m, con coefficienti a[0], a[1]... a[m]

void polyfit(int n,double *x,double *y,double *s,int m,double *a)
  {
   double **c=dmatrix(1,m+1,1,m+1);
   int *inx=ivector(1,m+1);
   double d0=0;
   for (int k=0;k<=1+m;k++)
     {
      for (int i=1;i<=1+m;i++) for (int j=1;j<=1+m;j++) c[i][j]=0;
      for (int i=1;i<=1+m;i++) if (k==i)
        {
         // colonna dei termini noti...
         for (int j=1;j<=1+m;j++) for (int l=0;l<n;l++) c[i][j]+=y[l]*pow(x[l],j-1)/(s[l]*s[l]);
        }
      else
        {
         for (int j=1;j<=1+m;j++) for (int l=0;l<n;l++) c[i][j]+=pow(x[l],i+j-2)/(s[l]*s[l]);
        }
      double d;
      ludcmp(c,1+m,inx,&d);
      for (int i=1;i<=1+m;i++) d*=c[i][i];
      if (k>0)
        {
         a[k-1]=d/d0;
        }
      else d0=d;
     }
   free(inx+1);
   free(c[1]+1);
   free(c+1);
  }
#endif
#endif
