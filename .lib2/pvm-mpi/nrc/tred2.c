#ifndef __TRED2_H__
#define __TRED2_H__

void tred2(double **a, int n, double *d, double *e,int flag=1);

#ifndef __HEADERS__
#include <stdlib.h>
#include <math.h>

void tred2(double **_a, int n, double *_d, double *_e,int flag)		// se flag=0 vuol dire che vogliamo solo gli autovalori (non gli autovettori)
{
	double **a=(double**)calloc(n,sizeof(double*))-1;
        for (int j=1;j<=n;j++) a[j]=_a[j-1]-1;
        double *d=_d-1;
        double *e=_e-1;

	int l,k,j,i;
        double scale,hh,h,g,f;

	for (i=n;i>=2;i--) {
		l=i-1;
		h=scale=0.0;
		if (l > 1) {
			for (k=1;k<=l;k++)
				scale += fabs(a[i][k]);
			if (scale == 0.0)
				e[i]=a[i][l];
			else {
				for (k=1;k<=l;k++) {
					a[i][k] /= scale;
					h += a[i][k]*a[i][k];
				}
				f=a[i][l];
				g=(f >= 0.0 ? -sqrt(h) : sqrt(h));
				e[i]=scale*g;
				h -= f*g;
				a[i][l]=f-g;
				f=0.0;
				for (j=1;j<=l;j++) {
					a[j][i]=a[i][j]/h;
					g=0.0;
					for (k=1;k<=j;k++)
						g += a[j][k]*a[i][k];
					for (k=j+1;k<=l;k++)
						g += a[k][j]*a[i][k];
					e[j]=g/h;
					f += e[j]*a[i][j];
				}
				hh=f/(h+h);
				for (j=1;j<=l;j++) {
					f=a[i][j];
					e[j]=g=e[j]-hh*f;
					for (k=1;k<=j;k++)
						a[j][k] -= (f*e[k]+g*a[i][k]);
				}
			}
		} else
			e[i]=a[i][l];
		d[i]=h;
	}
	d[1]=0.0;
	e[1]=0.0;
        if (flag)
          {
	   for (i=1;i<=n;i++) {		// Contents of this loop can be omitted if eigenvectors not wanted except for statement d[i]=a[i][i];
	  	l=i-1;
		if (d[i]) {
			for (j=1;j<=l;j++) {
				g=0.0;
				for (k=1;k<=l;k++)
					g += a[i][k]*a[k][j];
				for (k=1;k<=l;k++)
					a[k][j] -= g*a[k][i];
			}
		}
		d[i]=a[i][i];
		a[i][i]=1.0;
		for (j=1;j<=l;j++) a[j][i]=a[i][j]=0.0;
	       }
          }
       else for (i=1;i<=n;i++) d[i]=a[i][i];

       free(a+1);
}
#endif
#endif
