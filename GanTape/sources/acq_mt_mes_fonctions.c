#ifndef __MES_FONCTIONS_C
#define __MES_FONCTIONS_C
/*****************************************************************************
 *                                                                           *
 *       Projet       :  Bibiotheque GANIL_TAPE                              *
 *                                                                           *
 *       Programme    :  ACQ_MT_MES_FONCTIONS.c                              *
 *                                                                           *
 *       Auteur       :  OPDEBECK                                            *
 *       Date         :  16 juillet 1996                                     *
 *                      				                     *
 *                                                                           *
 *       Objet        :   Ensemble de fonctions modifiables a volonte par    *
 *			l'utilisateur permettant de traiter les differents   *
 *                      buffers.                                             *  
 *       Modifications :
 *             B. Raine 20 janvier 2003
 *             B. Raine 13 septembre 2004 acq_get_parbr_label
 *             B. Raine 21 octobre 2004 affichage embryonnaire des echelles
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GEN_TYPE.H"
#include "STR_EVT.H"
#include "gan_acq_buf.h"
#include "acq_mt_mes_fonctions.h"

#include "gan_tape_erreur.h"


/*****************************************************************************
 *                                                                           *
 *           Function  : traite_params                                       *
 *           Objet     : Traite un buffer de parametres.                     *
 *                       Programme d'exemple affichant les parametres a      *
 *                       l'ecran en fonction de l'argument Display           *
 *              Le format des buffers parametres est le suivant :            *
 *               entete:      " PARAM  ",N0 Buffer                           *
 *               suivi de     "nom_param,numero,nb bits utiles RC" (RC=0x0D) *
 *                             repetes autant de fois que necessaire         *
 *               fin de buffer:  -  nom_param = "(4 espaces)"                *
 *                                                si autre buffer suit       *
 *                               -  nom_param = "!!!!"    si dernier buffer  *
 *                                                                           *
 *           Entree    : Buffer -- Buffer de structure IN2P3.                *
 *                       Display - Booleen indiquant si on veut afficher les *
 *                                 parametres a l'ecran                      *
 *                                                                           *
 *           Retour    : Status -- Etat d'execution de la commande.          *
 *                              - ACQ_OK si aucun probleme.                  *
 *                              - Code d'erreur VMS ou UNIX sinon.           *
 *
 *           Modifications :
 *             B. Raine 20 janvier 2003
 *              Prise en compte de plusieurs 0xOD entre noms de parametres
 *              Prise en compte de plusieurs blocs parametres 
 *                                                                           *
 *****************************************************************************/
#define NB_PAR_MAX 65534

char * CopyParam (char *Dest, char *Source)
{
  char c;
  int len = 0;

   do {
     len++;
     c = *Source++;
     if ( (c == ',') || (c == 0x0d) )
       c = '\0';
     if ( len < PAR_NAME_LEN )
       *Dest++ = c;
     else
       *Dest = '\0';
   } while ( c != '\0' );
   /* Verifier qu'on n'a pas plusieurs 0x0D de suite */
   while ( *Source == 0x0d )
     Source ++;
   return( Source );
}

int traite_params	( in2p3_buffer_struct * Buffer, int Display ) {

   char *CurrPointer;
   char NomParam[PAR_NAME_LEN], chaine[PAR_NAME_LEN];
   int  NumPar, NbBits;
   int  NbPar = 0; /* Nombre de parametres */
   char NumParHexa[30];


   if ( Display )
     { 
       printf ("\n>>> 'TRAITE_PARAMS' est en cours.\n");
       printf("        Nom parametres        Label              NbBits\n");
     }

   CurrPointer = (char *)Buffer->les_donnees.cas.Buf_param;

   if ( StatParamTable == 0 )
     { /* creer la table des statisques sur les parametres */
       StatParamTable = calloc(NB_PAR_MAX, sizeof(STAT_PARAM));
       if ( StatParamTable == 0 )
	 {
	   printf("Probleme d'allocation memoire pour statistiques sur les parametres\n");
	   printf(" il n'y aura donc pas de statistiques \n");
	 }
       NumParMax = 0;
       NbPar = 0;
     }

   if ( StatParamTable != 0 )
     while ( strncmp(CurrPointer, "    ",4) != 0 &&  
	     strncmp(CurrPointer, "!!!!",4) != 0   )
       {
	 NbPar++;
	 CurrPointer = CopyParam(NomParam, CurrPointer);
	 CurrPointer = CopyParam( chaine, CurrPointer);
	 sscanf( chaine,"%d",&NumPar);
	 if ( NumPar > NumParMax )
	   NumParMax = NumPar; /* Chercher le plus grand numero de paramatre */
	 CurrPointer = CopyParam( chaine, CurrPointer);
	 sscanf( chaine,"%d",&NbBits);
	 strcpy((char *)StatParamTable[NumPar].ParName, NomParam);
	 StatParamTable[NumPar].ParRang = NbPar;
	 StatParamTable[NumPar].ParNbBits = NbBits;
	 StatParamTable[NumPar].ParNbCoups = 0;
	 if ( Display )
	   {
	     sprintf(NumParHexa,"(0x%x)", NumPar);
	     printf(" %2d      %-16s     %6.0d %-10s    %-6.0d\n", 
		    NbPar, NomParam, NumPar, NumParHexa, NbBits );
	   }
       }
   return ( ACQ_OK );

}


/*****************************************************************************
 *                                                                           *
 *  Function  :                                                              *
 *    int acq_get_parbr_label(char *ParName, int *Label, int *NbBits)        *
 *                                                                           *
 *   Objet     : Retourne le Label et le nombre de bits du parametre         *
 *               en fonction de son nom.                                     *
 *                                                                           *
 *           Entree    : ParName -- Nom du parametre                         *
 *                                                                           *
 *           Retour    : Status -- Etat d'execution de la commande.          *
 *                              - 1 si parametre existe                      *
 *                              - 0 sinon                                    *
 *  
 *   Exemple :
 *     en C  
 *       int MonLabel, MonNbBits;
 *       if ( acq_get_parbr_label("MonADC", &MonLabel, &MonNbBits) == 1 )
 *          printf("parametre %s : Label = %d, NbBits = %s\n",
 *                 "MonADC", MonLabel, MonNbBits);
 *       else 
 *          printf("parametre %s n'existe pas\n", "MonADC");
 *
 *     en Fortran
 *       integer*4 acq_get_parbr_label
 *       external acq_get_parbr_label
 *       integer*4 MonLabel, MonNbBits
 *       if ( acq_get_parbr_label_for("MonADC"//char(0), MonLabel, MonNbBits) .eq. 1 )
 *       then
 *          print *,'parametre MonADC : Label = ', MonLabel, ', NbBits = ', NbBits
 *       else
 *          print *,'parametre MonADC n'existe pas'
 *       endif
 *   => Voir aussi interface dediee fortran dans ac_mt_mes_fonctions_f.for
 *
 *           Modifications :
 *                                                                           *
 *****************************************************************************/
int acq_get_parbr_label(char *ParName, int *Label, int *NbBits)
{
  int NumPar;
  int Status = 0;

  for(NumPar = 0; NumPar < NB_PAR_MAX && Status == 0; NumPar++)
    {
      if ( strcmp(ParName, StatParamTable[NumPar].ParName) == 0 )
	{
	  Status = 1;
	  *Label = NumPar;
	  *NbBits = StatParamTable[NumPar].ParNbBits;
	}
    }
  return Status;
}

/* point d'entree fortran */
int acq_get_parbr_label_for_(char *ParName, int *Label, int *NbBits)
{
 return acq_get_parbr_label(ParName, Label, NbBits);

}


/*****************************************************************************
 *                                                                           *
 *           Function  : traite_echelles                                     *
 *           Objet     : Traite un buffer d'echelles.                        *
 *                                                                           *
 *           Entree    : Buffer -- Buffer de structure IN2P3.                *
 *                                                                           *
 *           Retour    : Status -- Etat d'execution de la commande.          *
 *                              - ACQ_OK si aucun probleme.                  *
 *                              - ACQ_INVSCALERBUF si pas buffer echelle     *
 *                              - Code d'erreur VMS ou UNIX sinon.           *
 *                                                                           *
 *****************************************************************************/
#define NbPerLine 4
int traite_echelles	( in2p3_buffer_struct *Buffer ) {

  int i,j;
  int err = ACQ_OK;
  char Header[9];


  scale_struct *ScaleChannel = Buffer->les_donnees.cas.scale.jbus_scale.UnScale;

  strncpy(Header, Buffer->les_donnees.Ident, 8);
  Header[8]='\0';

  if ( strcmp ( Header , SCALER_Id ) != 0 ) 
    {
      err = ACQ_INVSCALERBUF;
    }

  else
    {
      printf ("\n>>> 'TRAITE_ECHELLES' est en cours.");
      printf("  Nbchannel = %d, Acq_status = %d\n", 
	     Buffer->les_donnees.cas.scale.Nb_channel,
	     Buffer->les_donnees.cas.scale.Acq_status);

 
      printf ("                Count    Freq |  Count    Freq |  Count    Freq |  Count    Freq |\n"); 

      for (i=0; i < Buffer->les_donnees.cas.scale.Nb_channel; i+=NbPerLine)
	{
	  printf ("Channel %3.1d : ",i); 
	  for (j=0; j < NbPerLine; j++)
	    {
	      printf ("%8.1d  %5.1d |", ScaleChannel->Count, ScaleChannel->Freq);
	      ScaleChannel++;
	    }
	  printf ("\n");
	}
    }

  return ( err );

} 

#endif
