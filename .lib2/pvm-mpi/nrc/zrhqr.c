#ifndef __ZRHQR_H__
#define __ZRHQR_H__

int zrhqr(double *a,int m,double *rtr,double *rti);

#ifndef __HEADERS__
#include <stdlib.h>
#include <math.h>
#include <pvm.h>

#define MAXM 50
#define EPS 1e-10

int zrhqr(double *a,int m,double *rtr,double *rti)
{
	int j,k;
	double **hess,xr,xi;

        if (m>MAXM) debug(9,"zrhqr: aumentare MAXM?\n");
        while (m>0 && fabs(a[m])<EPS) m--;
        if (m==0)
          {
           return 0;
          }
        else if (m>0)
          {
	   hess=dmatrix(1,MAXM,1,MAXM);
	   for (k=1;k<=m;k++) {
	 	hess[1][k] = -a[m-k]/a[m];
		for (j=2;j<=m;j++) hess[j][k]=0.0;
		if (k != m) hess[k+1][k]=1.0;
	   }
	   balanc(hess,m);
	   hqr(hess,m,rtr,rti);
	   for (j=2;j<=m;j++) {
		xr=rtr[j];
		xi=rti[j];
		for (k=j-1;k>=1;k--) {
			if (rtr[k] <= xr) break;
			rtr[k+1]=rtr[k];
			rti[k+1]=rti[k];
		}
		rtr[k+1]=xr;
		rti[k+1]=xi;
	   }   
	   free(hess[1]+1);
           free(hess+1);
           return m;
          }
       else return -1;
}
#endif
#endif
