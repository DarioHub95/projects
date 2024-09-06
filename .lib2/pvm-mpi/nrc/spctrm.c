#ifndef __SPCTRM_H__
#define __SPCTRM_H__

void spctrm(double *fp,double *p, int m, int k, int ovrlap);

#ifndef __HEADERS__
#include <math.h>
#include <stdio.h>
#include <pvm.h>

#define WINDOW(j,a,b) (1.0-fabs((((j)-1)-(a))*(b)))       /* Bartlett */

void spctrm(double *fp,double *p, int m, int k, int ovrlap)
{
	int mm,m44,m43,m4,kk,joffn,joff,j2,j,c;
	double w,facp,facm,*w1,*w2,*p1,sumw=0.0,den=0.0;

        c=0;
	mm=m+m;
	m43=(m4=mm+mm)+3;
	m44=m43+1;
	w1=dvector(1,m4);
	w2=dvector(1,m);
        p1=dvector(1,m);
	facm=m;
	facp=1.0/m;
	for (j=1;j<=mm;j++)
          {
           double s1=WINDOW(j,facm,facp);
           sumw += s1*s1;
          }
	if (ovrlap)
		for (j=1;j<=m;j++) w2[j]=fp[c++];
	for (kk=1;kk<=k;kk++) {
		for (joff = -1;joff<=0;joff++) {
			if (ovrlap) {
				for (j=1;j<=m;j++) w1[joff+j+j]=w2[j];
				for (j=1;j<=m;j++) w2[j]=fp[c++];
				joffn=joff+mm;
				for (j=1;j<=m;j++) w1[joffn+j+j]=w2[j];
			} else {
				for (j=joff+2;j<=m4;j+=2) w1[j]=fp[c++];
			}
		}
		for (j=1;j<=mm;j++) {
			j2=j+j;
			w=WINDOW(j,facm,facp);
			w1[j2] *= w;
			w1[j2-1] *= w;
		}
		four1(w1,mm,1);
                double s1=w1[1];
                double s2=w1[2];
		p1[1] += s1*s1+s2*s2;
		for (j=2;j<=m;j++) {
			j2=j+j;
                        s1=w1[j2];
                        s2=w1[j2-1];
                        double s3=w1[m44-j2];
                        double s4=w1[m43-j2];
			p1[j] += s1*s1+s2*s2+s3*s3+s4*s4;
		}
		den += sumw;
	}
	den *= m4;
	for (j=1;j<=m;j++) p[j]+=p1[j]/den;
        free(p1+1);
	free(w2+1);
	free(w1+1);
}
#endif
#endif
