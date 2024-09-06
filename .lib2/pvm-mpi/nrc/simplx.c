#ifndef __SIMPLX_H__
#define __SIMPLX_H__

void simplx(double **a, int m, int n, int m1, int m2, int m3, int *icase,int izrov[], int iposv[]);

#ifndef __HEADERS__
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static int *ivector(int imin,int imax)
  {
   return (int*)calloc((long)(imax-imin+1),sizeof(int))-imin;
  }

static void simp1(double **a, int mm, int ll[], int nll, int iabf, int *kp, double *bmax)
{
	int k;
	double test;

	if (nll <= 0)
		*bmax=0.0;
	else {
		*kp=ll[1];
		*bmax=a[mm+1][*kp+1];
		for (k=2;k<=nll;k++) {
			if (iabf == 0)
				test=a[mm+1][ll[k]+1]-(*bmax);
			else
				test=fabs(a[mm+1][ll[k]+1])-fabs(*bmax);
			if (test > 0.0) {
				*bmax=a[mm+1][ll[k]+1];
				*kp=ll[k];
			}
		}
	}
}

#define EPS 1.0e-6

static void simp2(double **a, int m, int n, int *ip, int kp)
{
	int k,i;
	double qp,q0,q,q1;

	*ip=0;
	for (i=1;i<=m;i++)
		if (a[i+1][kp+1] < -EPS) break;
	if (i>m) return;
	q1 = -a[i+1][1]/a[i+1][kp+1];
	*ip=i;
	for (i=*ip+1;i<=m;i++) {
		if (a[i+1][kp+1] < -EPS) {
			q = -a[i+1][1]/a[i+1][kp+1];
			if (q < q1) {
				*ip=i;
				q1=q;
			} else if (q == q1) {
				for (k=1;k<=n;k++) {
					qp = -a[*ip+1][k+1]/a[*ip+1][kp+1];
					q0 = -a[i+1][k+1]/a[i+1][kp+1];
					if (q0 != qp) break;
				}
				if (q0 < qp) *ip=i;
			}
		}
	}
}

static void simp3(double **a, int i1, int k1, int ip, int kp)
{
	int kk,ii;
	double piv;

	piv=1.0/a[ip+1][kp+1];
	for (ii=1;ii<=i1+1;ii++)
		if (ii-1 != ip) {
			a[ii][kp+1] *= piv;
			for (kk=1;kk<=k1+1;kk++)
				if (kk-1 != kp)
					a[ii][kk] -= a[ip+1][kk]*a[ii][kp+1];
		}
	for (kk=1;kk<=k1+1;kk++)
		if (kk-1 != kp) a[ip+1][kk] *= -piv;
	a[ip+1][kp+1]=piv;
}

#define FREEALL free(l3+1);free(l1+1);

void simplx(double **a, int m, int n, int m1, int m2, int m3, int *icase, int izrov[], int iposv[])
{
	int i,ip,is,k,kh,kp,nl1;
	int *l1,*l3;
	double q1,bmax;

	if (m != (m1+m2+m3)) {fprintf(stderr,"Bad input constraint counts in simplx\n"); exit(1);}
	l1=ivector(1,n+1);
	l3=ivector(1,m);
	nl1=n;
	for (k=1;k<=n;k++) l1[k]=izrov[k]=k;
	for (i=1;i<=m;i++) {
		if (a[i+1][1] < 0.0) {fprintf(stderr,"Bad input tableau in simplx\n"); exit(1);}
		iposv[i]=n+i;
	}
	if (m2+m3) {
		for (i=1;i<=m2;i++) l3[i]=1;
		for (k=1;k<=(n+1);k++) {
			q1=0.0;
			for (i=m1+1;i<=m;i++) q1 += a[i+1][k];
			a[m+2][k] = -q1;
		}
		for (;;) {
			simp1(a,m+1,l1,nl1,0,&kp,&bmax);
			if (bmax <= EPS && a[m+2][1] < -EPS) {
				*icase = -1;
				FREEALL return;
			} else if (bmax <= EPS && a[m+2][1] <= EPS) {
				for (ip=m1+m2+1;ip<=m;ip++) {
					if (iposv[ip] == (ip+n)) {
						simp1(a,ip,l1,nl1,1,&kp,&bmax);
						if (bmax > EPS)
							goto one;
					}
				}
				for (i=m1+1;i<=m1+m2;i++)
					if (l3[i-m1] == 1)
						for (k=1;k<=n+1;k++)
							a[i+1][k] = -a[i+1][k];
				break;
			}
			simp2(a,m,n,&ip,kp);
			if (ip == 0) {
				*icase = -1;
				FREEALL return;
			}
	one:	simp3(a,m+1,n,ip,kp);
			if (iposv[ip] >= (n+m1+m2+1)) {
				for (k=1;k<=nl1;k++)
					if (l1[k] == kp) break;
				--nl1;
				for (is=k;is<=nl1;is++) l1[is]=l1[is+1];
			} else {
				kh=iposv[ip]-m1-n;
				if (kh >= 1 && l3[kh]) {
					l3[kh]=0;
					++a[m+2][kp+1];
					for (i=1;i<=m+2;i++)
						a[i][kp+1] = -a[i][kp+1];
				}
			}
			is=izrov[kp];
			izrov[kp]=iposv[ip];
			iposv[ip]=is;
		}
	}
	for (;;) {
		simp1(a,0,l1,nl1,0,&kp,&bmax);
		if (bmax <= EPS) {
			*icase=0;
			FREEALL return;
		}
		simp2(a,m,n,&ip,kp);
		if (ip == 0) {
			*icase=1;
			FREEALL return;
		}
		simp3(a,m,n,ip,kp);
		is=izrov[kp];
		izrov[kp]=iposv[ip];
		iposv[ip]=is;
	}
}
#endif
#endif
