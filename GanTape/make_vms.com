$ if (f$getsyi("ARCH_NAME") .eqs. "Alpha")
$ then
$   if f$search("vms_alpha_lib.dir") .eqs. "" then create/dir [.vms_alpha_lib]
$   if f$search("vms_alpha_exe.dir") .eqs. "" then create/dir [.vms_alpha_exe]
$   mms /description=makefile.vms/macro=("VERSION=vms_alpha", "MAKEEXTENSION=c")
$ else
$ if (f$getsyi("ARCH_NAME") .eqs. "VAX")
$ then
$   if f$search("vms_vax_lib.dir") .eqs. "" then create/dir [.vms_vax_lib]
$   if f$search("vms_vax_exe.dir") .eqs. "" then create/dir [.vms_vax_exe]
$   mms /description=makefile.vms/macro=("VERSION=vms_vax", "MAKEEXTENSION=c")
$ endif
$ endif
