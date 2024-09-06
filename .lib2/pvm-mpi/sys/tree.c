#ifndef __TREE_H__
#define __TREE_H__

int maketree(int k,int N,int **next);

#ifndef __HEADERS__
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pvm.h>

#define SWAP(i,j) {wtmp=W[i]; W[i]=W[j]; W[j]=wtmp;}

static void check(int k,int N,int **next)
  {
   for (int i=0;i<N;i++)
     {
      for (int j=0;j<=k;j++)
        {
         int i2=next[j][i];
         if (i2<0 || i2>=N) debug(9,"tree: errore nella creazione del reticolo...\n");
         int l=0;
         for (;l<=k;l++) if (next[l][i2]==i) break;
         if (l>k) debug(9,"tree: errore nella creazione del reticolo...\n");
         if (i2==i) debug(9,"tree: self loop...\n");
         for (l=0;l<j;l++) if (next[l][i]==i2) debug(9,"tree: edge multiplo...\n");
        }
     }
  }

int maketree(int k,int N,int **next)
  {
   int swap=10;					// quanti swap per ogni link
   int zmax=(1+k)*N;
   int *z=ivector(0,N-1);
   int *W=ivector(0,zmax-1);
   while (1)
     {
      zmax=(1+k)*N;
      int u=0;
      for (int j=0;j<N;j++)
        {
         for (int i=0;i<=k;i++) W[u++]=j;
         z[j]=0;
        }
      while (zmax>0)
        {
         int wtmp;
         u=(int)floor(zmax*Xrandom());
         int j=W[u];
         zmax--;
         SWAP(u,zmax);
         // cerca nell'insieme W[0]...W[zmax-1] un terminale che vada bene
         int zmin=zmax;
         while (zmin>0)
           {
            int u2=(int)floor(zmin*Xrandom());
            int j2=W[u2];
            int l;
            for (l=0;l<z[j];l++) if (next[l][j]==j2) break;
            if (j!=j2 && l==z[j])
              {
               next[z[j]++][j]=j2;
               next[z[j2]++][j2]=j;
               zmax--;
               SWAP(u2,zmax);
               break;
              }
            zmin--;
            SWAP(u2,zmin);
           }
         //if (cpuint()) printf("creazione reticolo: edge da inserire=%i/%i (%g)\n",zmax,(1+k)*N,walltime());
         // se zmin==0 vuol dire che non c'è un terminale che vada bene per il terminale u-mo
         if (zmin==0 || cputerm()) break;
        }
      // se zmax==0 vuol dire che siamo riusciti a creare il reticolo
      // se zmax>0 e c'è ancora tempo di cpu, ricomincia tutto daccapo
      if (zmax==0 || cputerm()) break;
     }
   free(W);
   free(z);
   if (zmax==0)
     {
      long long nmax=(long long)(swap*(1+k)*0.5*N);
      //printf("eseguo %Li edge swap...\n",nmax);
      while (nmax>0)
        {
         int i=(int)floor(N*Xrandom());
         int i2=(int)floor(N*Xrandom());
         if (i==i2) continue;
         int l=(int)floor((1+k)*Xrandom());
         int j=next[l][i];
         int l2=(int)floor((1+k)*Xrandom());
         int j2=next[l2][i2];
         if (j==i2 || j2==i) continue;
         int u=0;
         for (;u<=k;u++) if (next[u][i]==j2) break;
         if (u<=k) continue;
         u=0;
         for (;u<=k;u++) if (next[u][i2]==j) break;
         if (u<=k) continue;
         int y=0;
         for (;y<=k;y++) if (next[y][j]==i) break;
         int y2=0;
         for (;y2<=k;y2++) if (next[y2][j2]==i2) break;
         next[l][i]=j2;
         next[l2][i2]=j;
         next[y][j]=i2;
         next[y2][j2]=i;
         nmax--;
         //if (cpuint()) printf("edge swap da eseguire: nmax=%Li (%g)\n",nmax,walltime());
        }
     }
   check(k,N,next);
   return (zmax>0);			// restituisce 0 se ha eseguito tutte le operazioni, 1 se il tempo cpu è terminato
  }
#endif
#endif
