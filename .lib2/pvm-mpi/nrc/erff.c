#ifndef __ERFF_H__
#define __ERFF_H__

double erff(double x);

#ifndef __HEADERS__
#include <pvm.h>

double erff(double x)
{
	double gammp(double a, double x);

	return x < 0.0 ? -gammp(0.5,x*x) : gammp(0.5,x*x);
}
#endif
#endif
