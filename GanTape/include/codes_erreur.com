$ !
$ !
$ ! creation d'un fichier include FORTRAN a partir d'un fichier .LIS, resultat
$ ! de la compilation par MESSAGE d'un fichier source d'erreurs .MSG
$ !
$ ! le fichier de sortie a le meme nom que le fichier d'entree avec une
$ ! extension .FOR
$ ! 
$ ! modifications... creation fichier .h de define C pour le meme usage
$ !
$ !
$ ! le parametre P1 est le nom du fichier 
$ !
$ set nover
$ !
$ say = "write sys$output"
$ esc[0,7] = %x1b
$ !
$ on control_y then goto ctrl_exit
$ !
$ if p1 .eqs. ""
$ then 
$    inquire p1 "Enter source filename"
$ endif
$ ! extension par defaut = .LIS
$ if f$locate(".",p1) .eqs. f$length(p1)
$ then
$    file_msg_in = p1+".LIS"
$ else
$    file_msg_in = p1
$ endif
$ ! test existence fichier d'entree
$ if f$search(file_msg_in) .eqs. ""
$ then
$    say "I don't find your file ",file_msg_in,"...     Bye!"
$    exit
$ endif
$ !
$ ! fichiers include "message"
$ ! --------------------------
$  ip1='f$locate(".",file_msg_in) 
$  file_msg_out_for:="''f$extract(0,ip1,file_msg_in)'.for"
$  file_msg_out_c:="''f$extract(0,ip1,file_msg_in)'.h"
$ !
$  open/read  msg_in 'file_msg_in 
$  open/write msg_out_for 'file_msg_out_for
$  open/write msg_out_c 'file_msg_out_c
$ !--------------------------------------------------------------------- 
$ count = 0
$ count_line = 0
$ line_in_progress = esc+"[1B"+"              ... working on line "
$ say line_in_progress
$ line_in_progress = esc+"[1A"+"              ... working on line "
$ say line_in_progress,"1"
$boucle_1: 
$ !   lire une ligne du fichier include fortran
$ !   -----------------------------------------
$    count = count+1
$    count_line = count_line+1
$    if count .eq. 10
$    then
$       say line_in_progress,'count_line'
$       count = 0
$    endif
$    read  msg_in line_in /end_of_file=end_of_file_in
$    l_line_in = f$length(line_in)
$ !                                              
$ !   recherche du numero du message
$ !   ------------------------------
$    numero_msg :="''f$extract(25,8,line_in)'" 
$ !
$ !   recherche du numero.
$ !   --------------------
$    ip='f$locate(".END",line_in)
$    if ( ip .ne. l_line_in )          then goto  end_of_file_in
$    if ( numero_msg .eqs. "        " ) then goto end_if0
$    if ( numero_msg .eqs. "" )         then goto end_if0
$ !
$ !      est-ce .facility ?
$ !      ------------------
$       ip='f$locate("FACILITY",line_in)
$       if ( ip .eq. l_line_in )         then goto else_if2
$ !      oui, on cherche le /prefix.
$ !      ---------------------------
$       ip='f$locate("/PREFIX=",line_in)
$       prefix := "''f$extract(ip+8,l_line_in,line_in)'" 
$       l = f$length(prefix)
$       i = 0
$       boucle_3:
$             car := "''f$extract(i,1,prefix)'"
$             if ( car .eqs. " " ) then  goto end_boucle_3
$             i = i + 1
$             if (i .eq. l) then  goto end_boucle_3
$             goto boucle_3
$          end_boucle_3:
$          prefix    := "''f$extract(0,i,prefix)'"
$ !
$       goto end_if2         
$       else_if2:
$ !      non, c'est un numero d'erreur.
$ !      ------------------------------
$ !         il y a au plus 15 carac. dans le champs ...
$ !         -------------------------------------------
$          nom_msg   := "''f$extract(40,15,line_in)'"
$          l = f$length(nom_msg)
$ ! 
$          i = 0
$          boucle_2:
$             car := "''f$extract(i,1,nom_msg)'"
$             if ( car .eqs. " " ) then  goto end_boucle_2
$             if ( car .eqs. "<" ) then  goto end_boucle_2
$             i = i + 1
$             if (i .eq. l) then  goto end_boucle_2
$             goto boucle_2
$          end_boucle_2:
$          nom_msg   := "''f$extract(0,i,nom_msg)'"
$          line_out_for := -
              "       PARAMETER " 'prefix''nom_msg'" = '"'numero_msg'"'X" 
$          line_out_c := -
              "#define" 'prefix''nom_msg'"  0x"'numero_msg' 
$          write msg_out_for  line_out_for
$          write msg_out_c  line_out_c
$ !
$ !         et on ecrit le message.
$ !         -----------------------
$          ip1='f$locate("<",line_in)
$          ip2='f$locate(">",line_in)
$          il = ip2 - ip1 + 1
$          comment := "''f$extract(ip1,il,line_in)'"
$          comment_for := "C     ''comment'"
$          comment_c := " /* ''comment' */"
$          write msg_out_for   comment_for
$          write msg_out_c   comment_c
$ !
$       end_if2:         
$    end_if0:
$    goto boucle_1
$ !
$ctrl_exit:
$    close msg_out_for
$    close msg_out_c
$    close msg_in
$    file_msg_out_for = file_msg_out_for+";"
$    delete /noconf /log 'file_msg_out_for
$    file_msg_out_c = file_msg_out_c+";"
$    delete /noconf /log 'file_msg_out_c
$    exit
$end_of_file_in:
$    say " end_of_file_in with line ",'count_line'
$    close msg_out_for
$    close msg_out_c
$    close msg_in
$ exit
