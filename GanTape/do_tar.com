$say "!!!!!!!!!!!!!!!!!!!!         ATTENTION         !!!!!!!!!!!!!!!!!!!!!!!!"
$say "! le portage de vms vers unix pose le probleme des noms de            !"
$say "! fichier en majuscule : ERR_GAN.H, EQUIPDES.H, GEN_TYEP.H, STR_EVT.H !"
$say "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
$pu/keep=1 [...]
$del GanTape_1_0.tar*;*/noconf 
$vmstar -cvf GanTape_1_0.tar *.com make* [.doc] [.include] [.sources]
$gzip GanTape_1_0.tar 
