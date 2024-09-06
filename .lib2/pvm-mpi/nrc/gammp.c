#ifndef __GAMMP_H__
#define __GAMMP_H__

double gammp(double a,double x);

#ifndef __HEADERS__
#include <stdio.h>
#include <stdlib.h>
#define __HEADERS__
#include "gcf.c"
#include "gser.c"

double gammp(double a, double x)
{
	double gamser,gammcf,gln;

	if (x < 0.0 || a <= 0.0)
          {
           fprintf(stderr,"gammp: invalid arguments\n");
           exit(1);
          }
	if (x < (a+1.0)) {
		gser(&gamser,a,x,&gln);
		return gamser;
	} else {
		gcf(&gammcf,a,x,&gln);
		return 1.0-gammcf;
	}
}
#endif
#endif
