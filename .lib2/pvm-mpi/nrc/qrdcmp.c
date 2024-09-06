#ifndef __QRDCMP_H__
#define __QRDCMP_H__

void qrdcmp(double **a, int n, double *c, double *d, int *sing);

#ifndef __HEADERS__
#include <math.h>

static double sqrarg;
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)

static double maxarg1,maxarg2;
#define FMAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ? (maxarg1) : (maxarg2))

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

void qrdcmp(double **a, int n, double *c, double *d, int *sing)
{
	int i,j,k;
	double scale,sigma,sum,tau;

	*sing=0;
	for (k=1;k<n;k++) {
		scale=0.0;
		for (i=k;i<=n;i++) scale=FMAX(scale,fabs(a[i][k]));
		if (scale == 0.0) {
			*sing=1;
			c[k]=d[k]=0.0;
		} else {
			for (i=k;i<=n;i++) a[i][k] /= scale;
			for (sum=0.0,i=k;i<=n;i++) sum += SQR(a[i][k]);
			sigma=SIGN(sqrt(sum),a[k][k]);
			a[k][k] += sigma;
			c[k]=sigma*a[k][k];
			d[k] = -scale*sigma;
			for (j=k+1;j<=n;j++) {
				for (sum=0.0,i=k;i<=n;i++) sum += a[i][k]*a[i][j];
				tau=sum/c[k];
				for (i=k;i<=n;i++) a[i][j] -= tau*a[i][k];
			}
		}
	}
	d[n]=a[n][n];
	if (d[n] == 0.0) *sing=1;
}
#endif
#endif
