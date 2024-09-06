#ifndef __STRING_H__
#define __STRING_H__

char *stristr(const char *haystack,const char *needle);
int stricmp(const char *a,const char *b);

#ifndef __HEADERS__
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *stristr(const char *haystack,const char *needle)
  {
   int l1=strlen(haystack);
   int l2=strlen(needle);
   char *s1=(char*)calloc(l1+1,sizeof(char));
   char *s2=(char*)calloc(l2+1,sizeof(char));
   strcpy(s1,haystack);
   strcpy(s2,needle);
   for (int i=0;i<l1;i++) s1[i]=tolower(s1[i]);
   for (int i=0;i<l2;i++) s2[i]=tolower(s2[i]);
   char *p=strstr(s1,s2);
   if (p) p=(char*)haystack+(int)(p-s1);
   free(s2);
   free(s1);
   return p;
  }

int stricmp(const char *s1,const char *s2)
  {
   int i=0;
   while (s1[i]!='\0' || s2[i]!='\0')
     {
      int d=tolower(s1[i])-tolower(s2[i]);
      if (d!=0) return d;
      i++;
     }
   return 0;
  }
#endif
#endif
