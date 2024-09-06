#ifndef __PERMUTA_H__
#define __PERMUTA_H__

void permuta(int n,int N,int *stack);

#ifndef __HEADERS__
#include <math.h>
#include <pvm.h>

//
//
//   fa una permutazione casuale del vettore stack di lunghezza N
//   (solo i primi n elementi di stack devono essere usati)
//
//

void permuta(int n,int N,int *stack)
  {
   for (int i=0;i<n;i++)
     {
      int j=(int)floor((N-i)*Xrandom());
      int itmp=stack[i];
      stack[i]=stack[i+j];
      stack[i+j]=itmp;
     }
  }
#endif
#endif
