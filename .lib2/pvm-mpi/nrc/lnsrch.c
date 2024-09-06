#ifndef __LNSRCH_H__
#define __LNSRCH_H__

void lnsrch(int n, double *xold, double fold, double *g, double *p, double *x, double *f, double stpmax, int *check, double (*func)(double *));

#ifndef __HEADERS__
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static double maxarg1,maxarg2;
#define FMAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ? (maxarg1) : (maxarg2))

#define ALF 1.0e-4
#define TOLX 1.0e-7

void lnsrch(int n, double *xold, double fold, double *g, double *p, double *x, double *f, double stpmax, int *check, double (*func)(double *))
{
	int i;
	double a,alam,alam2=0,alamin,b,disc,f2=0,rhs1,rhs2,slope,sum,temp,test,tmplam;

	*check=0;
	for (sum=0.0,i=1;i<=n;i++) sum += p[i]*p[i];
	sum=sqrt(sum);
	if (sum > stpmax)
		for (i=1;i<=n;i++) p[i] *= stpmax/sum;
	for (slope=0.0,i=1;i<=n;i++)
		slope += g[i]*p[i];
	if (slope >= 0.0)
          {
           fprintf(stderr,"lnsrch: roundoff problem\n");
           exit(1);
          }
	test=0.0;
	for (i=1;i<=n;i++) {
		temp=fabs(p[i])/FMAX(fabs(xold[i]),1.0);
		if (temp > test) test=temp;
	}
	alamin=TOLX/test;
	alam=1.0;
	for (;;) {
		for (i=1;i<=n;i++) x[i]=xold[i]+alam*p[i];
		*f=(*func)(x);
		if (alam < alamin) {
			for (i=1;i<=n;i++) x[i]=xold[i];
			*check=1;
			return;
		} else if (*f <= fold+ALF*alam*slope) return;
		else {
			if (alam == 1.0)
				tmplam = -slope/(2.0*(*f-fold-slope));
			else {
				rhs1 = *f-fold-alam*slope;
				rhs2=f2-fold-alam2*slope;
				a=(rhs1/(alam*alam)-rhs2/(alam2*alam2))/(alam-alam2);
				b=(-alam2*rhs1/(alam*alam)+alam*rhs2/(alam2*alam2))/(alam-alam2);
				if (a == 0.0) tmplam = -slope/(2.0*b);
				else {
					disc=b*b-3.0*a*slope;
					if (disc < 0.0) tmplam=0.5*alam;
					else if (b <= 0.0) tmplam=(-b+sqrt(disc))/(3.0*a);
					else tmplam=-slope/(b+sqrt(disc));
				}
				if (tmplam > 0.5*alam)
					tmplam=0.5*alam;
			}
		}
		alam2=alam;
		f2 = *f;
		alam=FMAX(tmplam,0.1*alam);
	}
}
#endif
#endif
