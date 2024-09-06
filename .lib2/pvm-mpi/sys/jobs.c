#ifndef __JOBS_H__
#define __JOBS_H__
#include <stdio.h>
#include <stdlib.h>

int jobdirs(char *name,const char *JOBDIRS=".");		// fa una lista di directory che contengono un file serie-[name]-000.dat
FILE *jobfile(int m,int k);
char *jobpath(int m);
char *jobname(char *name);

#ifndef __HEADERS__
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <pvm.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

static int filetype(const char *path)
  {
   struct stat buf;
   int err=stat(path,&buf);
   if (err==0)
     {
      if (S_ISREG(buf.st_mode)) return 1;
      else if (S_ISDIR(buf.st_mode)) return 2;
     }
   return 0;
  }

static char *fillspace(char *name)
  {
   static char name2[PATH_MAX];
   int k=0;
   while (name[k])
     {
      if (name[k]==' ') name2[k]='-'; else name2[k]=name[k];
      k++;
     }
   return name2;
  }

char *jobname(char *name)
  {
   static char name2[PATH_MAX];
   int k=0;
   while (name[k])
     {
      if (k>0 && name[k]=='-' && name[k-1]!=' ') name2[k]=' '; else name2[k]=name[k];
      k++;
     }
   name2[k]='\0';
   return name2;
  }

static char name[PATH_MAX];
static int n;				// numero di directory trovate
static char **path;			// path delle directory

static void scanjob(const char *jobdir)
  {
   char root[PATH_MAX],buffer[PATH_MAX];
   int npes;
   FILE *stream=fopen(jobdir,"r");
   if (fscanf(stream,"%s",root));
   if (fscanf(stream,"%i",&npes));
   fclose(stream);
   for (int m=0;m<npes;m++)
     {
      sprintf(buffer,"%s/RUN/%03i/serie-%s-000.dat",root,m,name);
      if (filetype(buffer)==1)
        {
         path=(char**)getmem(path,(1+n)*sizeof(char**));
         path[n]=(char*)getmem(NULL,PATH_MAX);
         sprintf(path[n],"%s/RUN/%03i/",root,m);
         n++;
        }
     }
  }

static void scandir(const char *dirname)
  {
   DIR *dir=opendir(dirname);
   struct dirent *dirent;
   while ((dirent=readdir(dir)))
     {
      if (strncmp(dirent->d_name,"jobdir",6)) continue;
      scanjob(dirent->d_name);
     }
   closedir(dir);
  }

int jobdirs(char *_name,const char *JOBDIRS)
  {
   char *name2=fillspace(_name);
   if (strcmp(name,name2)) n=0;
   strcpy(name,name2);
   if (filetype(JOBDIRS)==1)	// JOBDIRS è un file
     {
      char jobdir[PATH_MAX];
      FILE *stream=fopen(JOBDIRS,"r");
      while (fscanf(stream,"%s",jobdir)==1)
        {
         // se jobdir è una directory esegui scandir...
         scanjob(jobdir);
        }
      fclose(stream);
     }
   else if (filetype(JOBDIRS)==2) scandir(JOBDIRS); // JOBDIRS è una directory
   return n;
  }

char *jobpath(int m)
  {
   if (m<0 || m>=n) return NULL;
   return path[m];
  }

FILE *jobfile(int m,int k)
  {
   if (m<0 || m>=n) return NULL;
   char buffer[PATH_MAX];
   sprintf(buffer,"%s/serie-%s-%03i.dat",path[m],name,k);
   FILE *stream=fopenf(buffer,"r");
   return stream;
  }
#endif
#endif
