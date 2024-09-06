#ifndef __GAMMQ_H__
#define __GAMMQ_H__

double gammq(double a, double x);

#ifndef __HEADERS__
#include <stdio.h>
#include <pvm.h>

double gammq(double a, double x)
{
	double gamser,gammcf,gln;

	if (x < 0.0 || a <= 0.0)
          {
           fprintf(stderr,"gammq: invalid arguments...\n");
          }
	if (x < (a+1.0)) {
		gser(&gamser,a,x,&gln);
		return 1.0-gamser;
	} else {
		gcf(&gammcf,a,x,&gln);
		return gammcf;
	}
}
#endif
#endif
