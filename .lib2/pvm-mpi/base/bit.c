#ifndef __BIT_H__
#define __BIT_H__
#include <stdio.h>

void bitopen(FILE *stream);
void bitwrite(int a,int n);
int bitread(int n);
int bitclose(void);

#ifndef __HEADERS__
#include <stdio.h>
static FILE *_stream=NULL;
static int w,r,nb;
static unsigned char A;

#define BITS 8

void bitopen(FILE *stream)
  {
   _stream=stream;
   w=r=0;
   A=0;
   nb=0;
  }

void bitwrite(int a,int n)
  {
   for (int i=0;i<n;i++)
     {
      if (a&(1<<i)) A|=(unsigned char)1<<w;
      w++;
      if (w==BITS)
        {
         int m=fwrite(&A,sizeof(unsigned char),1,_stream);
         if (m==1 && nb>=0) nb++; else nb=-1;
         w=0;
         A=0;
        }
     }
  }

int bitread(int n)
  {
   int a=0;
   for (int i=0;i<n;i++)
     {
      if (r==0)
        {
         int m=fread(&A,sizeof(unsigned char),1,_stream);
         if (m==1 && nb>=0) nb++; else nb=-1;
        }
      a|=((A&((unsigned char)1<<r))?(1<<i):0);
      r=(r+1)%BITS;
     }
   return a;
  }

int bitclose(void)
  {
   if (w>0)
     {
      int m=fwrite(&A,sizeof(unsigned char),1,_stream);
      if (m==1 && nb>=0) nb++; else nb=-1;
     }
   w=r=0;
   A=0;
   return nb;
  }
#endif
#endif
