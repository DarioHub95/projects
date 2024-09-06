#ifndef __TQLI_H__
#define __TQLI_H__
#include <stdlib.h>

void tqli(double *d, double *e, int n, double **z=NULL);

#ifndef __HEADERS__
#include <math.h>
#include <stdio.h>
#define __HEADERS__
#include "pythag.c"

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

#define MAXITER 100

void tqli(double *_d, double *_e, int n, double **_z)
{
        double **z=NULL;
        if (_z) {
		z=(double**)calloc(n,sizeof(double*))-1;
        	for (int j=1;j<=n;j++) z[j]=_z[j-1]-1;
        }
        double *d=_d-1;
        double *e=_e-1;

	int m,l,iter,i,k;
	double s,r,p,g,f,dd,c,b;

	for (i=2;i<=n;i++) e[i-1]=e[i];
	e[n]=0.0;
	for (l=1;l<=n;l++) {
		iter=0;
		do {
			for (m=l;m<=n-1;m++) {
				dd=fabs(d[m])+fabs(d[m+1]);
				if ((double)(fabs(e[m])+dd) == dd) break;
			}
			if (m != l) {
				if (iter++ == MAXITER)
                                  {
                                   fprintf(stderr,"tqli: too many iterations\n");
                                   exit(1);
                                  }
				g=(d[l+1]-d[l])/(2.0*e[l]);
				r=pythag(g,1.0);
				g=d[m]-d[l]+e[l]/(g+SIGN(r,g));
				s=c=1.0;
				p=0.0;
				for (i=m-1;i>=l;i--) {
					f=s*e[i];
					b=c*e[i];
					e[i+1]=(r=pythag(f,g));
					if (r == 0.0) {
						d[i+1] -= p;
						e[m]=0.0;
						break;
					}
					s=f/r;
					c=g/r;
					g=d[i+1]-p;
					r=(d[i]-g)*s+2.0*c*b;
					d[i+1]=g+(p=s*r);
					g=c*r-b;
                                        if (z) {				// calcola autovettori
					for (k=1;k<=n;k++) {
						f=z[k][i+1];
						z[k][i+1]=s*z[k][i]+c*f;
						z[k][i]=c*z[k][i]-s*f;
					}}
				}
				if (r == 0.0 && i >= l) continue;
				d[l] -= p;
				e[l]=g;
				e[m]=0.0;
			}
		} while (m != l);
	}
      if (z) free(z+1);
}
#endif
#endif
