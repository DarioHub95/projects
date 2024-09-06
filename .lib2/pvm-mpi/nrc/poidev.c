#ifndef __POIDEV_H__
#define __POIDEV_H__

int poidev(double mu);

#ifndef __HEADERS__
#include <math.h>
#include <pvm.h>

#define EPS 1.2e-7

int poidev(double mu)
{
	static double sq,alxm,g,oldm=(-1.0);
	double xm,em,t,y;

	xm=mu;
	if (xm < 12.0) {
		if (xm != oldm) {
			oldm=xm;
			g=exp(-xm);
		}
		em = -1;
		t=1.0;
		do {
			++em;
			t *= Xrandom();
		} while (t > g);
	} else {
		if (xm != oldm) {
			oldm=xm;
			sq=sqrt(2.0*xm);
			alxm=log(xm);
			g=xm*alxm-gammln(xm+1.0);
		}
		do {
			do {
				y=tan(M_PI*Xrandom());
				em=sq*y+xm;
			} while (em < 0.0);
			em=floor(em);
			t=0.9*(1.0+y*y)*exp(em*alxm-gammln(em+1.0)-g);
		} while (Xrandom() > t);
	}
	return (int)floor(em+EPS);
}
#endif
#endif
