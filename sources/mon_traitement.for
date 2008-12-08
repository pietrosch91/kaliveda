C
C-----routine utilisateur de traitement d'un evenement de controle...
C
      integer*4 function mon_traitement(evt)
      integer*2 evt(*)

c      integer*4 function acq_get_parbr_label
      integer*4  acq_get_parbr_label
      external  acq_get_parbr_label
      integer*4 MonLabel, MonNbBits
      character*80 MonPar 
      integer*4 Ret


      if ( mod(evt(2),100).eq.0 ) then 

         print '(1x,a,i5,a,i4)','   ===>>> evenement numero',evt(2),
     1                       ' longueur',evt(1)

c        Ret =  acq_get_parbr_label("V550_2_220_", MonLabel, MonNbBits)

        MonPar = "V550_2_220_"
        Ret = acq_get_parbr_label(MonPar, MonLabel, MonNbBits)
        if ( Ret .eq.1 ) 
     1  then
           print *,'parametre ', MonPar ,' : Label = ',  MonLabel, 
     1           ', NbBits = ', MonNbBits
        else
           print '(1x,a)','parametre V550_2_220_ n''existe pas'
        endif

      endif
C      print '(10(2x,z4.4))',(evt(i),i=1,evt(1))

      mon_traitement = 0
 
      return
      end
 
      
