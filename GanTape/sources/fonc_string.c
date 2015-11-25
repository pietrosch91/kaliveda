
#include <stdio.h>
#include <string.h>

#ifdef __VMS
#include <stdlib.h>
#endif

#include "fonc_string_def.h"



/*------------------------------------*/
/* passage string C a string fortran  */
/*------------------------------------*/

int cstring_to_forstring(char* cstring, char* forstring, int len_for)
{
   register i;
   int len;
   int count;

   /* longueur de la chaine C */
   len = strlen(cstring);

   /* on recopie cstring dans forstring */
   for (i = 0; i < len; i++)
      forstring[i] = cstring[i];

   /* 0n complete avec des blancs */
   for (i = len; i < len_for; i++)
      forstring[i] = ' ';

   return 0;

}
int cstring_to_forstring_(char* cstring, char* forstring, int len_for)
{
   return cstring_to_forstring(cstring, forstring, len_for);
}

#ifdef _LUIGI_SUPP

/*--------------*/
/* dump  */
/*--------------*/

void dump(char* p, int nb)
{
   int i, j, k;
   char* car;
   unsigned short* pShort;
   unsigned char* pChar;

   j = nb / 16;
   if (j * 16 < nb) j++;

   pShort = (unsigned short*)p;
   pChar = (unsigned char*)p;

   for (i = 0; i < j; i++) {

      printf("\n%8x:", pChar);
      for (k = 0; k < 8; k++)
         printf(" %04x", *pShort++);

      printf("    ");

      for (k = 0; k < 16; k++) {
         if ((*pChar >= 32) && (*pChar < 127))
            printf("%c", *pChar);
         else
            printf(".");

         pChar++;

      }

   }
   printf("\n\r");
}
#endif



/*-----------*/
/* strtrim   */
/*-----------*/

int strtrim(char* str, int len)
{
   int i, pos;
   int nb;
   char old_c;

   /* init certaines variables */
   old_c = *str;
   pos = 0;
   nb = 0;

   /* on cherche un car blanc */
   for (i = 1; i < len; i++) {
      if (str[i] == ' ') {
         if (old_c != ' ')
            pos = i;

         nb++;

         /* plus de 2 blancs consecutifs */
         if (nb >= 2)
            break;
      } else {
         pos = 0;
         nb = 0;
      }
      old_c = str[i];
   }

   /* si on a trouve des blancs en fin de chaine */
   if (nb != 0)
      str[pos] = '\0';
   else
      str[len] = '\0';
   return 0;

}

/*-----------------------------------*/
/* passage string fortran a string C */
/*-----------------------------------*/

int forstring_to_cstring(char* ForString, char* CString, int len)
{
   int ret = -1;

   char* str;

   /* on recopie la chaine fortran */
   strncpy(CString, ForString, len - 1);

   /* on enleve les blancs en fin de chaine */
   ret = strtrim(CString, len - 1);

   return ret;

}

int forstring_to_cstring_(char* ForString, char* CString, int len)
{
   return forstring_to_cstring(ForString, CString, len);
}
