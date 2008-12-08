/*****************************************************************************
 *****************************************************************************
 *                                                                           *
 *       Projet       :  Bibiotheque GANIL_TAPE                              *
 *                                                                           *
 *       Programme    :  TEST_BANDES.c                                       *
 *                                                                           *
 *       Auteur       :  OPDEBECK                                            *
 *       Date         :  27 juin 1996                                        *
 *                                                                           *
 *       Objet        :  Programme de test permettant de verifier que le     *
 *			contenu des bandes est correcte lors du passage de   *
 *			VMS a UNIX et de UNIX a VMS.                         *
 *                                                                           *
 *****************************************************************************
 *****************************************************************************/


#include <stdlib.h>
#include <stdio.h>    
#include <string.h>

#include "gan_tape_alloc.h"  
#include "gan_tape_mount.h"
#include "gan_tape_file.h"
#include "gan_tape_test.h" 
#include "general.h"        
#include "param.h" 
                      
#if defined ( __unix__ ) || (__unix)              /***** Cas de UNIX *****/

#include "def_err.h"       
#define DEVICE Lun
                                 
#endif  /************************************************************/ 


#if defined ( __VMS ) || defined ( VMS )     /***** Cas de VMS ******/

#include <unixio.h>
#include "acq_codes_erreur.h"      
#define DEVICE NomDev
 
#endif  /************************************************************/   


/*****************************************************************************
	main 
 *****************************************************************************/


main ( void ) {

	char NomDev[MAX_CARACTERES],ChaineRetour[MAX_CARACTERES];
	char Buffer[BUFSIZE],BufLect[BUFSIZE];
	int Status,Lun,Choix,LunTempo,Boucle,WritBuf,Taille;
	bool Suite=true;

	char Menu[]="\n\n--- Programme de test d'ecriture sur bande ---\n\n"
"1- Initialisation d'une bande.\n2- Ecriture d'une procedure de test sur une"
" bande.\n3- Tester le contenu.\n4- Sortir.\n";

	while ( Suite == true ) {
           puts ( Menu );
	   printf ("\n>>> Choix :");
	   scanf ("%d",&Choix);
	   switch ( Choix ) {

/***************************************************************************/
	   case 1 : 
		puts ("\n>>> Initialisation de la bande...");
		printf ("\n >> Nom du device : ");		
		scanf ("  %s",NomDev);

#if defined (__VMS) || defined (VMS)
		Status = acq_mt_alloc_c ( DEVICE );
		if ( Status != ACQ_OK ) {
			puts ("\n>>> Erreur d'allocation.");
			ERREUR ( Status,"/nErreur d'allocation");
		}
		else puts ("\n>>> Lecteur alloue ...");
		Status = acq_mt_mount_c ( DEVICE , DENSITY , BUFSIZE );
		if ( Status != ACQ_OK ) {
			puts ("\n>>> Erreur de montage.");
			ERREUR ( Status,"/nErreur de montage");
		}
		else puts ("\n>>> Lecteur monte ...");
#endif
   
/*		Status = acq_mt_open_c ( NomDev , o_read , &Lun );
		if ( Status != ACQ_OK ) {
			puts ("\n>>> Erreur d'ouverture.");
			ERREUR ( Status,"/nErreur d'ouverture");
		}
		else puts ("\n>>> Bande ouverte ...");   
*/
/* Inutile car reouverture necessaire en cas d'ecriture */
		break;

/****************************************************************************/

	   case 2 :
		
		puts ("\n>>> Debut de la sequence de test ...");
		Status = acq_mt_open_c ( NomDev, o_write, &Lun );
		if ( Status != ACQ_OK ) {
			puts ("\n>>> Erreur d'ouverture.");
			ERREUR ( Status,"/nErreur d'ouverture"); 
		}
		else puts ("\n>>> Bande ouverte ..."); 		

/* La sequence de test consiste a ecrite plusieurs blocs sur la bande. Les 
donnees ecrites seront les lettres de l'alphabet. */
		
		for ( Boucle = 0; Boucle < 26; Boucle++ ) {
		    for ( WritBuf=0; WritBuf < BUFSIZE; WritBuf++ ) 
			Buffer[WritBuf]='A'+Boucle;
		    Status = acq_mt_write_c ( Lun , Buffer , BUFSIZE );		
		    if ( Status != ACQ_OK ) {
			puts ("\n>>> Erreur d'ecriture sur la bande");
			ERREUR ( Status,"/nErreur d'ecriture"); 
		    }
		}

		Status = acq_mt_close_c ( Lun );
	        break;

/****************************************************************************/

	   case 3 :

/* La sequence de test consiste a lire les blocs et a verifier le contenu
d'apres ce qui a ete ecrit */                         	

		puts ("\n>>> Debut de la sequence de comparaison ...");
		Status = acq_mt_open_c ( NomDev, o_read, &Lun );
		if ( Status != ACQ_OK ) {
			puts ("\n>>> Erreur d'ouverture.");
			ERREUR ( Status,"/nErreur d'ouverture"); 
		}
		else puts ("\n>>> Bande ouverte ..."); 

		for ( Boucle = 0; Boucle < 26; Boucle++ ) {
		    for ( WritBuf=0; WritBuf < BUFSIZE; WritBuf++ ) 
			Buffer[WritBuf]='A'+Boucle;
		    Taille = BUFSIZE;
		    Status = acq_mt_read_c ( Lun, BufLect, &Taille );
		    if ( Status != ACQ_OK ) {
			puts ("\n>>> Erreur de lecture sur la bande.");
		   	ERREUR ( Status,"/nErreur de lecture"); 
              	    }
		    Status = 0;
		    for ( WritBuf=0; WritBuf < BUFSIZE; WritBuf++ ) 
                        if ( Buffer[WritBuf] != BufLect[WritBuf] )
				Status ++; 
		    if ( Status != 0 ) {
			printf ("\n >> Bloc no %d : erreur de"
				"correspondance ...",Boucle );
			ERREUR (Status,"\nErreur de correspondance");
		    }
		    else printf ("\n>>> Bloc no %d correct",Boucle );
		}

		Status = acq_mt_close_c ( Lun );
		break;

/****************************************************************************/

	   case 4 :

		Status = acq_mt_close_c ( Lun );
		Status = acq_mt_dismount_c ( DEVICE, 2 );
		Suite = false;
                break;
		
		}

	}
}


