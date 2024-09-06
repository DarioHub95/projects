#ifndef __FACTLN_H__
#define __FACTLN_H__

double factln(int n);

#ifndef __HEADERS__
#include <stdio.h>
#include <stdlib.h>
#include <pvm.h>

double factln(int n)
{
	static double a[101];
	if (n<0)
          {
           fprintf(stderr,"factln: negative factorial\n");
           exit(1);
          }
	else if (n <= 1) return 0.0;
	else if (n <= 100)
          {
           if (a[n]==0) a[n]=gammln(n+1.0);
           return a[n];
          }
	else return gammln(n+1.0);
}
#endif
#endif
