/* 
  	Programme de test de relecture de spectres



*/

#include <stdio.h>

#include <string.h>

#include <stdlib.h>

#include "GEN_TYPE.H"

#include "gan_acq_buf.h"

#include "gan_acq_swap_buf.h"

#include "def_spectre.h"



#define SPECTRE_SIZE 1024*1024

unsigned char Table[SPECTRE_SIZE];
tete_spec SpectreEntete;
char FileName[256];
char NewFileName[256];
char buf[80];
   

int main (int argc, char *argv[])
{
  int status,error;

  printf("Test_read_Spectre (debug)\n\r");

 /* passage en mode debug */
/*  set_debug_spectre(1); */

 /* saisie du nom de fichier */
  printf("Entrez un nom de fichier : ");
  if(gets(FileName))
    printf("FileName = %s\n\r",FileName);
  else
  {
    printf("Read_Spectre nom_fichier\n\r");
    exit(-1);
  }

 /* lecture du fichier spectre */
  status = read_spectre(FileName, &SpectreEntete, Table, SPECTRE_SIZE, &error);
  printf("read_spectre : status = %d error = %d\n\r",status,error);

 /* test si lecture spectre OK */
  if(status == 0)
  {
   /* affichage info spectre */ 
    affiche_spectre_info(&SpectreEntete);

    /* affichage des 128 premiers mots du spectre */
    printf("\nAffichage des 128 premiers elements du spectre :\n");
    affiche_spectre_data(&SpectreEntete, Table, 128);

   /* sauvegarde du spectre */
    printf("Voulez vous le sauvegarder (o/N) ? ");
    gets(buf);
    if((*buf == 'O')
      ||(*buf == 'o'))
    {

     /* test si argument */
      printf("Entrez un nom de fichier de save : ");
      if(gets(NewFileName))
        printf("NewFileName = %s\n\r",NewFileName);
      else
      {
        printf("Bad NewFileName\n\r");
        exit(-1);
      }
    /* sauvegarde du spectre */
      status = save_spectre (NewFileName, &SpectreEntete, Table, SPECTRE_SIZE,&error);
      printf("save_spectre : status = %d\n\r",status);
    }
  }
  
}  
