#ifndef __RK4_H__
#define __RK4_H__

void rk4(double y[], double dydx[], int n, double x, double h, double yout[],void (*derivs)(double, double [], double []));

#ifndef __HEADERS__
#include <pvm.h>

void rk4(double y[], double dydx[], int n, double x, double h, double yout[],void (*derivs)(double, double [], double []))
{
	int i;
	double xh,hh,h6,*dym,*dyt,*yt;

	dym=dvector(1,n);
	dyt=dvector(1,n);
	yt=dvector(1,n);
	hh=h*0.5;
	h6=h/6.0;
	xh=x+hh;
	for (i=1;i<=n;i++) yt[i]=y[i]+hh*dydx[i];
	(*derivs)(xh,yt,dyt);
	for (i=1;i<=n;i++) yt[i]=y[i]+hh*dyt[i];
	(*derivs)(xh,yt,dym);
	for (i=1;i<=n;i++) {
		yt[i]=y[i]+h*dym[i];
		dym[i] += dyt[i];
	}
	(*derivs)(x+h,yt,dyt);
	for (i=1;i<=n;i++)
		yout[i]=y[i]+h6*(dydx[i]+dyt[i]+2.0*dym[i]);
	free(yt+1);
	free(dyt+1);
	free(dym+1);
}
#endif
#endif
