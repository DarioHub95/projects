#ifndef __HISTO_H__
#define __HISTO_H__

//	utilizzo:
//	histo *Y=new histo(xmin,xmax);		// ppdec: numero di punti per decade (scala log) oppure totali (scala lin)
//						// (type&1)!=0 se scala lineare, (type&1)==0 se scala logaritmica
//						// (type&2)!=0 se variabile continua, (type&2)==0 se variabile discreta... es: type=3 per scala lineare & variabile continua
//	double *H=dvector(0,Y->hmax);
//      ...
//	   int h=Y->bin(x);
//	   H[h]++;
//	   count++
//      ...
// 	for (int h=1;h<Y->hmax;h++) if (Y->norm[h]>0) printf("%12g %12g\n",Y->xtmp[h],H[h]/(count*Y->norm[h]));

class histo
  {
   public:
   histo(double xmin,double xmax,int type=0,int ppdec=0);
   int bin(double x);

   int type;
   int hmax;
   double xmin;
   double xmax;
   double *x1;			// estremo inferiore intervallo
   double *x2;			// estremo superiore
   double *norm;		// normalizzazione (numero interi nell'intervallo se variabile discreta, altrimenti larghezza dell'intervallo)
   double *xtmp;		// punto medio dell'intervallo
  };

#ifndef __HEADERS__
#include <math.h>
#include <pvm.h>

// N.B: i bin da 1 a hmax-1 corrispondono all'intervallo [xmin,xmax]
// i bin 0 e hmax a tutto quello che è fuori dall'intervallo

#define EPS 1e-8

histo::histo(double _xmin,double _xmax,int _type,int ppdec)
  {
   xmin=_xmin;
   xmax=_xmax;
   type=_type;
   if (type&1)
     {
      // scala lineare
      if (ppdec<=0) ppdec=100;			// default per scala lineare
      hmax=ppdec;
     }
   else
     {
      // scala logaritmica
      if (ppdec<=0) ppdec=20;					// default per scala logaritmica
      hmax=1+(int)rint(ppdec*log(xmax/xmin)/M_LN10);
     }
   x1=dvector(0,hmax);
   x2=dvector(0,hmax);
   norm=dvector(0,hmax);
   xtmp=dvector(0,hmax);
   for (int h=1;h<hmax;h++)
     {
      if (type&1)
        {
         // scala lineare
         x1[h]=xmin+(h-1)*(xmax-xmin)/(hmax-1);
         x2[h]=xmin+h*(xmax-xmin)/(hmax-1);
        }
      else
        {
         // scala logaritmica
         x1[h]=xmin*exp((h-1)*log(xmax/xmin)/(hmax-1));
         x2[h]=xmin*exp(h*log(xmax/xmin)/(hmax-1));
        }
      if ((type&2)==0)
        {
         // variabile discreta: trova intero iniziale e finale dell'intervallo
         x1[h]=ceil(x1[h]-EPS);
         x2[h]=floor(x2[h]-EPS);
         if (x2[h]>=x1[h]) norm[h]=1+x2[h]-x1[h];
        }
      else norm[h]=x2[h]-x1[h];
      if (type&1) xtmp[h]=(x1[h]+x2[h])/2; else xtmp[h]=sqrt(x1[h]*x2[h]);	// punto centrale dell'intervallo
     }
#if 0
   // check che il calcolo di norm[h] sia corretto (variabile discreta)
   double *norm2=dvector(0,hmax);
   for (double x=xmin;x<=xmax;x++) norm2[bin(x)]++;
   for (int h=1;h<hmax;h++) if (norm[h]>0 || norm2[h]>0)
     {
      fprintf(stderr,"%12i %12g %12g %12g %12g %12g %12g\n",h,x1[h],x2[h],xtmp[h],norm[h],norm2[h],norm[h]-norm2[h]);
     }
   free(norm2);
   exit(1);
#endif
  }

int histo::bin(double x)
  {
   int h=0;
   if (type&1)
     {
      // scala lineare
      h=1+(int)floor((hmax-1)*(x-xmin)/(xmax-xmin));
     }
   else
     {
      // scala logaritmica
      h=1+(int)floor((hmax-1)*log(x/xmin)/log(xmax/xmin));
     }
   if (h<0) h=0; else if (h>hmax) h=hmax;
   return h;
  }
#endif
#endif
