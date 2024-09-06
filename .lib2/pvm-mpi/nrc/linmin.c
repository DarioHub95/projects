#ifndef __LINMIN_H__
#define __LINMIN_H__

void linmin(double p[], double xi[], int n, double *fret, double (*func)(double []));

#ifndef __HEADERS__
#include <stdlib.h>
#include <pvm.h>
#define __HEADERS__
#include "brent.c"
#include "f1dim.c"
#include "mnbrak.c"

#define TOL 2.0e-4

int ncom;
double *pcom,*xicom,(*nrfunc)(double []);

void linmin(double p[], double xi[], int n, double *fret, double (*func)(double []))
{
	int j;
	double xx,xmin,fx,fb,fa,bx,ax;

	ncom=n;
	pcom=dvector(1,n);
	xicom=dvector(1,n);
	nrfunc=func;
	for (j=1;j<=n;j++) {
		pcom[j]=p[j];
		xicom[j]=xi[j];
	}
	ax=0.0;
	xx=1.0;
	mnbrak(&ax,&xx,&bx,&fa,&fx,&fb,f1dim);
	*fret=brent(ax,xx,bx,f1dim,TOL,&xmin);
	for (j=1;j<=n;j++) {
		xi[j] *= xmin;
		p[j] += xi[j];
	}
	free(xicom+1);
	free(pcom+1);
}
#endif
#endif
