#ifndef __INDEX_H__
#define __INDEX_H__

// scrive l'array indx in maniera che arr[indx[0]] <= arr[indx[1]] <= ... <= arr[indx[n-1]]
// N.B: convenzione C sui vettori: A[0...N-1]

void index(int n, double *arr, int *indx);

#ifndef __HEADERS__
#include <stdio.h>
#include <stdlib.h>
#define __HEADERS__

#define SWAP(a,b) itemp=(a);(a)=(b);(b)=itemp;
#define M 7
#define NSTACK 50

void index(int n, double *_arr, int *_indx)
  {
	int i,indxt,ir=n,itemp,j,k,l=1;
	int jstack=0,*istack,*indx;
	double a,*arr;

        arr=_arr-1;
        indx=_indx-1;
	istack=(int*)calloc(NSTACK,sizeof(int))-1;
	for (j=1;j<=n;j++) indx[j]=j;
	for (;;) {
		if (ir-l < M) {
			for (j=l+1;j<=ir;j++) {
				indxt=indx[j];
				a=arr[indxt];
				for (i=j-1;i>=l;i--) {
					if (arr[indx[i]] <= a) break;
					indx[i+1]=indx[i];
				}
				indx[i+1]=indxt;
			}
			if (jstack == 0) break;
			ir=istack[jstack--];
			l=istack[jstack--];
		} else {
			k=(l+ir) >> 1;
			SWAP(indx[k],indx[l+1]);
			if (arr[indx[l]] > arr[indx[ir]]) {
				SWAP(indx[l],indx[ir])
			}
			if (arr[indx[l+1]] > arr[indx[ir]]) {
				SWAP(indx[l+1],indx[ir])
			}
			if (arr[indx[l]] > arr[indx[l+1]]) {
				SWAP(indx[l],indx[l+1])
			}
			i=l+1;
			j=ir;
			indxt=indx[l+1];
			a=arr[indxt];
			for (;;) {
				do i++; while (arr[indx[i]] < a);
				do j--; while (arr[indx[j]] > a);
				if (j < i) break;
				SWAP(indx[i],indx[j])
			}
			indx[l+1]=indx[j];
			indx[j]=indxt;
			jstack += 2;
			if (jstack > NSTACK)
                          {
                           fprintf(stderr,"NSTACK too small in index\n");
                           exit(1);
                          }
			if (ir-i+1 >= j-l) {
				istack[jstack]=ir;
				istack[jstack-1]=i;
				ir=j-1;
			} else {
				istack[jstack]=j-1;
				istack[jstack-1]=l;
				l=i;
			}
		}
	}
	free(istack+1);
        for (i=0;i<n;i++) _indx[i]--;
}
#endif
#endif
