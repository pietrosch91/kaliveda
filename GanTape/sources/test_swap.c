/*****************************************************************************
 *                                                                           *
 *       Projet       :  Bibiotheque GANIL_TAPE                              *
 *                                                                           *
 *       Programme    :  EXEMPLE_RELECTURE.c                                 *
 *                                                                           *
 *       Auteur       :  OPDEBECK                                            *
 *       Date         :  10 juillet 1996                                     *
 *                                                                           *
 *       Modifications:                                                      *
 *           B. Raine le 26 nov 97 : prise en compte des blocs EVENTCT       *
 *                       23 oct 98 : erreur test status                      *
 *                      				                     *
 *                                                                           *
 *       Objet        :  Utilitaire de relecture de bandes compilable sous   *
 *			VMS et sous UNIX.                                    *
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>		/* Entetes standard C	*/
#include <stdlib.h>
#include <string.h>

#include "gan_tape_param.h"	/* Fichier de parametres modifiables */
		                /* utilises par defaut 		     */

#include "gan_tape_alloc.h"  	/* Prototypes de la bibliotheque GANIL_TAPE */
#include "gan_tape_mount.h"
#include "gan_tape_file.h"
#include "gan_tape_test.h" 
#include "gan_tape_general.h"
#include "GEN_TYPE.H"           /* Definitions de types de donnees	*/

#include "acq_mt_fct_ganil.h" /* Prototypes bibliotheque EXEMPLE_RELECTURE */
#include "acq_mt_mes_fonctions.h"
#include "acq_ebyedat_get_next_event.h"

#include "ERR_GAN.H"	/* Definitions de messages pour RD_EVSTR.C et	*/
			/* S_EVCTRL.C .					*/

#include "gan_tape_get_parametres.h"/* Pour la recuperation des parametres */
				/* en ligne de commande ou environnement   */

#include "gan_tape_erreur.h"

#if defined ( __VMS ) || defined ( VMS ) /*******************************/

#include <unixlib.h>

char ChaineParam[]="\n"
"   Attention, vous devez passer le nom du lecteur de bande par variable \n"
"d'environnement; entrez la commande DCL : [ DEFINE DEVICE xxx ] ou xxx est\n" 
" le nom du device.\n"; 

/************************************************************************/

#elif defined ( __unix__ ) || ( __unix )  /******************************/


char ChaineParam[]="\n"
"   Attention, vous devez passer le nom du lecteur de bande par variable\n"
"d'environnement; entrez la commande UNIX : [ SETENV DEVICE xxx ou xxx est\n"
"le nom du device.\n"; 

#else 	/****************************************************************/
#error "Ce package n'est pas compatible avec votre OS"
#endif /*****************************************************************/
 

/****************************************************************************
	  Prototypes pour tests
 ****************************************************************************/
int mon_traitement_exogam(UNSINT16 *);


/****************************************************************************
	variables globales
 ****************************************************************************/

bool AutoswBuf,Swbufon;  /* Variables utilisees pour les parametres */
			/* provenant de la ligne de commande	*/

int BufSize;

/*declaration of dump
extern void dump(char *Buf, int Size);*/
/****************************************************************************
	main
 ****************************************************************************/

main ( int argc , char **argv ) {

	in2p3_buffer_struct Buffer;

	gan_tape_desc DEVICE;

	static char Struct_event[STRUCTEVENTSIZE];

/* Tableau declares en dynamique				*/
	static short int *EventBrut,*EventCtrl;	

	int Struct_event_size;
	int Status,Lun,RunNumber,EventNumber, Boucle, Choix;
        int CtrlForm;
	int Density,Evbsize,Evcsize;

	val_ret DeRetour;   /* UNION defini dans gan_tape_get_parametres.h */

	char NomDev[MAX_CARACTERES],*NomEnv,Header[9];
	
	bool Buffer_suivant,Event_suivant,EstAlloue,SousProcess=false;

/* On recupere les variables voulues ( toutes dans notre cas )	*/

/* Mais on recupere le lun en ligne de commande	*/
Status = acq_get_param_env ( lun_id, &DeRetour, argc, argv );
	if ( DeRetour.Val_INT >= 0 ) {
		Lun = DeRetour.Val_INT;
		SousProcess = true;
		puts ("\n >> Sous process en cours ...");
		}

Status = acq_get_param_env ( device_id, &DeRetour, argc, argv );
	if ( Status != ACQ_OK )	{
		printf ("\n>>> Nom du lecteur de bandes :");
		gets ( NomDev );
		if ( NomDev == NULL ) {
			puts ("\nChaine erronnee, veuillez recommencer ");
			exit (1);
			}
		strcpy ( DEVICE.DevName, NomDev );
	} 
	else 	
		strcpy ( DEVICE.DevName, DeRetour.Val_CAR );
	
	printf ("\nNom du Device : %s", DEVICE.DevName );
  

Status = acq_get_param_env ( ctrlform_id, &DeRetour, argc, argv );
	CtrlForm = DeRetour.Val_INT;
	printf ("\nValeur de GT_CTRLFORM : %s",CtrlForm == EVCT_VAR ? 	
					"EVCT_VAR":"EVCT_FIX"); 
        if ( Status == -1 ) printf ("  --  pris par defaut");
  
Status = acq_get_param_env ( autoswbuf_id, &DeRetour, argc, argv );
	AutoswBuf = DeRetour.Val_BOL;
	printf ("\nValeur de GT_AUTOSWAPBUF : %s",AutoswBuf == true ?
					"TRUE":"FALSE"); 
        if ( Status == -1 ) printf ("  --  pris par defaut");
		
Status = acq_get_param_env ( swbufon_id, &DeRetour, argc, argv );
	Swbufon = DeRetour.Val_BOL;
	printf ("\nValeur de GT_SWAPBUFON : %s",Swbufon == true ?
					"TRUE":"FALSE"); 
        if ( Status == -1 ) printf ("  --  pris par defaut");

Status = acq_get_param_env ( density_id, &DeRetour, argc, argv );
	Density = DeRetour.Val_INT;
	printf ("\nValeur de GT_DENSITY : %d", Density );
        if ( Status == -1 ) printf ("  --  pris par defaut");

Status = acq_get_param_env ( bufsize_id, &DeRetour, argc, argv );
	BufSize = DeRetour.Val_INT;
	printf ("\nValeur de GT_BUFSIZE : %d", BufSize );
        if ( Status == -1 ) printf ("  --  pris par defaut");

Status = acq_get_param_env ( evbsize_id, &DeRetour, argc, argv );
	Evbsize = DeRetour.Val_INT;
	printf ("\nValeur de GT_EVBSIZE : %d", Evbsize );
        if ( Status == -1 ) printf ("  --  pris par defaut");

Status = acq_get_param_env ( evcsize_id, &DeRetour, argc, argv );
	Evcsize = DeRetour.Val_INT;
	printf ("\nValeur de GT_EVCSIZE : %d", Evcsize );
        if ( Status == -1 ) printf ("  --  pris par defaut");
 
/* Il faut maintenant declarer les tableaux en dynamique	*/
EventBrut = CALLOC (Evbsize,short int);
if ( EventBrut == NULL ) {
	printf ("\n>>> Attention : Creation du buffer d'evt Brut impossible");
	exit (0);	
	}
EventCtrl = CALLOC (Evcsize,short int);
if ( EventCtrl == NULL ) {
	printf ("\n>>> Attention : Creation du buffer d'evt Ctrl impossible");
	exit (0);
	}

/* Attention, ici il faut tester si le device est alloue car si c'est le cas,
  il ne faut pas le dealloue a la fin ( pb de privilieges ).		*/	

if ( !SousProcess ) 
	{

	printf ("\n >> Initialisation du device %s ...",DEVICE.DevName);

	Status = acq_dev_is_alloc_c ( DEVICE );
	if ( Status == ACQ_OK ) EstAlloue = true;
	else if (( Status == ACQ_NOTALLOC ) || ( Status == ACQ_ISNOTATAPE )) 
		EstAlloue = false;
	else gan_tape_erreur ( Status, "test d'allocation");

/* Maintenant on alloue et on en profite pour tester si le lecteur appartient
  a quelqu'un d'autre.		*/
	Status = acq_mt_alloc_c ( DEVICE );
        if ( ( Status != ACQ_OK ) && (Status != ACQ_ISNOTATAPE ) ) 
	  {
	  if ( Status == ACQ_NOTALLOC ) 
		puts ("\n !! Ce lecteur est deja utilise.");
	  else gan_tape_erreur ( Status, "allocation");
	}

	if ( SousProcess == false ) 
	  {
	    Status = acq_mt_mount_c ( DEVICE , Density , BufSize ); 
	    if ( (Status != ACQ_OK) && (Status != ACQ_ISNOTATAPE ) 
		&& ( Status != ACQ_ALREADYMOUNT) )
	      gan_tape_erreur ( Status , "mount");
	    else if ( Status == ACQ_ALREADYMOUNT ) 
	      {
		puts ("\n\n>>> La bande que vous utilisez est deja montee");
		puts ("\n  > A-t-elle ete montee avant de lancer ce programme [y/n]");
		gets ( NomDev );
		if ( strcmp ( NomDev,"y" )==0 || strcmp ( NomDev,"Y")==0 )
		  {
		    puts ("\n... Fin du programme. Veuillez demonter cette bande.");
		    exit (0);
		  }
		else 
		  puts ("\n... Suite du programme .");
	      }
	    else if ( Status == ACQ_ISNOTATAPE )
	      Status = ACQ_OK;
			
	  }
        
	if ( Status != ACQ_OK ) {
		puts ("\n>>> Erreur d'init du device.\n>>> Fin ...\n");
  		exit (1);
	      }
	else printf ("\n>>> Device %s ready.",DEVICE.DevName );
	/* Ici, la bande est allouee ,montee et ouverte  ....	*/   
}	 

	if ( SousProcess == false || Lun <= 0 ) {
		      Status = acq_mt_open_c ( &DEVICE , o_read , &Lun );
		      if ( Status != ACQ_OK ) {
				gan_tape_erreur ( Status , "open");
                                exit(1);
                        }
		}


	RunNumber = 0;
        Struct_event_size = STRUCTEVENTSIZE;

        /* Initialisation du contexte du run */
	Status = acq_mt_ini_run_c ( Lun, &Buffer, BufSize, &RunNumber,
		Struct_event, Struct_event_size );

        if ( Status != ACQ_OK ) {
	  gan_tape_erreur (Status , "Init Run");
	}
	else {
	  printf ("\n>>> Ouverture ok pour run %d ......",RunNumber);
	}


	/* Debut de traitement d'exemple relecture	*/
	/* =======================================      */

	if ( Status == ACQ_OK )
		Buffer_suivant = true;
	else Buffer_suivant = false;


	while ( Buffer_suivant ) {
	  
	  Status = acq_mt_read_c ( Lun, Buffer.Buffer, &BufSize );
	  if ( Status == ACQ_OK ) {
	    printf("apres swap \n");
	    dump((char *)&Buffer, 128);
	  }
	  else if ( Status == ACQ_ENDOFFILE ) {
	     Buffer_suivant = false;
	     printf("\n Fin du fichier");
	   }
	  else {
	     Buffer_suivant = false;
	     gan_tape_erreur ( Status, "lecture du fichier");
	   }
		
	}


if ( SousProcess == false ) {


/* Si on ne fait rien d'autre, on rembobine la bande		*/
	puts ("\n>>> Rewind ......\n");
	Status = acq_mt_rewind_c ( DEVICE );
	if ( ( Status != ACQ_OK ) && ( Status != ACQ_ISNOTATAPE ) )
		gan_tape_erreur ( Status, "rewind ");

	Status = acq_mt_close_c ( DEVICE );
	if ( Status != ACQ_OK ) 
		gan_tape_erreur ( Status ,"fermeture de fichier");
	
/*		Pour traiter un autre run, il faut reprendre les operations 
 *	depuis l'ouverture du fichier .... sans rembobiner la bande .
 */
	
	/* UNLOAD   == decharge la bande			*/
	/* NOUNLOAD == conserve la bande dans le derouleur	*/
	Status = acq_mt_dismount_c ( DEVICE , NOUNLOAD );
	if ( ( Status != ACQ_OK ) && ( Status != ACQ_ISNOTATAPE ) )
		gan_tape_erreur ( Status, "demontage de la bande ");

/* On ne dealloue la bande que si elle ne l'etait pas au debut ... */
	if ( EstAlloue == false ) {
	Status = acq_mt_dealloc_c ( DEVICE );
	if ( ( Status != ACQ_OK ) && ( Status != ACQ_ISNOTATAPE ) )
		gan_tape_erreur ( Status, "deallocation de la bande ");
	}
}
else puts ("\n >> Fin du process Exemple_relecture.exe .... ");

/* Et on s'en va ...................	*/
	exit(0);	

}

/************************************  FIN ***********************************/
