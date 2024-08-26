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
   loop(void);
   void bond(int m1,int m2);		// mette un bond tra due spin
   void dinamica(double *A);
   void check(void);

   double beta;
   double t;
   double V;
   double eps;
   double ham1;
   double ham2;
   double ham3;
   double hmax;
   int pbc;
   int L;			// numero siti spaziali
   int N;			// numero slice temporali
   int M;			// numero totale siti
   int np;			// numero particelle
   int type;    // tipo di campo magnetico: 0 uniforme, -1 antiferro, >0 random
   int tw;

   double tau;
   double *hi;

   int *S;
   int *nhop;
   int htot;			// numero totale hopping
   double **pbreak;
   int *G;
   int nc;		// numero cluster
   int *F;		// superspin
   double *B;
   int *stack;
   unsigned long seed;
  };

#ifndef __HEADERS__
#include <stdio.h>
#include <math.h>
#include <pvm.h>
#define __HEADERS__
#include "kernel.c"

void loop::check(void)
  {
   #if 0
   static int count;
   int ntot=0;
   int nk=0;
   int htmp=0;
   for (int j=0;j<N;j++)
     {
      //int nl=0;
      //for (int i=0;i<L;i++) nl+=S[i+j*L];
      //debug(0,"tempo %i: n=%i\n",j,nl);
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
   if (ntot%N || htmp!=htot) debug(9,"errore 103... ntot=%i N=%i\n",ntot,N);
   count++;
   printf("check %i OK: ntot=%i htot=%i nk=%i\n",count,ntot/N,htot,nk);
   #endif
  }

loop::loop(void)
  {
   L=getarg_i("L",8);
   tw=getarg_i("tw",1);                                
   tau=getarg_d("tau",0.2);
   beta=getarg_d("beta",1);
   t=getarg_d("Jxy",1);                   // termini di hopping
   V=getarg_d("Jz",0);                  // interazione statica intracella (repulsiva per V>0)
   pbc=getarg_i("pbc",0);
   eps=getarg_d("eps",0);
   type=getarg_i("type",-1,"0 uniforme, -1 antiferro, >0 random");
   hi=dvector(0,L-1);

   // definizione del campo magnetico secondo il type + salvataggio stato generatore di numeri casuali
   if (type>0)
     {
      unsigned long S1=poprandom();
      // unsigned long seed=(unsigned long)time(NULL);
      // unsigned int mype=pvm_mype();
      init_random(seed);
      // init_random(type);
      for (int i=0;i<L;i++) hi[i]=eps*(2*Xrandom()-1);
      pushrandom(S1);
     }
   else if (type==0) for (int i=0;i<L;i++) hi[i]=eps;
   else if (type<0) for (int i=0;i<L;i++) hi[i]=eps*(i%2?1:-1);

   // dichiarazione delle costanti nell'Hamiltoniana //MODIFICATO
   ham1=0.5;
   ham2=-1;
   ham3=1;
   hmax=0;
   htot=0;

   for (int i=0;i<L;i++) if (fabs(hi[i])>hmax) hmax=fabs(hi[i]);

   N=4*(int)rint(0.5*beta/tau);
   // N.B: il numero di slice è doppio, perché i termini di hopping agiscono solo su metà delle placchette...
   
   if (L<=2 || N<4 || N%2) debug(9,"errore 101...\n");

   int Sz=getarg_i("Sz",0);
   // N.B: Sz dovrebbe essere uguale a -L, -L+2, ... L-2, L
   // se invece è ad esempio -L+1, prendiamo il valore inferiore, ovvero -L

   np=(L+Sz)/2;
   // numero di particelle (half filling per Sz=0) N.B: se L è dispari il numero di particelle è (L-1)/2

   tau=2*beta/N;
   // N.B: il tau definito qui corrisponde a \Delta\tau di Evertz
   // l'operatore da considerare sulla placchetta ombreggiata è quindi exp(-tau*H) dove H è il termine di hopping dell'hamiltoniana corrispondente alla coppia di siti
   // per ogni coppia di siti primi vicini ci sono infatti N/2 placchette ombreggiate
   // nella notazione di Evertz H = (Jx/2) * ( S^+_i S^-_{i+1} + S^+_{i+1} S^-_i )
   // nella nostra notazione al posto di (Jx/2) abbiamo ham1*t per cui se ham1==0.5 si ha che t corrisponde a Jx di Evertz

   M=L*N;											
   // numero totale spin

   S=ivector(0,M-1);       // contiene tutti gli spin, vederlo come una configurazione del reticolo
   nhop=ivector(0,L-1);
   pbreak=dmatrix(0,15,0,2);
   G=ivector(0,M-1);
   F=ivector(0,M-1);
   B=dvector(0,M-1);
   stack=ivector(0,N-1);

   // kernel del bagno termico
   kernel(N);

   // inizializzazione della configurazione iniziale degli LxN spin (i=indice spaziale, j=indice temporale)
   int ntot=0;
   for (int i=0;i<L;i++)
     {
      int s1=(i%2?1:0);				// stato di neel
      for (int j=0;j<N;j++) S[i+j*L]=s1;
      ntot+=s1;
     }

   if (ntot!=np) debug(9,"numero di particelle iniziale errato...\n");
  }

 // le placchette piene sono quelle in cui agisce un termine di hopping dell'hamiltoniana
 // le placchette piene hanno angolo in basso a sinistra (i,j) con i+j dispari, mentre le placchette vuote con i+j pari

void loop::bond(int m1,int m2)    // associa 2 spin qualsiasi allo stesso cluster
  {
   int l1=m1;
   int l2=m2;
   while (l1!=G[l1]) l1=G[l1];
   while (l2!=G[l2]) l2=G[l2];
   int lmax=(l1<l2?l2:l1);
   G[m1]=G[m2]=G[l1]=G[l2]=lmax;
  }

void loop::dinamica(double *A)
  {

// CONFIGURAZIONE CORRENTE: di seguito viene stampata la configurazione del reticolo

    // int s=8;
    // printf("Configurazione %i di spin:\n",(int)A[0]);
    // if(s!=L) debug(9,"Attenzione: la funzione printf() è predisposta solo per %i siti\n",s);
    // else
    // {
    //   printf("\n");
    // for(int j=0;j<N;j++) printf("%4i%4i%4i%4i%4i%4i%4i%4i\n",S[0+j*L],S[1+j*L],S[2+j*L],S[3+j*L],S[4+j*L],S[5+j*L],S[6+j*L],S[7+j*L]);
    // // NOTA: ad ogni slice temporale per j=1,...,N si ripete la configurazione della catena
    //   printf("\n");
    // }

// FUNZIONE DI CHECK
   //check();

// CLUSTERIZZAZIONE: ad ogni spin associamo uno ed un solo tag in [0,M-1]
   // ovvero nello stato iniziale ad ogni spin è associato un cluster
   for (int m=0;m<M;m++) G[m]=m;
  
   if (pbc && L%2) debug(9,"condizioni periodiche incompatibili con L dispari...\n");
   int imax=(pbc?L-1:L-2);

// TERMINE DI HOPPING dell'hamiltoniana. N.B: stiamo considerando t uguale per tutte le coppie (i,i+1)
    double thop=ham1*t;		
    // N.B: leggendo Evertz considerare che Jx corrisponde a 2*thop,
    // mentre se usiamo hamiltoniana elettronica e poniamo ham1=1 abbiamo che thop è uguale al valore di t
    // il peso di una placchetta ombreggiata è uguale a
    // < 0 1 | exp(-tau*H) | 1 0 > = < 1 0 | exp(-tau*H) | 0 1 > = sinh(tau*thop) (hopping)
    // < 0 1 | exp(-tau*H) | 0 1 > = < 1 0 | exp(-tau*H) | 1 0 > = cosh(tau*thop) 
    // < 0 0 | exp(-tau*H) | 0 0 > = < 1 1 | exp(-tau*H) | 1 1 > = 1

    double ztmp=exp(tau*thop);
    double ytmp=exp(-tau*thop);
    // pesi di tutte le possibili configurazioni di una placchetta ombreggiata
    pbreak[0][0]=pbreak[15][0]=0.5*(1+ytmp);
    pbreak[0][2]=pbreak[15][2]=0.5*(1-ytmp);
    pbreak[3][0]=pbreak[12][0]=(1+ytmp)/(ztmp+ytmp);
    pbreak[3][1]=pbreak[12][1]=(ztmp-1)/(ztmp+ytmp);
    pbreak[10][1]=pbreak[5][1]=(ztmp-1)/(ztmp-ytmp);		
    pbreak[10][2]=pbreak[5][2]=(1-ytmp)/(ztmp-ytmp);
    //pbreak[0][]: no hopping, tutti spin down nella placchetta
    //pbreak[3][]: no hopping, lo spin (i,j) evolve in (i,j+1)
    //pbreak[5][]: hopping verso destra, lo spin salta da (i,j)--->(i+1,j+1)
    //pbreak[10][]: hopping verso sinistra, lo spin salta da (i+1,j)--->(i,j+1)
    //pbreak[12][]: no hopping, lo spin (i+1,j) evolve in (i+1,j+1)
    //pbreak[15][]: no hopping, gli spin evolvono da j--->j+1

   for (int i=0;i<=imax;i++)
     {
      int ip=(i<L-1?i+1:0);
      // consideriamo adesso il termine di hopping da i a i+1
      // agisce dal tempo j a j+1 dove j ha la stessa parità di i
      for (int j=i%2;j<N;j+=2)
        {
         int jp=(j<N-1?j+1:0);
         int m1=i+j*L;				// SOUTH-WEST
         int m2=i+jp*L;				// NORTH-WEST
         int m3=ip+jp*L;			// NORTH-EAST
         int m4=ip+j*L;				// SOUTH-EAST
         int stmp=S[m1]+2*S[m2]+4*S[m3]+8*S[m4];	
         // stmp indica nella matrice pbreak quale peso dare alla configurazione di spin della placchetta, 
         // valore compreso da 0 a 15
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
   if (pbc==0)
     {
      // step temporali non appartenenti a nessuna placchetta
      // il sito i==0 non appartiene a nessuna placchetta per (j,j+1) con j dispari
      for (int j=1;j<N;j+=2)
        {
         int m1=j*L;
         int m2=(j<N-1?j+1:0)*L;
         bond(m1,m2);			// trasporta lo stato da j a j+1
        }
      // il sito i==L-1 non appartiene a nessuna placchetta per (j,j+1) con j avente la stessa parità di L-1
      for (int j=(L-1)%2;j<N;j+=2)
        {
         int m1=(L-1)+j*L;
         int m2=(L-1)+(j<N-1?j+1:0)*L;
         bond(m1,m2);                   // trasporta lo stato da j a j+1
        }
     }

// INTERAZIONI STATICHE TRA SPIN
   if (fabs(V)>0)
     {
      // N.B: l'interazione tra due spin vicini è ham2*s1*s2*tau*V/2, dove il fattore 1/2 tiene conto del fatto che abbiamo N slice temporali, ma tau=2*beta/N
      // la probabilità di mettere il bond è 1-exp(deltaS), dove deltaS=-2*(ham2*tau*fabs(V)/2) è la diminuzione dell'azione
      double pbond=1-exp(-ham2*tau*fabs(V));
      for (int j=0;j<N;j++)
        {
         for (int i=0;i<L;i++)
           {
            int m=i+j*L;
            int s1=(S[m]?1:-1);         // magnetizzazione dello spin 
            if (pbc || i<L-1)
              {
               int ip=(i<L-1?i+1:0);
               int m2=ip+j*L;
               int s2=(S[m2]?1:-1);
               if (s1*s2*V<0 && Xrandom()<pbond) bond(m,m2);		// se s1*s2*V<0 gli spin soddisfano l'interazione (V>0 per interazione antiferro)
              }
           }
        }
     }

// DISSIPAZIONE sull'ultimo sito della catena //MODIFICATO
   int nb=0;
   for (int i=0;i<L;i++) if (i==L-1)		
     {
      // dissipazione sul sito L-mo: lungo la worldline del sito centrale 
      // vengono estratti n siti in modo random salvati in stack[]
      // e per ognuno viene fatto il bond con il sito centrale
      for (int j=0;j<N;j++)
        {
         int m1=i+j*L;
         int n=luijten2(N,stack); 
         for (int k=0;k<n;k++)
           {
            int j2=(j+1+stack[k])%N;		// N.B: stack[k] è la distanza meno uno
            int m2=i+j2*L;
            bond(m1,m2);
            nb++;
           }
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
   for (int m=0;m<M;m++) G[m]=F[G[m]];	// metti G[m] uguale al numero del cluster che va da 0 a nc-1...

// CALCOLA VARIAZIONE del numero di particelle dovuto al flip dei loop
   #if 0
   for (int p=0;p<nc;p++)
     {
      int ns=0;
      int n1=0;
      for (int m=0;m<M;m++) if (G[m]==p)
        {
         ns++;
         n1+=S[m];
        }
      int ndel=ns-2*n1;
      if (ndel%N)
        {
         for (int j=0;j<N;j++)
           {
            for (int i=0;i<L;i++)
              {
               int m=i+j*L;
               if (G[m]==p)
                 {
                  if (S[m]>0) printf("1"); else printf("0");
                 }
               else printf(" ");
              }
            printf("\n");
           }
         for (int m=0;m<M;m++) if (G[m]==p) printf("j=%i i=%i\n",m/L,m%L);
         debug(9,"errore 201...\n");
        }
     }
    #endif

// INTERAZIONE del superspin con il campo magnetico
   for (int p=0;p<M;p++) B[p]=0;
   for (int m=0;m<M;m++)
     {
      int i=m%L;
      int s1=(S[m]?1:-1);
      double btmp=ham3*s1*tau*hi[i]/2;   // N.B: 1/2 perché tau=2*beta/N (mentre qui consideriamo N slice temporali, non N/2)
      B[G[m]]+=btmp;
     }

// FLIP DEI LOOP (SUPER-SPIN)
   for (int p=0;p<nc;p++)
     {
      // abbiamo un termine B[p]*S nell'azione, dove S=1, per cui la variazione dell'azione è -2*B[p] se lo spin diventa -1
      double ptmp=1/(1+exp(2*B[p]));		// probabilità di flippare
      F[p]=(Xrandom()<ptmp?-1:1);
     }
   for (int m=0;m<M;m++) S[m]=(F[G[m]]<0?1-S[m]:S[m]);

// CALCOLO DENSITA' di particelle e di hopping

   htot=0;					// numero totale hopping
   for (int i=0;i<L;i++)
     {
      nhop[i]=0;
      int ip=(i<L-1?i+1:0);
      for (int j=i%2;j<N;j+=2)
        {
         int jp=(j<N-1?j+1:0);
         int m1=i+j*L;         // SOUTH-WEST
         int m2=i+jp*L;        // NORTH-WEST
         int m3=ip+jp*L;       // NORTH-EAST
         int m4=ip+j*L;        // SOUTH-EAST
         int stmp=S[m1]+2*S[m2]+4*S[m3]+8*S[m4];        // configurazione di spin della placchetta, da 0 a 15
         if (stmp==10 || stmp==5) nhop[i]++;			     // N.B: stmp==5 è un hopping a destra, stmp==10 un hopping a sinistra
        }
      htot+=nhop[i];
      if (i==L-2) A[22]=(double)nhop[i];

     }
     
// MEDIA OSSERVABILI

   A[0]++;                     // indice di step della simulazione
   A[28]=nb/(double)N;         // numero di bond del bagno termico allo step MC corrente
   A[29]+=nb/(double)N;        // media temporale del numero di bond del bagno termico
   A[30]+=nc;

   // settore che seleziona le config a magnetizzazione fissata (o a numero di particelle np)
   // su cui fare le medie statistiche
   int ntot=0;
   for (int m=0;m<M;m++) ntot+=S[m];
   if (ntot==N*np)        
     {  
      A[1]++;               // numero effettivo di config a np fissato
      A[3]-=htot/beta;      // numero hopping/beta

      // interazioni
      double utmp=0;
      double htmp=0;
      for (int i=0;i<L;i++)
      {
        int ssum=0;
        for (int j=0;j<N;j++)
        {
         int m=i+j*L;
         int s1=(S[m]?1:-1);
         htmp-=ham3*hi[i]*s1;
         ssum+=s1;        // somma tutti gli spin immaginari

         // Parte di codice per autocorrelazione1 spin-spin
         A[32+m]=s1; // la configurazione di spin corrente

         if (pbc || i<L-1)
           {
            int ip=(i<L-1?i+1:0);
            int m2=ip+j*L;
            int s2=(S[m2]?1:-1);
            utmp+=ham2*V*s1*s2;
           }
        }

        // Parte di codice per autocorrelazione2 spin-spin
          A[13+i]=(double)ssum/N; // media sugli spin immaginari


        // Parte di codice per componenti ultimo spin
        if(i==L-1)
        {
          A[21]=0;          // Sx
          // A[22]=(double)sy;     // Sy
          A[23]=(double)ssum/N;     // Sz
        } 


      }

      A[10]+=utmp/N;       // media su tempo immag di interazione spin-spin
      A[12]+=htmp/N;       // media su tempo immag di interazione con B
      A[2]=A[3]+A[10]+A[12];           // media temporale Energia interna


     }

  }


#endif
#endif
