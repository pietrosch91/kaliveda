/*****************************************************************************
 *                                                                           *
 *           Function  : mon_traitement         B. Raine Juin 99             *
 *
 *           Objet     : Traitement personnel du buffer d'evenement.         *
 *                                                                           *
 *           Entree    : Event -- Buffer d'evenement a traiter.              *
 *                                                                           *
 *           Retour    : Status -- Etat d'execution de la commande.          *
 *                              - ACQ_OK si aucun probleme.                  *
 *                              - Code d'erreur VMS ou UNIX sinon.           *
 *
 * Cet exemple de traitement dump un evenemenent sur 0x10000 et cree une 
 * statistque sur le nombre de coups par parametre
 * Le point d'entree mon_traitement_exogam a ete utilise pour les tests de
 * mise au point de la routine acq_ebyedat_get_next_event.c et permet de 
 * dumper l'evenement brut exogam correspondant a l'evenement de control 
 * precedemment dumpe par mon_traitement grace a la variable static OnDump
 *                                                                           *
 *****************************************************************************/ 

#include <stdio.h>

#include "GEN_TYPE.H"
#include "STR_EVT.H"
#include "gan_acq_buf.h"

#include "gan_tape_erreur.h"
#include "gan_tape_param.h"
#include "acq_mt_mes_fonctions.h"

void dump(char *p, int nb);

static int OnDump;
static int MyCpt = 0;

int mon_traitement 	( short int *Event ) {

  CTRL_EVENT *pCtrlEvent;
  UNSINT16 * CurPar;   /* Pointeur courant sur les parametres */
  UNSINT16 * EvtEnd;
  int        NumPar;   /* Numero de parametre courant */
  int        NbPar;    /* Nombre de parametres pour traitement en fixe */
  int nError=0;

  int eventLength;
  int k,l;
  UNSINT16 *brutData;
  
  pCtrlEvent = (CTRL_EVENT *)Event;
 
	/*	if ( (pCtrlEvent->ct_nb % 1000) == 0 ){
	 */

  if ( (MyCpt % 0x10000) == 0 ){
    printf ("\n>>> 'MON_TRAITEMENT' dump evenement de control num 0x%x de longueur %d.\n",
	    pCtrlEvent->ct_nb, pCtrlEvent->ct_len);
    OnDump = TRUE;
    dump((char *)Event, pCtrlEvent->ct_len*2);

    { /* B. Raine 13 tests de acq_get_parbr_label 
        int MonLabel, MonNbBits;
	char *MonADC = "V550_2_239_";
        if ( acq_get_parbr_label(MonADC, &MonLabel, &MonNbBits) == 1 )
           printf("parametre %s : Label = %d, NbBits = %d\n",
                  MonADC, MonLabel, MonNbBits);
        else 
           printf("parametre %s n'existe pas\n", MonADC);
      */
    }
  } 
  else
    OnDump=FALSE;
  
  MyCpt++;

  if ( StatParamTable != 0 )
    {/* Faire statistique sur nb coups par parametres */
      EvtEnd = (UNSINT16 *)pCtrlEvent + pCtrlEvent->ct_len;
      CurPar = (UNSINT16 *)&pCtrlEvent->ct_par;

      if ( CTRLFORM == EVCT_VAR )
	{
	  /* Evt de control variable */
	  while ( CurPar < EvtEnd )
	    {
	      NumPar = *CurPar++;
	      if ( NumPar > NumParMax )
		{
		  printf ("\n>>> 'MON_TRAITEMENT' evenement 0x%x de longueur %d.\n",
			  pCtrlEvent->ct_nb, pCtrlEvent->ct_len);
		  printf("           Numero de parametre %d(0x%x) > NumParMax(%d)\n", 
			 NumPar, NumPar, NumParMax);
		}
	      else
		StatParamTable[NumPar].ParNbCoups++;
	      CurPar++;
	    }

	  eventLength = pCtrlEvent->ct_len;
	  nError=0;
	  brutData = (UNSINT16 *)&pCtrlEvent->ct_par;
	  for(k=0;k<eventLength-6-2;k+=2)
	    {       
	      for(l=k+2;l<eventLength-6;l+=2)
	      {
		if(brutData[k] == brutData[l])
		  {
		    printf ("Found ??? Data[%d]=Data[%d]=0x%x\n",k,l,brutData[k]);
		    nError++;
		  }
	      }
	    } 
	  if ( nError != 0)
	    {
	      OnDump = TRUE;
	      printf ("\n>>> 'MON_TRAITEMENT' dump evenement de control num 0x%x de longueur %d.\n",
		      pCtrlEvent->ct_nb, pCtrlEvent->ct_len);
	      printf("nError = %d\n", nError);
	      dump((char *)Event, pCtrlEvent->ct_len*2);
	    }
	}
      else  if ( CTRLFORM == EVCT_FIX )
	{ /* Evt de control fixe */
	  NbPar = pCtrlEvent->ct_len - sizeof(CTRL_EVENT)/2 -2;
	  if ( NbPar > NumParMax )
	    {
	      printf ("\n>>> 'MON_TRAITEMENT' evenement 0x%x de longueur %d.\n",
		      pCtrlEvent->ct_nb, pCtrlEvent->ct_len);
	      printf("          Nombres de parametres %d(0x%x) > NumParMax(%d)\n", 
		     NbPar, NbPar, NumParMax);
	    }
	  else
	    for ( NumPar = 1; NumPar <= NbPar; NumPar++ )
	      {
		if ( *CurPar != 0xffff )
		  StatParamTable[NumPar].ParNbCoups++;
	      }
	}
    }


     return ( ACQ_OK );
                                             
}

int mon_traitement_exogam 	( short int *Event ) {

   EBYEDAT_EVENT_HD *pEvent;

        pEvent = (EBYEDAT_EVENT_HD *)Event;
 
	if ( OnDump ){
	  printf ("\n>>> 'MON_TRAITEMENT_EXOGAM' evenement %x de longueur %d.\n",
                  pEvent->num_lower, pEvent->length);
	  dump((char *)Event, pEvent->length*2);
	} 

        return ( ACQ_OK );
                                             
}


void AfficheStat()
{
  int NumPar;
  char NumParHexa[30];

  if ( StatParamTable != 0 )
    {/* Afficher statistiques sur nb coups par parametres */
      printf("\nStatistiques : pour %d evenements\n", MyCpt);
      printf("\n   Nom parametres            Label         Nombre de coups\n");
      for ( NumPar = 0; NumPar <= NumParMax; NumPar++ )
	if ( StatParamTable[NumPar].ParNbCoups != 0 )
	  {
	    sprintf(NumParHexa,"(0x%x)", NumPar);
	    printf("%2d %-20s %6d %-10s %10d \n",
		   StatParamTable[NumPar].ParRang, 
		   StatParamTable[NumPar].ParName, 
		   NumPar, NumParHexa,
		   StatParamTable[NumPar].ParNbCoups);
	  }
    }
  else
    printf("Pas de table de statistiques \n");
}
