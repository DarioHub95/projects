#ifndef __KERNEL_H__

void kernel(int N);

#ifndef __HEADERS__
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pvm.h>

void kernel(int N)
  {
   double alpha=getarg_d("alpha",0);
   double beta=getarg_d("beta",1000);
   double wc=getarg_d("wc",10);					// frequenza di cut-off
   double ss=getarg_d("ss",1);						// esponente fuzione spettrale (ss==1: caso ohmico)
   int M=1000;							// termini dello sviluppo in serie
   double norm=exp(gammln(1+ss)+(1-ss)*log(wc));
   double delta=beta/N;
   int N2=N/2;
   double *K=dvector(0,N2);
   double t0=walltime();
   for (int i=0;i<=N2;i++)
     {
      double tau=(beta*i)/N;
      double sum=0;
      for (int n=0;n<M;n++)
        {
         if (i>0 || n>0)
           {
            double l1=n*beta+tau;
            sum+=norm*pow(l1,-1-ss)*gammp(1+ss,l1*wc);
           }
         else sum+=0.5*wc*wc/(1+ss);
         double l2=(n+1)*beta-tau;
         sum+=norm*pow(l2,-1-ss)*gammp(1+ss,l2*wc);
        }
      K[i]=0.5*alpha*delta*delta*sum;
      if (cpuint(9))
        {
         double t1=walltime();
         printf("calcola kernel: i=%i/%i ETC: %g s\n",i,N2,(t1-t0)*(N2-i)/(1+i));
        }
     }
#if 0
   // output kernel
   FILE *stream=fopen("kernel.txt","w");
   for (int i=0;i<=N2;i++)
     {
      double tau=(beta*i)/N;
      double ytmp=(M_PI/beta)/sin(M_PI*tau/beta);				// va come tau per tau << beta
      double ktmp=0.5*alpha*ytmp*ytmp;
      fprintf(stream,"%12g %12g %12g %12g\n",tau,K[i],ktmp,K[i]/ktmp);
     }
   fclose(stream);
   exit(1);
#endif
   // calcola probabilità 
   for (int i=0;i<=N2;i++)
     {
      double ktmp=2*K[i];		// variazione dell'azione nel caso di flip degli spin
      double ptmp=1-exp(-ktmp);
      K[i]=(i==N2?0.5:1)*ptmp;		// una coppia di spin a distanza N2 viene presa due volte, quindi moltiplica ptmp per 0.5
     }
   luijten1(N2,K+1);
   free(K);
  }
#endif
#endif 
