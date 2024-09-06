#ifndef __SPRSIN_H__

void sprsin(double **a, int n, double thresh, unsigned long nmax,double sa[],unsigned long ija[]);

#ifndef __HEADERS__
#include <math.h>
#include <pvm.h>

void sprsin(double **a, int n, double thresh, unsigned long nmax,double sa[],unsigned long ija[])
{
	int i,j;
	unsigned long k;

	for (j=1;j<=n;j++) sa[j]=a[j][j];
	ija[1]=n+2;
	k=n+1;
	for (i=1;i<=n;i++) {
		for (j=1;j<=n;j++) {
			if (fabs(a[i][j]) >= thresh && i != j) {
				if (++k > nmax) debug(9,"sprsin: nmax too small\n");
				sa[k]=a[i][j];
				ija[k]=j;
			}
		}
		ija[i+1]=k+1;
	}
}
#endif
#endif
