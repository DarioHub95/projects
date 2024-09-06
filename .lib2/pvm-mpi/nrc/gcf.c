#ifndef __GCF_H__
#define __GCF_H__

void gcf(double *gammcf, double a, double x, double *gln);

#ifndef __HEADERS__
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define __HEADERS__
#include "gammln.c"

#define ITMAX 100
#define EPS 3.0e-7
#define FPMIN 1.0e-30

void gcf(double *gammcf, double a, double x, double *gln)
{
	int i;
	double an,b,c,d,del,h;

	*gln=gammln(a);
	b=x+1.0-a;
	c=1.0/FPMIN;
	d=1.0/b;
	h=d;
	for (i=1;i<=ITMAX;i++) {
		an = -i*(i-a);
		b += 2.0;
		d=an*d+b;
		if (fabs(d) < FPMIN) d=FPMIN;
		c=b+an/c;
		if (fabs(c) < FPMIN) c=FPMIN;
		d=1.0/d;
		del=d*c;
		h *= del;
		if (fabs(del-1.0) < EPS) break;
	}
	if (i > ITMAX)
          {
           fprintf(stderr,"gcf: a too large, ITMAX too small\n");
           exit(1);
          }
	*gammcf=exp(-x+a*log(x)-(*gln))*h;
}
#endif
#endif
