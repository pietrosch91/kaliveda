c************************************************************************
c	Eric Bonnet 04/2010
c  Derniere compilation sous SL5 64 bits
c  g77 -w -I/afs/in2p3.fr/home/throng/indra/veda1/for fortran.f -c


c************************************************************************
c                                                                       *
c  KALI : ou (K)artouches (A) (L)ire pour (I)ndra ...                   *
c                                                                       *
c************************************************************************
c                                                                       *
c  Ce programme lit:                                                    *
c  - les cartouches DST_95 creees avec la version IBM(VM) de VEDA       *
c  - les cartouches DST_96 creees avec la version Anastasie de VEDA     *
c  - les cartouches DST_96 "reduites"                                   *
c                                                                       *
c  Version : 3 Juin 1996 (J-L. Charvet)                                 *
c************************************************************************
c
c---------------------------------------------------------------------  
c                                                                       
c  Ce programme a ete modifiee pour gerer l'ecriture de DST "reduites". 
c  Il permet apres selection des evts de les re-ecrire sur une DST
c  dans la meme structure que les bandes DST originelles
c    
c Il necessite le fichier ANALYSE FORTRAN pour fonctionner;             
c celui-ci contient les routines propres a l'utilisateur comme          
c INI_HBOOK, INI_RUN, TRAITEMENT, FIN_RUN et CLOSE_HB.                  
c
c Variable logique a utiliser dans TRAITEMENT pour ecrire des evts selectionnes
c
c   copy_event = .true.          
c                                                                       
c---------------------------------------------------------------------  
c                                                                       
c  3/3/00 Ce programme a ete modifie pour gerer la nouvelle calibration
c  ------ et identification des couronnes 10-17 (N. Le Neindre)
c
c  Si on veut l'utiliser il suffit de mettre dans INI_HBOOK
c
c           new_calib_c1017=.true.
c
c  Quand une nouvelle calib. et ident. a lieu un parametre "new_cali"
c  prend la valeur "1" sinon  new_cali=0
c  
c  les anciennes valeurs sont dans les tableaux: 
c                kz_old(i)       ---- > z(i)
c                ka_old(i)       ---- > a(i)
c                z_indra_old(i)  ---- > z_indra(i)
c                a_indra_old(i)  ---- > a_indra(i)
c                ener_old(i)     ---- > ener(i)
c                code_vieux(i)   ---- > code(i)
c                                             avec  i=1,npart_traite
c
c  Toutes ces variables et tableaux sont dans 'veda_rel.incl'
c
c  les nouvelles sont dans les tableaux standards de 'veda_6.incl':
c  z(i), a(i), ....
c
c
c---------------------------------------------------------------------  
c En cas de problemes, contactez les personnes suivantes :              
c                                                                       
c  - O. Lopez, LPC Caen, (LOPEZO@FRCPN11)                               
c  - J. L. Charvet, Saclay, (CHARVET@FRCPN11)                           
c  - D. Cussol, LPC Caen, (CUSSOL@FRCPN11)                              
c                                                                       
c                                                                       
c-------------------------------------------------------------------- 
c  D.Cussol & JDF: 27/09/2007  Version qui copie les fichiers 
c            HPSS sur le repertoire courant (RFIO n'existe plus)
  
      program Kali                                                  
                                                                        
c      logical   end_of_file,end_of_program
      integer*2 ibuff(8192),itab(8),ktab(100)
      integer*1 clinux(16392),clin(16392) 
      Character*1  char1                                    
      character*8  typebloc
      character*80 filein,fileout,namefil
      real*4       time                                                 
      Character*6   cart_dst
c - JDF 27092007
c      Character*6   cartou
c need 9 characters to hold 'campagne1'
      Character*9   cartou
c for 'rfcp' command
      character*160 commande 
      character*80 hpssdir
c - JDF 27092007
      Character*8   datj,hj     
      Integer*4     ifin,iq,jq
      integer*4    lun,irc,numruni,new_run
      Integer*4  iascii(256)
      Integer*4  nsito,nsito1,nsico
      integer*4  nblocecr_tot,nevtecr_tot,nevtcor
      logical*1  time_max,linux
      Character*70  critere,dst_file
      integer*4     long_critere
      
      Character*6  op_system
      Common /SYST/op_system
      
      Common /RED1/new_run,long_critere,critere                                               
      Common /ASKI/iascii
      common /CORESIL/nsito,nsito1,nsico
      character*8 tywrbloc(4)                                           
      data tywrbloc/'VEDARED1','BLOCDATA','ETAT_IND',' SCALER '/        
c                                                                       
c --- Definition des variables indispensables                           
c                                                                       
      include 'veda_rel.incl'                                         
      include 'veda_wri.incl'
      include 'veda_5.incl'                                             
      include 'veda_6.incl' 
      include 'veda_11.incl'                                            
      include 'veda_12.incl'                                            
      include 'veda_logic.incl'                                            

      call INITC 
      
c ---- PATH des fichiers data de VEDA ----------------------------------         
      
      linux=.false.
      call GETENVF ('SYSTEM',op_system)
      write(*,'(/,'' Operating system = '',a)')op_system
      if(op_system(1:5).eq.'Linux')then
         linux=.true.
      endif	 
      call VAR_ENVIRONNEMENT  

      namefil=nomjob(1:long_job)//'.sortie'

      print*,'INFO namefil=',namefil,' iout=',iout
		open (i_out,file=namefil)
      iwt=-1

      call DATIMH (datj,hj)                                             
      write(i_out,'(//,'' Date debut : '',a8,''  '',a8)')datj,hj 
      iwf=i_out
      call INI_ASCII                                                      
 
      correction_piedestal_SiPG=3.
      lfiltre=.false.
      lsimulation=.false. 
      dst_reduite=.false.
      new_calib_c1017=.false.
      
      call INI_HBOOK
                                                                                  
c      call CPUTIME(t1,irc)
      long_critere=70
      time_max=.false.    
      char_ascii=.false.                                          
      end_of_program=.false.
      end_of_file=.false. 
      fin_lect_run=.false.  
      copy_event=.false. 
      nblocecr_tot=0
      nevtecr_tot=0 
      nsito=0.
      nsito1=0.
      nsico=0.
      nevt_dump=100 ! Nbre d'evts a "DUMPER"   
                               
c --- Lecture du drapeau d'ecriture sur cartouche DST                   
                                                                        
      ecrit_dst=.False.                                                 
      
      write(i_out,100)
      write(*,100)
100   format(/,' **** TRAITEMENT des bandes DST 1ere Camp. INDRA **** ',
     & 'version: 14 Novembre 1995',/,
     &         '      ------------------------------------------      ')
                                                                                                     
c --- Boucle sur le nbre de fichiers a lire                             
 		print*,'INFO end_of_program ', end_of_program
      
		!do while (.not.end_of_program)
      	print*,'INFO boucle while ', end_of_program
	      
			filein=nomjob(1:long_job)//'.dst'
			call OPEN_FILE(filein,lun)

			end_of_file=.False.                                            

			if(lun.eq.-100) then
				print *,'Probleme a l''ouverture du fichier : ',filein(1:8)
				print *,'On passe au fichier suivant...'
				end_of_file=.True.
			endif 

      	nbloclus=0                                                     
      	nevtlus=0                                                      
      	nb_scaler=0 
      	nkbyta=0                                                       
      	nkbyte=0 
         
       
c --- Boucle sur la lecture des blocs du fichier                        
                                                                        
      	ll=0
      	do while(.not.end_of_file)                                     
      		ll=ll+1
                                                                       
c --- Controle du temps restant (batch)                                 
                                                                        
      		call TIMEL(time)                                            

				if(time.lt.5.) then  ! Temps limite atteint, c'est fini... 
                                                                        
            	write(i_out,*)
            	write(i_out,*) '**** Temps limite atteint  '    			
            	write(6,*) '**** Temps limite atteint **** '        		
            	end_of_file=.True.													
            	time_max=.true.
                                                                       
      		else
           
				call READ_BUFFER (lun,nbloclus,ibuff,istatus)


c --- Swap de ibuff (LINUX seulement)

				if(linux)then
					do i=1,8192
						ibuf=ibuff(i)   
						ibuf1=jbyt(ibuf,1,8)                                             
						ibuf2=jbyt(ibuf,9,8)                                             
						ibuff(i)=ibuf1*256+ibuf2  ! buffer swappe                                   
					end do
				endif  

				if(nbloclus.le.5)then
c					write(*,'(8i6)')(ibuff(k),k=1,256)
c              write(*,'(15(1x,z4))')(ibuff(k),k=1,256)
				endif		 


				if(istatus.eq.-1) then
					end_of_file=.True.
					call CFCLOS (lun,0)
	     
				elseif(istatus.gt.0) then ! pb de lecture du bloc
					write(*,'('' On passe au run suivant ! '')')
c            nbloc_tot=nbloc_tot+1 ! on passe au bloc suivant
					end_of_file=.True.
					call CFCLOS (lun,0)
	                                             
				else    ! istatus=0  Bon bloc
                                            
					nbloc_tot=nbloc_tot+1                                          
            	nkbyta=nkbyta+16384                                            

	            if(ibuff(2).le.127)then   ! en ASCII
						typebloc='        '
						do i=1,8
							char1=CHAR(ibuff(i))
							typebloc(i:i)=char1
						enddo
	              	char_ascii=.true.
	            else                      ! en EBCDIC   
	           		if(ibuff(1).eq.229)typebloc='VEDADST3'
						if(ibuff(1).eq.194)typebloc='BLOCDATA'
						if(ibuff(1).eq.197)typebloc='ETAT_IND'
						if(ibuff(3).eq.195)typebloc=' SCALER '
	            endif 
	                        
c --- Aiguillage suivant le type de bloc
 
            	if(typebloc.eq.'VEDADST3'.or.typebloc.eq.'VEDADST4'.
     &        	or.typebloc.eq.'VEDADST5'.or.typebloc.eq.'VEDARED1' )then
            
              		if(typebloc.eq.'VEDARED1')then
               		dst_reduite=.true.
               		write(i_out,104)
104            		format(/,' ** TRAITEMENT des bandes DST <reduite> ** ')
						endif
 
              		call DEC_BLOC_HEAD (ibuff,iversion,irc)
                   
             
              		if(irc.ne.0) then                                           
							write(i_out,*) ' Erreur lecture du fichier : '
     &							,filein
							write(6,*) ' Erreur lecture du fichier : ',filein
							end_of_program=.true.
							goto 999
						end if 
            
						print *,' *** RUN = ',numerun
	    
	      			if(new_calib_c1017)then
	       				call INIT_CALIB_C1017 (numerun)
	      			endif  

              		if(.not.dst_reduite)then ! correction seulement en lecture DST physique 

c $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

c --- appel a des routines d'initialisation de calibration (JLC 25/6/97)
c     pour correction de la calibration silicium (voir coresi.f)

c-----> Silicium  : IPN Orsay
 
      					icode=-10
      					call INITSI (iannee,imois,numerun,icode)
      					print *,'Initialisation Calibration Siliciums: icode = ',icode
 
      					if(icode.eq.0)then
          write(iwf,'('' *** Calibration Silicium (Orsay) : OK ***'')')
      					else
       write(iwf,'('' *** ATTENTION : Calib. Silicium INOPERANT ***'')')
       write(iwf,'('' date ou run non traite   INITSI... icode = '',
     &        i2)')icode
       write(*,'('' Prob. a l initialisation INITSI... icode = '',
     &        i2)')icode
       					endif
       
c-----> CsI (fragments) - LPC (couronnes 2-9 en Argon)
 
      					icode=-10
      					call INICALF (numerun,imois,iannee,icode)
      print *,'Initialisation Calibration Csi (fragments)'
 
      					if(icode.eq.0)then
          write(iwf,'('' *** Cali. CsI fragm. cour. 2-9   : OK ***'')')
      					elseif(icode.eq.-1)then
       write(iwf,'('' *** ATTENTION : Calibration CsI fragment  ***'')')
       write(iwf,'('' init. INICALF... icode = '',
     &        i2,'' : Signaux SiPG et GG utilises'')')icode
       write(*,'('' init. INICALF... icode = '',
     &        i2,'' : Signaux SiPG et GG utilises'')')icode
       					else
       write(iwf,'('' *** ATTENTION : Cali. CsI frag. INOPERANT ***'')')
       write(iwf,'('' Prob. a l initialisation INICALF.. icode = '',
     &        i2)')icode
       write(*,'('' Prob. a l initialisation INICALF.. icode = '',
     &        i2)')icode
       					endif
       
c $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

						endif
            
              		call INI_RUN 
                                                                         
            	elseif(typebloc.eq.'BLOCDATA') then
					 	if(iversion.eq.3)then 
                   	call DEC_BLOC_DST_3  (ibuff)
               	elseif(iversion.eq.4)then
                   	call DEC_BLOC_DST_4  (ibuff)
               	elseif(iversion.eq.5)then
                   	call DEC_BLOC_DST  (ibuff)
               	else
                   	STOP 'BLOCDATA  impossible a lire'
               	endif      
               	
						if(fin_lect_run)then
                  	end_of_file=.true.
                  	time_max=.true.
               	endif   
               elseif(typebloc.eq.' SCALER ') then
          			nb_echelle_a_imprimer=16
             		imp_all=1
             		nb_scaler=nb_scaler+1
             		write(*,*) ' '
             		write(*,*) ' DERNIER bloc = bloc Scaler du run'
             		write(*,*) ' bloc Scaler = ',nbloclus
             		Call DECODE_SCALER
     &        (ibuff,echelle,nb_echelle_a_imprimer,Iwt)
             		call PRINT_SCALER(i_out)
            	elseif(typebloc.eq.'ETAT_IND') then
 						call DEC_BLOC_ETAT(ibuff,irc)         
 						if(irc.ne.0) then                                           
               write(i_out,*) ' Erreur a la lecture du bloc ETAT_IND '  
               write(6,*) ' Erreur a la lecture du bloc ETAT_IND '      
             		end if  
           		else
 
            		write(6,25) nbloclus,typebloc
25          		format(1x,' Type du bloc #',i6,' : ',a8,' inconnu !')
c            do i=1,8                                                       
c              itab(i)=ibuff(i)
c              print *,i,itab(i)
c            end do
 
           		end if
	                                                                           
c --- Pointeur de bloc                                                  
                                                                       
           		iloop=mod(nbloclus,250)                                        
           		iwoop=mod(nblocecr,250) 
                                                                                 
           		if(iloop.eq.0.and.nbloclus.ne.1) then 
             write(6,'('' ## Blocs lus = '',i5,'' Evts lus = '',i8,
     &               '' Temps restant ='',f10.2,'' sec.'')')
     &                                        nbloclus,nevtlus,time                            
           		end if
	                                                         
				endif   ! fin du test sur istatus
                 
			endif  
		
		end do 
                                                                             
c --- Fin de la lecture, qq rappels...                                  

		write(i_out,*) ' '																
		write(i_out,*) 'Fin de lecture du fichier DST : ',filein 		 
		write(i_out,*) 'Nombre de blocs lus 		:',nbloclus 				
		write(i_out,*) 'Nombre de blocs SCALER 	:',nb_scaler
		write(i_out,*) 'Nombre d''evts lus  		 :',nevtlus 				
		write(i_out,*) 'Numero du dernier evt lu  :',num_evt_brut			
		write(i_out,*)'-----------------------------------------------'
		
		write(6,*) ' ' 																	
		write(6,*) ' ---- Fin de lecture fichier : ',filein					  
		write(6,*) 'Nombre de blocs lus  	  :',nbloclus  					
		write(6,*) 'Nombre d''evts lus			:',nevtlus  					
		write(6,*) ' --------------------------------------------'  		

		write(6,*) ' ' 																	
		call FIN_RUN																		
		write(i_out,*) ' ' 
		if(time_max)end_of_program=.true. 
 
c --- Fermeture                                                         
                                                                        
999   write(i_out,*) ' '                                                
      write(i_out,*) ' -- Cumul du nb de blocs lus : ',nbloc_tot    
      write(i_out,*) ' -- Cumul du nb d''evts   lus : ',nevt_tot    
      write(i_out,*) ' '                                                
      write(6,*) ' '                                                    
      write(6,*) ' -- Cumul du nb de blocs lus : ',nbloc_tot        
      write(6,*) ' -- Cumul du nb d''evts   lus : ',nevt_tot        
      write(6,*) ' '
       
      if(.not.dst_reduite)then ! correction seulement en lecture DST physique 
      
        write(6,*) ' Bilan correction en energie des SiPG cour. 2->9 :' 
        write(6,*) ' -------------------------------------------------'   
        write(6,*) ' Correction si variation de piedestal SiPG >= ',
     &             correction_piedestal_SiPG                                            
        write(6,*) ' '                                                   
        write(6,*) ' -- Nb LCP & IMF  total ....... = ',nsito
        write(6,*) ' -- Nb LCP & IMF  (SiGG > 3800) = ',nsito1
        write(6,*) ' -- Nb LCP & IMF  (E. corrigee) = ',nsico
        write(6,*) ' '
      
      endif 
                                                         
                                                                        
200   call CLOSE_HB                                                     
                                                                        
      write(i_out,*) ' '                                                
      write(i_out,*) ' ---- FIN DE LECTURE ----'                        
      call DATIMH (datj,hj)                                             
      write(i_out,'(//,'' Date fin  : '',a8,''  '',a8)')datj,hj         
                                                                        
      end                                                               
                                                                        
c---------------------------------------------------------------------- 
c --- Routine de decodage des blocs evts   (VEDA  iversion=5)           
c---------------------------------------------------------------------- 
      subroutine DEC_BLOC_DST(ibuff)                                    
                                                                        
      integer*2 ibuff(8192),code2                                       
      integer*4 ivalz(300),jvalz
      integer*4 nsito,nsito1,nsico                                      
      integer*2 nevt_low,nevt_high
      integer*4 code16,code4(4),ien
      logical   evt_a_lire,prt 
      integer*2 kbyt(2)
                                                                        
      Real*4       tab_pw(12),hit_pw(12)
      common /TABPW/tab_pw,hit_pw      

      include 'veda_rel.incl'                                           
      include 'veda_wri.incl'
      include 'veda_5.incl'                                             
      include 'veda_6.incl'                                             
                                                                        
      common /VEDADUMP/ivalz
      common /CORESIL/nsito,nsito1,nsico
c      Equivalence (ien,kbyt)
 
      ipt=9                                                             
      evt_a_lire=.True.                                                 
      prt=.False.  
                                                                        
                                                                        
c --- Boucle sur le nombre d'evt a decoder dans le bloc                 
                                                                        
c      nbloc=nbloc+1                                                    
      nbuffevt=0                                                        
c      print *,' nbloc = ',nbloclus
                                                                    
      if(prt) then                                                      
         write(10,*) 'Impression Bloc #',nbloc                          
         do i=1,8192,8                                               
         write(10,1) (ibuff(i+k),k=0,7)                                 
         end do                                                         
1     format(1x,8(i5,1x))                                               
      end if                                                            
                                                                        
      do while(evt_a_lire)                                              
                                                                        
         ilong=ibuff(ipt)                                               
         nkbyte=nkbyte+ilong*2                                          
                                                                        
c --- Fin de lecture du bloc courant                                    
                                                                        
         if(ilong.eq.0.or.ipt.ge.8192) then   ! JLC 06/02/04            
c          if(ilong.eq.0) then                                          
                                                                       
            evt_a_lire=.False. 
                                                     
         else                                                           
                                                                        
c --- Decodage de l'evt NEVTLUS                                         
                                                                        
            nbuffevt=nbuffevt+1                                         
            nevtlus=nevtlus+1                                           
            nevt_tot=nevt_tot+1                                         
            nevt_low=ibuff(ipt+1)                                       
            nevt_high=ibuff(ipt+2)                                      
            npart_traite=ibuff(ipt+3)                                   
            code_evt=ibuff(ipt+4)                                       
            mrejet=ibuff(ipt+5)                                         
            iztot=ibuff(ipt+6)                                          
            ztot=float(iztot)                                           
            mcha=ibuff(ipt+7)                                           
                                                                        
c --- Reconstitution de NEVT...                                         
 
            iflag=0
            if(nevt_high.lt.0) iflag=1                                  
            if(iflag.eq.0) then
               num_evt_brut=nevt_low*2**16+nevt_high                    
            else
               num_evt_brut=nevt_low*2**16+nevt_high+65536              
            end if
            nevt=nevt+1
            
c --- Remplissage du buffer d'ecriture "ievt" pour l'evenement 
                          
            if(ilong.gt.wmax)then
               print *,' -------------------------------------------'
               print *,' ATTENTION DANGER ... dans le bloc = ',nbloclus
               print *,' ilong = ',ilong,' > wmax = ',wmax
             print *,' risque de violation de memoire ! (veda_wri.incl)'
               print *,' evt = ',nevtlus,' evt brut = ',num_evt_brut
               print *,' npart_traite, iztot , mcha = ',
     &                   npart_traite,iztot,mcha
               print *,' On se repositionne sur le bloc suivant'
c               print *,' Avant: dump de cet evt: '
c               do k=1,ilong
c               print *,'    k,ibuff = ',k,ibuff(ipt+k-1)
c               enddo
               print *,' -------------------------------------------'
c	       write(*,'(8i6)')(ibuff(k),k=1,8193)
c	       print *
c              write(*,'(15(1x,z4))')(ibuff(k),k=1,8193)

c               stop 'DEC_BLOC_DST'
                return
            endif  
                                                                                                 
            if(ilong.gt.1200)then
               print *,' -------------------------------------------'
               print *,' ATTENTION EVENEMENT BIZARRE ...'
               print *,' Longueur evt un peu grande: = ',ilong,' > 1200'
               print *,' evt = ',nevtlus,' evt brut = ',num_evt_brut
               print *,' npart_traite, Ztot , Mult.(code>=2) = ',
     &                   npart_traite,iztot,mcha
               print *,' *** Danger s''ils sont trop nombreux ! ***'
               print *,' -------------------------------------------'
            endif  

            do k=1,ilong
            ievt(k)=ibuff(ipt+k-1)
            enddo
            do k=ilong+1,wmax
            ievt(k)=0
            enddo 
              
c --- tableau jbuff donne pour chaque constituant i de l'evt 
c     l'adresse (ou pointeur) ou il commence dans l'evt  

            jbuff(1)=9
            do i=2,npart_traite
            ik=jbuff(i-1)
            jbuff(i)=ik+ievt(ik)
            enddo  
                                                                                 
c --- Positionnement du pointeur de lecture                             
                                                                        
            ipt=ipt+i_entete                                            
                                                                        
c --- Decodage caracteristiques des particules...                       
                                                                        
            do i=1,npart_traite                                         
                                                                        
               de1(i)=0.
               de2(i)=0.
               de3(i)=0.
               de4(i)=0.
               de5(i)=0.
               de_mylar(i)=0.
               code_cali(i)=0.
               ener(i)=0.
               ecode(i)=0
               do k=1,4
               code_energie(k,i)=0
               enddo
               canal(i,1)=0
               canal(i,2)=0
               canal(i,4)=0
               canal(i,5)=0
               canal(i,7)=0
               canal(i,8)=0
 
               ilongevt=ibuff(ipt)                                      
               module=ibuff(ipt+1)                                      
               icou(i)=module/100                                       
               imod(i)=mod(module,100)                                  
               
                                                                        
c --- Si Gamma ( Code = 0 )                                             
                                                                        
               if(ilongevt.eq.i_long1) then                             
                                                                        
                  z(i)=0                                                
                  a(i)=0                                                
                  z_indra(i)=0.                                         
                  a_indra(i)=0.                                         
 
                  code_part(i)=11*16  ! code gamma=11 dans IDENLCP      
                  code16=code_part(i)                                   
                  call DECODE_PART(icou(i),code16,code4)                
                  code(i)=code4(1)                                      
                  do k=1,4
                  code_idf(k,i)=code4(k)                                
                  enddo
 
                  if(icou(i).eq.1)then
                    de1(i)=ibuff(ipt+2)                                 
                  else
                    de3(i)=ibuff(ipt+2)                                 
                  endif
                  mt(i)=ibuff(ipt+3)                                    
                                                                        
              else
 
                  ib=ibuff(ipt+2)                                       
                  z_indra(i)=float(ib)/100.                             
                  z(i)=nint(z_indra(i))                                 
                  ib=ibuff(ipt+3)                                       
                  a_indra(i)=float(ib)/100.                             
                  a(i)=nint(a_indra(i))                                 
                  code_part(i)=ibuff(ipt+4)                             
                  code16=code_part(i)                                   
                  call DECODE_PART(icou(i),code16,code4)                
                  code(i)=code4(1)                                      
                  do k=1,4
                  code_idf(k,i)=code4(k)                                
                  enddo
 
c --- Correction des cartouches 38KDC creees entre le 19 et 25/6/95 ---
c     Pb du code=9 trop souvent present, corrige ensuite dans VEDA 3.2
 
                  if((kjour.ge.19.and.kjour.le.25).and.
     &                kmois.eq.6.and.kanne.eq.95)then
                    if(icou(i).ge.10)call CORRECT_BACK (num_evt_brut,i)
                  endif
c -----
                  code_cali(i)=ibuff(ipt+5)
 
                  code16=code_cali(i)                                   
                  code_energie(1,i)=jbyt(code16,1,4)                    
                  code_energie(2,i)=jbyt(code16,5,4)                    
                  code_energie(3,i)=jbyt(code16,9,4)                    
                  code_energie(4,i)=jbyt(code16,13,4)                   
                  ecode(i)=code_energie(1,i)                            
                                                                        
                  mt(i)=ibuff(ipt+6)
 
c --- Cas des Phoswichs (couronne = 1)                                  
                                                                        
                  if(ilongevt.eq.i_long2) then                          
                                                                        
                    kbyt(1)=ibuff(ipt+7)
                    kbyt(2)=ibuff(ipt+8)
		    ien=kbyt(1)*(2**16)+kbyt(2)
		    if(kbyt(2).lt.0)ien=ien+2**16
                    de1(i)=float(ien)
                    de1(i)=de1(i)/10.
c		    if(nevt.le.100)then
c       write(*,'(z4,1x,z4,1x,z8,2x,3i8)')ibuff(ipt+7),ibuff(ipt+8),ien,
c     &                            ibuff(ipt+7),ibuff(ipt+8),ien
c                    endif
                                                                        
c --- Cas des couronnes 2 a 9                                           
                                                                        
                  elseif(ilongevt.eq.i_long3) then                      
                                                                        
                    de_mylar(i)=(ibuff(ipt+7))/100.                     
                    de1(i)=(ibuff(ipt+8))/10.                           
                                                                        
                    kbyt(1)=ibuff(ipt+9)
                    kbyt(2)=ibuff(ipt+10)
		    ien=kbyt(1)*(2**16)+kbyt(2)
		    if(kbyt(2).lt.0)ien=ien+2**16
                    de2(i)=float(ien)
                    de2(i)=de2(i)/10.
                                                                        
                    kbyt(1)=ibuff(ipt+11)
                    kbyt(2)=ibuff(ipt+12)
		    ien=kbyt(1)*(2**16)+kbyt(2)
		    if(kbyt(2).lt.0)ien=ien+2**16
                    de3(i)=float(ien)
                    de3(i)=de3(i)/10.
                    
                    canal(i,1)=ibuff(ipt+13)  !  canal  ChIo GG
                    canal(i,2)=ibuff(ipt+14)  !  canal  ChIo PG
                    canal(i,4)=ibuff(ipt+15)  !  canal  SI GG
                    canal(i,5)=ibuff(ipt+16)  !  canal  SI PG
                    canal(i,7)=ibuff(ipt+17)  !  canal  CsI R
                    canal(i,8)=ibuff(ipt+18)  !  canal  CsI L
                    
                    if(.not.dst_reduite)then  ! la correction a ete faite lors 
                                             ! de la gen. de la DST reduite          
                    
c --- Correction de de2(i) et de3(i) du fait de la correction en energie
c     du silicium PG (voir coresi.f - MFR,JLC 25/6/97) 

                      nsito=nsito+1.  ! compteur (LCP & IMF: cou.2-9)
                      if(canal(i,4) .gt. 3800) then
                        nsito1=nsito1+1 
                        call cor_can(i)
                      endif
                      
                    endif  
                    
c --- Cas des couronnes 10 a 17  (sans Etalons)                         
                                                                        
                  elseif(ilongevt.eq.i_long4) then                      
                                                                        
                    de_mylar(i)=(ibuff(ipt+7))/100.                     
                    de1(i)=(ibuff(ipt+8))/10.                           
                                                                        
                    kbyt(1)=ibuff(ipt+9)
                    kbyt(2)=ibuff(ipt+10)
		    ien=kbyt(1)*(2**16)+kbyt(2)
		    if(kbyt(2).lt.0)ien=ien+2**16
                    de3(i)=float(ien)
                    de3(i)=de3(i)/10.
                    
                    canal(i,1)=ibuff(ipt+11)  !  canal  ChIo GG
                    canal(i,2)=ibuff(ipt+12)  !  canal  ChIo PG
                    canal(i,7)=ibuff(ipt+13)  !  canal  CsI R
                    canal(i,8)=ibuff(ipt+14)  !  canal  CsI L           
                                                                        
c     --- Correction des ecode(i)=3 --- JLC 20/10/97

                     if(z(i).ge.1.and.z(i).le.2)then
                       ico=0
                       call COR_ECSI_LIMIT (icou(i),imod(i),z(i),a(i),
     &                                      de3(i),ico)
                       if(ico.eq.7)then  !  E > E_max (sinon ico=1)
                          ecode(i)=3
                          code_energie(2,i)=7
                       else
                          if(ecode(i).eq.3)ecode(i)=1
                          code_energie(2,i)=1
                       endif
                           
                       code_energie(1,i)=ecode(i)
                         
                       do k=1,4
                       code4(k)=code_energie(k,i)
                       enddo
                       call CODAGE (code4,code2)
                       ievt(jbuff(i)+5)=code2 
                     endif  
                    
c --- Cas des couronnes 10 a 17  (avec Etalons)                         
                                                                        
                  elseif(ilongevt.eq.i_long5) then                      
 
                    de_mylar(i)=(ibuff(ipt+7))/100.                     
                    de1(i)=(ibuff(ipt+8))/10.                           
                    de4(i)=(ibuff(ipt+9))/10.                           
                                                                        
                    kbyt(1)=ibuff(ipt+10)
                    kbyt(2)=ibuff(ipt+11)
 		    ien=kbyt(1)*(2**16)+kbyt(2)
		    if(kbyt(2).lt.0)ien=ien+2**16

                    de5(i)=float(ien)
                    de5(i)=de5(i)/10.
                                                                        
                    kbyt(1)=ibuff(ipt+12)
                    kbyt(2)=ibuff(ipt+13)
 		    ien=kbyt(1)*(2**16)+kbyt(2)
		    if(kbyt(2).lt.0)ien=ien+2**16

                    de3(i)=float(ien)
                    de3(i)=de3(i)/10.
                    
                    canal(i,1)=ibuff(ipt+14)  !  canal  ChIo GG
                    canal(i,2)=ibuff(ipt+15)  !  canal  ChIo PG
                    canal(i,7)=ibuff(ipt+16)  !  canal  CsI R
                    canal(i,8)=ibuff(ipt+17)  !  canal  CsI L
                                                                        
c     --- Correction des ecode(i)=3 --- JLC 20/10/97

                     if(z(i).ge.1.and.z(i).le.2)then
                       ico=0
                       call COR_ECSI_LIMIT (icou(i),imod(i),z(i),a(i),
     &                                      de3(i),ico)
                       if(ico.eq.7)then  !  E > E_max (sinon ico=1)
                          ecode(i)=3
                          code_energie(2,i)=7
                       else
                          if(ecode(i).eq.3)ecode(i)=1
                          code_energie(2,i)=1
                       endif
                           
                       code_energie(1,i)=ecode(i)
                         
                       do k=1,4
                       code4(k)=code_energie(k,i)
                       enddo
                       call CODAGE (code4,code2)
                       ievt(jbuff(i)+5)=code2 
                     endif  
                    
                  else                                                  
                                                                        
                    write(6,*) 'Bloc=',nbloclus,' Evt:',nevt,'bizarre!'
                    write(6,*) 'En effet, ilongevt =',ilongevt,' !?'    
                    write(6,*) 'On passe au bloc suivant !!!?'
                    
                    return   
                                                                        
                  end if                                                
 
                  dde1=de1(i)
                  dde2=de2(i)
                  dde3=de3(i)
                  dde4=de4(i)
                  dde5=de5(i)
                  if(dde1.lt.0)dde1=0.
                  if(dde2.lt.0)dde2=0.
                  if(dde3.lt.0)dde3=0.
                  if(dde4.lt.0)dde4=0.
                  if(dde5.lt.0)dde5=0.
                  ener(i)=(dde1+dde2+dde3+dde4+dde5)+de_mylar(i)
 
              endif
 
              ipt=ipt+abs(ilongevt)                                     
 
            end do
                
c --- Routine de correction des Identifications Etalons (L. Tassan-Got) 
 
            call PROPNET
 
c --- Nouvelle calibration couronne 10-17 (JLC: 25/2/00)

	    new_cali=0
            if(new_calib_c1017)then
               do i=1,npart_traite                            
               kz_old(i)=z(i)
	       ka_old(i)=a(i)
	       z_indra_old(i)=z_indra(i)
	       a_indra_old(i)=a_indra(i)
               ener_old(i)=ener(i)
	       code_vieux(i)=code(i)
               enddo
	       call NOUVELLE_CALIBRATION_IDENTIFICATION_C10_17 

c --- le remplissage des nouvelles valeurs z(i),a(i), .... est fait
c     dans la routine "NOUVELLE_CALIBRATION_IDENTIFICATION_C10_17"
c     A noter: code(i) est en fait inchange
 		 
	    endif 
	      
c --- Routine utilisateurs pour le traitement evt/evt 
                  
            copy_event=.false.                                          
            call TRAITEMENT
            
            if(fin_lect_run)evt_a_lire=.false.
                                                                        
101         format(1x,5(i6,2x))                                         
                                                                        
         end if                                                         
                                                                        
      end do                                                            
                                                                        
      return                                                            
      end                                                               
                                                                        

c--------------------------------------------------------------------   
c --- Routine de decodage du bloc Entete du fichier DST               
c--------------------------------------------------------------------   
      subroutine DEC_BLOC_HEAD (ibuff,iversion,irc)                     
                                                                        
      integer*2   ibuff(8192),ktab(100)                                 
      character*8 day,hour,nom                                          
      character*5 type                                                  
      integer*4  irc,iversion                                           
      integer*4  kheure,kminut,ksecon
      Integer*4  iascii(256)
      Character*70  critere
      integer*4    new_run,long_critere
      
      Common /RED1/new_run,long_critere,critere                         
      Common/ASKI/iascii
                                                                        
      include     'veda_rel.incl'                                       
      include     'veda_wri.incl'                                       
      include     'veda_5.incl'                                         
                                                                        
      ipt=8                                                             
      irc=0                                                             
                                                                        
      numerun=ibuff(ipt+1)                                              
      ijour=ibuff(ipt+2)                                                
      imois=ibuff(ipt+3)                                                
      iannee=ibuff(ipt+4)                                               
      zproj=ibuff(ipt+5)                                                
      aproj=ibuff(ipt+6)                                                
      zcib=ibuff(ipt+7)                                                 
      acib=ibuff(ipt+8)                                                 
      esura=ibuff(ipt+9)                                                
      iversion=ibuff(ipt+10)                                            
      ipt=ipt+10 
              
      write(i_out,'(/,'' *** Lecture Run numero '',i4)')numerun         
      write(i_out,1) ijour,imois,iannee                                 
1     format(/,' Date de creation du run : ',i2,'/',i2,'/',i4)
                                                     
      if(char_ascii)then
                                                     
        do i=1,8                                                        
          ilettre=ibuff(ipt+i)                                          
c          print *,' DAY : i,ilettre = ',i,ilettre
          day(i:i)=CHAR(ilettre)                                        
        end do
        kjour=(ibuff(ipt+1)-48)*10+ibuff(ipt+2)-48
        kmois=ibuff(ipt+4)*10+ibuff(ipt+5)-528
        kanne=ibuff(ipt+7)*10+ibuff(ipt+8)-528
        
c        print *,' ASCII : jour,mois,annee = ',kjour,kmois,kanne
        if(iversion.eq.3.and.(kmois.ge.5.and.kanne.eq.96))then
c           print *,' changement de version .....'
           iversion=4
        endif
                      
        ipt=ipt+8                                                       
        do i=1,8                                                        
          ilettre=ibuff(ipt+i)                                          
c          print *,' HOUR : i,ilettre = ',i,ilettre
          hour(i:i)=CHAR(ilettre)                                       
        end do
        write(i_out,4) day,hour                                         
4       format(' Fichier DST cree le     : ',a8,' a ',a8)               
        
      else                                                              
 
        kjour=(ibuff(ipt+1)-240)*10+ibuff(ipt+2)-240
        kmois=ibuff(ipt+4)*10+ibuff(ipt+5)-2640
        kanne=ibuff(ipt+7)*10+ibuff(ipt+8)-2640
        do l=1,8
        ibuff(ipt+l)=iascii(ibuff(ipt+l))
        enddo
        
        ipt=ipt+8                                                       
        kheure=(ibuff(ipt+1)-240)*10+ibuff(ipt+2)-240
        kminut=(ibuff(ipt+4)-240)*10+ibuff(ipt+5)-240
        ksecon=(ibuff(ipt+7)-240)*10+ibuff(ipt+8)-240
        write(i_out,7)kjour,kmois,kanne,kheure,kminut,ksecon
7       format(' Fichier DST cree le     : ',i2,'/',i2,'/19',i2,' a ',
     &                                       i2,'h.',i2,'m.',i2,'s.')   
        do l=1,8
        ibuff(ipt+l)=iascii(ibuff(ipt+l))
        enddo
                              
      endif
      
      write(i_out,2) zproj,aproj,esura                                  
2     format(/,1x,'Projectile ==>  Z=',f4.0,'   A=',f4.0,               
     &         '   E/A=',f5.1)                                          
      write(i_out,3) zcib,acib                                          
3     format(1x,'Cible      ==>  Z=',f4.0,'   A=',f4.0)                 
      
      ipt=ipt+9                                                         
      numbytes=ibuff(ipt)                                               
c      do i=1,5                                                         
c         idummy=ibuff(ipt+i)                                           
c         print *,' TYPE : i,idummy = ',i,idummy
c         type(i:i)=CHAR(idummy)                                        
c      end do  
      type(1:5)='INT*2'                                                 
      ipt=ipt+6                                                         
      i_entete=ibuff(ipt)                                               
      i_long1=ibuff(ipt+1)                                              
      i_long2=ibuff(ipt+2)                                              
      i_long3=ibuff(ipt+3)                                              
      i_long4=ibuff(ipt+4)                                              
      i_long5=ibuff(ipt+5)                                              
      nkbyte=nkbyte+ipt*2                                               

      if(dst_reduite)then
        ipt=ipt+6
        new_run=ibuff(ipt)
        ipt=ipt+1
        long_critere=ibuff(ipt)
        do l=1,long_critere
        ktab(l)=ibuff(ipt+l)
        enddo
        call ATOC (ktab,long_critere,critere)
      endif           
                                                                        
      write(*,5) iversion                                           
5     format(/,' Version du programme VEDA/DST :',i4)                   
      write(*,*) ' Longueurs (mots) suivant l''identification :'    
      write(*,*) '   - Gamma                      :',i_long1        
      write(*,*) '   - Phoswich                   :',i_long2        
      write(*,*) '   - Cour. 2 a 9                :',i_long3        
      write(*,*) '   - Cour 10 a 17 (sans si_etal):',i_long4        
      write(*,*) '   - Cour 10 a 17 (avec si_etal):',i_long5        
      write(*,*) ' '                                                
      write(*,*) 'Longueur des blocs :',numbytes,' octets'          
      write(*,6) type                                               
6     format(1x,'Type des mots      : ',a5)                             
                                                                        
      return                                                            
      end  
       
c---------------------------------------------------------------------- 
c --- Routine de decodage des blocs evts   (VEDA Version 4)             
c---------------------------------------------------------------------- 
      subroutine DEC_BLOC_DST_4(ibuff)                                  
                                                                        
      integer*2 ibuff(8192)                                             
      integer*4 ivalz(300),jvalz                                        
      integer*2 nevt_low,nevt_high
      integer*4 code16,code4(4),ien
      logical   evt_a_lire,prt 
      integer*2 kbyt(2)
                                                                        
      Real*4       tab_pw(12),hit_pw(12)
      common /TABPW/tab_pw,hit_pw      

      include 'veda_rel.incl'                                           
      include 'veda_wri.incl'
      include 'veda_5.incl'                                             
      include 'veda_6.incl'                                             
                                                                        
      common /VEDADUMP/ivalz
      Equivalence (ien,kbyt)
 
      ipt=9                                                             
      evt_a_lire=.True.                                                 
      prt=.False.  
                                                                        
                                                                        
c --- Boucle sur le nombre d'evt a decoder dans le bloc                 
                                                                        
      nbloc=nbloc+1                                                     
      nbuffevt=0                                                        
                                                                        
      if(prt) then                                                      
         write(10,*) 'Impression Bloc #',nbloc                          
         do i=1,8192,8                                                  
         write(10,1) (ibuff(i+k),k=0,7)                                 
         end do                                                         
1     format(1x,8(i5,1x))                                               
      end if                                                            
                                                                        
      do while(evt_a_lire)                                              
                                                                        
         ilong=ibuff(ipt)                                               
         nkbyte=nkbyte+ilong*2                                          
                                                                        
c --- Fin de lecture du bloc courant                                    
                                                                        
         if(ilong.eq.0.or.ipt.ge.8192) then   ! JLC 06/02/04            
c         if(ilong.eq.0) then                                           
                                                                        
            evt_a_lire=.False. 
                                                     
         else                                                           
                                                                        
c --- Decodage de l'evt NEVTLUS                                         
                                                                        
            nbuffevt=nbuffevt+1                                         
            nevtlus=nevtlus+1                                           
            nevt_tot=nevt_tot+1                                         
            nevt_low=ibuff(ipt+1)                                       
            nevt_high=ibuff(ipt+2)                                      
            npart_traite=ibuff(ipt+3)                                   
            code_evt=ibuff(ipt+4)                                       
            mrejet=ibuff(ipt+5)                                         
            iztot=ibuff(ipt+6)                                          
            ztot=float(iztot)                                           
            mcha=ibuff(ipt+7)                                           
                                                                        
c --- Reconstitution de NEVT...                                         
 
            iflag=0
            if(nevt_high.lt.0) iflag=1                                  
            if(iflag.eq.0) then
               num_evt_brut=nevt_low*2**16+nevt_high                    
            else
               num_evt_brut=nevt_low*2**16+nevt_high+65535              
            end if
            nevt=nevt+1                                                 
            
c --- Remplissage du buffer d'ecriture "ievt" pour l'evenement 
                          
            if(ilong.gt.wmax)then
               print *,' -------------------------------------------'
               print *,' ATTENTION DANGER ...'
               print *,' ilong = ',ilong,' > wmax = ',wmax
               print *,' voir veda_wri.incl ! violation de memoire ! '
               print *,' evt = ',nevtlus,' evt brut = ',num_evt_brut
               print *,' npart_traite, iztot , mcha = ',
     &                   npart_traite,iztot,mcha
               print *,' dump de cet evt: '
               do k=1,ilong
               print *,'    k,ibuff = ',k,ibuff(ipt+k-1)
               enddo
               print *,' -------------------------------------------'

c               stop 'DEC_BLOC_DST'
                return
            endif  
                                                                                                 
            do k=1,ilong
            ievt(k)=ibuff(ipt+k-1)
            enddo
            do k=ilong+1,wmax
            ievt(k)=0
            enddo   

                                                                                 
c --- Positionnement du pointeur de lecture                             
                                                                        
            ipt=ipt+i_entete                                            
                                                                        
c --- Decodage caracteristiques des particules...                       
                                                                        
            do i=1,npart_traite                                         
                                                                        
               de1(i)=0.
               de2(i)=0.
               de3(i)=0.
               de4(i)=0.
               de5(i)=0.
               de_mylar(i)=0.
               code_cali(i)=0.
               ener(i)=0.
               ecode(i)=0
               do k=1,4
               code_energie(k,i)=0
               enddo
               canal(i,4)=-1
               canal(i,7)=-1
               canal(i,8)=-1
 
               ilongevt=ibuff(ipt)                                      
               module=ibuff(ipt+1)                                      
               icou(i)=module/100                                       
               imod(i)=mod(module,100)                                  
               
                                                                        
c --- Si Gamma ( Code = 0 )                                             
                                                                        
               if(ilongevt.eq.i_long1) then                             
                                                                        
                  z(i)=0                                                
                  a(i)=0                                                
                  z_indra(i)=0.                                         
                  a_indra(i)=0.                                         
 
                  code_part(i)=11*16  ! code gamma=11 dans IDENLCP      
                  code16=code_part(i)                                   
                  call DECODE_PART(icou(i),code16,code4)                
                  code(i)=code4(1)                                      
                  do k=1,4
                  code_idf(k,i)=code4(k)                                
                  enddo
 
                  if(icou(i).eq.1)then
                    de1(i)=ibuff(ipt+2)                                 
                  else
                    de3(i)=ibuff(ipt+2)                                 
                  endif
                  mt(i)=ibuff(ipt+3)                                    
                                                                        
              else
 
                  ib=ibuff(ipt+2)                                       
                  z_indra(i)=float(ib)/100.                             
                  z(i)=nint(z_indra(i))                                 
                  ib=ibuff(ipt+3)                                       
                  a_indra(i)=float(ib)/100.                             
                  a(i)=nint(a_indra(i))                                 
                  code_part(i)=ibuff(ipt+4)                             
                  code16=code_part(i)                                   
                  call DECODE_PART(icou(i),code16,code4)                
                  code(i)=code4(1)                                      
                  do k=1,4
                  code_idf(k,i)=code4(k)                                
                  enddo
 
c --- Correction des cartouches 38KDC creees entre le 19 et 25/6/95 ---
c     Pb du code=9 trop souvent present, corrige ensuite dans VEDA 3.2
 
                  if((kjour.ge.19.and.kjour.le.25).and.
     &                kmois.eq.6.and.kanne.eq.95)then
                    if(icou(i).ge.10)call CORRECT_BACK (num_evt_brut,i)
                  endif
c -----
                  code_cali(i)=ibuff(ipt+5)
 
                  code16=code_cali(i)                                   
                  code_energie(1,i)=jbyt(code16,1,4)                    
                  code_energie(2,i)=jbyt(code16,5,4)                    
                  code_energie(3,i)=jbyt(code16,9,4)                    
                  code_energie(4,i)=jbyt(code16,13,4)                   
                  ecode(i)=code_energie(1,i)                            
                                                                        
                  mt(i)=ibuff(ipt+6)
 
c --- Cas des Phoswichs (couronne = 1)                                  
                                                                        
                  if(ilongevt.eq.i_long2) then                          
                                                                        
                    kbyt(1)=ibuff(ipt+7)
                    kbyt(2)=ibuff(ipt+8)
                    de1(i)=float(ien)
                    de1(i)=de1(i)/10.
                                                                        
c --- Cas des couronnes 2 a 9                                           
                                                                        
                  elseif(ilongevt.eq.i_long3) then                      
                                                                        
                    de_mylar(i)=(ibuff(ipt+7))/100.                     
                    de1(i)=(ibuff(ipt+8))/10.                           
                                                                        
                    kbyt(1)=ibuff(ipt+9)
                    kbyt(2)=ibuff(ipt+10)
                    de2(i)=float(ien)
                    de2(i)=de2(i)/10.
                                                                        
                    kbyt(1)=ibuff(ipt+11)
                    kbyt(2)=ibuff(ipt+12)
                    de3(i)=float(ien)
                    de3(i)=de3(i)/10.
                    
                    canal(i,4)=ibuff(ipt+13)  !  canal  SI GG
                    canal(i,7)=ibuff(ipt+14)  !  canal  CsI R
                    canal(i,8)=ibuff(ipt+15)  !  canal  CsI L
                    
c --- Cas des couronnes 10 a 17  (sans Etalons)                         
                                                                        
                  elseif(ilongevt.eq.i_long4) then                      
                                                                        
                    de_mylar(i)=(ibuff(ipt+7))/100.                     
                    de1(i)=(ibuff(ipt+8))/10.                           
                                                                        
                    kbyt(1)=ibuff(ipt+9)
                    kbyt(2)=ibuff(ipt+10)
                    de3(i)=float(ien)
                    de3(i)=de3(i)/10.
                    
                    canal(i,7)=ibuff(ipt+11)  !  canal  CsI R
                    canal(i,8)=ibuff(ipt+12)  !  canal  CsI L           
                                                                        
c --- Cas des couronnes 10 a 17  (avec Etalons)                         
                                                                        
                  elseif(ilongevt.eq.i_long5) then                      
 
                    de_mylar(i)=(ibuff(ipt+7))/100.                     
                    de1(i)=(ibuff(ipt+8))/10.                           
                    de4(i)=(ibuff(ipt+9))/10.                           
                                                                        
                    kbyt(1)=ibuff(ipt+10)
                    kbyt(2)=ibuff(ipt+11)
                    de5(i)=float(ien)
                    de5(i)=de5(i)/10.
                                                                        
                    kbyt(1)=ibuff(ipt+12)
                    kbyt(2)=ibuff(ipt+13)
                    de3(i)=float(ien)
                    de3(i)=de3(i)/10.
                    
                    canal(i,7)=ibuff(ipt+14)  !  canal  CsI R
                    canal(i,8)=ibuff(ipt+15)  !  canal  CsI L
                                                                        
                  else                                                  
                                                                        
                    write(6,*) 'Bloc=',nbloclus,' Evt:',nevt,'bizarre!'
                    write(6,*) 'En effet, ilongevt =',ilongevt,' !?'    
                    write(6,*) 'On passe au bloc suivant !!!?' 
                    return   
                                                                        
                  end if                                                
 
                  dde1=de1(i)
                  dde2=de2(i)
                  dde3=de3(i)
                  dde4=de4(i)
                  dde5=de5(i)
                  if(dde1.lt.0)dde1=0.
                  if(dde2.lt.0)dde2=0.
                  if(dde3.lt.0)dde3=0.
                  if(dde4.lt.0)dde4=0.
                  if(dde5.lt.0)dde5=0.
                  ener(i)=(dde1+dde2+dde3+dde4+dde5)+de_mylar(i)
 
              endif
 
              ipt=ipt+abs(ilongevt)                                     
 
            end do

                
c --- Routine de correction des Identifications Etalons (L. Tassan-Got) 
 
            call PROPNET
 
c --- Routine utilisateurs pour le traitement evt/evt 
                  
            copy_event=.false.                                          
            call TRAITEMENT
            
            if(fin_lect_run)evt_a_lire=.false.
                                                                        
101         format(1x,5(i6,2x))                                         
                                                                        
         end if                                                         
                                                                        
      end do                                                            
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                  
c---------------------------------------------------------------------- 
c --- Routine de decodage des blocs evts   (VEDA Version 3)             
c---------------------------------------------------------------------- 
      subroutine DEC_BLOC_DST_3(ibuff)                                  
                                                                        
      integer*2 ibuff(8192)                                             
      integer*4 ivalz(300),jvalz                                        
      integer*2 nevt_low,nevt_high
      integer*4 code16,code4(4),ien
      logical   evt_a_lire,prt 
      integer*2 kbyt(2)
                                                                        
      Real*4       tab_pw(12),hit_pw(12)
      common /TABPW/tab_pw,hit_pw      

      include 'veda_rel.incl'                                           
      include 'veda_wri.incl'
      include 'veda_5.incl'                                             
      include 'veda_6.incl'                                             
                                                                        
      common /VEDADUMP/ivalz
      Equivalence (ien,kbyt)
 
      ipt=9                                                             
      evt_a_lire=.True.                                                 
      prt=.False.  
                                                                        
                                                                        
c --- Boucle sur le nombre d'evt a decoder dans le bloc                 
                                                                        
      nbloc=nbloc+1                                                     
      nbuffevt=0                                                        
                                                                        
      if(prt) then                                                      
         write(10,*) 'Impression Bloc #',nbloc                          
         do i=1,8192,8                                                  
         write(10,1) (ibuff(i+k),k=0,7)                                 
         end do                                                         
1     format(1x,8(i5,1x))                                               
      end if                                                            
                                                                        
      do while(evt_a_lire)                                              
                                                                        
         ilong=ibuff(ipt)                                               
         nkbyte=nkbyte+ilong*2                                          
                                                                        
c --- Fin de lecture du bloc courant                                    
                                                                        
         if(ilong.eq.0.or.ipt.ge.8192) then   ! JLC 06/02/04            0
c         if(ilong.eq.0) then                                           
                                                                        
            evt_a_lire=.False. 
                                                     
         else                                                           
                                                                        
c --- Decodage de l'evt NEVTLUS                                         
                                                                        
            nbuffevt=nbuffevt+1                                         
            nevtlus=nevtlus+1                                           
            nevt_tot=nevt_tot+1                                         
            nevt_low=ibuff(ipt+1)                                       
            nevt_high=ibuff(ipt+2)                                      
            npart_traite=ibuff(ipt+3)                                   
            code_evt=ibuff(ipt+4)                                       
            mrejet=ibuff(ipt+5)                                         
            iztot=ibuff(ipt+6)                                          
            ztot=float(iztot)                                           
            mcha=ibuff(ipt+7)                                           
                                                                        
c --- Reconstitution de NEVT...                                         
 
            iflag=0
            if(nevt_high.lt.0) iflag=1                                  
            if(iflag.eq.0) then
               num_evt_brut=nevt_low*2**16+nevt_high                    
            else
               num_evt_brut=nevt_low*2**16+nevt_high+65535              
            end if
            nevt=nevt+1                                                 
            
c --- Remplissage du buffer d'ecriture "ievt" pour l'evenement 
                          
            if(ilong.gt.wmax)then
               print *,' -------------------------------------------'
               print *,' ATTENTION DANGER ...'
               print *,' ilong = ',ilong,' > wmax = ',wmax
               print *,' voir veda_wri.incl ! violation de memoire ! '
               print *,' evt = ',nevtlus,' evt brut = ',num_evt_brut
               print *,' npart_traite, iztot , mcha = ',
     &                   npart_traite,iztot,mcha
               print *,' dump de cet evt: '
               do k=1,ilong
               print *,'    k,ibuff = ',k,ibuff(ipt+k-1)
               enddo
               print *,' -------------------------------------------'

c               stop 'DEC_BLOC_DST'
                return
            endif  
                                                                        
            do k=1,ilong
            ievt(k)=ibuff(ipt+k-1)
            enddo
            do k=ilong+1,wmax
            ievt(k)=0
            enddo   
                                                                        
c --- Positionnement du pointeur de lecture                             
                                                                        
            ipt=ipt+i_entete                                            
                                                                        
c --- Decodage caracteristiques des particules...                       
                                                                        
            do i=1,npart_traite                                         
                                                                        
               de1(i)=0.
               de2(i)=0.
               de3(i)=0.
               de4(i)=0.
               de5(i)=0.
               de_mylar(i)=0.
               code_cali(i)=0.
               ener(i)=0.
               ecode(i)=0
               do k=1,4
               code_energie(k,i)=0
               enddo
 
               ilongevt=ibuff(ipt)                                      
               module=ibuff(ipt+1)                                      
               icou(i)=module/100                                       
               imod(i)=mod(module,100)                                  
               
                                                                        
c --- Si Gamma ( Code = 0 )                                             
                                                                        
               if(ilongevt.eq.i_long1) then                             
                                                                        
                  z(i)=0                                                
                  a(i)=0                                                
                  z_indra(i)=0.                                         
                  a_indra(i)=0.                                         
 
                  code_part(i)=11*16  ! code gamma=11 dans IDENLCP      
                  code16=code_part(i)                                   
                  call DECODE_PART(icou(i),code16,code4)                
                  code(i)=code4(1)                                      
                  do k=1,4
                  code_idf(k,i)=code4(k)                                
                  enddo
 
                  if(icou(i).eq.1)then
                    de1(i)=ibuff(ipt+2)                                 
                  else
                    de3(i)=ibuff(ipt+2)                                 
                  endif
                  mt(i)=ibuff(ipt+3)                                    
                                                                        
              else
 
                  ib=ibuff(ipt+2)                                       
                  z_indra(i)=float(ib)/100.                             
                  z(i)=nint(z_indra(i))                                 
                  ib=ibuff(ipt+3)                                       
                  a_indra(i)=float(ib)/100.                             
                  a(i)=nint(a_indra(i))                                 
                  code_part(i)=ibuff(ipt+4)                             
                  code16=code_part(i)                                   
                  call DECODE_PART(icou(i),code16,code4)                
                  code(i)=code4(1)                                      
                  do k=1,4
                  code_idf(k,i)=code4(k)                                
                  enddo
 
c --- Correction des cartouches 38KDC creees entre le 19 et 25/6/95 ---
c     Pb du code=9 trop souvent present, corrige ensuite dans VEDA 3.2
 
                  if((kjour.ge.19.and.kjour.le.25).and.
     &                kmois.eq.6.and.kanne.eq.95)then
                    if(icou(i).ge.10)call CORRECT_BACK (num_evt_brut,i)
                  endif
c -----
                  code_cali(i)=ibuff(ipt+5)
 
                  code16=code_cali(i)                                   
                  code_energie(1,i)=jbyt(code16,1,4)                    
                  code_energie(2,i)=jbyt(code16,5,4)                    
                  code_energie(3,i)=jbyt(code16,9,4)                    
                  code_energie(4,i)=jbyt(code16,13,4)                   
                  ecode(i)=code_energie(1,i)                            
                                                                        
                  mt(i)=ibuff(ipt+6)
 
c --- Cas des Phoswichs (couronne = 1)                                  
                                                                        
                  if(ilongevt.eq.i_long2) then                          
                                                                        
                    kbyt(1)=ibuff(ipt+7)
                    kbyt(2)=ibuff(ipt+8)
                    de1(i)=float(ien)
                    de1(i)=de1(i)/10.
                                                                        
c --- Cas des couronnes 2 a 9                                           
                                                                        
                  elseif(ilongevt.eq.i_long3) then                      
                                                                        
                    de_mylar(i)=(ibuff(ipt+7))/100.                     
                    de1(i)=(ibuff(ipt+8))/10.                           
                                                                        
                    kbyt(1)=ibuff(ipt+9)
                    kbyt(2)=ibuff(ipt+10)
                    de2(i)=float(ien)
                    de2(i)=de2(i)/10.
                                                                        
                    kbyt(1)=ibuff(ipt+11)
                    kbyt(2)=ibuff(ipt+12)
                    de3(i)=float(ien)
                    de3(i)=de3(i)/10.
                                                                        
c --- Cas des couronnes 10 a 17  (sans Etalons)                         
                                                                        
                  elseif(ilongevt.eq.i_long4) then                      
                                                                        
                    de_mylar(i)=(ibuff(ipt+7))/100.                     
                    de1(i)=(ibuff(ipt+8))/10.                           
                                                                        
                    kbyt(1)=ibuff(ipt+9)
                    kbyt(2)=ibuff(ipt+10)
                    de3(i)=float(ien)
                    de3(i)=de3(i)/10.
                                                                        
c --- Cas des couronnes 10 a 17  (avec Etalons)                         
                                                                        
                  elseif(ilongevt.eq.i_long5) then                      
 
                    de_mylar(i)=(ibuff(ipt+7))/100.                     
                    de1(i)=(ibuff(ipt+8))/10.                           
                    de4(i)=(ibuff(ipt+9))/10.                           
                                                                        
                    kbyt(1)=ibuff(ipt+10)
                    kbyt(2)=ibuff(ipt+11)
                    de5(i)=float(ien)
                    de5(i)=de5(i)/10.
                                                                        
                    kbyt(1)=ibuff(ipt+12)
                    kbyt(2)=ibuff(ipt+13)
                    de3(i)=float(ien)
                    de3(i)=de3(i)/10.
                                                                        
                  else                                                  
                                                                        
                write(i_out,*) 'Evt #',nevt,',Part. non repertoriee :',i
                write(i_out,*) 'En effet, Longpart =',ilongevt,' !?'    
                                                                        
                  end if                                                
 
                  dde1=de1(i)
                  dde2=de2(i)
                  dde3=de3(i)
                  dde4=de4(i)
                  dde5=de5(i)
                  if(dde1.lt.0)dde1=0.
                  if(dde2.lt.0)dde2=0.
                  if(dde3.lt.0)dde3=0.
                  if(dde4.lt.0)dde4=0.
                  if(dde5.lt.0)dde5=0.
                  ener(i)=(dde1+dde2+dde3+dde4+dde5)+de_mylar(i)
 
              endif
 
              ipt=ipt+ilongevt                                          
 
            end do

                
c --- Routine de correction des Identifications Etalons (L. Tassan-Got) 
 
            call PROPNET
 
c --- Routine utilisateurs pour le traitement evt/evt 
                  
            copy_event=.false.                                          
            call TRAITEMENT
            
            if(fin_lect_run)evt_a_lire=.false.
                                                                        
101         format(1x,5(i6,2x))                                         
                                                                        
         end if                                                         
                                                                        
      end do                                                            
                                                                        
      return                                                            
      end                                                               
                                                                        
c---------------------------------------------------------------------- 
                                                                        
c--------------------------------------------------------------------   
c  Routine de travail, dumpe un evt                                     
c--------------------------------------------------------------------   
      subroutine DUMP_EVT(iout)                                         
                                                                        
      include 'veda_rel.incl'                                           
      include 'veda_6.incl'                                             
 
      integer*4  ivalz(300)
      common /VEDADUMP/ivalz
 
      write(iout,3)                                                     
      write(iout,2) nevtlus,npart_traite                                
      write(iout,4)                                                     
      do i=1,npart_traite                                               
         write(iout,1) icou(i),imod(i),z_indra(i),a_indra(i),code(i),
     &                 z(i),a(i),code_part(i),(code_idf(k,i),k=1,4)
      end do                                                            
                                                                        
      write(iout,3)                                                     
      write(iout,*) ' '                                                 
                                                                        
1     format(1x,i2,':',i2,3x,f6.2,1x,f6.2,1x,i4,2i5,4x,z4,2x,4i5)
2     format(1x,'Evt numero ',i8,' : ',i4,' particules')                
3     format(60('-'))                                                   
4     format(1x,' Cour       Z     A     Code  Z    A   code_part',
     &          '         code_idf')                                    
      return                                                            
      end                                                               
                                                                        
c---------------------------------------------------------------------- 
c     Routine de decodeage des codes d'identification 16 bits DST       
c                                                                       
c  Elle fournit sous forme d'un tableau de 4 entiers*4 les differents   
c  codes ORIGINAUX d'identification ainsi que le code general defini    
c  dans le fichier VEDA_6 INCL sur le disque INDRA 200 .                
c-------------------------------------------------------------------    
      Subroutine DECODE_PART (jcou,icode,code4)                         
                                                                        
      Integer*4   code4(4)                                              
      Integer*4   icode,jcou,ik,icodg                                   
      integer*2   codganil(0:15),codlpc(0:15)                           
                                                                        
      data codganil/10,15,12,16,30,35,32,-200,20,22,-200,11,            
     &               -99,-999,-200,-111/                                
      data codlpc/0,100,1,101,2,102,3,4,-300,-300,-300,-300,-300,       
     &               997,998,999/                                       
                                                                        
      code4(1)=jbyt(icode,1,4)                                          
      code4(2)=jbyt(icode,5,4)                                          
      code4(3)=jbyt(icode,9,4)                                          
      code4(4)=jbyt(icode,13,4)                                         
                                                                        
      icodg=code4(1)                                                    
      if(jcou.eq.1)then                                                 
         ik=code4(2)                                                    
         if(ik.eq.4)code4(2)=-1                                         
                                                                        
      elseif (jcou.le.9)then                                            
                                                                        
         if(icodg.eq.0)then
                       ik=11
                       code4(2)=codganil(ik)
         endif                                                          
         if(icodg.eq.1.or.icodg.eq.2.or.icodg.eq.9.or.icodg.eq.10)then  
                           ik=code4(2)                                  
                           code4(2)=codganil(ik)                        
         elseif(icodg.eq.3)then                                         
                       code4(2)=codganil(1)
                       ik=code4(3)                                      
                       code4(3)=codlpc(ik)                              
         endif
      endif                                                             
                                                                        
c     write(*,'(2x,i6,2x,z4,4x,4i4)')jcou,icode,(code4(i),i=4,1,-1)     
      return                                                            
      end                                                               
c---------------------------------------------------------------------  

      subroutine ATOC(itab,long,chaine)
 
      integer*2     itab(*)
      integer*4     long
      character*(*) chaine
 
      do i=1,long
         ic=itab(i)
         chaine(i:i)=CHAR(ic)
      end do
 
      return
      end
c---------------------------------------------------------------------  

      subroutine ATOC_EBCDIC (itab,long,chaine)
 
      integer*2     itab(*)
      integer*4     long
      character*(*) chaine
      Integer*4  iascii(256)      
      Common/ASKI/iascii
 
      do i=1,long
         ic=itab(i)
         ic=iascii(ic)  ! transformation code EBCDIC en code ASCII
         chaine(i:i)=CHAR(ic)
      end do
 
      return
      end
c----------------------------------------------------------------       
c Cette routine realise la conversion Caractere --> Integer*2 en        
c code ASCII ( contenu dans le tableau Ich)                             
c----------------------------------------------------------------       
      subroutine CTOA(chaine,long,ich)                                  
                                                                        
      character*(*) chaine                                              
      integer*4     long,icha                                           
      integer*2     ich(*)                                              
                                                                        
      do i=1,long                                                       
         icha=ICHAR(chaine(i:i))                                        
         ich(i)=icha                                                    
      end do                                                            
                                                                        
      return                                                            
      end                                                               
      
c------------------------------------------------------------------

      Subroutine DECODE_SCALER
     &      (buffer,contenu,nb_echelle_a_imprimer,iwt)
 
c *** Routine qui decode le bloc SCALER et remplit le tableau "contenu"
 
c --- 25-01-94 --- Auteur: J-L Charvet (Charvet@FRCPN11)
 
c     iwt = 0   : pas d'impression de tous les blocs "SCALER"
c     iwt <>0   : imp. sur file associe au num. logique iwt
c     iwt =-1   : retour qui signifie un pb dans le decodage
 
c ----------------------------------------------------------------------
 
      Parameter     (nb_echelle_max=255)
 
      Integer*2    buffer(8192),ibuf1,ibuf2
      Integer*4    buffer_Scaler(4096),ibuf4
      Integer*4    iwt
      Integer*4    nb_echelle_a_imprimer
 
      Integer*4    Icount_bloc
      Integer*4    Long_scaler
      Integer*4    Nb_channel
      Integer*4    Status_fix
      Integer*4    Reserv_fix1,Reserv_fix2
 
      Integer*4    Label(nb_echelle_max)
      Integer*4    Status(nb_echelle_max)
      Integer*4    Contenu(nb_echelle_max)
      Integer*4    Frequence(nb_echelle_max)
      Integer*4    Tics(nb_echelle_max)
      Integer*4    Reserve(nb_echelle_max,3)
 
c      Common       ibuf2,ibuf1
c      Equivalence  (ibuf4,ibuf2)
 
      do i=1,4096
      k=2*i-1
      ibuf1=buffer(k)
      ibuf2=buffer(k+1)
      ibuf4=ibuf2*(2**16)+ibuf1
      if(ibuf1.lt.0)ibuf4=ibuf4+2**16
      buffer_scaler(i)=ibuf4
      enddo
 
      k=0
      Icount_bloc=buffer_scaler(3)
      Long_scaler=buffer_scaler(4)
      Nb_Channel =buffer_scaler(5)
      Status_fix =buffer_scaler(6)
      Reserv_fix1=buffer_scaler(7)
      Reserv_fix2=buffer_scaler(8)
      do i=9,2048,8
      k=k+1
      if(k.gt.nb_echelle_max)then
         write
     &   (*,'('' DECODE_SCALER: nb_echelle > '',i4)')nb_echelle_max
         iwt=-1
         return
      endif
      Label(k)=buffer_scaler(i)
      Status(k)=buffer_scaler(i+1)
      Contenu(k)=buffer_scaler(i+2)
      Frequence(k)=buffer_scaler(i+3)
      Tics (k)=buffer_scaler(i+4)
      Reserve(k,1)=buffer_scaler(i+5)
      Reserve(k,2)=buffer_scaler(i+6)
      Reserve(k,3)=buffer_scaler(i+7)
      enddo
 
      nb_echelle_lu=k
      if(iwt.gt.0)then
 
      write(iwt,100)icount_bloc,nb_echelle_lu
100   format(/,' Compteur bloc = ',i8,/,
     &         ' Nb_echelle lu = ',i8)
      write(iwt,101)long_scaler,nb_channel,status_fix,
     &reserv_fix1,reserve_fix2
101   format(/,' Long_scaler   = ',i8,/,
     &         ' Nb_channel    = ',i8,/,
     &         ' Status_fixe   = ',i8,/,
     &         ' Reserve_1 & 2 = ',2i8)
 
      write(iwt,102)
102   format(/,80('-'),/,
     &' Numero    Label    Status   Contenu   Frequence',
     &'   Tics(*10ms)    Reserve(3)',/,
     &80('-'))
 
      do k=1,nb_echelle_a_imprimer
      write(iwt,103)k,Label(k),Status(k),Contenu(k),Frequence(k),
     &Tics(k),(Reserve(k,j),j=1,3)
103   format(i4,2i10,2x,i10,1x,i7,6x,i10,1x,3i5)
      enddo
      endif
 
c      write(iwt,'(/,'' Bloc_scaler: '',i5)') nbloc_scaler
c      write(iwt,'(10(1x,z4))')(buffer(i),i=1,512)
c      write(iwt,'(/)')
c      write(iwt,'(5(1x,z8))')(buffer_scaler(i),i=1,256)
c      write(iwt,'(/)')
 
      return
      end
 
c **********************************************************************
 
      Subroutine PRINT_SCALER (iwt)
 
c *** Routine qui imprime le contenu des "echelles" sur iwt
 
c --- 25-01-94 --- Auteur: J-L Charvet (Charvet@FRCPN11)
 
c     iwt = 0 : impression sur console (par defaut)
c     iwt > 0 : impression sur fichier associe au num. logique iwt
 
c ----------------------------------------------------------------------
 
      Include 'veda_rel.incl'  
      Character*18  Nom_echelle(nb_echelle_max)
c
      
      nom_echelle(1)='Faraday 1'
      nom_echelle(2)='Faraday 2'
      nom_echelle(3)='DEC'
      nom_echelle(4)='FC'
      nom_echelle(5)='OK'
      nom_echelle(6)='FT'
      nom_echelle(7)='AVL'
      nom_echelle(8)='OCD'
      nom_echelle(9)='Gene LASER'
      nom_echelle(10)='Gene ELECT'
      nom_echelle(11)='Gene TEST'
      nom_echelle(12)='Gene DIRECT'
      nom_echelle(13)='Gene TM'
      nom_echelle(14)='Gene MARQ'
      nom_echelle(15)='OA'
      nom_echelle(16)='RAZ'
 
      do i=17,nb_echelle_max
      Nom_echelle(i)=' '
      enddo
 
      if(iwt.eq.0)iwt=6
 
      write(iwt,100)
100   format(//,10x,' *** IMPRESSION DES ECHELLES ***',/,
     &11x,31('*'))
 
      write(iwt,102)
102   format(/,7x,40('-'),/,7x,
     &' Echelle          Nom           Contenu ',/,7x,40('-'))
 
      do k=1,nb_echelle_a_imprimer
      write(iwt,103)k,Nom_echelle(k),echelle(k)
103   format(7x,i5,5x,a,i10)
      enddo
 
      return
      end
 
                                                                        
c--------------------------------------------------------------------   
c --- Routine de decodage du bloc ETAT d'INDRA                          
c--------------------------------------------------------------------   
      subroutine DEC_BLOC_ETAT(ibuff,irc)                               
                                                                        
      integer*2     ibuff(8192)                                         
      character*80  message,msg(0:100)
      integer*4     lon_msg,nb_msg,code_msg(0:100)
      integer*2     itab(80)
      integer*4     ierr(4)
                                                                        
      include     'veda_rel.incl'                                       
      include     'veda_wri.incl' 
      include     'veda_6.incl'  
 
c --- Initialisation de Module_panne (=0 : tout OK)
 
      do i=1,17
      do j=1,24
      do k=1,5
      Module_panne(i,j,k)=0                                             
      enddo
      enddo
      enddo
c ---
      irc=0
      iwf=i_out
c      write(iwf,100)
      write(*,100)
100   format(/,' ETAT D INDRA PENDANT LE RUN ',/,28('-'),/)
 
c      print *,'DECODAGE DU Bloc ETAT '
c      write(*,'(20i4)')(ibuff(i),i=1,256)
      i1=1
      ipt=8
 
c --- signification des codes panne
 
      lon_msg=ibuff(ipt+1)
      nb_msg=ibuff(ipt+2)
      ipt=ipt+2
      do i=1,nb_msg
      code_msg(i)=ibuff(ipt+1)
      ii=code_msg(i)
      ipt=ipt+1
      do j=1,lon_msg
      itab(j)=ibuff(ipt+j)
      enddo
      if(char_ascii)then
         call ATOC (itab,lon_msg,message)
      else
         call ATOC_EBCDIC (itab,lon_msg,message)
      endif   
      msg(ii)=message
      ipt=ipt+lon_msg
      enddo
      irc=ibuff(ipt+1)+1
      if(irc.ne.0)return
      ipt=ipt+1
 
c ---
 
      do while (i1.ne.0)
      i1=ibuff(ipt+1)
      i2=ibuff(ipt+2)
      if(i1.ne.0)then
         ierr(1)=i1/100
         ierr(2)=mod(i1,100)
         ierr(3)=i2/100
         ierr(4)=mod(i2,100)
         message=msg(ierr(4))
         call WARNING_INDRA(iwf,ierr,message,lon_msg)
         if(ierr(2).eq.0)then
            do j=1,24
            Module_panne(ierr(1),j,ierr(3))=ierr(4)
            enddo
         else
            Module_panne(ierr(1),ierr(2),ierr(3))=ierr(4)
         endif
         ipt=ipt+2
      endif
      enddo
 
c     do i=1,17
c     do j=1,24
c     do k=1,5
c     ico=Module_panne(i,j,k)                                           
c     if(ico.ne.0)then
c       print *,' c = ',i,' m = ',j,' Det = ',k,' cod = ',ico,msg(ico)
c     endif
c     enddo
c     enddo
c     enddo
c ---
      return                                                            
      end                                                               
                                                                        
c ----------------------------------------------------------------------
 
       subroutine WARNING_INDRA (iwf,ierr,msg,lon_msg)
 
       character*80  msg
       character*10  tip(5), tipo
       integer*4 iwf,ierr(4),lon_msg
 
 
       data tip /'CHIO','SI','CsI','SI75','SILI'/
 
       if(ierr(1).eq.1)then
        tipo = 'PHOSWICH'
       else
        tipo = tip(ierr(3))
       endif
       if(ierr(2).eq.0)then
c        write(iwf,10)ierr(1),tipo,ierr(4),msg(1:lon_msg)
        write(*,10)ierr(1),tipo,ierr(4),msg(1:lon_msg)
       else
c        write(iwf,20)ierr(1),ierr(2),tipo,ierr(4),msg(1:lon_msg)
        write(*,20)ierr(1),ierr(2),tipo,ierr(4),msg(1:lon_msg)
       endif
 
10     format(' Cour. ',i3,' Tous modules  ',a5,2x,'(',i2,') : ',a)
20     format(' Cour. ',i3,' Module : ',i3,2x,a5,2x,'(',i2,') : ',a)
       return
       end
 
 
c-------------------------------------------------------------------    
                                                                        
      Subroutine CORRECT_BACK (nevt,i)                                  
                                                                        
      integer*4  nevt,i,cod                                             
                                                                        
      Include 'veda_6.incl'                                             
                                                                        
      cod=code_idf(1,i)
      if(cod.eq.9)then
         if(code_idf(2,i).eq.7)cod=2
      endif
 
      if(cod.eq.10)then
         if(code_idf(2,i).eq.1.or.code_idf(2,i).eq.2)cod=3
         if(code_idf(4,i).eq.2)cod=4
      endif
 
      code_idf(1,i)=cod
      code(i)=cod
 
      return
      end
 
c ----------------------------------------------------------------------
 
      Subroutine READ_BUFFER(lun,nbloc,buffer,istatus)                 
                                                                  
      Parameter (lr=8192)
      integer*2  buffer(lr)
      integer*4  lun,irc,nwrec,nwtak,istatus

      nwrec=4096
      nwtak=nwrec
      istatus=0

c --- lecture d'un bloc                                           
                                                                    
      call cfget (lun,0,nwrec,nwtak,buffer,irc)
      istatus=irc ! irc=-1 : fin de fichier detecte
      nbloc=nbloc+1

      if(irc.gt.0)then
         print *,' !!!! Error cfget : irc = ',irc,' nbloc = ',nbloc
c          stop 'READ_BUFFER'    
      endif   
      
      return                                                    
      end
                                                                  
c ----------------------------------------------------------------------
 
      Subroutine OPEN_FILE(filein,lun)                 

      Character*80 filein
      integer*4    lun,irc,nwrec,nwtak

      nwrec=4096
      nwtak=nwrec
     
      call cfopen (lun,0,nwrec,'r',0,filein,irc)
     
      if(irc.ne.0)then
           print *, ' !!!! Error cfopen : irc = ',irc,' file : ',filein
           lun=-100
           return
      endif     

      return
      end
c ----------------------------------------------------------------------
 
      Subroutine WRITE_BUFFER(lun,nbloc,buffer,istatus)                 
                                                                  
      Parameter (lr=8192)
      integer*2  buffer(lr)
      integer*4  lun,istatus

      istatus=0

c --- ecriture d'un bloc                                           
                                                                    
      write(lun)buffer
      nbloc=nbloc+1

      return                                                    
      end
                                                                        
c ----------------------------------------------------------------------

      Subroutine VAR_ENVIRONNEMENT 
      
      include   'veda_11.incl'
            
      call GETENVF ('VEDA_DATA',vedafil)
      i=0
      long_path=0
      do while (long_path.eq.0)
      i=i+1
      if(vedafil(i:i).eq.' ')long_path=i-1
      enddo
      if(long_path.gt.3)then
         write(*,'(/,'' Chemin des fichiers DATA      = '',a)')
     &                                          vedafil(1:long_path)
      endif
      
!      call GETENVF ('VEDA_REFE',vedafil2)
!      i=0
!      long_path2=0
!      do while (long_path2.eq.0)
!      i=i+1
!      if(vedafil2(i:i).eq.' ')long_path2=i-1
!      enddo            
!      if(long_path2.gt.3)then
!         write(*,'('' Chemin des fichiers REFERENCE = '',a)')
!     &                                          vedafil2(1:long_path2)
!     endif
      
!      call GETENVF ('VEDA_PNIA',vedapnia)
!      i=0
!      long_dap=0
!      do while (long_dap.eq.0)
!      i=i+1
!      if(vedapnia(i:i).eq.' ')long_dap=i-1
!      enddo
!      if(long_dap.gt.3)then
!         write(*,'('' Chemin des fichiers DAPNIA    = '',a)')
!     &                                          vedapnia(1:long_dap)
!     endif
            
!      call GETENVF ('VEDA_OUT',vedaout)
!      i=0
!      long_out=0
!      do while (long_out.eq.0)
!      i=i+1
!      if(vedaout(i:i).eq.' ')long_out=i-1
!      enddo
!      if(long_out.gt.3)then
!         write(*,'('' Chemin des fichiers OUT (HBK) = '',a)')
!     &                                          vedaout(1:long_out)
!      endif
            
      call GETENVF ('RUN_PREFIX',nomjob)
      i=0
      long_job=0
      do while (long_job.eq.0)
      i=i+1
      if(nomjob(i:i).eq.' ')long_job=i-1
      enddo
      if(long_job.gt.3)then
         write(*,'('' Nom du Prefix ='',a)')
     &                                          nomjob(1:long_job)
      endif
      
      
		write(*,*)
            
      return
      end
      
c ----------------------------------------------------------------------
c ----------------------------------------------------------------------
c
c   *********************************************************
c   * ROUTINE DE CORRECTION RELATIVE AUX IDENT. DES ETALONS *
c   *********************************************************
c                  L. Tassan-Got
c
c  Modification du 27/7/95:
c  Des particules etaient creees dans la Chio (couronnes 10 a 17)
c   quand seules des particules atteignant le Csi a travers un etalon
c   etaient presentes : la fourchette de coherence restait a 0.
c
c    Modification du 18/10/95 pour affecter le code=3 (entre les lignes)
c  aux Z=1 et Z=2 identifies dans des etalons et ayant des masses hors
c  gamme.
c ----------------------------------------------------------------------

      subroutine propnet
      implicit  none
c
      include 'veda_6.incl'  
      include 'veda_rel.incl'  
c
      integer     i, j, jp, ic, im, ic2, im2, icch, imch, nte, ncs
      integer     izz, iaa, kan
      integer*4   mn_cou, mx_cou
      parameter   (mn_cou=10, mx_cou=17)
      integer*4   n_cou(mn_cou:mx_cou), m_etl(mn_cou:mx_cou)
      integer*4   mch_etl(mn_cou:mx_cou)
      integer*4   nucou_ch(mn_cou:mx_cou)
      logical     bcl, okc, oks, mdf1, mdf2
      integer     iabs, mod
c
      data  n_cou/ 2,1,1,1,2,1,2,1 /
      data  m_etl/ 2,2,2,4,3,3,2,2 /
      data  mch_etl/ 1,1,1,4,3,3,2,2 /
      data  nucou_ch/ 10,10,12,13,14,14,16,16/
c
      kan = mod(kanne,100)
c  --- Suppression des part. crees dans la Chio apres passage etalon ---
      mdf1 = kan.le.95 .and. kmois.le.7  .and. kjour.le.27
c  --- Affectation code entre lignes pour A aberrant sur Z=1 ou 2 ---
      mdf2 = kan.le.95 .and. kmois.le.10 .and. kjour.le.18
c
      if(.not.mdf2)return
      i = 1
      do while (i.le.npart_traite)
        ic = icou(i)
        im = imod(i)
        if(mdf2 .and. ic.ge.10 .and. ic.le.17 .and. code(i).eq.3) then
          izz = z(i)
          iaa = iabs(a(i))
          oks = izz.eq.1 .and. (iaa.lt.1 .or. iaa.gt.3)
          oks = oks .or. (izz.eq.2 .and. (iaa.lt.3 .or. iaa.gt.6))
          if(oks) then
            code(i) = 10
            code_idf(1,i) = 10
          endif
        endif
        if(mdf1 .and. ic.ge.10 .and. ic.le.17 .and. code(i).eq.7) then
          oks = im.eq.mch_etl(ic) .and. z(i).eq.1
          okc = im.eq.1 .and. ic.le.14
          if(oks.or.okc) then
            nte = 0
            ncs = 0
            do jp = -1,1,2
              j = i+jp
              bcl = .true.
              do while (bcl .and. j.ge.1 .and. j.le.npart_traite)
                ic2 = icou(j)
                im2 = imod(j)
                icch = nucou_ch(ic2)
                if(icch.eq.13) then
                  imch = ((im2-1)/3)*3+1
                else if(icch.eq.16) then
                  imch = im2
                else
                  imch = ((im2-1)/2)*2+1
                endif
                bcl = icch.eq.ic .and. imch.eq.im
                if(bcl) then
                  if(code(j).eq.3) then
                    nte = nte+1
                  else
                    ncs = ncs+1
                  endif
                endif
                j = j+jp
              enddo
            enddo
            if(nte.ge.1 .and. ncs.eq.0 .and. oks) then
              mcha = mcha-1
              ztot = ztot-z(i)
              npart_traite = npart_traite-1
              do j = i,npart_traite
                ic = j+1
                z(j) = z(ic)
                a(j) = a(ic)
                z_indra(j) = z_indra(ic)
                a_indra(j) = a_indra(ic)
                icou(j) = icou(ic)
                imod(j) = imod(ic)
                code_part(j) = code_part(ic)
                code_cali(j) = code_cali(ic)
                code(j) = code(ic)
                ecode(j) = ecode(ic)
                do jp = 1,4
                  code_idf(jp,j) = code_idf(jp,ic)
                enddo
                do jp = 1,4
                  code_energie(jp,j) = code_energie(jp,ic)
                enddo
                do jp = 1,15
                  canal(j,jp) = canal(ic,jp)
                enddo
                ener(j) = ener(ic)
                de1(j) = de1(ic)
                de2(j) = de2(ic)
                de3(j) = de3(ic)
                de4(j) = de4(ic)
                de5(j) = de5(ic)
                de_mylar(j) = de_mylar(ic)
                mt(j) = mt(ic)
              enddo
              i = i-1
            else if(nte.eq.0 .and. ncs.eq.0 .and. okc) then
              code(i) = 5
              code_idf(1,i) = 5
              code_part(i) = code_part(i)-2
            endif
          endif
        endif
        i = i+1
      enddo
      return
      end
c ----------------------------------------------------------------------------

      Subroutine ECRIT_EVT_SELECT 
      
      include 'veda_rel.incl'                                         
      include 'veda_wri.incl'

      integer*2   itab(8)
               
      if(wpointeur.eq.0)then
      
         call CTOA ('BLOCDATA',8,itab)            
         do i=1,8
         wbuffer(i)=itab(i)
         enddo
         do i=9,8192
         wbuffer(i)=0
         enddo
         wpointeur=8
         
      else
      
         ifin=wpointeur + ievt(1)
         if(ifin.ge.8192)then 
         
           call WRITE_BUFFER (lunw,nblocecr,wbuffer,istatus)

           call CTOA ('BLOCDATA',8,itab)            
           do i=1,8
           wbuffer(i)=itab(i)
           enddo
           do i=9,8192
           wbuffer(i)=0
           enddo
           wpointeur=8
                     
         endif
         
       endif
       
       do i=1,ievt(1)
       k=wpointeur+i
       wbuffer(k)=ievt(i)
           
       enddo
       wpointeur=wpointeur+ievt(1)
       nevtecr=nevtecr+1
       
       return
       end
       
c ------------------------------------------------------------------------- 
            
       Subroutine INI_ASCII
       
c Routine construisant le tableau iascci : code ASCII = iascii (code EBCDIC)       
       
       Integer*4  iascii(256)
       Common/ASKI/iascii
 
       do i=1,256
       iascii(i)=0
       enddo
       
c ---- 0 -> 9
             
       do i=240,249
       l=i-192
       iascii(i)=l
       enddo

c ---- A -> Z
       
       do i=193,202  ! A -> I
       l=i-128
       iascii(i)=l
       enddo
       
       do i=209,217  ! J -> R
       l=i-135
       iascii(i)=l
       enddo

       do i=226,233  ! S -> Z
       l=i-143
       iascii(i)=l
       enddo
       
c ---- a -> z

       do i=129,137
       l=i-32
       iascii(i)=l
       enddo

       do i=145,153
       l=i-39
       iascii(i)=l
       enddo
       
       do i=162,169
       l=i-47
       iascii(i)=l
       enddo
       
c ---- Divers

       iascii(64)=32  ! space
       iascii(90)=33  ! !
       iascii(127)=34 ! "
       iascii(123)=35 ! #
       iascii(91)=36  ! $
       iascii(108)=37 ! %
       iascii(80)=38  ! &
       iascii(125)=39 ! '
       iascii(77)=40  ! (
       iascii(93)=41  ! )
       iascii(92)=42  ! *
       iascii(78)=43  ! +
       iascii(107)=44 ! ,
       iascii(96)=45  ! -
       iascii(75)=46  ! .
       iascii(97)=47  ! /
       iascii(122)=58 ! :
       iascii(94)=59  ! ;
       iascii(76)=60  ! <
       iascii(126)=61 ! =
       iascii(110)=62 ! >
       iascii(111)=63 ! ?
       iascii(124)=64 ! @
       iascii(224)=92 ! \
       iascii(109)=95 ! _
       iascii(121)=96 ! `
       iascii(192)=123 ! {
       iascii(79)=124  ! |
       iascii(208)=125 ! }
       iascii(161)=126 ! ~
       
       return
       end
       
c----------------------------------------------------------------------------
c --- Routine ecriture de l'entete du fichier                           
c-------------------------------------------------------------------    
      subroutine ECRIT_HEADER_REDUITE (iout,numerun,ibuff)                                    
                                                                        
      character*8 day,hour,nom                                          
      character*5 type                                                  
      integer*2   itab(8)                                               
      integer*2   ibuff(8192),ktab(100)
      integer*4   i_long1,i_long2,i_long3,i_long4,i_long5
      integer*4   iout
      Character*70  critere
      integer*4     new_run,long_critere
      
      Common /RED1/new_run,long_critere,critere                         				
                                                                        
      include 'veda_5.incl'                                             
                                                                        
      character*8 typebloc(4)                                           
      data typebloc/'VEDARED1','BLOCDATA','ETAT_IND',' SCALER '/        
                                                                        
      iversion=5    ! Numero de version de l'ecriture VEDA (1ere campagne)              
                                                                        
c --- Structure du fichier DST, longueurs des evts...                   
                                                                        
      i_entete=8    ! Nombre de mots pour entete EVT                    
      i_long1=4     ! Nombre de mots pour les Gammas                    
      i_long2=9     ! Nombre de mots pour les Phoswichs                 
      i_long3=19    ! Nbre de mots pour les Cour. 2 a 9                 
      i_long4=15    ! Nbre de mots pour les Cour. 10 a 17 (sans Etalons)
      i_long5=18    ! Nbre de mots pour les Cour. 10 a 17 (avec Etalons)

c --- Qq messages d'informations                                        
                                                                        
      write(6,*) ' '                                                    
      write(6,*) ' Ecriture Bloc HEADER_REDUITE :'             
      write(6,*) ' '                                                    

      call DATIMH(day,hour)                                             
                                                                        
      ipt=0                                                             

c --- Ecriture du mot d'entete de bloc                                  
                                                                        
      call CTOA(typebloc(1),8,itab)                                     
      do i=1,8                                                          
         ibuff(ipt+i)=Itab(i)                                           
      end do                                                            
      ipt=ipt+8                                                         
      ibuff(ipt+1)=numerun                                              
      ibuff(ipt+2)=ijour                                                
      ibuff(ipt+3)=imois                                                
      ibuff(ipt+4)=iannee                                               
      ibuff(ipt+5)=nint(zproj)                                          
      ibuff(ipt+6)=nint(aproj)                                          
      ibuff(ipt+7)=nint(zcib)                                           
      ibuff(ipt+8)=nint(acib)                                           
      ibuff(ipt+9)=nint(esura)                                          
      ibuff(ipt+10)=iversion                                            
      ipt=ipt+10                                                        
c      call CTOA(day,8,itab)     ! on garde la date de creation DST     								
c      do i=1,8                  ! a cause des corrections "propnet"    								 
c         ibuff(ipt+i)=Itab(i)                                          
c      end do                                                           
      ipt=ipt+8                                                         
c      call CTOA(hour,8,itab)                                           
c      do i=1,8                                                         
c         ibuff(ipt+i)=Itab(i)                                          
c      end do                                                           
      ipt=ipt+8                                                         
                                                                        
c --- taille des blocs enregistres                                      
                                                                        
      ibuff(ipt+1)=16384                                                
                                                                        
c --- format des mots                                                   
                                                                        
      type='INT*2'                                                      
      ipt=ipt+1                                                         
      call CTOA(type,5,itab)                                            
      do i=1,5                                                          
         ibuff(ipt+i)=Itab(i)                                           
      end do                                                            
      ipt=ipt+6                                                         
      ibuff(ipt)=i_entete                                               
      ibuff(ipt+1)=i_long1                                              
      ibuff(ipt+2)=i_long2                                              
      ibuff(ipt+3)=i_long3                                              
      ibuff(ipt+4)=i_long4                                              
      ibuff(ipt+5)=i_long5                                              
      
      ipt=ipt+6 
      ibuff(ipt)=new_run
      ipt=ipt+1                                                        
      ibuff(ipt)=long_critere
      call CTOA (critere,long_critere,ktab)
      do l=1,long_critere
      ibuff(ipt+l)=ktab(l)
      enddo
      ipt=ipt+long_critere+1
                                                                        
      do i=ipt,8192                                                     
         ibuff(i)=0                                                     
      end do                                                            
                                                                        
c      write(6,'(15(1x,z2))')(ibuff(i),i=1,ipt)                         											
                                                                        
c --- Ecriture des caracteristiques du run sur fichier #Iout            
                                                                        
      write(iout,100)                                                   
100   format(/,' Ecriture Bloc HEADER sur DST Reduite',/,1x,36('-')) 
      write(iout,4) numerun,day,hour                                    
4     format(/,' Run = ',i4,' :  DST_Reduite creee le : ',a8,' a ',a8)  		
                                                                        
      return          																								
      end                                                               
                                                                        
c-------------------------------------------------------------------    

************************************************************************
c
c   Correction apportee aux code energie  des Csi couronnes: 10-17
c
c   Lors de la generation des DST si  E_csi > E. limite que peut mesurer
c   le CsI , la routine CSICALOR retourne le code: code_energie (2,i)=7 
c   Ce code donne ensuite dans VEDA un code general : ecode(i)=3
c
c   Il y avait une erreur sur ce test lors de la generation des DST 
c
c   Le test sur l'energie limite fonction de la longueur du CsI est 
c   donc refait par cette routine a la lecture des DST.
c
c   JLC 8/7/97
c 
************************************************************************

      subroutine COR_ECSI_LIMIT (icou,imod,iz,ia,e_csi,iret)
      
      implicit none
      integer     mindet, maxdet
      PARAMETER   (MINDET  =193, MAXDET  = 336)
      integer     icou,imod,iz,ia,iret
      integer     i, nudet, nmod(17)/2*12, 11*24, 2*16, 2*8/
      real        a,e_csi,emax_a,pre/0.4318/
      real        emax(mindet:maxdet,2)  
     #            /48*166.,24*126.,24*144.,48*129.,
     #             48*664.,24*504.,24*577.,48*517./
     
      nudet = 0
      do i = 1,icou-1
        nudet = nudet + nmod(i)
      enddo
      nudet =  nudet + imod
      
      iret=1
      a=ia
      
* calcul de l'energie max arretee dans le CsI pour Z=1 et 2 et test:
* E(A,e) = E(A0,e) * (A/A0)**0.4318 -- base sur R = C E**1.76 et
* E(A,e) = (A/A0)* E(A0,e*a0/A) pour un Z donne

      if(iz.eq.1) then
        emax_a = emax(nudet,1) * a**pre
        if(e_csi .gt. emax_a) iret = 7
      elseif(iz.eq.2) then
        emax_a = emax(nudet,2) * (a/4.)**pre
        if(e_csi .gt. emax_a) iret = 7
      endif
      
      return
      end
      
c-------------------------------------------------------------------    

      
      
      
