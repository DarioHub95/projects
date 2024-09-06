#ifndef __QROMB_H__
#define __QROMB_H__

double qromb(double (*func)(double), double a, double b,double eps);

#ifndef __HEADERS__
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pvm.h>

#define JMAX 20
#define JMAXP (JMAX+1)
#define K 5

double qromb(double (*func)(double), double a, double b,double eps)		// N.B: eps=1e-6 nella versione originale (con reali a singola precisione)
{
	double ss,dss;
	double s[JMAXP],h[JMAXP+1];
	int j;

	h[1]=1.0;
        s[0]=0;
	for (j=1;j<=JMAX;j++) {
		s[j]=trapzd(func,a,b,j,s[j-1]);				// non è necessario settare s[0], per j==1 la routine trapzd non legge il valore di s
		if (j >= K) {
			polint(&h[j-K],&s[j-K],K,0.0,&ss,&dss);
			if (fabs(dss) <= eps*fabs(ss)) return ss;
		}
		h[j+1]=0.25*h[j];
	}
	fprintf(stderr,"qromb: too many steps...\n");
        exit(1);
	return 0.0;
}
#endif
#endif
