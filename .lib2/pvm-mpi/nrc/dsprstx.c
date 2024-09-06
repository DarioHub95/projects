#ifndef __DSPRSTX_H__

void dsprstx(double sa[], unsigned long ija[], double x[], double b[], unsigned long n);

#ifndef __HEADERS__
#include <pvm.h>

void dsprstx(double sa[], unsigned long ija[], double x[], double b[], unsigned long n)
{
	unsigned long i,j,k;
	if (ija[1] != n+2) debug(9,"mismatched vector and matrix in dsprstx\n");
	for (i=1;i<=n;i++) b[i]=sa[i]*x[i];
	for (i=1;i<=n;i++) {
		for (k=ija[i];k<=ija[i+1]-1;k++) {
			j=ija[k];
			b[j] += sa[k]*x[i];
		}
	}
}
#endif
#endif
