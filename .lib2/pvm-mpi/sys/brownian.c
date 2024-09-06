#ifndef __BROWNIAN_H__
#define __BROWNIAN_H__

void step_uno(int n,double *xm,double *vx,double *ax,double *gamma,double *v2m,double dt);
void step_due(int n,double *vx,double *ax,double *gamma,double dt);

#ifndef __HEADERS__
#include <math.h>
#include <pvm.h>

// primo step della dinamica browniana
// gamma è il coefficiente di damping, ovvero l'inverso del tempo caratteristico (tempo in cui l'autocorrelazione della velocità decade a zero)
// v2m è il RMS della velocità all'equilibrio: corrisponde in pratica alla radice della temperatura diviso la massa della particella

void step_uno(int n,double *xm,double *vx,double *ax,double *gamma,double *v2m,double dt)
  {
   for (int i=0;i<n;i++)
     {
      double gdt=gamma[i]*dt;
      double c0=exp(-gdt);
      double c1,c2,sr,csv,gsv;
      if (gdt>1e-4)
        {
         c1=(1-c0)/gdt;
         c2=(1-c1)/gdt;
         sr=sqrt((2-(3-4*c0+c0*c0)/gdt)/gdt);
         csv=(1-c0)*(1-c0)/(gdt*sr);
         gsv=sqrt((1-c0*c0)-csv*csv);
        }
      else
        {
         c1=1-gdt/2;
         c2=0.5*(1-gdt/3);
         gsv=sqrt(gdt/2);
         sr=1.15470053837925152902*gsv;                         // (2/sqrt(3))*gsv*dt
         csv=1.73205080756887729352*gsv;                        // sqrt(3)*gsv
        }
      double xtmp=Grandom();
      double ytmp=Grandom();
      xm[i]+=(c1*vx[i]+c2*ax[i]*dt+v2m[i]*sr*xtmp)*dt;
      vx[i]=c0*vx[i]+(c1-c2)*ax[i]*dt+v2m[i]*(csv*xtmp+gsv*ytmp);
     }
  }

void step_due(int n,double *vx,double *ax,double *gamma,double dt)
  {
   for (int i=0;i<n;i++)
     {
      double gdt=gamma[i]*dt;
      double c2;
      if (gdt>1e-4)
        {
         double c0=exp(-gdt);
         double c1=(1-c0)/(gdt);
         c2=(1-c1)/(gdt);
        }
      else c2=0.5*(1-gdt/3);
      vx[i]+=c2*dt*ax[i];
     }
  }
#endif
#endif
