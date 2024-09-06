#ifndef __PYTHAG_H__
#define __PYTHAG_H__

double pythag(double a, double b);

#ifndef __HEADERS__
#include <math.h>

#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)

double pythag(double a, double b)
{
	double absa,absb,sqrarg;
	absa=fabs(a);
	absb=fabs(b);
	if (absa > absb) return absa*sqrt(1.0+SQR(absb/absa));
	else return (absb == 0.0 ? 0.0 : absb*sqrt(1.0+SQR(absa/absb)));
}
#endif
#endif
