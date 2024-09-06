#ifndef __BICO_H__
#define __BICO_H__

double bico(int n,int k);

#ifndef __HEADERS__
#include <math.h>
#include <pvm.h>

double bico(int n, int k)
{
	return floor(0.5+exp(factln(n)-factln(k)-factln(n-k)));
}
#endif
#endif
