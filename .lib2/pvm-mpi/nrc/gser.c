#ifndef __GSER_H__
#define __GSER_H__

void gser(double *gamser, double a, double x, double *gln);

#ifndef __HEADERS__
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define __HEADERS__
#include "gammln.c"

#define ITMAX 100
#define EPS 3.0e-7

void gser(double *gamser, double a, double x, double *gln)
{
	int n;
	double sum,del,ap;

	*gln=gammln(a);
	if (x <= 0.0) {
		if (x < 0.0)
                  {
                   fprintf(stderr,"gser: x less than 0\n");
                   exit(1);
                  }
		*gamser=0.0;
		return;
	} else {
		ap=a;
		del=sum=1.0/a;
		for (n=1;n<=ITMAX;n++) {
			++ap;
			del *= x/ap;
			sum += del;
			if (fabs(del) < fabs(sum)*EPS) {
				*gamser=sum*exp(-x+a*log(x)-(*gln));
				return;
			}
		}
		fprintf(stderr,"gser: a too large, ITMAX too small\n");
                exit(1);
	}
}
#endif
#endif
