#ifndef __ODEINT_H__
#define __ODEINT_H__

void odeint(double ystart[], int nvar, double x1, double x2, double eps, double *h1,double hmin, int *nok, int *nbad,void (*derivs)(double, double [], double []),
        void (*rkqs)(double [], double [], int, double *, double, double, double [], double *, double *, void (*)(double, double [], double [])));

#ifndef __HEADERS__
#include <math.h>
#include <pvm.h>

#define MAXSTP 10000
#define TINY 1.0e-30

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

//extern int kmax,kount;
//extern double *xp,**yp,dxsav;

void odeint(double ystart[], int nvar, double x1, double x2, double eps, double *h1,double hmin, int *nok, int *nbad,void (*derivs)(double, double [], double []),
	void (*rkqs)(double [], double [], int, double *, double, double, double [], double *, double *, void (*)(double, double [], double [])))
{
	int nstp,i;
	double x,hnext,hdid,h;
	double *yscal,*y,*dydx;
        //double xsav;

	yscal=dvector(1,nvar);
	y=dvector(1,nvar);
	dydx=dvector(1,nvar);
	x=x1;
	h=SIGN(*h1,x2-x1);
	*nok = (*nbad) = 0; // kount = 0;
	for (i=1;i<=nvar;i++) y[i]=ystart[i];
	//if (kmax > 0) xsav=x-dxsav*2.0;
	for (nstp=1;nstp<=MAXSTP;nstp++) {
		(*derivs)(x,y,dydx);
		for (i=1;i<=nvar;i++)
			yscal[i]=fabs(y[i])+fabs(dydx[i]*h)+TINY;
#if 0
		if (kmax > 0 && kount < kmax-1 && fabs(x-xsav) > fabs(dxsav)) {
			xp[++kount]=x;
			for (i=1;i<=nvar;i++) yp[i][kount]=y[i];
			xsav=x;
		}
#endif
		if ((x+h-x2)*(x+h-x1) > 0.0) h=x2-x;
		(*rkqs)(y,dydx,nvar,&x,h,eps,yscal,&hdid,&hnext,derivs);
		if (hdid == h) ++(*nok); else ++(*nbad);
		if ((x-x2)*(x2-x1) >= 0.0) {
			for (i=1;i<=nvar;i++) ystart[i]=y[i];
#if 0
			if (kmax) {
				xp[++kount]=x;
				for (i=1;i<=nvar;i++) yp[i][kount]=y[i];
			}
#endif
			free(dydx+1);
			free(y+1);
			free(yscal+1);
                        *h1=h;
			return;
		}
		if (fabs(hnext) <= hmin) debug(9,"odeint: step size too small...\n");
		h=hnext;
	}
	debug(9,"odeint: too many steps...");
}
#endif
#endif
