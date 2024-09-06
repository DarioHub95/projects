#ifndef __POLINT_H__
#define __POLINT_H__

void polint(double *xa,double *ya,int n,double x,double *y,double *dy);

#ifndef __HEADERS__
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pvm.h>

void polint(double *xa,double *ya,int n,double x,double *y,double *dy)
{
	int i,m,ns=1;
	double den,dif,dift,ho,hp,w;
	double *c,*d;

	dif=fabs(x-xa[1]);
	c=dvector(1,n);
	d=dvector(1,n);
	for (i=1;i<=n;i++) {
		if ( (dift=fabs(x-xa[i])) < dif) {
			ns=i;
			dif=dift;
		}
		c[i]=ya[i];
		d[i]=ya[i];
	}
	*y=ya[ns--];
	for (m=1;m<n;m++) {
		for (i=1;i<=n-m;i++) {
			ho=xa[i]-x;
			hp=xa[i+m]-x;
			w=c[i+1]-d[i];
			if ( (den=ho-hp) == 0.0) debug(0,"error in routine polint...\n");
			den=w/den;
			d[i]=hp*den;
			c[i]=ho*den;
		}
		*y += (*dy=(2*ns < (n-m) ? c[ns+1] : d[ns--]));
	}
	free(d+1);
	free(c+1);
}
#endif
#endif
