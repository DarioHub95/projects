#ifndef __PFAFF_H__
#define __PFAFF_H__

int pfaff(int N,double **A);

#ifndef __HEADERS__
#include <math.h>

/*

   calcola il pfaffiano di una matrice A[0..N-1][0..N-1]
   solo gli elementi A[i][j] con j>i devono essere definiti
   
   double p=pfaff(N,A,&p);
   for (int i=0;i<(N>>1);i++) p*=A[i][N-i-1];

*/

int pfaff(int N,double **A)
  {
   int p=1;
   for (int i=0;i<(N>>1)-1;i++)
     {
      int m=N-i-1;
      int jtmp=m;
      double amax=fabs(A[i][m]);
      double atmp;
      for (int j=jtmp-1;j>i;j--) if ((atmp=fabs(A[i][j]))>amax) {m=j; amax=atmp;}
      if (amax==0.0) break;
      if (m<jtmp)
        {
         for (int k=i;k<m;k++) {atmp=A[k][jtmp]; A[k][jtmp]=A[k][m]; A[k][m]=atmp;}
         for (int k=N-1;k>jtmp;k--) {atmp=A[jtmp][k]; A[jtmp][k]=A[m][k]; A[m][k]=atmp;}
         A[m][jtmp]=-A[m][jtmp];
         for (int k=m+1;k<jtmp;k++) {atmp=A[m][k]; A[m][k]=-A[k][jtmp]; A[k][jtmp]=-atmp;}
         p=-p;
        }
      atmp=A[i][jtmp];
      for (m=i+1;m<jtmp;m++)
        {
         double xtmp=-A[i][m]/atmp;
         for (int k=i;k<m;k++) A[k][m]+=xtmp*A[k][jtmp];
         for (int k=N-1;k>jtmp;k--) A[m][k]+=xtmp*A[k][jtmp];
         for (int k=m+1;k<jtmp;k++) A[m][k]-=xtmp*A[k][jtmp];
        }
     }
   return p;
  }
#endif
#endif
