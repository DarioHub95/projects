#ifndef __BROYDN_H__
#define __BROYDN_H__

void broydn(double *x, int n, int *check,void (*vecfunc)(int, double *, double *),double TOLF);

#ifndef __HEADERS__
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pvm.h>

#define MAXITS 200
#define EPS 1.0e-10		// era 1.e-7
//#define TOLF 1.0e-7		// era 1.e-4
#define TOLX EPS
#define STPMX 100.0
#define TOLMIN 1.0e-6
#define FREERETURN {free(fvec+1);free(xold+1);free(w+1);free(t+1);free(s+1);free(r[1]+1);free(r+1);free(qt[1]+1);free(qt+1);free(p+1);\
	free(g+1);free(fvcold+1);free(d+1);free(c+1);return;}

int nn;
double *fvec;
void (*nrfuncv)(int n, double *v, double *f);

static double sqrarg;
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)

static double maxarg1,maxarg2;
#define FMAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ? (maxarg1) : (maxarg2))

void broydn(double *x, int n, int *check,void (*vecfunc)(int, double *, double *),double TOLF)
{
	int i,its,j,k,restrt,sing,skip;
	double den,f,fold,stpmax,sum,temp,test,*c,*d,*fvcold;
	double *g,*p,**qt,**r,*s,*t,*w,*xold;

	c=dvector(1,n);
	d=dvector(1,n);
	fvcold=dvector(1,n);
	g=dvector(1,n);
	p=dvector(1,n);
	qt=dmatrix(1,n,1,n);
	r=dmatrix(1,n,1,n);
	s=dvector(1,n);
	t=dvector(1,n);
	w=dvector(1,n);
	xold=dvector(1,n);
	fvec=dvector(1,n);
	nn=n;
	nrfuncv=vecfunc;
	f=fmin(x);
	test=0.0;
	for (i=1;i<=n;i++)
		if (fabs(fvec[i]) > test)test=fabs(fvec[i]);
	if (test < 0.01*TOLF) {
		*check=0;
		FREERETURN
	}
	for (sum=0.0,i=1;i<=n;i++) sum += SQR(x[i]);
	stpmax=STPMX*FMAX(sqrt(sum),(double)n);
	restrt=1;
	for (its=1;its<=MAXITS;its++) {
		if (restrt) {
			fdjac(n,x,fvec,r,vecfunc);
			qrdcmp(r,n,c,d,&sing);
			if (sing)
                          {
                           //debug(9,"broydn: singular Jacobian\n");
                           *check=21;
                           FREERETURN
                          }
			for (i=1;i<=n;i++) {
				for (j=1;j<=n;j++) qt[i][j]=0.0;
				qt[i][i]=1.0;
			}
			for (k=1;k<n;k++) {
				if (c[k]) {
					for (j=1;j<=n;j++) {
						sum=0.0;
						for (i=k;i<=n;i++)
							sum += r[i][k]*qt[i][j];
						sum /= c[k];
						for (i=k;i<=n;i++)
							qt[i][j] -= sum*r[i][k];
					}
				}
			}
			for (i=1;i<=n;i++) {
				r[i][i]=d[i];
				for (j=1;j<i;j++) r[i][j]=0.0;
			}
		} else {
			for (i=1;i<=n;i++) s[i]=x[i]-xold[i];
			for (i=1;i<=n;i++) {
				for (sum=0.0,j=i;j<=n;j++) sum += r[i][j]*s[j];
				t[i]=sum;
			}
			skip=1;
			for (i=1;i<=n;i++) {
				for (sum=0.0,j=1;j<=n;j++) sum += qt[j][i]*t[j];
				w[i]=fvec[i]-fvcold[i]-sum;
				if (fabs(w[i]) >= EPS*(fabs(fvec[i])+fabs(fvcold[i]))) skip=0;
				else w[i]=0.0;
			}
			if (!skip) {
				for (i=1;i<=n;i++) {
					for (sum=0.0,j=1;j<=n;j++) sum += qt[i][j]*w[j];
					t[i]=sum;
				}
				for (den=0.0,i=1;i<=n;i++) den += SQR(s[i]);
				for (i=1;i<=n;i++) s[i] /= den;
				qrupdt(r,qt,n,t,s);
				for (i=1;i<=n;i++) {
					if (r[i][i] == 0.0)
                                          {
                                           //debug(9,"broydn: r singular\n");
                                           *check=22;
                                           FREERETURN
                                          }
					d[i]=r[i][i];
				}
			}
		}
		for (i=1;i<=n;i++) {
			for (sum=0.0,j=1;j<=n;j++) sum += qt[i][j]*fvec[j];
			p[i] = -sum;
		}
		for (i=n;i>=1;i--) {
			for (sum=0.0,j=1;j<=i;j++) sum -= r[j][i]*p[j];
			g[i]=sum;
		}
		for (i=1;i<=n;i++) {
			xold[i]=x[i];
			fvcold[i]=fvec[i];
		}
		fold=f;
		rsolv(r,n,d,p);
		lnsrch(n,xold,fold,g,p,x,&f,stpmax,check,fmin);
		test=0.0;
		for (i=1;i<=n;i++)
			if (fabs(fvec[i]) > test) test=fabs(fvec[i]);
		if (test < TOLF) {
			*check=0;
			FREERETURN
		}
		if (*check) {
			if (restrt) FREERETURN
			else {
				test=0.0;
				den=FMAX(f,0.5*n);
				for (i=1;i<=n;i++) {
					temp=fabs(g[i])*FMAX(fabs(x[i]),1.0)/den;
					if (temp > test) test=temp;
				}
				if (test < TOLMIN) FREERETURN
				else restrt=1;
			}
		} else {
			restrt=0;
			test=0.0;
			for (i=1;i<=n;i++) {
				temp=(fabs(x[i]-xold[i]))/FMAX(fabs(x[i]),1.0);
				if (temp > test) test=temp;
			}
			if (test < TOLX) FREERETURN
		}
#ifdef __PVM_LIB__
         if (cpuint(9)) debug(0,"broydn: %i/%i\n",its,MAXITS);
#endif
	}
	//debug(9,"broydn: MAXITS exceeded\n");
        *check=23;
	FREERETURN
}
#endif
#endif
