#ifndef __FMIN_H__
#define __FMIN_H__

double fmin(double *x);

#ifndef __HEADERS__

extern int nn;
extern double *fvec;
extern void (*nrfuncv)(int n, double v[], double f[]);

static double sqrarg;
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)

double fmin(double *x)
{
	int i;
	double sum;

	(*nrfuncv)(nn,x,fvec);
	for (sum=0.0,i=1;i<=nn;i++) sum += SQR(fvec[i]);
	return 0.5*sum;
}
#endif
#endif
