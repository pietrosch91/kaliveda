   Ce package permet de relire une bande acquisition GANIL contenant des 
evenements bruts et de reconstruire des evenements de type control GANIL. 
La version 1.3 permet de lire des buffers EXOGAM de type EBYEDAT.
Une routine et un programme de test de relecture des spectres ganil sont 
egalement disponibles dans ce package


   Un programme exemple_relecture est fourni pour vous guider dans l'ecriture 
de votre propre application. 

   Ce package a ete teste sous VMS pour vax et alpha et sous UNIX pour
sun/solaris, alpha/osf, hp, linux

Recuperation du package
-----------------------
   recuper le fichier gan_tape_x_x.tar.gz
   le decompresser par les commandes :
	gunzip gan_tape_x_x.tar.gz
	tar -xvf gan_tape_x_x.tar

   Vous obtenez un arborescence de la forme :
      Repertoire courant ---|
                            |-> doc
                            |-> sources
                            |-> include
   Le repertoire courant contient les fichiers de commande et makefile 
necessaires a la generation de exemple_relecture
 
   Pour regenerer l'executable de exemple_relecture, sur une machine UNIX, 
taper make_xxx (avec xxx = osf, sun, hp, linux_i386 en fonction de 
l'environnement sur lequel vous travaillez) ou make_xxx_fortran si vous voulez
utiliser une fonction de traitement en fortran (cf §Exemple_relecture). Deux 
repertoires sont crees automatiquement :
      xxx_lib contenant les fichiers objet
      xxx_exe contenant les executables.

   Pour regenerer l'executable de exemple_relecture, sur une machine VMS,
taper @make_vms ou make_vms_fortran si vous voulez utiliser une fonction de 
traitement en fortran (cf §Exemple_relecture) sur la racine de l'arbre cree a 
l'installation du package. Deux repertoires sont crees automatiquement :
      vms_yyy_lib contenant les fichiers objet
      vms_yyy_exe contenant les executables  
avec yyy = alpha ou vax en fonction de la machine sur laquelle vous etes.



Exemple_relecture
------------------
Le programme exemple_relecture appelle des routines de traitement pour les 
differents types de buffers et la routine "mon_traitement" pour le traitement 
des evenements de control. Il suffit d'ecrire ces routines a votre sauce pour 
obtenir des traitements specifiques. L'ensemble de ces routines est contenu
dans le fichier acq_mt_mes_fonctions.c, exceptee "mon_traitement" contenue dans
le fichier mon_traitement.c ou mon_traitement.f

ATTENTION : pour utiliser une fonction mon_traitement.f, il faut 
            utiliser les scripts make_xxx_fortran au lieu de make_xxx sans 
            rien modifier dans exemple_relecture.c


Ce programme admet en argument un certain nombre de parametres de la forme : 
NomParam = valeur. Les NomParam ainsi definis permettent de modifier le 
contexte du programme. Il est possible aussi de passer par des variables 
d'environnement du meme nom. 
   ex.: exemple_relecture gt_device=MonFichierData, gt_ctrlform=evct_fix

   Le fichier a lire (disque ou bande) est defini par le parametre ou la 
variable d'environnememt gt_device. Si cette variable n'est pas definie, le 
nom de fichier a lire sera demande au clavier au demarrage du programme.

   Vous pouvez configurer le type d'evenement de control desire (FIXE ou 
VARIABLE en agissant sur la macro CTRLFORM dans le fichier gan_tape_param.h 
ou sur le parametre ou la variable d'environnement gc_ctrlform.

   ATTENTION, dans cette version la routine  acq_ebyedat_get_next_event.c ne sait
pas reconstruire des evenements de control FIXE. Par contre, elle sait pointer
sur un debut d'evenement sans effectuer aucun traitement. Voir son mode d'emploi
dans l'entete du source.

   Le swapping des mots est assure en fonction des types de buffer et de 
machine (big ou little indian). Par defaut ce swap est realise automatiquement.
Vous pouvez modifier la liste des machines little indian en jouant sur la 
definition de la macros Machine_LSB dans gan_tape_param.h.
Vous pouvez agir sur les options de swap en jouant sur les macros AutoSwapBuf 
et SwapBufOn dans le fichier gan_tape_param.h ou sur les parametres ou les 
variables d'environnement gt_autoswapbuf et gt_swapbufon. 



Une documentation plus complete est disponible en format postcript et html dans
le repertoire doc.

  Pour tout probleme et/ou renseignement, contacter :
	Bruno RAINE
	tel.: 02 32 45 46 91
	email : raine@ganil.fr
