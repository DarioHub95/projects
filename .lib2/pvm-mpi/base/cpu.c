#ifndef __CPU_H__
#define __CPU_H__
#include <stdlib.h>

void cpustart(double cpu=0.,double save=0.);
double cputime(double *stime=NULL);
double walltime(void);
int cpuint(int level=0);
int cputerm(void);
int cpunext(int skip=0);
void gprof(int level=-1);

#ifndef __HEADERS__
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static double cpu;		// massimo tempo cpu disponibile per il processo
static double save;

static int init_cpu;
static double start;		// epoca a cui è iniziato il job
static int flag;		// flag=1 se il tempo CPU è terminato
static double ctrc;	// indica il tempo dell'ultimo SIGINT o CTRL-C
static double last;		// ultimo tempo in cui cpuint() ha restituito TRUE
static struct timeval tv;

static double tmin=10;		// tempo next iniziale		
static double tmax=10000;	// tempo next massimo
static double tdelta;		// intervallo next
static double tnext;		// tempo next successivo

#define lmax 16

static int ftmp[lmax];
static int ltmp;
static double clast;
static double csum[lmax];		// tempo di CPU utilizzato dal livello level

#define TBIG 1e30

static void handler(int signum)
  {
   if (signum==SIGTERM) flag=1;
   else if (signum==SIGINT)
     {
      double now=walltime();
      if (ctrc<TBIG && now<ctrc+1)
        {
         ctrc=TBIG;
         flag=1;	// secondo ctrl-C a meno di un secondo dal primo... restituisci TRUE quando sarà chiamato cputerm()
        }
      else ctrc=now;		// primo ctrl-C
     }
   signal(signum,handler);
  }

// calcola tempo di cpu utilizzato dal processo
double cputime(double *_stime)
  {
   struct rusage usage;
   if (getrusage(RUSAGE_SELF,&usage))
     {
      fprintf(stderr,"cputime: errore in getrusage\n");
      exit(1);
     }
   double utime=usage.ru_utime.tv_sec+1e-6*usage.ru_utime.tv_usec;		// user CPU time
   double stime=usage.ru_stime.tv_sec+1e-6*usage.ru_stime.tv_usec;		// system CPU time
   if (_stime) *_stime=stime;
   return utime+stime;
  }

void gprof(int level)
  {
   if (level>=0 && level<lmax)
     {
      double ctmp=cputime();
      csum[ltmp]+=ctmp-clast;
      clast=ctmp;
      ltmp=level;
     }
   else
     {
      double ctot=cputime();				// cputime calcola il tempo effettivamente utilizzato dal processo... non il tempo trascorso
      FILE *stream=fopen("gprof.log","w");
      fprintf(stream,"CPU=%12g\n",ctot);
      for (int l=0;l<lmax;l++) if (csum[l]>0) fprintf(stream," %3i: %12g (%6.3f%%)\n",l,csum[l],100*csum[l]/ctot);
      fclose(stream);
     }
  }

double walltime(void)
  {
   if (init_cpu==0) cpustart(0.,0.);
   // N.B: sarebbe meglio usare clock_gettime con CLOCK_MONOTONIC oppure CLOCK_MONOTONIC_RAW...
   // che funziona anche se l'amministratore cambia la data mentre il run è in esecuzione...
   gettimeofday(&tv,NULL);
   return (tv.tv_sec+1e-6*tv.tv_usec)-start;
  }

void cpustart(double _cpu,double _save)
  {
   init_cpu++;			// quante volte è stato chiamato cpu_start
   if (_cpu>0) cpu=_cpu;
   if (_save>0) save=_save;
   if (init_cpu==1) start=walltime();
   signal(SIGINT,handler);
   signal(SIGTERM,handler);
   ctrc=TBIG;
   //printf("cpustart: cpu=%g save=%g start=%g last=%g...\n",cpu,save,start,last);
   tdelta=tnext=tmin;
   ltmp=0;
   clast=cputime();
  }

int cpunext(int skip)
  {
   //printf("cpunext %g/%g\n",walltime(),tnext);
   double now=walltime();
   if (now>=tnext)
     {
      if (skip==0)	// N.B: skip>0 se si vuole solo testare il valore di ritorno, senza aggiornare tdelta e tnext
        {
         if (tdelta<tmax) tdelta*=2;
         do {tnext+=tdelta;} while (tnext<=now);
        }
      return 1;
     }
   else return 0;
  }

// le chiamate a cpuint vanno annidate in maniera che le chiamate interne siano quelle con il livello più alto
// se viene chiamato ad esempio cpuint(4) è restituisce 1, tutti i cpuint(n) con n>=4 daranno in seguito 0 (a meno che non si prema di nuovo ctrl-C)
// mentre cpuint(n) con n<4 daranno ancora 1 la prima volta che vengono chiamati

int cpuint(int level)
  {
   if (level<0) level=0;
   if (level>=lmax) level=lmax-1;
   if (flag && level==0)    
     {
      // è stato chiamato cpuint(0) dopo che il tempo CPU è terminato...
      static int count;
      count++;
      int cmax=100;
      if (count>=cmax)
        {
         fprintf(stderr,"... loop infinito cpuint? ...\n");
         exit(1);
        }
     }
   // verifica se è tempo di un break
   // se ctrc>=0 allora è stato premuto ctrl-C una sola volta, se è passato 1 secondo aggiorna last e restituisci TRUE
   double now=walltime();
   int flag2=0;
   if (now>ctrc+1)
     {
      ctrc=TBIG;
      last=now;
      flag2=1;
     }
   else if (save>0 && now>last+save) // restituisci TRUE perché è passato un tempo save
     {
      //printf("tempo save: last=%g save=%g now=%g\n",last,save,now);
      last=now;
      flag2=1;
     }
   int fret=ftmp[level];
   if (flag2)
     {
      for (int l=0;l<level;l++) ftmp[l]=1;
      fret=1;
     }
   else ftmp[level]=0;
   //printf("cpuint: %i\n",fret);
   //if (level==0 && cpunext(1)) fret=1;		// N.B: in questa maniera cpuint(0) testa anche se è tempo di un cpunext()
   return fret;
  }

int cputerm(void)
  {
   static double next;
   double now=walltime();
   if (cpu<=0 || now<cpu)
     {
      // il tempo non è ancora terminato...
      if (now>next)
        {
         // se esiste un file "STOP" nella directory termina il job
         struct stat buf;
         if (lstat("STOP",&buf)==0 && S_ISREG(buf.st_mode)) flag=1;
         next+=600;
        }
     }
   else flag=1;
   return flag;
  }
#endif
#endif
