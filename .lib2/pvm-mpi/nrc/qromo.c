#ifndef __QROMO_H__
#define __QROMO_H__

double qromo(double (*func)(double), double a, double b,double (*choose)(double(*)(double), double, double, int),double eps);

#ifndef __HEADERS__
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pvm.h>

#define JMAX 20			// N.B: JMAX=14 nella versione originale...
#define JMAXP (JMAX+1)
#define K 5

double qromo(double (*func)(double), double a, double b,double (*choose)(double(*)(double), double, double, int),double eps)	// N.B: eps=1e-6 nella versione originale...
{
	int j;
	double ss,dss,h[JMAXP+1],s[JMAXP];

	h[1]=1.0;
	for (j=1;j<=JMAX;j++) {
		s[j]=(*choose)(func,a,b,j);
		if (j >= K) {
			polint(&h[j-K],&s[j-K],K,0.0,&ss,&dss);
			if (fabs(dss) <= eps*fabs(ss)) return ss;
		}
		h[j+1]=h[j]/9.0;
	}
	fprintf(stderr,"qromo: too many steps...\n");
        exit(1);
}
#endif
#endif
