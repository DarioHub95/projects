
/************************************ BIT ************************************/
#ifndef __BIT_H__
#define __BIT_H__
#include <stdio.h>

void bitopen(FILE *stream);
void bitwrite(int a,int n);
int bitread(int n);
int bitclose(void);

#endif
/************************************ CPU ************************************/
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

#endif
/************************************ DEBUG ************************************/
#ifndef __DEBUG_H__
#define __DEBUG_H__

void debug(int level,const char *format,...);

#endif
/************************************ FOPENF ************************************/
#ifndef __FOPENF_H__
#define __FOPENF_H__
#include <stdio.h>

void rootdir(const char *format,...);
void removef(const char *format,...);
void makedir(const char *path,int mode=0755);
FILE *fopenf(const char *format,const char *mode,...);
void fprint2(FILE *stream,const char *format,...);

#endif
/************************************ GETARG ************************************/
#ifndef __GETARG_H__
#define __GETARG_H__
#include <stdlib.h>

void init_args(int *argc=NULL,char ***argv=NULL);
int getarg_i(const char *name,int ndef=0,const char *srem=NULL);
double getarg_d(const char *name,double xdef=0.,const char *srem=NULL);
char *getarg_s(const char *name,const char *sdef=NULL,const char *srem=NULL);
int getlinearg(const char *path,int nline=0,const char *name=NULL);

#endif
/************************************ GETMEM ************************************/
#ifndef __GETMEM_H__
#define __GETMEM_H__
#include <stdlib.h>

void *getmem(void *ptr,long size);
int *ivector(int imin,int imax,int *ptr=NULL);
int **imatrix(int imin,int imax,int jmin,int jmax,int **ptr=NULL);
int ***itensor(int imin,int imax,int jmin,int jmax,int kmin,int kmax);
long int *lvector(int imin,int imax,long int *ptr=NULL);
long int **lmatrix(int imin,int imax,int jmin,int jmax,long int **ptr=NULL);
double *dvector(int imin,int imax,double *ptr=NULL);
double **dmatrix(int imin,int imax,int jmin,int jmax,double **ptr=NULL);
double ***dtensor(int imin,int imax,int jmin,int jmax,int kmin,int kmax);

#endif
/************************************ MPI ************************************/
#ifndef __MPI_H__
#define __MPI_H__
#include <stdlib.h>

void mpi_init(int *argc,char ***argv);
int mpi_mype(void);
int mpi_npes(void);
void mpi_barrier(void);
void mpi_ibcast(int *ptr,int n,int root);
void mpi_isend(int *ptr,int n,int tid);
void mpi_irecv(int *ptr,int n,int tid);
void mpi_dbcast(double *ptr,int n,int root);
void mpi_dsend(double *ptr,int n,int tid);
void mpi_drecv(double *ptr,int n,int tid);
int mpi_term(void);
void mpi_end(void);

#endif
/************************************ OOM ************************************/
#ifndef __OOM_H__
#define __OOM_H__

void oomscore(int score);

#endif
/************************************ PVM ************************************/
#ifndef __PVM_H__
#define __PVM_H__
#include <stdlib.h>

void pvm_init(int *argc=NULL,char ***argv=NULL);
void pvm_end(void);
int pvm_mype(void);
int pvm_debug(void);
int pvm_nfile(int nfile=0);
void pvm_load(void);
void pvm_save(void);

#endif
/************************************ RANDOM ************************************/
#ifndef __RANDOM_H__
#define __RANDOM_H__
#include <stdlib.h>

void init_random(unsigned long seed=0,unsigned int seed2=0);
unsigned long poprandom(void);
void pushrandom(unsigned long S);
double Xrandom(void);
double Grandom(void);

#endif
/************************************ STRING ************************************/
#ifndef __STRING_H__
#define __STRING_H__

char *stristr(const char *haystack,const char *needle);
int stricmp(const char *a,const char *b);

#endif
/************************************ BALANC ************************************/
#ifndef __BALANC_H__
#define __BALANC_H__

void balanc(double **a, int n);

#endif
/************************************ BICO ************************************/
#ifndef __BICO_H__
#define __BICO_H__

double bico(int n,int k);

#endif
/************************************ BNLDEV ************************************/
#ifndef __BNLDEV_H__
#define __BNLDEV_H__

int bnldev(double pp, int n);

#endif
/************************************ BRENT ************************************/
#ifndef __BRENT_H__
#define __BRENT_H__

double brent(double ax, double bx, double cx, double (*f)(double), double tol, double *xmin);

#endif
/************************************ BROYDN ************************************/
#ifndef __BROYDN_H__
#define __BROYDN_H__

void broydn(double *x, int n, int *check,void (*vecfunc)(int, double *, double *),double TOLF);

#endif
/************************************ COVSRT ************************************/
#ifndef __COVSRT_H__
#define __COVSRT_H__

void covsrt(double **covar, int ma, int ia[], int mfit);

#endif
/************************************ DSPRSAX ************************************/
#ifndef __DSPRSAX_H__

void dsprsax(double sa[], unsigned long ija[], double x[], double b[], unsigned long n);

#endif
/************************************ DSPRSTX ************************************/
#ifndef __DSPRSTX_H__

void dsprstx(double sa[], unsigned long ija[], double x[], double b[], unsigned long n);

#endif
/************************************ ELMHES ************************************/
#ifndef __ELMHES_H__
#define __ELMHES_H__

void elmhes(double **a, int n);

#endif
/************************************ ERFF ************************************/
#ifndef __ERFF_H__
#define __ERFF_H__

double erff(double x);

#endif
/************************************ F1DIM ************************************/
#ifndef __F1DIM_H__
#define __F1DIM_H__

double f1dim(double x);

#endif
/************************************ FACTLN ************************************/
#ifndef __FACTLN_H__
#define __FACTLN_H__

double factln(int n);

#endif
/************************************ FDJAC ************************************/
#ifndef __FDJAC_H__
#define __FDJAC_H__

void fdjac(int n, double *x, double *fvec, double **df, void (*vecfunc)(int, double *, double *));

#endif
/************************************ FFT ************************************/
#ifndef __FFT_H__
#define __FFT_H__

void fft(double *x,int nn);

#endif
/************************************ FIT ************************************/
#ifndef __FIT_H__
#define __FIT_H__

void fit(double *x, double *y, int ndata, double *sig, int mwt, double *a,double *b, double *siga, double *sigb, double *chi2, double *q);

#endif
/************************************ FMIN ************************************/
#ifndef __FMIN_H__
#define __FMIN_H__

double fmin(double *x);

#endif
/************************************ FOUR1 ************************************/
#ifndef __FOUR1_H__
#define __FOUR1_H__

void four1(double *data,unsigned long nn,int isign);

#endif
/************************************ GAMMLN ************************************/
#ifndef __GAMMLN_H__
#define __GAMMLN_H__

double gammln(double xx);

#endif
/************************************ GAMMP ************************************/
#ifndef __GAMMP_H__
#define __GAMMP_H__

double gammp(double a,double x);

#endif
/************************************ GAMMQ ************************************/
#ifndef __GAMMQ_H__
#define __GAMMQ_H__

double gammq(double a, double x);

#endif
/************************************ GAUSSJ ************************************/
#ifndef __GAUSSJ_H__
#define __GAUSSJ_H__

void gaussj(double **a, int n, double **b, int m);

#endif
/************************************ GCF ************************************/
#ifndef __GCF_H__
#define __GCF_H__

void gcf(double *gammcf, double a, double x, double *gln);

#endif
/************************************ GSER ************************************/
#ifndef __GSER_H__
#define __GSER_H__

void gser(double *gamser, double a, double x, double *gln);

#endif
/************************************ HPSORT ************************************/
#ifndef __HPSORT_H__
#define __HPSORT_H__
#include <stdlib.h>

void hpsort(int n,double *ra);
void hpsort3(int n,double *ra,int *rb=NULL,int *rc=NULL);

#endif
/************************************ HQR ************************************/
#ifndef __HQR_H__
#define __HQR_H__

void hqr(double **a, int n, double *wr, double *wi);

#endif
/************************************ INDEX ************************************/
#ifndef __INDEX_H__
#define __INDEX_H__

// scrive l'array indx in maniera che arr[indx[0]] <= arr[indx[1]] <= ... <= arr[indx[n-1]]
// N.B: convenzione C sui vettori: A[0...N-1]

void index(int n, double *arr, int *indx);

#endif
/************************************ LINBCG ************************************/
#ifndef __LINBCG_H__

void linbcg(unsigned long n, double b[], double x[], int itol, double tol, int itmax, int *iter, double *err);

#endif
/************************************ LINMIN ************************************/
#ifndef __LINMIN_H__
#define __LINMIN_H__

void linmin(double p[], double xi[], int n, double *fret, double (*func)(double []));

#endif
/************************************ LNSRCH ************************************/
#ifndef __LNSRCH_H__
#define __LNSRCH_H__

void lnsrch(int n, double *xold, double fold, double *g, double *p, double *x, double *f, double stpmax, int *check, double (*func)(double *));

#endif
/************************************ LUBKSB ************************************/
#ifndef __LUBKSB_H__
#define __LUBKSB_H__

void lubksb(double **a, int n, int *indx, double *b);

#endif
/************************************ LUDCMP ************************************/
#ifndef __LUDCMP_H__
#define __LUDCMP_H__

int ludcmp(double **a, int n, int *indx, double *d);

#endif
/************************************ MIDPNT ************************************/
#ifndef __MIDPNT_H__
#define __MIDPNT_H__

double midpnt(double (*func)(double), double a, double b, int n);

#endif
/************************************ MNBRAK ************************************/
#ifndef __MNBRAK_H__
#define __MNBRAK_H__

void mnbrak(double *ax, double *bx, double *cx, double *fa, double *fb, double *fc, double (*func)(double));

#endif
/************************************ MRQCOF ************************************/
#ifndef __MRQCOF_H__
#define __MRQCOF_H__

void mrqcof(double x[], double y[], double sig[], int ndata, double a[], int ia[],
        int ma, double **alpha, double beta[], double *chisq,
        void (*funcs)(double, double [], double *, double [], int));

#endif
/************************************ MRQMIN ************************************/
#ifndef __MRQMIN_H__
#define __MRQMIN_H__

void mrqmin(double x[], double y[], double sig[], int ndata, double a[], int ia[],
        int ma, double **covar, double **alpha, double *chisq,
        void (*funcs)(double, double [], double *, double [], int), double *alamda);

#endif
/************************************ ODEINT ************************************/
#ifndef __ODEINT_H__
#define __ODEINT_H__

void odeint(double ystart[], int nvar, double x1, double x2, double eps, double *h1,double hmin, int *nok, int *nbad,void (*derivs)(double, double [], double []),
        void (*rkqs)(double [], double [], int, double *, double, double, double [], double *, double *, void (*)(double, double [], double [])));

#endif
/************************************ POIDEV ************************************/
#ifndef __POIDEV_H__
#define __POIDEV_H__

int poidev(double mu);

#endif
/************************************ POLINT ************************************/
#ifndef __POLINT_H__
#define __POLINT_H__

void polint(double *xa,double *ya,int n,double x,double *y,double *dy);

#endif
/************************************ POWELL ************************************/
#ifndef __POWELL_H__
#define __POWELL_H__

void powell(double p[], double **xi, int n, double ftol, int *iter, double *fret, double (*func)(double []));

#endif
/************************************ POWER ************************************/
#ifndef __POWER_H__
#define __POWER_H__

void power(double *x,int n,double *p);

#endif
/************************************ PYTHAG ************************************/
#ifndef __PYTHAG_H__
#define __PYTHAG_H__

double pythag(double a, double b);

#endif
/************************************ QRDCMP ************************************/
#ifndef __QRDCMP_H__
#define __QRDCMP_H__

void qrdcmp(double **a, int n, double *c, double *d, int *sing);

#endif
/************************************ QROMB ************************************/
#ifndef __QROMB_H__
#define __QROMB_H__

double qromb(double (*func)(double), double a, double b,double eps);

#endif
/************************************ QROMO ************************************/
#ifndef __QROMO_H__
#define __QROMO_H__

double qromo(double (*func)(double), double a, double b,double (*choose)(double(*)(double), double, double, int),double eps);

#endif
/************************************ QRUPDT ************************************/
#ifndef __QRUPDT_H__
#define __QRUPDT_H__

void qrupdt(double **r, double **qt, int n, double *u, double *v);

#endif
/************************************ REALFT ************************************/
#ifndef __REALFT_H__
#define __REALFT_H__

void realft(double *x,int n);

#endif
/************************************ RK4 ************************************/
#ifndef __RK4_H__
#define __RK4_H__

void rk4(double y[], double dydx[], int n, double x, double h, double yout[],void (*derivs)(double, double [], double []));

#endif
/************************************ RKCK ************************************/
#ifndef __RKCK_H__
#define __RKCK_H__

void rkck(double y[], double dydx[], int n, double x, double h, double yout[], double yerr[], void (*derivs)(double, double [], double []));

#endif
/************************************ RKQS ************************************/
#ifndef __RKQS_H__
#define __RKQS_H__

void rkqs(double y[], double dydx[], int n, double *x, double htry, double eps, double yscal[], double *hdid, double *hnext, void (*derivs)(double, double [], double []));

#endif
/************************************ ROTATE ************************************/
#ifndef __ROTATE_H__
#define __ROTATE_H__

void rotate(double **r, double **qt, int n, int i, double a, double b);

#endif
/************************************ RSOLV ************************************/
#ifndef __RSOLV_H__
#define __RSOLV_H__

void rsolv(double **a, int n, double *d, double *b);

#endif
/************************************ RTSEC ************************************/
#ifndef __RTSEC_H__
#define __RTSEC_H__

double rtsec(double (*func)(double), double x1, double x2, double xacc);

#endif
/************************************ SIMPLX ************************************/
#ifndef __SIMPLX_H__
#define __SIMPLX_H__

void simplx(double **a, int m, int n, int m1, int m2, int m3, int *icase,int izrov[], int iposv[]);

#endif
/************************************ SNRM ************************************/
#ifndef __SNRM_H__

double snrm(unsigned long n, double sx[], int itol);

#endif
/************************************ SPCTRM ************************************/
#ifndef __SPCTRM_H__
#define __SPCTRM_H__

void spctrm(double *fp,double *p, int m, int k, int ovrlap);

#endif
/************************************ SPLINE ************************************/
#ifndef __SPLINE_H__
#define __SPLINE_H__

void spline(double *x,double *y,int n,double yp1,double ypn,double *y2);

#endif
/************************************ SPLINT ************************************/
#ifndef __SPLINT_H__
#define __SPLINT_H__

void splint(double *xa,double *ya,double *y2a,int n,double x,double *y);

#endif
/************************************ SPRSIN ************************************/
#ifndef __SPRSIN_H__

void sprsin(double **a, int n, double thresh, unsigned long nmax,double sa[],unsigned long ija[]);

#endif
/************************************ TQLI ************************************/
#ifndef __TQLI_H__
#define __TQLI_H__
#include <stdlib.h>

void tqli(double *d, double *e, int n, double **z=NULL);

#endif
/************************************ TRAPZD ************************************/
#ifndef __TRAPZD_H__
#define __TRAPZD_H__

double trapzd(double (*func)(double), double a, double b, int n,double s);

#endif
/************************************ TRED2 ************************************/
#ifndef __TRED2_H__
#define __TRED2_H__

void tred2(double **a, int n, double *d, double *e,int flag=1);

#endif
/************************************ ZBRENT ************************************/
#ifndef __ZBRENT_H__
#define __ZBRENT_H__

double zbrent(double (*func)(double), double x1, double x2, double tol);

#endif
/************************************ ZRHQR ************************************/
#ifndef __ZRHQR_H__
#define __ZRHQR_H__

int zrhqr(double *a,int m,double *rtr,double *rti);

#endif
/************************************ BISECT ************************************/
#ifndef __BISECT_H__
#define __BISECT_H__

int select(int mmax,double *x);
int bisect(int mmax,double *x,double xtmp);

#endif
/************************************ BROWNIAN ************************************/
#ifndef __BROWNIAN_H__
#define __BROWNIAN_H__

void step_uno(int n,double *xm,double *vx,double *ax,double *gamma,double *v2m,double dt);
void step_due(int n,double *vx,double *ax,double *gamma,double dt);

#endif
/************************************ CYCLO ************************************/
#ifndef __CYCLO_H__
#define __CYCLO_H__
#include <stdlib.h>

class cyclo
  {
   public:
   cyclo(long cmax,long tmax,int ppdec);
   void pop(void);                       // trova cnext e next successivi
   int push(void);             // se restituisce un intero >=0, fai partire una nuova serie

   long cmax;
   long tmax;
   long *delta;
   int imax;
   int nmax;		// numero massimo serie
   int ntmp;		// numero successivo serie
   long *W;
   int *z;
   long cnext;		// prossimo step di correlazione
   int next;		// serie corrispondente a cnext
   int inx;		// indice della correlazione
   long count;
  };

#endif
/************************************ HISTO ************************************/
#ifndef __HISTO_H__
#define __HISTO_H__

//	utilizzo:
//	histo *Y=new histo(xmin,xmax);		// ppdec: numero di punti per decade (scala log) oppure totali (scala lin)
//						// (type&1)!=0 se scala lineare, (type&1)==0 se scala logaritmica
//						// (type&2)!=0 se variabile continua, (type&2)==0 se variabile discreta... es: type=3 per scala lineare & variabile continua
//	double *H=dvector(0,Y->hmax);
//      ...
//	   int h=Y->bin(x);
//	   H[h]++;
//	   count++
//      ...
// 	for (int h=1;h<Y->hmax;h++) if (Y->norm[h]>0) printf("%12g %12g\n",Y->xtmp[h],H[h]/(count*Y->norm[h]));

class histo
  {
   public:
   histo(double xmin,double xmax,int type=0,int ppdec=0);
   int bin(double x);

   int type;
   int hmax;
   double xmin;
   double xmax;
   double *x1;			// estremo inferiore intervallo
   double *x2;			// estremo superiore
   double *norm;		// normalizzazione (numero interi nell'intervallo se variabile discreta, altrimenti larghezza dell'intervallo)
   double *xtmp;		// punto medio dell'intervallo
  };

#endif
/************************************ JOBS ************************************/
#ifndef __JOBS_H__
#define __JOBS_H__
#include <stdio.h>
#include <stdlib.h>

int jobdirs(char *name,const char *JOBDIRS=".");		// fa una lista di directory che contengono un file serie-[name]-000.dat
FILE *jobfile(int m,int k);
char *jobpath(int m);
char *jobname(char *name);

#endif
/************************************ LUIJTEN ************************************/
#ifndef __LUIJTEN_H__
#define __LUIJTEN_H__

void luijten1(int N,double *P);
int luijten2(int N,int *stack);

#endif
/************************************ NEIGHBORS ************************************/
#ifndef __NEIGHBORS_H__
#define __NEIGHBORS_H__

#define BC_PERIODIC	0
#define BC_OPEN		1

int **neighbors(int d,int L,int bc=BC_PERIODIC);

#endif
/************************************ PERMUTA ************************************/
#ifndef __PERMUTA_H__
#define __PERMUTA_H__

void permuta(int n,int N,int *stack);

#endif
/************************************ PFAFF ************************************/
#ifndef __PFAFF_H__
#define __PFAFF_H__

int pfaff(int N,double **A);

#endif
/************************************ POLYFIT ************************************/
#ifndef __POLYFIT_H__
#define __POLYFIT_H__

void polyfit(int n,double *x,double *y,double *s,int m,double *a);

#endif
/************************************ TREE ************************************/
#ifndef __TREE_H__
#define __TREE_H__

int maketree(int k,int N,int **next);

#endif
/************************************ WEIGHT ************************************/
#ifndef __WEIGHT_H__
#define __WEIGHT_H__

class weight
  {
   public:
   weight(int N);
   void push(int i,double wtmp);
   int pop(void);
   void restart(void);

   int M;
   int K;
   double *W;
   double *B;
   int mmax;
};

#endif
