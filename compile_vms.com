$! This command file supposes that your directories are organised as follow:
$!    root--->               contains command files
$!        |
$!        |__ sources	     contains sources files
$!        |__ include        contains include files
$!        |__ vms_alpha_lib  contains library files on ALPHA architecture
$!        |__ vms_alpha_exe  contains exec files on ALPHA architecture
$!        |__ vms_vax_lib    contains library files on VAX architecture
$!        |__ vms_vax_exe    contains exec files on VAX architecture
$!
$! This command file creates a library libdir:gan_tape_lib.olb
$! and two main programs : exedir:exemple_relecture.exe 
$!                         exedir:acq_mt_tape_test.exe
$!
$! Logical files libdir and exedir are defined in the command file in
$! accordance with the machine on which you are running (VAX or ALPHA)  
$!
$!
$ if (f$getsyi("ARCH_NAME") .eqs. "Alpha")
$ then
$   if f$search("vms_alpha_lib.dir") .eqs. "" then create/dir [.vms_alpha_lib]
$   if f$search("vms_alpha_exe.dir") .eqs. "" then create/dir [.vms_alpha_exe]
$   define libdir [.vms_alpha_lib]
$   define exedir [.vms_alpha_exe]
$ else
$ if (f$getsyi("ARCH_NAME") .eqs. "VAX")
$ then
$   if f$search("vms_vax_lib.dir") .eqs. "" then create/dir [.vms_vax_lib]
$   if f$search("vms_vax_lib.dir") .eqs. "" then create/dir [.vms_vax_exe]
$   define libdir [.vms_vax_lib]
$   define exedir [.vms_vax_exe]
$ endif
$ endif
$ define srcdir [.sources]
$ define incdir [.include]
$ cc = "cc /decc/prefix=all /nomember /Include=incdir:"
$!
$! Generation des fichiers d'erreurs
$!
$ message /obj=LIBDIR:acq_codes_erreur.obj -
	/LIST=INCDIR:acq_codes_erreur.lis INCDIR:acq_codes_erreur.msg
$ @incdir:codes_erreur INCDIR:acq_codes_erreur.lis
$!
$! Compilation et mise en librairie des differents modules
$!
$@clib gan_tape_erreur
$@clib gan_tape_alloc  
$@clib gan_tape_mount  
$@clib gan_tape_file  
$@clib gan_tape_test  
$@clib gan_acq_swap_buf
$@clib acq_mt_fct_ganil                             
$@clib acq_mt_mes_fonctions
$@clib rd_evstr
$@clib s_evctrl
$@clib mon_traitement 
$@clib gan_tape_get_parametres    
$!
$! compilation et edition de lien des mains
$!
$cc/obj=libdir: srcdir:exemple_relecture 
$link/exe=exedir: libdir:exemple_relecture.obj, libdir:gan_tape_lib.olb/lib
$ say "  ==> compilation and linkedit for exemple_relecture done"
$!
$cc/obj=libdir: srcdir:acq_mt_tape_test
$link/exe=exedir: libdir:acq_mt_tape_test.obj, libdir:gan_tape_lib.olb/lib
$ say "  ==> compilation and linkedit for acq_mt_tape_test done"
$!
$pu/keep=1 srcdir:
$pu/keep=1 incdir:
$pu/keep=1 libdir:
$pu/keep=1 exedir:
$exit
