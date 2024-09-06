#ifndef __DSPRSAX_H__

void dsprsax(double sa[], unsigned long ija[], double x[], double b[], unsigned long n);

#ifndef __HEADERS__
#include <pvm.h>

void dsprsax(double sa[], unsigned long ija[], double x[], double b[], unsigned long n)
{
	unsigned long i,k;

	if (ija[1] != n+2) debug(9,"dsprsax: mismatched vector and matrix\n");
	for (i=1;i<=n;i++) {
		b[i]=sa[i]*x[i];
		for (k=ija[i];k<=ija[i+1]-1;k++) b[i] += sa[k]*x[ija[k]];
	}
}
#endif
#endif
