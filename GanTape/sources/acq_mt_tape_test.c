/*****************************************************************************
 *                                                                           *
 *       Projet       :  Bibiotheque GANIL_TAPE                              *
 *                                                                           *
 *       Programme    :  ACQ_MT_TAPE_TEST.c                                  *
 *                                                                           *
 *       Auteur       :  OPDEBECK                                            *
 *       Date         :  15 avril 1996                                       *
 *                       Modifie pour UNIX le 13 juin 1996                   *
 *                                                                           *
 *       Objet        :  Essais de la bibliotheque de gestion des bandes.    *
 *                                                                           *
 *       Modifications :
 *           B. Raine le 23 sep 98
 *             dans case 15 :
 *                ifdef vax devient ifdef VMS
 *                remplace PassParam2+7 par PassParam2+strlen(PassParam2)
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GEN_TYPE.H"

#include "gan_tape_alloc.h"   /* Prototypes de la bibliotheque    */
#include "gan_tape_mount.h"   /**/
#include "gan_tape_file.h" /**/
#include "gan_tape_test.h" /**/
#include "gan_tape_general.h"

#include "gan_tape_param.h"         /* Fichier de parametres a modifier */

#include "gan_tape_erreur.h"

#if defined ( __unix__ ) || defined ( unix )

#include <unistd.h>
#if  defined ( __hpux )
#include <vfork.h>
#endif

#include <sys/types.h>
#include <sys/wait.h>
#endif

#if defined ( __VMS ) || defined ( VMS )

#include <processes.h>
#include <unixio.h>
#include <unistd.h>
#include <wait.h>

#endif

/*****************************************************************************
   Variables globales
*****************************************************************************/

bool Swbufon, AutoswBuf;
int BufSize;


/*****************************************************************************
   main
 *****************************************************************************/


main(int argc , char** argv)
{


   void EntierToChaine(int, char*);

   gan_tape_desc DEVICE;
   gan_tape_desc WrFile;

   char NomDev[MAX_CARACTERES], ChaineRetour[MAX_CARACTERES], NomOpen[MAX_CARACTERES];
   char Buffer[BUFSIZE];
   char PassParam1[MAX_CARACTERES], PassParam2[MAX_CARACTERES];
   int Lun = 0, LunOpen = 0;
   int Status, Choix, Boucle;
   bool Init = false, Wr_Open = false;


   char Menu[] = "\n\n\nActions de test proposees :\n  1- Type de device "
                 "(bande/disque,nom,pret).\n  2- Allocation d'un device.\n  3- Deallocation d'un"
                 " device.\n  4- Montage d'un device.\n  5- Demontage d'un device.\n  6- Ouver"
                 "ture d'un fichier.\n  7- Fermeture d'un fichier.\n  8- Lecture dans un "
                 "fichier.\n  9- Ecriture dans un fichier.\n  10- Initialise une lecture.\n"
                 "  11- Re-embobine la bande.\n  12- Skip files.\n  13- Skip blocks.\n"
                 "  14- Skip to EOT.\n\n 15- Lancer Exemple_relecture (parametres par defaut)\n\n"
                 ">>> Attention, les actions sur skip bloc risquent de creer des erreurs lors du\n"
                 " >> lancement d'exemple_relcture ...\n";

   Swbufon = SwapBufOn;
   AutoswBuf = AutoSwapBuf;
   BufSize = BUFSIZE;

   printf("\n>>>Nom du device ");
   scanf("%s", DEVICE.DevName);
   strcpy(NomDev, DEVICE.DevName);

   while (true) {
      puts(Menu);

#if defined ( __unix__ ) || ( __unix )
      puts("\n\n!!!!!!!  Il est necessaire d'ouvrir la bande avant toute"
           " action  !!!!!!!!");
#endif

      printf("\nChoix :");
      scanf("%d", &Choix);
      switch (Choix) {

         /*****************************************************************************/

         case 1 :
            Status = acq_dev_is_tape_c(DEVICE);

            if (Status == ACQ_OK) {
               printf("\n >>' %s ' est une bande", DEVICE.DevName);
               Status = acq_get_tape_type_c(DEVICE, ChaineRetour);
               printf("\n >>Le device ' %s ' est un %s....",
                      NomDev, ChaineRetour);
               if (Status != ACQ_OK)
                  gan_tape_erreur(Status, "Test de bande");
               Status = acq_real_dev_name_c(DEVICE , ChaineRetour);
               printf("\n >>Son nom physique est %s...", ChaineRetour);
               if (Status != ACQ_OK)
                  gan_tape_erreur(Status, "real_dev_name");

            } else if (Status == ACQ_ISNOTATAPE)
               printf("\n >>' %s ' est un disque", NomDev);
            else
               gan_tape_erreur(Status, "dev_is_tape");

            break;

         /*****************************************************************************/

         case 2 :
            printf("\n>>>Allocation de %s", NomDev);
            Status = acq_mt_alloc_c(DEVICE);
            if (Status == ACQ_OK)
               printf("\n >>' %s ' est alloue ......", NomDev);
            else if (Status == ACQ_ISNOTATAPE)
               printf("\n >>' %s ' est un disque ...", NomDev);
            else if (Status == ACQ_ALRALLOC)
               printf("\n >>Attention ' %s ' est deja alloue"
                      , NomDev);
            else
               gan_tape_erreur(Status, "allocation");

            break;

         /******************************************************************************/

         case 3 :
            printf("\n>>>De-allocation de %s", NomDev);
            Status = acq_mt_dealloc_c(DEVICE);
            if (Status == ACQ_OK)
               printf("\n >>' %s ' est de-alloue ......", NomDev);
            else if (Status == ACQ_ISNOTATAPE)
               printf("\n >>' %s ' est un disque ...", NomDev);
            else
               gan_tape_erreur(Status, "de-allocation");

            break;

         /*****************************************************************************/

         case 4 :
            Status = acq_mt_mount_c(DEVICE , DENSITY , BUFSIZE);

            if (Status == ACQ_OK) {
               printf("\n >>' %s ' est monte ......", NomDev);
               Status = acq_dev_is_wr_protect_c(DEVICE);
               if (Status == ACQ_DEVWRITLOCK)
                  printf("\n >>' %s ' est protege en ecriture",
                         NomDev);
               else printf("\n >>' %s ' est Read/Write", NomDev);
            } else if (Status == ACQ_ISNOTATAPE)
               printf("\n >>' %s ' est un disque ..", NomDev);
            else if (Status == ACQ_ALREADYMOUNT ||
                     Status == ACQ_NOTALLOC)
               printf("\n >>!! Bande non allouee ou deja montee ..");
            else
               gan_tape_erreur(Status, "montage");

            break;

         /*****************************************************************************/

         case 5 :
            printf("\n  >Ejecter (-1-)\n  >Ne pas ejecter (-2-) :");
            scanf("%d", &Choix);

            Status = acq_mt_dismount_c(DEVICE , Choix);

            if (Status == ACQ_OK)
               printf("\n >>' %s ' est demonte ......", NomDev);
            else if (Status == ACQ_ISNOTATAPE)
               printf("\n >>' %s ' est un disque ..", NomDev);
            else
               gan_tape_erreur(Status, "demontage");

            break;

         /*****************************************************************************/

         case 6 :
            printf("\n>>>Mode ( 1- Read , 2- Write ) :");
            scanf("%d", &Choix);

            enum MODE_R_W _Choix;
            if (Choix == 1) _Choix = o_read;
            else if (Choix == 2) _Choix = o_write;
            else _Choix = o_read;  /* Par defaut en lecture   */ /*GARP*/

            Status = acq_mt_open_c(&DEVICE , _Choix , &Lun);
            if (Status == ACQ_OK) {
               printf("\n>>>Le fichier - %s - est ouvert ......", NomDev);
               Init = true;
               DEVICE.Lun = Lun;
            } else
               gan_tape_erreur(Status, "open");

            break;

         /*****************************************************************************/

         case 7 :
            printf("\n>>>Fermeture du dernier fichier ouvert");

            Status = acq_mt_close_c(DEVICE);

            if (Status == ACQ_OK) {
               printf("\n >>Le fichier est ferme ......");
               Init = false ;
            } else
               gan_tape_erreur(Status, "fermeture bande");

            if (Wr_Open == true)  {
               Status = acq_mt_close_c(WrFile);

               if (Status == ACQ_OK) {
                  printf("\n >>Le fichier est ferme ......");
                  Wr_Open = false ;
               }

               else
                  gan_tape_erreur(Status, "fermeture fichier disque");

            }

            break;

         /*****************************************************************************/

         case 8 :
            printf("\n>>>Lecture d'un bloc de donnees dans le fichier : %s",
                   NomDev);

            if (Init == true) {

               int Taille = BUFSIZE;

               Status = acq_mt_read_c(Lun , Buffer , &Taille);
               /* Le nombre d'octets lus est renvoye dans Taille */

               if (Status == ACQ_OK) {
                  printf("\n >>Lecture correcte ......\n");
                  puts(" >> Quelle taille de buffer voulez-vous afficher :");
                  scanf("%d", &Choix);
                  write(2, Buffer, Choix); /* Affiche le buffer   */
                  printf("[EOB]\n >> %d caracteres lus .... ", Taille);
               } else
                  gan_tape_erreur(Status, "Test de bande");


            } else printf("\nFichier non initialise ..........");

            break;

         /*****************************************************************************/

         case 9 :
            printf("\n>>>Ecriture d'un buffer plein !!!");

            if (Wr_Open != true) {   /* Teste si le fichier est ouvert */

               printf("\n>>>Fichier a ouvrir en ecriture :");
               scanf("%s", WrFile.DevName);
               Status = acq_mt_open_c(&WrFile , o_write , &LunOpen);
               if (Status == ACQ_OK) {
                  if (WrFile.Is_protected == false) {
                     printf("\n >>Ouverture correcte ......");
                     Wr_Open = true;
                  } else if (WrFile.Is_protected == true)
                     printf("\n >>Fichier protege en ecriture...");
               } else gan_tape_erreur(Status, "open wr file");

            }
            if (Status == ACQ_OK) {
               Status = acq_mt_write_c(LunOpen , Buffer , BUFSIZE);
               if (Status == ACQ_OK)
                  printf("\n >>Ecriture correcte ......");
               else printf("\n >>Erreur d'ecriture ...........");
            } else  Status = acq_mt_close_c(WrFile);

            break;

         /*****************************************************************************/

         case 10 :
            printf("\n>>>Initialise un lecteur .........\n >> Allocation, Montage, Ouverture ...");

            Status = acq_mt_alloc_c(DEVICE);

            if (Status == ACQ_OK)
               printf("\n >>' %s ' est alloue ......", DEVICE.DevName);
            else
               gan_tape_erreur(Status, "allocation");

            Status = acq_mt_mount_c(DEVICE , DENSITY , BUFSIZE);

            if (Status == ACQ_OK)
               printf("\n >>' %s ' est monte ......", DEVICE.DevName);
            else
               gan_tape_erreur(Status, "montage");


            printf("\nMode d'ouverture ( 1- Read , 2- Write ) :");
            scanf("%d", &Choix);

            if (Choix == 1) _Choix = o_read;
            else if (Choix == 2) _Choix = o_write;
            else _Choix == o_read;

            Status = acq_mt_open_c(&DEVICE , _Choix , &Lun);
            if (Status == ACQ_OK) {
               printf("\n >>Le fichier ' %s ' est ouvert ......", DEVICE.DevName);
               Init = true;
            } else
               gan_tape_erreur(Status, "ouverture");


            break;

         /*****************************************************************************/

         case 11  :
            printf("\n>>>Request to rewind tape ....");
            Status = acq_mt_rewind_c(DEVICE);

            if (Status == ACQ_OK) puts("\n >> Ok...");
            else if (Status == ACQ_ISNOTATAPE)
               printf("\n >> !! ' %s ' est un disque", NomDev);
            else
               gan_tape_erreur(Status, "rewind");

            break;

         /*****************************************************************************/

         case 12   :
            printf("\n>>>Nombre de fichiers a passer ? ...");
            scanf("%d", &Boucle);
            printf("\n>>>Request to skip %d files on %s", Boucle, NomDev);
            Status = acq_mt_skip_file_c(DEVICE , Boucle);

            if (Status == ACQ_OK || Status == ACQ_ENDOFTAPE)
               puts("\n >> Ok...");
            else if (Status == ACQ_ISNOTATAPE)
               printf("\n >> !! ' %s ' est un disque", NomDev);
            else
               gan_tape_erreur(Status, "skip file");

            break;

         /*****************************************************************************/

         case 13 :
            printf("\n>>>Nombre de blocks a passer ? ...");
            scanf("%d", &Boucle);
            printf("\n>>>Request to skip %d blocs on %s", Boucle, NomDev);
            Status = acq_mt_skip_block_c(DEVICE , Boucle);

            if (Status == ACQ_OK || Status == ACQ_ENDOFFILE)
               puts("\n>> Ok...");
            else if (Status == ACQ_ISNOTATAPE)
               printf("\n >> !! ' %s ' est un disque", NomDev);
            else
               gan_tape_erreur(Status, "skip block");

            break;

         /*****************************************************************************/

         case 14  :
            printf("\n>>>Request to skip to EOT ... ");
            Status = acq_mt_skip_to_eot_c(DEVICE , &Boucle);
            if (Status == ACQ_OK) {
               puts("\n >> EOT Ok ...");
               printf("\n >> %d files skipped", Boucle);
            } else if (Status == ACQ_ISNOTATAPE)
               printf("\n >> !! ' %s ' est un disque", NomDev);
            else
               gan_tape_erreur(Status, "skip to EOT");

            break;

         /*****************************************************************************/

         case 15  :
            printf("\n>>> Launch 'Exemple_relecture'... ");

            Status = vfork();

            if (Status == -1)
               gan_tape_erreur(Status, "creation d'un processus");

            else if (Status == 0) {

               strcpy(PassParam1, "gt_device=");
               strncat(PassParam1, DEVICE.DevName, MAX_CARACTERES - 15);

#if defined ( VMS ) || defined ( __VMS )
               /* En passant Lun=0, on indique a exemple_relecture que la bande est montee
                mais pas ouverte */
               strcpy(PassParam2, "gt_lun=0");
#else
               strcpy(PassParam2, "gt_lun=");
               EntierToChaine(DEVICE.Lun,
                              PassParam2 + strlen(PassParam2));
#endif

               puts("\n >> Process clone en cours.");
               Status = execl("exemple_relecture.exe",
                              "exemple_relecture.exe", PassParam1,
                              PassParam2 , NULL);
               if (Status < 0)  {
                  Status = ACQ_ERRPARAM;
                  gan_tape_erreur(Status, "Exemple_relecture");
               }
            } else {
               int n, m;
               puts("\n >> Process pere en cours");
               m = wait(&n);
            }
            break;

         /*****************************************************************************/

         default :
            puts("\nBye ..............");
            exit(0);

            break;
      }

   }
}


/*****************************************************************************/


void EntierToChaine(int LeEntier, char* ChRet)
{

   int Modulo, Divis, Compt = 0, Index = 1, IndexDix = 1;

   Modulo = LeEntier;
   while ((Divis = (Modulo / 10)) != 0) {
      Index++;
      IndexDix *= 10;
      Modulo = Divis;
   }
   do {

      Divis = LeEntier / IndexDix;
      Modulo = LeEntier % IndexDix;

      if (Divis != 0  && (Divis / 10) == 0) {
         ChRet[Compt] = (char)(Divis + 48);
         LeEntier = Modulo;
      }

      else if (Modulo != 0) {
         ChRet[Compt] = (char)(Modulo + 48);
         LeEntier = 0;
      } else ChRet[Compt] = '\0';

      Index--;
      IndexDix /= 10; /* ? 1 : IndexDix / 10*/
      Compt++;

   } while ((Modulo != 0 || Divis != 0) &&  Index != 0);
}


/******************************** END  ***************************************/
