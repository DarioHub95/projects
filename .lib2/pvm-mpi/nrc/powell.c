#ifndef __POWELL_H__
#define __POWELL_H__

void powell(double p[], double **xi, int n, double ftol, int *iter, double *fret, double (*func)(double []));

#ifndef __HEADERS__
#include <stdlib.h>
#include <math.h>
#include <pvm.h>
#define __HEADERS__
#include "linmin.c"

#define TINY 1.0e-25
#define ITMAX 200

static double sqrarg,sqrarg2;
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)
#define SQR2(a) ((sqrarg2=(a)) == 0.0 ? 0.0 : sqrarg2*sqrarg2)

void powell(double p[], double **xi, int n, double ftol, int *iter, double *fret, double (*func)(double []))
{
	int i,ibig,j;
	double del,fp,fptt,t,*pt,*ptt,*xit;

	pt=dvector(1,n);
	ptt=dvector(1,n);
	xit=dvector(1,n);
	*fret=(*func)(p);
	for (j=1;j<=n;j++) pt[j]=p[j];
	for (*iter=1;;++(*iter)) {
		fp=(*fret);
		ibig=0;
		del=0.0;
		for (i=1;i<=n;i++) {
			for (j=1;j<=n;j++) xit[j]=xi[j][i];
			fptt=(*fret);
			linmin(p,xit,n,fret,func);
			if (fptt-(*fret) > del) {
				del=fptt-(*fret);
				ibig=i;
			}
		}
		if (2.0*(fp-(*fret)) <= ftol*(fabs(fp)+fabs(*fret))+TINY) {
			free(xit+1);
			free(ptt+1);
			free(pt+1);
			return;
		}
		if (*iter == ITMAX) debug(9,"powell: exceeding maximum iterations\n");
		for (j=1;j<=n;j++) {
			ptt[j]=2.0*p[j]-pt[j];
			xit[j]=p[j]-pt[j];
			pt[j]=p[j];
		}
		fptt=(*func)(ptt);
		if (fptt < fp) {
			t=2.0*(fp-2.0*(*fret)+fptt)*SQR(fp-(*fret)-del)-del*SQR2(fp-fptt);
			if (t < 0.0) {
				linmin(p,xit,n,fret,func);
				for (j=1;j<=n;j++) {
					xi[j][ibig]=xi[j][n];
					xi[j][n]=xit[j];
				}
			}
		}
         if (cpuint(9))
           {
            debug(0,"powell: iter=%i fret=%g, p=",*iter,*fret);
            int nmax=(n<=10?n:10);
            for (i=1;i<=nmax;i++) debug(0," %g",p[i]);
            debug(0,"\n");
           }
	}
}
#endif
#endif
