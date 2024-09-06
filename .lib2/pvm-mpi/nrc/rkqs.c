#ifndef __RKQS_H__
#define __RKQS_H__

void rkqs(double y[], double dydx[], int n, double *x, double htry, double eps, double yscal[], double *hdid, double *hnext, void (*derivs)(double, double [], double []));

#ifndef __HEADERS__
#include <stdlib.h>
#include <math.h>
#include <pvm.h>

#define SAFETY 0.9
#define PGROW -0.2
#define PSHRNK -0.25
#define ERRCON 1.89e-4

static double maxarg1,maxarg2;
#define FMAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ? (maxarg1) : (maxarg2))

static double minarg1,minarg2;
#define FMIN(a,b) (minarg1=(a),minarg2=(b),(minarg1) < (minarg2) ? (minarg1) : (minarg2))


void rkqs(double y[], double dydx[], int n, double *x, double htry, double eps, double yscal[], double *hdid, double *hnext, void (*derivs)(double, double [], double []))
{
	int i;
	double errmax,h,htemp,xnew,*yerr,*ytemp;

	yerr=dvector(1,n);
	ytemp=dvector(1,n);
	h=htry;
	for (;;) {
		rkck(y,dydx,n,*x,h,ytemp,yerr,derivs);
		errmax=0.0;
		for (i=1;i<=n;i++) errmax=FMAX(errmax,fabs(yerr[i]/yscal[i]));
		errmax /= eps;
		if (errmax <= 1.0) break;
		htemp=SAFETY*h*pow(errmax,PSHRNK);
		h=(h >= 0.0 ? FMAX(htemp,0.1*h) : FMIN(htemp,0.1*h));
		xnew=(*x)+h;
		if (xnew == *x) debug(9,"rkqs: stepsize underflow...\n");
	}
	if (errmax > ERRCON) *hnext=SAFETY*h*pow(errmax,PGROW);
	else *hnext=5.0*h;
	*x += (*hdid=h);
	for (i=1;i<=n;i++) y[i]=ytemp[i];
	free(ytemp+1);
	free(yerr+1);
}
#endif
#endif
