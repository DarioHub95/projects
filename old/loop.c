#ifndef __LOOP_H__
#define __LOOP_H__
#include <stdlib.h>

// vedi Evertz, “The loop algorithm”, Adv. Phys. 52, 1 (2003), http://dx.doi.org/10.1080/0001873021000049195
// notazione di Evertz: numero di siti = N, numero di slice temporali = 2*L
// notazione qui: numero di siti = L, numero di slice temporali = N
// in Evertz \Delta\tau = \beta/L, mentre qui tau = 2*beta/N, quindi tau nostro e \Delta\tau di Evertz coincidono
// su ogni placchetta ombreggiata agisce il termine tau * H_i, dove Hi = ham1 * t * (c_i^+ c_{i+1} + c_{i+1}^+ c_i)
// nella nostra notazione ham1=1, mentre in Evertz Hi =  (Jx/2) * (c_i^+ c_{i+1} + c_{i+1}^+ c_i) quindi per far coincidere t e Jx bisogna mettere ham1=0.5

class loop
  {
   public:
   loop(int _L,int pbc,double Jz,double *h,double beta,double tau);
   void bond(int m1,int m2);		// mette un bond tra due spin
   void dinamica(double *A);

   int type;
   int flag;
   double beta;
   double t;
   double V;
   double ham1;
   double ham2;
   double ham3;
   int pbc;
   int L;			  // numero siti spaziali
   int N;			  // numero slice temporali
   int M;			  // numero totale siti
   int np;			// numero particelle
   double tau;
   double *hi;

   int *S;
   int *nhop;
   int htot;			// numero totale hopping
   double **pbreak;
   int *G;
   int nc;		// numero cluster
   int size;
   int *F;		// superspin
   double **W;		// interazioni tra superspin
   double *B;		// campo esterno sui superspin
   double memtot;
  };

#ifndef __HEADERS__
#include <stdio.h>
#include <math.h>
#include <pvm.h>
#define __HEADERS__

#if 0
void loop::check(void)
  {
   static int count;
   int ntot=0;
   int nk=0;
   int htmp=0;
   for (int j=0;j<N;j++)
     {
      int jp=(j<N-1?j+1:0);
      int i1=j%2;
      for (int i=i1;i<L;i+=2)
        {
         int ip=(i<L-1?i+1:0);
         int m1=i+j*L;
         int m2=i+jp*L;
         int m3=ip+jp*L;
         int m4=ip+j*L;
         int n1=S[m1]+S[m4];				// numero particelle sulla riga j
         int n2=S[m2]+S[m3];				// numero particelle sulla riga j+1
         if (n1!=n2) debug(9,"errore 102...\n");
         ntot+=n1;
         int stmp=S[m1]+2*S[m2]+4*S[m3]+8*S[m4];
         if (stmp==5 || stmp==10)
           {
            htmp++;						// numero placchette con hopping
            if (stmp==5) nk++; else nk--;			// hopping a destra meno hopping a sinistra
           }
        }
     }
   if (ntot%N || htmp!=htot) debug(9,"errore 103...\n");
   count++;
   printf("check %i OK: ntot=%i htot=%i nk=%i\n",count,ntot/N,htot,nk);
  }
#endif

loop::loop(int _L,int _pbc,double _Jz,double *_h,double _beta,double _tau)
  {
   L=_L;
   pbc=_pbc;
   V=_Jz;
   hi=_h;
   beta=_beta;
   tau=_tau;
   t=1;			// unità di energia
   ham1=0.5;
   ham2=0.25;
   ham3=0.5;
#if 0
   L=getarg_i("L",8);                   // numero siti
   tau=getarg_d("tau",0.2);
   beta=getarg_d("beta",1);
   t=getarg_d("Jxy",1);                   // termini di hopping
   V=getarg_d("Jz",0);			// interazione statica intracella (repulsiva per V>0)
   pbc=getarg_i("pbc",0);
#endif
   double hmax=0;
   for (int i=0;i<L;i++) if (fabs(hi[i])>hmax) hmax=fabs(hi[i]);
   flag=(V!=0 || hmax!=0?1:0);			// se flag>0, fai dinamica monte carlo sui loop
   // N.B: il numero di slice è doppio, perché i termini di hopping agiscono solo su metà delle placchette...
   N=4*(int)rint(0.5*beta/tau);
   if (L<=2 || N<4 || N%2) debug(9,"errore 101...\n");
   int Sz=getarg_i("Sz",0);
   if ((L+Sz)%2) debug(9,"errore 105...\n");
   np=(L+Sz)/2;							// numero di particelle
   // N.B: il tau definito qui corrisponde a \Delta\tau di Evertz
   // l'operatore da considerare sulla placchetta ombreggiata è quindi exp(-tau*H) dove H è il termine di hopping dell'hamiltoniana corrispondente alla coppia di siti
   // per ogni coppia di siti primi vicini ci sono infatti N/2 placchette ombreggiate
   // nella notazione di Evertz H = (Jx/2) * ( S^+_i S^-_{i+1} + S^+_{i+1} S^-_i )
   // nella nostra notazione al posto di (Jx/2) abbiamo ham1*t per cui se ham1==0.5 si ha che t corrisponde a Jx di Evertz
   tau=2*beta/N;
   M=L*N;											// numero totale spin
   // altro workspace
   S=ivector(0,M-1);
   nhop=ivector(0,L-1);
   pbreak=dmatrix(0,15,0,2);
   G=ivector(0,M-1);
   F=ivector(0,M-1);
   B=dvector(0,M-1);
   memtot=30.*M;
   W=NULL;
   size=0;
   // inizializzazione
   for (int i=0;i<L;i++) for (int j=0;j<N;j++) S[i+j*L]=(i%2?1:0);		// stato di neel
   htot=0;									// numero totale hopping
  }

// le placchette piene sono quelle in cui agisce un termine di hopping dell'hamiltoniana
// le placchette piene hanno angolo in basso a sinistra (i,j) con i+j dispari, mentre le placchette vuote con i+j pari

void loop::bond(int m1,int m2)
  {
   int l1=m1;
   int l2=m2;
   while (l1!=G[l1]) l1=G[l1];
   while (l2!=G[l2]) l2=G[l2];
   if (l1!=l2)
     {
      int lmin=(l1<l2?l1:l2);
      G[m1]=G[m2]=G[l1]=G[l2]=lmin;
     }
  }

void loop::dinamica(double *A)
  {
   // CLUSTERIZZAZIONE
   for (int m=0;m<M;m++) G[m]=m;
   for (int i=0;i<L;i++)
     {
      int ip=(i<L-1?i+1:0);
      double thop=0;
      if (pbc || i<L-1) thop=ham1*t;
      // N.B: leggendo Evertz considerare che Jx corrisponde a 2*thop, mentre \Delta\tau corrisponde esattamente a tau
      // mentre se usiamo hamiltoniana elettronica e poniamo ham1=1 abbiamo che thop è uguale al valore di t
      // il peso di una placchetta ombreggiata è uguale a
      // < 0 1 | exp(-tau*H) | 1 0 > = < 1 0 | exp(-tau*H) | 0 1 > = sinh(tau*thop) (hopping)
      // < 0 1 | exp(-tau*H) | 0 1 > = < 1 0 | exp(-tau*H) | 1 0 > = cosh(tau*thop) 
      // < 0 0 | exp(-tau*H) | 0 0 > = < 1 1 | exp(-tau*H) | 1 1 > = 1
      double ztmp=exp(tau*thop);
      double ytmp=exp(-tau*thop);
      pbreak[0][0]=pbreak[15][0]=0.5*(1+ytmp);
      pbreak[0][2]=pbreak[15][2]=0.5*(1-ytmp);
      pbreak[3][0]=pbreak[12][0]=(1+ytmp)/(ztmp+ytmp);
      pbreak[3][1]=pbreak[12][1]=(ztmp-1)/(ztmp+ytmp);
      pbreak[10][1]=pbreak[5][1]=(ztmp-1)/(ztmp-ytmp);		// hopping a destra (5) o a sinistra (10)
      pbreak[10][2]=pbreak[5][2]=(1-ytmp)/(ztmp-ytmp);
      for (int j=i%2;j<N;j+=2)
        {
         int jp=(j<N-1?j+1:0);
         int m1=i+j*L;				// SOUTH-WEST
         int m2=i+jp*L;				// NORTH-WEST
         int m3=ip+jp*L;			// NORTH-EAST
         int m4=ip+j*L;				// SOUTH-EAST
         int stmp=S[m1]+2*S[m2]+4*S[m3]+8*S[m4];		// configurazione di spin della placchetta, da 0 a 15
         double rtmp=Xrandom();
         int k=0;
         while (k<3 && rtmp>=pbreak[stmp][k]) {rtmp-=pbreak[stmp][k]; k++;}
         if (k==0)
           {
            bond(m1,m2);
            bond(m3,m4);
           }
         else if (k==1)
           {
            bond(m1,m4);
            bond(m2,m3);
           }
         else if (k==2)
           {
            bond(m1,m3);
            bond(m2,m4);
           }
         else debug(9,"errore 104...\n");
        }
     }
   // DEFINIZIONE SUPERSPIN
   nc=0;
   for (int m=0;m<M;m++) if (m!=G[m])
     {
      int l=m;
      while (l!=G[l]) l=G[l];
      G[m]=l;
     }
   else F[m]=nc++;
   for (int m=0;m<M;m++) G[m]=F[G[m]];
   if (flag)						// flag!=0 in caso di interazioni statiche
     {
      if (size<nc)
        {
         memtot+=8*((nc+nc*(double)nc)-(size+size*(double)size));
         //printf("memoria totale: %g Gb (nc=%i)\n",1e-9*memtot,nc);
         W=(double**)getmem(W,nc*sizeof(double*));
         W[0]=(double*)getmem(W[0],nc*nc*sizeof(double));
         for (int i=1;i<nc;i++) W[i]=W[i-1]+nc;
         size=nc;
        }
      // azzera interazioni tra loop
      for (int p=0;p<nc;p++)
        {
         for (int q=0;q<nc;q++) W[p][q]=0;
         B[p]=0;
        }
      // calcola interazioni tra loop
      for (int j=0;j<N;j++)
        {
         for (int i=0;i<L;i++)
           {
            int m=i+j*L;
            int s1=(S[m]?1:-1);		// magnetizzazione dello spin 
            int p=G[m];			// loop (cluster a cui appartiene lo spin)
            // N.B: 1/2 perché tau=2*beta/N (mentre qui consideriamo N slice temporali, non N/2)
            // B[p] deve essere positivo se la magnetizzazione è nella stessa direzione del campo magnetico (vedi sotto algoritmo di metropolis)
            B[p]+=ham3*s1*tau*hi[i]/2;
            if (pbc || i<L-1)
              {
               int ip=(i<L-1?i+1:0);
               int m2=ip+j*L;
               int s2=(S[m2]?1:-1);
               int q=G[m2];
               // N.B: 1/2 perché tau=2*beta/N (mentre qui consideriamo N slice temporali, non N/2)
               // -1 perché è antiferro (repulsiva) per V>0
               double wtmp=-ham2*s1*s2*tau*V/2;
               // se l'interazione è repulsiva (positiva) il contributo a W[p][q] sarà positivo nel caso di spin antiparalleli (o coppia di siti occupati da una particella)
               // quindi W[p][q] è positivo se gli spin soddisfano l'interazione
               W[p][q]+=wtmp;
               W[q][p]+=wtmp;
              }
           }
        }
      // DINAMICA METROPOLIS
      for (int p=0;p<nc;p++) F[p]=1;                       // valore iniziale dei super-spin
      int step1=nc;
      for (int k=0;k<step1;k++)
        {
         int p=(int)floor(nc*Xrandom());
         // etmp è la variazione di energia dovuta al flip di F[p] che cambia segno
         // se F[p]==1, ovvero il valore iniziale, l'energia aumenta se B[p]>0
         // questo vuol dire che B[p] deve essere positivo se la magnetizzazione del loop (cluster)
         // nello stato iniziale F[p]==1 è nella stessa direzione del campo magnetico
         double etmp=2*B[p]*F[p];
         for (int q=0;q<nc;q++) if (q!=p)
           {
            // variazione di energia delle interazioni statiche
            // se F[p]==F[q]==1, ovvero i valori iniziali, allora l'energia aumenta se W[p][q]>0
            // questo vuol dire che W[p][q] deve essere positivo se nello stato iniziale F[p]==F[q]==1 gli spin soddisfacevano l'interazione
            etmp+=2*W[p][q]*F[q]*F[p];
           }
         if (etmp<=0 || Xrandom()<exp(-etmp)) F[p]=-F[p];
        }
     }
   else for (int p=0;p<nc;p++) F[p]=(Xrandom()<0.5?1:-1);
   // FLIP DEI LOOP (SUPER-SPIN)
   for (int m=0;m<M;m++) S[m]=(F[G[m]]<0?1-S[m]:S[m]);
   // calcola densità di particelle e di hopping 
   htot=0;					// numero totale hopping
   for (int i=0;i<L;i++)
     {
      nhop[i]=0;
      int ip=(i<L-1?i+1:0);
      for (int j=i%2;j<N;j+=2)
        {
         int jp=(j<N-1?j+1:0);
         int m1=i+j*L;          // SOUTH-WEST
         int m2=i+jp*L;       // NORTH-WEST
         int m3=ip+jp*L;       // NORTH-EAST
         int m4=ip+j*L;       // SOUTH-EAST
         int stmp=S[m1]+2*S[m2]+4*S[m3]+8*S[m4];           // configurazione di spin della placchetta, da 0 a 15
         if (stmp==10 || stmp==5) nhop[i]++;			// N.B: stmp==5 è un hopping a destra, stmp==10 un hopping a sinistra
        }
      htot+=nhop[i];
     }
   // MEDIA OSSERVABILI
   A[0]++;
   A[30]+=nc;
   int ntot=0;
   for (int m=0;m<M;m++) ntot+=S[m];
   if (ntot==N*np)
     {
      // settore a numero di particelle np su cui fare le medie statistiche
      A[1]++;
      // numero hopping
      A[3]-=htot/beta;
      // interazioni
      double utmp=0;
      double htmp=0;
      for (int i=0;i<L;i++) for (int j=0;j<N;j++)
        {
         int m=i+j*L;
         int s1=(S[m]?1:-1);
         htmp-=ham3*hi[i]*s1;
         if (pbc || i<L-1)
           {
            int ip=(i<L-1?i+1:0);
            int m2=ip+j*L;
            int s2=(S[m2]?1:-1);
            utmp+=ham2*V*s1*s2;
           }
        }
      A[10]+=utmp/N;
      A[12]+=htmp/N;
     }
  }
#endif
#endif
