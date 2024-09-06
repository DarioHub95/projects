#ifndef __RANDOM_H__
#define __RANDOM_H__
#include <stdlib.h>

void init_random(unsigned long seed=0,unsigned int seed2=0);
unsigned long poprandom(void);
void pushrandom(unsigned long S);
double Xrandom(void);
double Grandom(void);

#ifndef __HEADERS__
#include <stdio.h>
#include <time.h>
#include <math.h>

/*

questo RNG è preso dalle routine g05caf, g05cbf delle NAG
ha un periodo di 2^57 (vedi https://www.afs.enea.it/software/libnag/nagdoc_fl21/pdf/G05/g05caf.pdf)
è basato sulle operazioni

seed = (seed * 13^13) modulo 2^60

X = 2^(-55) + 2^(-53) * (seed/2^7)

per cui 0 < X < 1

*/

#define M0   455329277UL                                // 13^13=302875106592253 
#define M1   282074UL

#define PW2A  1073741823UL                               // 2^(30)-1
#define PW2B 1152921504606846975UL                      // 2^(60)-1

#define rpw2b  1.11022302462515654e-16                  // 2^(-53)
#define rpw2c  2.77555756156289135e-17                  // 2^(-55)

static unsigned long S=1060485742695258666UL;			// 246913578*(2^32+1)

unsigned long poprandom(void)
  {
   return S;
  }

void pushrandom(unsigned long _S)
  {
   S=_S;
  }

double Xrandom(void)
  {
   unsigned long B0=S&PW2A;
   unsigned long B1=(S>>30)&PW2A;
   S=(B0*M0+((B0*M1+B1*M0)<<30))&PW2B;
   return rpw2c+rpw2b*(S>>7);
  }

double Grandom(void)
  {
   static int flip;
   static double rho,phi;
   if (flip==0)
     {
      flip=1;
      rho=sqrt(-2*log(Xrandom()));
      phi=2*M_PI*Xrandom();
      return rho*cos(phi);
     }
   else				// se flip==1 usa rho e phi già calcolati
     {
      flip=0;
      return rho*sin(phi);
     }
  }

// seed2 viene XOR-ed con i bit dal 29-mo in su di seed
// se seed=time(), e seed2 è il rango del processo, questo dovrebbe garantire serie random diverse per diversi processi
// (a meno che i processi non partano a 2^28 secondi = 8.5 anni di distanza)

#define bmax 32		// se seed è maggiore di 2^bmax (e seed2==0) prendi seed come valore di S
#define SKIP 10

void init_random(unsigned long seed,unsigned int seed2)
  {
   if ((seed>>bmax)==0)
     {
      if (seed==0) seed=(int)time(NULL);
      unsigned long B0=(2+(seed<<2))&PW2A;
      unsigned long B1=((seed>>28)^seed2)&PW2A;
      S=B0+(B1<<30);
      // chiama Xrandom() skip volte
      for (int i=0;i<SKIP;i++) Xrandom();
     }
   else
     {
      if (seed2!=0)
        {
         fprintf(stderr,"init_random: seed>=(2^%i) && seed2>0\n",bmax);
         exit(1);
        }
      S=seed;
     }
  }
#endif
#endif
