$!**************************************************************************
$!              Fichier de transfert FTP semi-automatique.                 *
$!                                                                         *
$!      Ce fichier permet de lancer un transfert FTP vers une machine      *
$!  voulue et de charger automatiquement les fichiers souhaites vers       *
$!  un compte UNIX par exemple.                                            *
$!                                                                         *
$!  BUT de ce FICHIER de COMMANDE :                                        *
$!  ------------------------------                                         *
$!      Il doit permettre de ne generer des fichiers sources, include et   *
$!  makefile que sur une plateforme VMS, independament du systeme de       *
$!  destination; pour tester le code, il suffit de lancer 'MAKEFTP', de    *
$!  selectionner les options et 'MAKEFTP se charge de transferer les       *
$!  fichiers voulus sur le compte de destination.                          *
$!                                                                         *
$!  RESTRICTIONS :                                                         *
$!  -------------                                                          *
$!       1- L'utilisateur doit posseder un compte sur la machine de        *
$!  destination.                                                           *
$!       2- Le compte de destination doit etre organise de facon identique *
$!  au compte source :                                                     *
$!                    ----------------------------------------             *
$!                    |       Racine de developpement.       |             *
$!                    | Ex: Repertoire ['nom_user'.gan_tape] |             *
$!                    ----------------------------------------             *
$!                                       |                                 *
$!         |------------------|----------|------|-----------------|        *
$!   ------|------     -------|-------     -----|------     ------|-----   *
$!   | Fichiers  |     | Fichiers de |     | Fichiers |     | Fichiers |   *
$!   | Compiles  |     |   Commande  |     | Include  |     | Sources  |   *
$!   |           |     |             |     |          |     |          |   *
$!   | [....***] |     | [.COMMANDE] |     |[.INCLUDE]|     |[.SOURCES]|   *   
$!   --|----------     --|------------     --|---------     --|---------   *
$!     |                 |                   |                |            *
$!     |- .EXE           |- makefile         |- .H            |- .C        *
$!     |                 |                                                 *
$!     |- .LIB           |- .COM                                           *
$!     |                                                                   *
$!     |- .OBJ                                                             *
$!                                                                         *
$!        Ou 'gan_tape' peut etre remplace par tout autre nom suivant      *
$!   l'avancement du projet.                                               *
$!       3- Le transfert des fichiers ne peut se faire que par repertoire  *
$!   entier ( ex: [.SOURCES] ) ou par type de fichiers dans un repertoire  *
$!   ( ex: *.com de commande ) ou un fichier dans un repertoire.           *
$!                                                                         *
$!************************************************************************** 
$!
$!
$!
$!
$ CLS
$ say ">>>   Programme de transfert automatique par FTP   <<<"
$ say ">>>"
$! INQUIRE DEST  ">>> Machine destination ( <ENTER> pour config. standard )"
$ READ /PROMPT=">>> Machine destination ( <ENTER> pour standard ) :" SYS$COMMAND DEST
$ IF DEST .eqs. ""
$ THEN 
  DEST = "ganilh"
  NOM  = "opdebeck"
  READ /PROMPT=">>> Mot de passe :" SYS$COMMAND PW  
  CLS
$ GOTO OUVERT
$ ENDIF
$!
$ READ /PROMPT=">>> Login        :" SYS$COMMAND NOM  
$ READ /PROMPT=">>> Mot de passe :" SYS$COMMAND PW 
$ CLS
$!
$ OUVERT:
$ OPEN/WRITE FICH doftp.
$ WRITE FICH NOM
$ WRITE FICH PW
$! WRITE FICH "bin"
$ WRITE FICH "cd gan_tape" 
$ CHREP: 
$ say ">>> Repertoire Source (Il est possible d'additionner les repertoires)"
$ say ""
$ say ">>>  0- Tous les fichiers de tous les repertoires."
$ say ">>>  1- Repertoire Source."
$ say ">>>  2- Repertoire Include."
$ say ">>>  3- Repertoire Commande."
$ say ""
$ INQUIRE CHOIX ">>> Choix "
$!     
$!
$ IF CHOIX .eq. 1
$ THEN 
  WRITE FICH "cd sources"
  say ""
  say ">>> Fichier(s) a transferer pour le repertoire 'Source'"
  READ /PROMPT=">>> Choix        :" SYS$COMMAND TRANSFERT 
  WRITE FICH "put [-.sources]" + TRANSFERT
$ ENDIF
$!              
$ IF CHOIX .eq. 2
$ THEN 
  WRITE FICH "cd include"
  say ""
  say ">>> Fichier(s) a transferer pour le repertoire 'Include'"
  READ /PROMPT=">>> Choix        :" SYS$COMMAND TRANSFERT
  WRITE FICH "put [-.include]" + TRANSFERT
$ ENDIF
$!              
$ IF CHOIX .eq. 3
$ THEN 
  WRITE FICH "cd commande"
  say ""
  say ">>> Fichier(s) a transferer pour le repertoire 'Commande'"
  READ /PROMPT=">>> Choix        :" SYS$COMMAND TRANSFERT
  WRITE FICH "put [-.commande]" + TRANSFERT
$ ENDIF
$!              
$ IF CHOIX .eq. 4
$ THEN 
  WRITE FICH "cd sources"
  say ""
  say ">>> Fichier(s) a transferer pour le repertoire 'Source'"
  READ /PROMPT=">>> Choix        :" SYS$COMMAND TRANSFERT
  WRITE FICH "put [-.sources]" + TRANSFERT
  WRITE FICH "cd .."
  WRITE FICH "cd commande"
  say ">>> Fichier(s) a transferer pour le repertoire 'Commande'"
  READ /PROMPT=">>> Choix        :" SYS$COMMAND TRANSFERT
  WRITE FICH "put [-.commande]" + TRANSFERT
$ ENDIF
$!              
$ IF CHOIX .eq. 5
$ THEN 
  WRITE FICH "cd include"
  say ""
  say ">>> Fichier(s) a transferer pour le repertoire 'Include'"
  READ /PROMPT=">>> Choix        :" SYS$COMMAND TRANSFERT
  WRITE FICH "put [-.include]" + TRANSFERT
  WRITE FICH "cd .."
  WRITE FICH "cd commande"
  say ">>> Fichier(s) a transferer pour le repertoire 'Commande'"
  READ /PROMPT=">>> Choix        :" SYS$COMMAND TRANSFERT
  WRITE FICH "put [-.commande]" + TRANSFERT
$ ENDIF
$!
$ IF CHOIX .eq. 6
$ THEN
  WRITE FICH "cd sources"
  say ""
  say ">>> Fichier(s) a transferer pour le repertoire 'Source'"
  READ /PROMPT=">>> Choix        :" SYS$COMMAND TRANSFERT
  WRITE FICH "put [-.sources]" + TRANSFERT
  WRITE FICH "cd .."
  WRITE FICH "cd include"
  say ">>> Fichier(s) a transferer pour le repertoire 'Include'"
  READ /PROMPT=">>> Choix        :" SYS$COMMAND TRANSFERT
  WRITE FICH "put [-.include]" + TRANSFERT
  WRITE FICH "cd .."
  WRITE FICH "cd commande"
  say ">>> Fichier(s) a transferer pour le repertoire 'Commande'"
  READ /PROMPT=">>> Choix        :" SYS$COMMAND TRANSFERT
  WRITE FICH "put [-.commande]" + TRANSFERT
$ ENDIF        
$!
$ IF CHOIX .eq. 0
$ THEN
  WRITE FICH "cd sources"
  WRITE FICH "put [-.sources]*.*"
  WRITE FICH "cd .."
  WRITE FICH "cd include"
  WRITE FICH "put [-.include]*.*"
  WRITE FICH "cd .."
  WRITE FICH "cd commande"
  WRITE FICH "put [-.commande]*.*"
$ ENDIF        
$!
$ IF CHOIX .lt. 0 .or. CHOIX .gt. 6
$ THEN GOTO CHREP
$ ENDIF
$!
$ CLOSE FICH  
$!
$ SPAWN FTP 'DEST' /input=doftp.
$!
$ DELETE doftp;*
$ CLS
$!
$!
