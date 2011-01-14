c************************************************************************
c	Eric Bonnet 04/2010
c  Derniere compilation sous SL5 64 bits
c  g77 -w -I/afs/in2p3.fr/home/throng/indra/veda2/for2 fortran.f -c


c***********************************************************************
c                                                                       
c  KALI : ou (K)artouches (A) (L)ire pour (I)ndra ...                   
c                                                                       
c***********************************************************************
c  Ce programme lit les cartouches DST-INDRA creees pour la 
c  2e campagne (1994). 
c  
c Version originale : 3 Juin 1996 (J-L. Charvet)  
c Mise a jour : 24 Mai 2001 par O. LOPEZ                                
c***********************************************************************
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
c Variable logique a utiliser dans TRAITEMENT pour ecrire des evts selec
c
c   copy_event = .true.          
c                                                                       
c---------------------------------------------------------------------  
c En cas de problemes, contactez les personnes suivantes :              
c                                                                       
c  - O. Lopez      , LPC Caen  , (lopezo@in2p3.fr)                               
c  - J. L. Laville , GANIL     , (Laville@ganil.fr)                               
c  - J. L. Charvet , Saclay    , (jcharvet@cea.fr)                           
c                                                                       
c-------------------------------------------------------------------- 
c  D.Cussol & JDF: 27/09/2007  Version qui copie les fichiers 
c            HPSS sur le repertoire courant (RFIO n'existe plus)

      program Kali_2                                                  
                                                                        
      logical   end_of_file,end_of_program
      integer*2 ibuff(8192),itab(8),ktab(100) 
      Character*1  char1                                    
      character*8  typebloc
      character*80 filein,fileout,namefil
      real*4       time                                                 
      Character*6   cart_dst
c - JDF 27092007
c      Character*6   cart_dst,cartou
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
      integer*4  nblocecr_tot,nevtecr_tot
      logical*1  time_max,linux
      Character*70  critere
      integer*4     long_critere
      integer*4 Nbloc_header,nbloc_etat,nbloc_data,nbloc_scaler,
     &          nbloc_inconnu
      Common /RED1/new_run,long_critere,critere                         
      Common /ASKI/iascii
      character*8 tywrbloc(4)                                           
      data tywrbloc/'VEDARED1','BLOCDATA','ETAT_IND',' SCALER '/        
      
      Character*6  op_system
      Common /SYST/op_system
      
c                                                                       
c --- Definition des variables indispensables                           
c                                                                       
       Logical*1 Laval
       common/laval29/laval
	
      include 'veda_rel.incl'                                         
      include 'veda_wri.incl'
      include 'veda_5.incl'                                             
      include 'veda_6.incl' 
      include 'veda_11.incl'                                            
      include 'veda_12.incl' 
C
      Real*4 emax_rap(2:17,2,4)      ! emax_rap(cour,iz,im)             
      Common/emaxenr/emax_rap                                           
C                                                 
       real*8 e_cible
	integer*4 ID_cible
	common /cible/ e_cible,ID_cible
	
	common /Zmax_ID / zzmax
	real*4 zzmax(10:17),zzmax_lourds(10:17),zzmax_UU(10:17),
     m	zzmax_NiNi(10:17) ,zzmax_NiAu(10:17)
	data nrun_nini_max,nrun_niau_max / 124 , 182 /
        data zzmax_lourds / 9.5,11.5,14.5,9.5,9.5,10.5,9.5,10.5 /
        data zzmax_UU /8.5,10.5,10.5,9.5,7.5,7.5,5.5,5.5 /
        data zzmax_NiNi / 13.5,11.5,10.5,7.5,6.5,6.5,6.5,6.5 /
c       data zzmax_NiAu / 16.5,13.5,15.5,10.5,9.5,8.5,6.5,6.5 /
        data zzmax_NiAu / 9.5,7.5,9.5,6.5,8.5,6.5,6.5,6.5 /
	
		print*,'program Kali_2--------- appel initc'
		call INITC 
      print*,'program Kali_2--------- sortie initc'      

c ---- PATH des fichiers data de VEDA ----------------------------------
      
      linux=.false.
      call GETENVF ('SYSTEM',op_system)
      write(*,'(/,'' Operating system = '',a)')op_system
      if(op_system(1:5).eq.'Linux')then
         linux=.true.
      endif	 
      call VAR_ENVIRONNEMENT  
      

		Laval=.True.
		if(Laval) then
		   call INI_TABAC_CSI_2_9
		endif
       
      namefil=nomjob(1:long_job)//'.sortie'
      
		print*,'INFO namefil=',namefil,' iout=',iout
		open (i_out,file=namefil)
      
      call DATIMH (datj,hj)                                             
      write(i_out,'(//,'' Date debut : '',a8,''  '',a8)')datj,hj 
      call INI_ASCII                                                    
      
      call INI_HBOOK

      lfiltre=.false.
      lsimulation=.false.                                               
c      call CPUTIME(t1,irc)
      long_critere=70
      dst_reduite=.false.
      time_max=.false.    
      char_ascii=.false.                                          
      end_of_program=.false.
      end_of_file=.false. 
      fin_lect_run=.false.  
      copy_event=.false. 
      nblocecr_tot=0
      nevtecr_tot=0 
      nevt_dump=100 ! Nbre d'evts a "DUMPER"   
                               
c --- Lecture du drapeau d'ecriture sur cartouche DST                   
                                                                        
      ecrit_dst=.False.                                                 

      write(i_out,100)
      write(*,100)
100   format(/,
     & ' **** TRAITEMENT des cartouches DST 2e Camp. INDRA **** ',
     & 'version: 24 Mai 2002 / 3.4',/,
     &         '      ------------------------------------------      ')
                              
                                                                       
c --- Boucle sur le nbre de fichiers a lire                             
 
!      do while (.not.end_of_program)
      
       
		nbloc_header=0
      nbloc_data=0
      nbloc_etat=0
      nbloc_scaler=0
      nbloc_inconnu=0
         
		filein=nomjob(1:long_job)//'.dst'
			
		call OPEN_FILE(filein,lun)
		
		end_of_file=.False.                                            

      if(lun.eq.-100) then
      	print *,'Probleme a l''ouverture du fichier : ',filein(1:8)
      	print *,'On passe au fichier suivant...'
      	end_of_file=.True.
      endif 

 !=====================================================================
		nbloclus=0                                                     
		nevtlus=0                                                      
		nb_scaler=0 
		nkbyta=0                                                       
		nkbyte=0 
 
          
c --- Boucle sur la lecture des blocs du fichier                        
                                                                        
      do while(.not.end_of_file)                                        
                                                                        
c --- Controle du temps restant (batch)                                 
                                                                        
      	call TIMEL(time)                                               
			if(time.lt.5.) then  ! Temps limite atteint, c'est fini... 
            write(i_out,*)
            write(i_out,*) '**** Temps limite atteint          ****'    
            write(6,*) '**** Temps limite atteint          ****'        
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


				if(istatus.eq.-1) then
					end_of_file=.True.
					call CFCLOS (lun,0)
				else
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
     &        or.typebloc.eq.'VEDADST5'.or.typebloc.eq.'VEDARED1' )then
            		if(typebloc.eq.'VEDARED1')then
               		dst_reduite=.true.
               		write(i_out,104)
104            		format(/,' ** TRAITEMENT des bandes DST <reduite> ** ')
            		endif
 
						call DEC_BLOC_HEAD (ibuff,iversion,irc)
		            
						if(irc.ne.0) then                                           
               		write(i_out,*) ' Erreur lecture du fichier : ',filein
               		write(6,*) ' Erreur lecture du fichier : ',filein
               		end_of_program=.true.
               		goto 999
            		end if 
            
            		if(dst_reduite)then
              			write(i_out,122)new_run,critere
122           format(/,' criteres de selection (run DST_reduite = ',i5,
     &              '): ',/,10('-'),/,8x,a70)  
            		endif
            
            		print *,' *** RUN = ',numerun

c
CCCC JLL 8 juin 2001 : code ENERGIE pour les E_max_Csi
						Call emaxcsi
C	
c --- Definition de la cible
c
	 					do i=10,17
							zzmax(i)=zzmax_lourds(i)
	 					enddo
C
C  EPAISSEURS DE CIBLE pour le calcul de la perte en energie dans la 1/2 cible
	 ID_cible = - 9
C	ID_cible =  4  POUR Ni
						if (  ( numerun.ge.1267 .and. numerun.le.1281 ) .OR.
     1	      ( numerun.ge.1300 .and. numerun.le.1311 ) .OR.
     2			( numerun.ge.1335 .and. numerun.le.1376 ) .OR.
     3	      ( numerun.ge.1412 .and. numerun.le.1426 ) .OR.
     4	      ( numerun.ge.1469 .and. numerun.le.1485 ) .OR.
     5	      ( numerun.ge.1521 .and. numerun.le.1555 ) )   THEN
	       			ID_cible =  4 
	       			e_cible = 0.0895
	       				do i=10,17
	          				zzmax(i)=zzmax_NiNi(i)
	      		 		enddo
	       			endif
	      
C	ID_cible = 10  POUR Au
						if (  ( numerun.ge.1001 .and. numerun.le.1019 ) .OR.
     1	      ( numerun.ge.1045 .and. numerun.le.1082 ) .OR.
     2	      ( numerun.ge.1114 .and. numerun.le.1148 ) .OR.
     3	      ( numerun.ge.1253 .and. numerun.le.1263 ) .OR.
     4	      ( numerun.ge.1282 .and. numerun.le.1299 ) .OR.
     5	      ( numerun.ge.1312 .and. numerun.le.1334 ) .OR.
     6	      ( numerun.ge.1377 .and. numerun.le.1411 ) .OR.
     7	      ( numerun.ge.1427 .and. numerun.le.1468 ) .OR.
     8	      ( numerun.ge.1486 .and. numerun.le.1520 ) .OR.
     9	      ( numerun.ge.1556 .and. numerun.le.1588 ) )  THEN
	       				ID_cible =  10
	       				e_cible = 0.100
	       				do i=10,17
	          				zzmax(i)=zzmax_NiAu(i)
	       				enddo
	       			endif 
C
C	ID_cible = 11  POUR U
        				if (  ( numerun.ge.1020 .and. numerun.le.1044 ) .OR.
     1        	( numerun.ge.1083 .and. numerun.le.1113 ) .OR.
     2			( numerun.ge.1170 .and. numerun.le.1189 ) )  THEN
							ID_cible =  11
	        				e_cible = 0.050
	        				do i=10,17
	          				zzmax(i)=zzmax_UU(i)
	        				enddo
	        			endif 
C
 	  					write(6,542) numerun,ID_cible,e_cible
  542	 format(1h , ' nature et epaisseur de cible /run ',2i6,F7.3/)    
C
CCC INITIALISATION DE VEDALOSS
C
       				PRINT * , '                       '
      				icode=0
      				numf=49  ! numero logique de fichier DAPNIA
      				call LOSS_INIT (numf,icode) ! ca se trouve dans vedaloss_2E.f
C                                   ===============================                         
      				print *,'Initialisation Table Perte d Energie: icode =  ',icode
      				if(icode.eq.0)then
          write(iwf,'('' *** Init. Table LOSS2 VEDASAC  : OK ***'')')
      				else
       write(iwf,'('' *** ATTENTION : Ini. Perte d Energie   ***'')')
       write(iwf,'('' fichier LOSS2 VEDASAC * not found  ... icode = '',
     &        i2)')icode
      				endif
C
            		call INI_clean_evt(numerun)

c --- Appel a l'init ID Chio-Csi pour correction code Bragg

            		call inipnch ( iannee, imois, numerun, istatus)                       
	            
            		call INI_RUN 
	     
	     				nbloc_header=nbloc_header+1                                            
                                                                        
          		elseif(typebloc.eq.'BLOCDATA') then

						if(iversion.eq.5)then
							call DEC_BLOC_DST  (ibuff)
               	else
							STOP 'BLOCDATA  impossible a lire'
						endif      
               
               	if(fin_lect_run)then
                  	end_of_file=.true.
                  	time_max=.true.
               	endif   
               	if(fin_lect_gen)then
                  	end_of_file=.true.
                  	time_max=.true.
               	endif   
	        			nbloc_data=nbloc_data+1                                            
               
          		elseif(typebloc.eq.' SCALER ') then
          
            		nb_echelle_a_imprimer=16
            		imp_all=1
            		nb_scaler=nb_scaler+1
            		write(*,*) ' '
            		write(*,*) ' DERNIER bloc = bloc Scaler du run'
            		write(*,*) ' bloc Scaler = ',nbloclus
            		Call DECODE_SCALER
     &      (ibuff,echelle,nb_echelle_a_imprimer,Iwt)
            		call PRINT_SCALER(i_out)
            
	     				nbloc_scaler=nbloc_scaler+1                                            
 
          		elseif(typebloc.eq.'ETAT_IND') then
 
            		call DEC_BLOC_ETAT(ibuff,irc)         
 
            		if(irc.ne.0) then                                           
               write(i_out,*) ' Erreur a la lecture du bloc ETAT_IND '  
               write(6,*) ' Erreur a la lecture du bloc ETAT_IND '      
            		end if  
                                                    
	     				nbloc_etat=nbloc_etat+1                                            
  
          		else
 
            		write(6,25) nbloclus,typebloc
25          		format(1x,' Type du bloc #',i6,' : ',a8,' inconnu !')
            		do i=1,8                                                    
              			itab(i)=ibuff(i)
            		end do
            		Print *,(itab(i),i=1,8)
	     				nbloc_inconnu=nbloc_inconnu+1                                            
         		
					end if
                                                                        
c --- Pointeur de bloc                                                  
                                                                       
         		iloop=mod(nbloclus,250)                                        
         		iwoop=mod(nblocecr,250) 
                                                                        
         		if(iloop.eq.0.and.nbloclus.ne.1) then 
           			write(6,'('' ## Blocs lus = '',i5,'' Evts lus = '',i8,
     &               '' Temps restant ='',f10.2,'' sec.'')')
     &					nbloclus,nevtlus,time     
         		end if
        
        		endif
         
      	endif  
      
		end do 
                                                                        
c --- Fin de la lecture, qq rappels...                                  

      if(.not.end_of_program)then  
      
        	write(i_out,*) ' '                                              
        	write(i_out,*) 'Fin de lecture du fichier DST : ',filein        
        	write(i_out,*) 'Nombre de blocs lus       :',nbloclus  
	 		write(i_out,*) 'dont :'
	 		write(i_out,*) '   Nombre de blocs HEADER  : ',nbloc_header
	 		write(i_out,*) '   Nombre de blocs DATA    : ',nbloc_data
	 		write(i_out,*) '   Nombre de blocs SCALER  : ',nbloc_scaler
	 		write(i_out,*) '   Nombre de blocs ETAT    : ',nbloc_etat
	 		if(Nbloc_inconnu.gt.0) then
	 			write(i_out,*) '   Nombre de blocs INCONNU : ',nbloc_inconnu
	 		endif         
        	write(i_out,*) 'Nombre d''evts lus         :',nevtlus           
        	write(i_out,*) 'Numero du dernier evt lu  :',num_evt_brut       
        	write(i_out,*)'-----------------------------------------------'
        
                                                                        
        	write(6,*) ' '                                                  
        	write(6,*) ' ---- Fin de lecture fichier : ',filein             
        	write(6,*) 'Nombre de blocs lus       :',nbloclus               
        	write(6,*) 'Nombre d''evts lus         :',nevtlus               
        	write(6,*) ' --------------------------------------------'      

        	write(6,*) ' '                                                  
        	call FIN_RUN                                                    
        	write(i_out,*) ' ' 
        	if(time_max) end_of_program=.true. 
		endif 
                                                                        
                                                                        
c --- Fermeture                                                         
                                                                        
999   write(i_out,*) ' '                                                
      write(i_out,*) ' -- Cumul du nb de blocs lus : ',nbloc_tot    
      write(i_out,*) ' -- Cumul du nb d''evts   lus : ',nevt_tot    
      write(i_out,*) ' '                                                
      write(6,*) ' '                                                    
      write(6,*) ' -- Cumul du nb de blocs lus : ',nbloc_tot        
      write(6,*) ' -- Cumul du nb d''evts   lus : ',nevt_tot        
      write(6,*) ' '                                                    

                                                                        
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
                                                                        
      integer*2 ibuff(8192)                                             
      integer*4 ivalz(300),jvalz,abi                                        
      integer*2 nevt_low,nevt_high
      integer*4 code16,code4(4),ien
      logical   evt_a_lire,prt 
      integer*2 kbyt(2)
                                                                        
      Real*4       tab_pw(12),hit_pw(12)
      common /TABPW/tab_pw,hit_pw  
C
      Real*4 emax_rap(2:17,2,4)      ! emax_rap(cour,iz,im)             
      Common/emaxenr/emax_rap                                           
C
       Logical*1 Laval
       common/laval29/laval
	Real*4 Tabacaru_2_9
	
	Real*8 Eres,Az,Am,De,Einc,ep_myl,Pres2_7,Pres8_17,Amassr
	real*8 e_cible
	integer*4 ID_cible
	common /cible/ e_cible,ID_cible
	
	common /Zmax_ID / zzmax
	real*4 zzmax(10:17),zzmax_lourds(10:17),zzmax_UU(10:17),
     m	zzmax_NiNi(10:17) ,zzmax_NiAu(10:17)
	data nrun_nini_max,nrun_niau_max / 124 , 182 /
        data zzmax_lourds / 9.5,11.5,14.5,9.5,9.5,10.5,9.5,10.5 /
        data zzmax_UU /8.5,10.5,10.5,9.5,7.5,7.5,5.5,5.5 /
        data zzmax_NiNi / 13.5,11.5,10.5,7.5,6.5,6.5,6.5,6.5 /
c       data zzmax_NiAu / 16.5,13.5,15.5,10.5,9.5,8.5,6.5,6.5 /
        data zzmax_NiAu / 9.5,7.5,9.5,6.5,8.5,6.5,6.5,6.5 /
	
      include 'veda_rel.incl'                                           
      include 'veda_wri.incl'
      include 'veda_5.incl'                                             
      include 'veda_6.incl'                                             
                                                                        
      common /VEDADUMP/ivalz
c      Equivalence (ien,kbyt) 
      data  ep_myl / 2.5 /
C 
      ipt=9                                                             
      evt_a_lire=.True.                                                 
      prt=.false.  
c
c                                                                        
c --- Boucle sur le nombre d'evt a decoder dans le bloc                 
                                                                        
c      nbloc=nbloc+1                                                     
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
                                                                        
         if(ilong.eq.0.or.ipt.ge.8192) then   ! JLC 06/02/04                                         VED03900
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
             print *,' risque de violation de memoire ! (veda_wri.incl)'
               print *,' evt = ',nevtlus,' evt brut = ',num_evt_brut
               print *,' npart_traite, iztot , mcha = ',
     &                   npart_traite,iztot,mcha
               print *,' On se repositionne sur le bloc suivant'
               print *,' Avant: dump de cet evt: '
               do k=1,ilong
               print *,'    k,ibuff = ',k,ibuff(ipt+k-1)
               enddo
               print *,' -------------------------------------------'

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
                 ic = icou(i)
                 im = imod(i)                                            
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
		    
c --- correction du module CsI 6:03 (ligne 3He et Alpha inversees)
c --- JLL/OL (7/11/2000) 
CCCCCCCC Re-inseree le 30/11/2000 a partir du run 1253 Ni
C
	if(Icou(i).eq.6.and.Imod(i).eq.3 .AND. numerun .ge. 1253) then
c	PRINT * , ' IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII ' 
c	PRINT * , ' RUN Icou(i)  Imod(i) ', Numerun ,Icou(i),Imod(i)
c		    PRINT * , ' Z_indra(i) a_indra(i)  z(i)   a(i) '
c		    WRITE(6,6782)Z_indra(i),a_indra(i),z(i) ,a(i) 
c 6782	  FORMAT ( 1H ,2F10.3 , 2I7 /)
           Zadi = Z_indra(i)
	   adi = a(i)		    
C
	 if(Zadi .gt.1.5 .and. Zadi .lt. 2.5 .AND.
     &	    adi.eq.3) then
			   a(i)=4
			   a_indra(i)=4.
			endif

	if(Zadi .gt.1.5 .and. Zadi .lt. 2.5 .AND.
     &	  adi  .eq.4) then	       
			   a(i)=3
			   a_indra(i)=3.
			endif
CCCC OL ? JLL 10 juillet 2001
CC	restauretion $H ===> 3He
	IF ( Z(i) .eq. 1 .and. a(i) .eq. 4 .and. code(i) .eq. 10 ) THEN
	Z(i) = 2
	A(i) = 3
	Z_indra(i) = 2.
	A_indra(i) = 3.
	code(i) = 2
	  End  If
C			
c		    WRITE(6,6782)Z_indra(i),a_indra(i),z(i) ,a(i) 
C
		    endif   !!!! Icou(i).eq.6.and.Imod(i).eq.3  /// RUN>=1253
C 
c --- Correction des cartouches 38KDC creees entre le 19 et 25/6/95 ---
c     Pb du code=9 trop souvent present, corrige ensuite dans VEDA 3.2
 
c                  if((kjour.ge.19.and.kjour.le.25).and.
c     &                kmois.eq.6.and.kanne.eq.95)then
c                    if(icou(i).ge.10)call CORRECT_BACK (num_evt_brut,i)
c                  endif
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
		    ien=kbyt(1)*(2**16)+kbyt(2)   ! JLC 21/04/04
		    if(kbyt(2).lt.0)ien=ien+2**16 ! JLC 21/04/04
                    de1(i)=float(ien)
                    de1(i)=de1(i)/10.

c --- Supression des Z trop eleves pour le faisceau de Nickel
c --- Passage en code=10 (entre les lignes) (OL, 07/06/01)

		      if(numerun.ge.1253) then
		         if(z(i).gt.32.and.icou(i).eq.1) code(i)=10
		      endif                                        
						                              
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
		      if(laval.and.z(i).ge.1) then
		         csir=canal(i,7)
		         csil=canal(i,8)
			  Ic=Icou(i)
			  Im=Imod(i)
C  R/L > 0			  
		         If (csir.gt.0..AND.csil.gt.0.) THEN	  
                           call LUMIERE(Ic,Im,csir,csil,hl,hr,icode)
		             abi = abs(a(i))
			      if(hl.gt.0.) 
     &                     de33 = tabacaru_2_9(Ic,Im,hl,z(i),abi,icode)
                           if(de33.gt.0.) de3(i)=de33
		         endif
		       endif
c
c --- Cas des Beryllium 8 (OL, 06/06/01)
c
	             if(z(i).eq.2.and.a(i).eq.8.and.code(i).eq.10) then
	                z(i)=4
	                code(i)=2
	                csir=canal(i,7)
	                csil=canal(i,8)
	                Ic=Icou(i)
	                Im=Imod(i)
                       If(csir.gt.0..AND.csil.gt.0.) THEN	  
                          call LUMIERE(Ic,Im,csir,csil,hl,hr,icode)
		            abi = abs(a(i))
		            de3new=de3(i)
	                   if(hl.gt.0.) 
     &                    de3new = tabacaru_2_9(Ic,Im,hl,z(i),abi,icode)
                          ener(i)=ener(i)+de3new-de3(i)
			     de3(i)=de3new
	                endif
c
c --- Calcul de la vraie contribution Si (24/05/02)
c
			  az=dfloat(z(i))
			  am=dfloat(a(i))
			  Eres=de3(i)
			  de=0.
                       call PARAM(Eres,az,am,1,DE,Einc,2,icode)
			  if(de.gt.0.) de2(i)=de
	             endif
		      
c
c --- Cas des neutrons (Pas de Silicium) (OL,24/05/02)
c
		      if(code(i).eq.2) then
		         esi_lim=0.4
			  if(z(i).eq.2) esi_lim=1.
			  if(z(i).eq.3) esi_lim=1.5
			  if(z(i).eq.4) esi_lim=1.5
			  if(de2(i).le.esi_lim) code(i)=1
		      endif
c
c --- Cas des Be8 detectes en code=3 (OL,24/05/02)
c
		      if(code(i).eq.3.and.z(i).eq.4) then
		         az=dfloat(z(i))
		         am=dfloat(abs(a(i)))
		         Eres=de3(i)
		         de=0.
                       call PARAM(Eres,az,am,1,DE,Einc,2,icode)
		         if(de.gt.0.) de2(i)=de
		      endif
c
c --- Cas des Z=1 spurieux identifies dans la ChIo (OL, 24/05/02)
c		      
		      ea=10.
                    if(a(i).ne.0.) ea=ener(i)/abs(a(i))
                    if(z(i).eq.1.and.code(i).eq.4.
     &                 and.ea.lt.1.2) code(i)=14.
	             if(code(i).eq.4.and.z(i).eq.2.
     &                 and.ener(i).lt.5.) code(i)=14
c			
c --- Calcul de la perte ChIo pour les particules de code=2 et 3(CsI) (JLL,OL 06/06/01)
c
                     if(code(i).ge.2.and.code(i).le.3) then
c			   
c --- Cas particulier des Z=5 code=2 passe en code=16 (coherence mal geree ...) (JLL,OL 06/06/01)
c
	                 if(z(i).eq.5.and.code(i).eq.2) then
			      code(i)=16
			   endif
			   
                        icode=0
                        Eres=de2(i)+de3(i)
                        az=dfloat(z(i))
                        am=dfloat(abs(a(i)))
                        if(am.eq.0.) then
c                           am=AMASS(nint(float(Z(i))))
                           am=AMASS(float(Z(i)))
                        endif
		          De2_mylar=0.
C
c --- Fenetre de sortie de la ChIo en Mylar: ep_mylar = 2.5 microns
C 
	                 amassr = 0.
                        call SET_THICKNESS(ep_myl,amassr,2)
                        call PARAM(Eres,az,am,2,DE,Einc,2,icode)
	                 IF ( DE .gt. 0. .AND. DE .lt. 500. ) 
     &                       De2_mylar=DE

			   Amassr=0.
		          if(ic.le.7) then
		             Pres2_7=49.
	                    call SET_THICKNESS(pres2_7,amassr,5) 
		          else
		             Pres8_17=30.
	                   call SET_THICKNESS(pres8_17,amassr,5) 
		          endif
		          Eres=de2_mylar+de2(i)+de3(i)
                        call PARAM(Eres,az,am,5,DE,Einc,2,icode)
                        IF ( DE .gt. 0. .AND. DE .lt. 1000. ) de1(i)=de
			endif			
c
c --- Csi 6:03 : probleme d'identification Si x CsIr pour GG
c		      
	            if(icou(i).eq.6.and.imod(i).eq.3.and.
     &                code(i).eq.3.and.z(i).gt.5) then
		        sigg=canal(i,4)
	               if(sigg.lt.3800.) then
	                 z_indra(i)=z_indra(i)-1.
		          z(i)=z(i)-1.
			   a(i)=-AMASS(float(z(i)))
			   a_indra(i)=float(a(i))
	               endif
		     endif

c
c --- Supression des certains Z spurieux dans les cartes Si - CsIr
c --- En fait, ce sont des particules de coherence Si (?)
c --- (OL,07/06/2001)
c
		     if((z(i).eq.4.or.z(i).eq.5).and.
     &                 canal(i,4).gt.3000) then
		        if(icou(i).eq.2) then
		           if(imod(i).eq.3.or.imod(i).eq.11
     &                      .or.imod(i).eq.15) then
				code(i)=7  
                         endif
		        elseif(icou(i).eq.3) then
			    if(imod(i).eq.11.or.imod(i).eq.12
     &                      .or.imod(i).eq.17.or.imod(i).eq.19
     &			       .or.imod(i).eq.22.or.imod(i).eq.23
     &                      .or.imod(i).eq.24) then
                            code(i)=7
			    endif
			 endif
		      endif
c
c --- Certaines particules 
c --- ont des energies trop importantes (Z=1/2) (OL,08/06/2001)
c 		      
		      if((z(i).eq.1.or.z(i).eq.2).and.code(i).eq.2.
     &                 .and.canal(i,4).gt.3000.) code(i)=9 		   
	
c		      
c --- Cas des couronnes 10 a 17  (sans Etalons)                         
c                                                                        
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
                  ener(i)=(dde1+dde2+dde3+dde4+dde5)
              endif
 
              ipt=ipt+abs(ilongevt)                                     
 
            end do

c --- Correction Pour prise en compte du code 5 Chio_csi correct ...
c --- OL (10/10/98)

	     Do i=1,npart_traite 

c --- On retire les Z = Zzmax (Limite du Zextrapole) OL/JLL (20/07/01)
c --- pour les couronnes 10 a 17 seulement

	        if(icou(i).ge.10.and.icou(i).le.17) then
		   if(z_indra(i).eq.Zzmax(icou(i))) then
		      code(i)=15
		      z(i)=0
		      z_indra(i)=0.
		      a(i)=0
		      a_indra(i)=0.
		      ener(i)=0.
		   endif 
		endif
c
c --- Gestion du code energie (casdurebroussement) (OL/JLL, 07/06/2001)
c  JLL 8 juin 2001 : utilisation emax_rap ( routine emaxcsi issue du module csi29_2_E.f )
	        ecode(i)=1
               if(z(i).ge.1.and.z(i).le.2)then
                  ico=0
C                   call COR_ECSI_LIMIT (icou(i),imod(i),z(i),a(i),
C     &                                      de3(i),ico)
	         abi = abs(a(i))
		 if ( z(i) .eq. 1 .and. abi.gt.3 ) abi=3
		 if ( z(i) .eq. 2 .and. abi.gt.4 ) abi=4
		 if ( z(i) .eq. 2 .and. abi.lt.3 ) abi=4
		E_max_Csi = emax_rap(icou(i),z(i),abi)
		If ( de3(i) .gt. E_max_Csi ) then  !  E > E_max (sinon ico=1)
                     ecode(i)=3
                     code_energie(2,i)=7
			
                  else
                     if(ecode(i).eq.3)ecode(i)=1
                     code_energie(2,i)=1
                  endif
c	Print *,' icou(i),imod(i),z(i),a(i),de3(i),E_max_Csi,ecode(i) '
c	Print * ,  icou(i),imod(i),z(i),abi,de3(i),E_max_Csi,ecode(i)
		 endif
		 
	        if(icou(i).ge.10.and.code(i).ge.4.and.code(i).le.5) then
		    Icsirap=Canal(i,7)
		    Ichiopg=Canal(i,2)
	           call Bragg_Chio_csi(Icou(i),Imod(i),icsirap,
     &                                ichiopg,istat)                   
		    if(Istat.eq.10) then
		       code(i)=5
	           else
		       code(i)=4
	           endif
		 endif
		 
c
		Ic = icou(i)
		Im = imod(i)
C
c --- Calcul des pertes d'energie Mylar, cible et ChIo defaillante 
c
c OL/JLL : 25/10/2000
c
	 If ( de1(i) .lt. 0. ) de1(i) = 0.
	 If ( de2(i) .lt. 0. ) de2(i) = 0.
	 If ( de3(i) .lt. 0. ) de3(i) = 0.
	 If ( de4(i) .lt. 0. ) de4(i) = 0.
	 If ( de5(i) .lt. 0. ) de5(i) = 0.
C
c	 PRINT * , ' DEBUT : de1(I) ,de2,de3,de4,de5, ' ,
c     &  de1(i),de2(i),de3(i),de4(i),de5(i)	  
c --- Corrections MYLAR pour tout le monde SVP !

               if(ic.gt.1) then
 
c --- Fenetre de sortie de la ChIo en Mylar
                icode=0
                 Eres=de2(i)+de3(i)
                 az=dfloat(z(i))
                 am=dfloat(abs(a(i)))
                 if(am.eq.0.) then
c                    am=AMASS(nint(float(Z(i))))
                    am=AMASS(float(Z(i)))
                 endif
                 De=0.
		 De1_mylar=0.
		 De2_mylar=0.
		 De_cible=0.
C
c --- Fenetre de sortie de la ChIo en Mylar: ep_mylar = 2.5 microns
C 
		IF (Eres .gt. 0.) THEN  !!!! JLL  29 janvier 2001
C			 
	     amassr = 0.
                  call SET_THICKNESS(ep_myl,amassr,2)
                 call PARAM(Eres,az,am,2,DE,Einc,2,icode)
	IF ( DE .gt. 0. .AND. DE .lt. 500. )	 
     t                 De2_mylar=DE
c              print * , ' De2_mylar=de ' ,De2_mylar
c --- Perte dans la ChIo : calculee si pas de calibration
C
                 if(de1(i).le.0.) then
c		 	PRINT * , ' Chio = 0 ==> de1(i) : ' ,de1(i)
		      Amassr=0.
		      if(ic.le.7) then
		         Pres2_7=49.
	                call SET_THICKNESS(pres2_7,amassr,5) 
		      else
		         Pres8_17=30.
	                call SET_THICKNESS(pres8_17,amassr,5) 
		      endif
		      Eres=de2_mylar+de2(i)+de3(i)
                    call PARAM(Eres,az,am,5,DE,Einc,2,icode)
              IF ( DE .gt. 0. .AND. DE .lt. 1000. )	 
     t			      de1(i)=de
c		      PRINT * , ' de1(i) calcule ' ,de1(i)
		ener(i) =  ener(i) + de1(i)
		   endif
C
c --- Fenetre d'entree de la ChIo en Mylar: ep_mylar = 2.5 microns
C                 
                 Eres=de1(i)+de2_mylar+de2(i)+de3(i)
                 call PARAM(Eres,az,am,2,DE,Einc,2,icode)
	      IF ( DE .gt. 0. .AND. DE .lt. 500. )	 
     t                de1_mylar=de
c		 PRINT * , ' de1_mylar=de ! ' , de1_mylar
                 de_mylar(i)=de1_mylar+de2_mylar
		   if(de_mylar(i).gt.0.) 
     &              ener(i)=ener(i)+de1_mylar+de2_mylar
              endif    !!!! cour 2 - 17 uniquement
c
C  Et on met la perte d'energie dans la cible (Ni,Au,U)
C                                      a definir dans l'INIT
               amassr = 0.
	        call SET_THICKNESS(e_cible,amassr,ID_cible) 
    	        Eres = de1_mylar+de1(i)+de2_mylar+de2(i)+de3(i)
               call PARAM(Eres,az,am,ID_cible,DE,Einc,2,icode)
	      IF ( DE .gt. 0. .AND. DE .lt. 500. )	 
     t 	          de_cible  =  DE  
C		 PRINT * , ' ID_cible,e_cible,de_cible  =  DE  ! ' ,
C     &		  ID_cible,e_cible,de_cible
C
		 if(de_cible.gt.0.) then
		    ener(i)=ener(i)+de_cible
                  de_mylar(i)=de_mylar(i)+de_cible
C	print * , ' de_mylar(i)+de_cible = ' ,de_mylar(i)
		 endif   ! ! !  if(de_cible.gt.0.)

CCCCCCCICI^
c	       PRINT * , 
c     &	' de1(i)  de2(i)  de3(i) de1_mylar de2_mylar de_cible ener(i) '
c        Write(6,543)
c     &	 de1(i) ,de2(i),de3(i),de1_mylar,de2_mylar,de_cible,ener(i) 
c 543	format(1h , 7f9.2/)
C	     		    
c		Energii =  de1(i) + de2(i) + de3(i)
c	PRINT * , 'Energii =  de1(i) + de2(i) + de3(i) ' , Energii 
CCCCCCCCCCCCCCCCCCC
		END  IF   !!!!!!!! IF (Eres .gt. 0. ) 
C
		If ( ic .eq .2  .AND.  im .eq. 1 ) then !!  JLL 29/01/2001
		ener(i)=0. 
		de1(i) = 0.
		de2(i) = 0.
		de3(i) = 0.
		de_mylar(i) = 0.
C		
		  END  IF  !!!! ic .eq .2  .AND.  im .eq. 1
C		  
             End do
		    
c --- Routine utilisateurs pour le traitement evt/evt 
                  
            copy_event=.false.    
                                                  
            call TRAITEMENT
            
            if(fin_lect_run)evt_a_lire=.false.
            if(fin_lect_gen)evt_a_lire=.false.
                                                                        
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

c --- Correction pour certains runs en Ni+Au a 90 A.MeV ...

	if(numerun.ge.1253.and.numerun.le.1263) then
	
	   zproj=28.
	   aproj=58.
	   zcib=79.
	   acib=197.
	   esura=89.7
	   
	endif
	              
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
      integer*4 ivalz(300),jvalz,abi                                        
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
      
       Logical*1 Laval
       common/laval29/laval
	Real*4 Tabacaru_2_9
 
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
                                                                        
         if(ilong.eq.0) then                                            
                                                                        
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
                    
		      if(laval.and.z(i).ge.1) then
		         csir=canal(i,7)
		         csil=canal(i,8)
			  Ic=Icou(i)
			  Im=Imod(i)
C  R/L > 0			  
		         If (csir.gt.0..AND.csil.gt.0.) THEN	  
                           call LUMIERE(Ic,Im,csir,csil,hl,hr,icode)
		             abi = abs(a(i))
C  TEST  58 Ni ???? le 16 nov 2000
C	  IF (Z_indra(i) .gt. 27 .AND. Z_indra(i) .lt. 29.) abi = 58. 			      
C			      
			      if(hl.gt.0.) 
     &                   de3(i) = tabacaru_2_9(Ic,Im,hl,z(i),abi,icode)
		         endif
			  
		       endif
			
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
		                                    
		If ( ic .eq .2  .AND.  im .eq. 1 ) then !!  JLL 29/01/2001
		ener(i)=0. 
		de1(i) = 0.
		de2(i) = 0.
		de3(i) = 0.
		de_mylar(i) = 0.
C		
		  END  IF  !!!! ic .eq .2  .AND.  im .eq. 1
 
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
      integer*4 ivalz(300),jvalz,abi                                        
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
      
       Logical*1 Laval
       common/laval29/laval
	Real*4 Tabacaru_2_9
 
 
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
                                                                        
         if(ilong.eq.0) then                                            
                                                                        
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
                                                                        
		      if(laval.and.z(i).ge.1) then
		         csir=canal(i,7)
		         csil=canal(i,8)
			  Ic=Icou(i)
			  Im=Imod(i)
C  R/L > 0			  
		         If (csir.gt.0..AND.csil.gt.0.) THEN	  
                           call LUMIERE(Ic,Im,csir,csil,hl,hr,icode)
		             abi = abs(a(i))
C  TEST  58 Ni ???? le 16 nov 2000
C	  IF (Z_indra(i) .gt. 27 .AND. Z_indra(i) .lt. 29.) abi = 58. 			      
C			      
			      if(hl.gt.0.) 
     &                   de3(i) = tabacaru_2_9(Ic,Im,hl,z(i),abi,icode)
		         endif
			  
		       endif
			
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
 
		If ( ic .eq .2  .AND.  im .eq. 1 ) then !!  JLL 29/01/2001
		ener(i)=0. 
		de1(i) = 0.
		de2(i) = 0.
		de3(i) = 0.
		de_mylar(i) = 0.
C		
		  END  IF  !!!! ic .eq .2  .AND.  im .eq. 1
		  
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
 
c------------------------------------------------------------------

      Subroutine DECODE_SCALER_AIX
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
 
      Common       ibuf2,ibuf1
      Equivalence  (ibuf4,ibuf2)
 
      do i=1,4096
      k=2*i-1
      ibuf1=buffer(k)
      ibuf2=buffer(k+1)
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
      if(iwt.ne.0)then
 
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
      if(irc.eq.0)then
         nbloc=nbloc+1
   
      elseif(irc.eq.-1)then
          istatus=irc  ! fin de fichier detecte
          
      else
          print *,' !!!! Error cfget : irc = ',irc
          stop 'READ_BUFFER'    
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
            
      call GETENVF ('VEDA_DATA',vedadata)
      i=0
      long_data=0
      do while (long_data.eq.0)
      i=i+1
      if(vedadata(i:i).eq.' ')long_data=i-1
      enddo
      if(long_data.gt.3)then
         write(*,'(/,'' Chemin des fichiers DATA      = '',a)')
     &                                          vedadata(1:long_data)
      endif
      
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
c ----------------------------------------------------------------------

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
       
c ----------------------------------------------------------------------
            
       Subroutine INI_ASCII
       
c Routine construisant le tableau iascci : code ASCII = iascii (code EBC
       
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
       
c-----------------------------------------------------------------------
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
                                                                        
      iversion=5    ! Numero de version de l'ecriture VEDA (1ere campagn
                                                                        
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
       subroutine ini_clean_evt(nrun)

       include 'veda_11.incl'
       include 'veda_5.incl'
       
       real*4 ebomb(19)
       data ebomb/32.8,32.8,39.6,39.6,24.0,63.6,63.6,32.0,32.0,
     &   40.0,74.6,74.6,51.9,51.9,89.7,89.7,89.7,82.0,82.0/
       character*50 system(19)
       data system/'181Ta + 238U 32.8 A.MeV',
     &      '181Ta + 197Au 32.8 A.MeV',
     &      '181Ta + 238U 39.6 A.MeV',
     &      '181Ta + 197Au 39.6 A.MeV','238U + 238U 24.0 A.MeV',
     &      '58Ni + 58Ni 63.6 A.MeV',
     &      '58Ni + 197Au 63.6 A.MeV','58Ni + 58Ni 32.0 A.MeV',
     &      '58Ni + 197Au 32.0 A.MeV',
     &      '58Ni + 58Ni 40.0 A.MeV','58Ni + 58Ni 74.6 A.MeV',
     &      '58Ni + 197Au 74.6 A.MeV',
     &      '58Ni + 58Ni 51.9 A.MeV','58Ni + 197Au 51.9 A.MeV',
     &      '58Ni + 58Ni 89.7 A.MeV',
     &      '58Ni + 197Au 89.7 A.MeV','58Ni + 58Ni 89.7 A.MeV',
     &      '58Ni + 58Ni 82.0 A.MeV','58Ni + 197Au 82.0 A.MeV'/
       
	logical*1 bon(17,24,3)
	common/cebon/bon
	
	integer*4 modu(200),type(200)
	
	open(40,file=vedadata(1:long_data)//'verif_calib.camp2',
     &      form='formatted',status='old')
	
	do ic=1,17
	   do im=1,24
	      do it=1,3
	         bon(ic,im,it)=.True.
	      enddo
	   enddo
	enddo
	
	isyst=0
	if(nrun.ge.1001.and.nrun.le.1044) then
	   isyst=1
	elseif(nrun.gt.1044.and.nrun.le.1082) then
	   isyst=2
	elseif(nrun.gt.1082.and.nrun.le.1113) then
	   isyst=3
	elseif(nrun.gt.1113.and.nrun.le.1169) then
	   isyst=4
	elseif(nrun.ge.1170.and.nrun.le.1252) then
	   isyst=5
	elseif(nrun.ge.1253.and.nrun.le.1263) then
	   isyst=16
	elseif(nrun.gt.1264.and.nrun.le.1281) then
	   isyst=6
	elseif(nrun.gt.1281.and.nrun.le.1299) then
	   isyst=7
	elseif(nrun.gt.1299.and.nrun.le.1311) then
	   isyst=8
	elseif(nrun.ge.1312.and.nrun.le.1334) then
	   isyst=9
	elseif(nrun.gt.1334.and.nrun.le.1360) then
	   isyst=10
	elseif(nrun.gt.1361.and.nrun.le.1376) then
	   isyst=11
	elseif(nrun.gt.1376.and.nrun.le.1411) then
	   isyst=12
	elseif(nrun.gt.1411.and.nrun.le.1426) then
	   isyst=13
	elseif(nrun.gt.1426.and.nrun.le.1468) then
	   isyst=14
	elseif(nrun.gt.1469.and.nrun.le.1485) then
	   isyst=15
	elseif(nrun.gt.1485.and.nrun.le.1520) then
	   isyst=16
	elseif(nrun.gt.1520.and.nrun.le.1537) then
	   isyst=17
	elseif(nrun.gt.1537.and.nrun.le.1555) then
	   isyst=18
	elseif(nrun.gt.1555.and.nrun.le.1597) then
	   isyst=19
	endif

	if(isyst.eq.0) then
	   Print *,'***** Controle ID/Calib.*****'
	   Print *,'Run #',nrun,' non repertorie pour le controle...'
	   print *,'On intuite que TOUS les modules sont corrects !!'
	   print *,' '
         else
           esura=ebomb(isyst)
	endif 
	ios=0
	nbad=0
	do while(ios.eq.0)
	ios=0
	   read(40,1,end=10) is,nmod
	   read(40,2) (modu(i),i=1,nmod)
	   read(40,2) (type(i),i=1,nmod)
	   
	   if(isyst.eq.is) then
	      do i=1,nmod
	         nbad=nbad+1
	         ic=modu(i)/100
	         im=mod(modu(i),100)
	         bon(ic,im,type(i))=.False.
	      enddo
	   endif
	enddo

10	continue
	
        print *,'************************************************',
     &          '******'
	print *,' '
	Print *,'Initialisation du controle des ID/Calibrations'
        Print *,' '
        Print *,'Version 1.1 / 20 Avril 1998 par O. LOPEZ (LPC Caen)'
        Print *,' '
	Print *,'Run numero  :',nrun
 	print *,'Systeme #',isyst,' : ',system(isyst)
	Print *,'Nbre de modules mal identifies/calibres :',nbad
	print *,'Liste :'
       do i=1,nbad
          if(type(i).eq.1) then
             print *,'- ChIo ',modu(i)
          elseif(type(i).eq.2) then
             print *,'- Si   ',modu(i)
          elseif(type(i).eq.3) then
             print *,'- CsI  ',modu(i)
          endif
       enddo
	print *,' '
       Print *,'Pour enlever les fragments/particules provenant '
       print *,'de ces modules INDRA mal calibres, placer dans votre '
       print *,'routine TRAITEMENT au tout debut l''appel suivant :'
       print *,' '
       print *,'             CALL CLEAN_EVT'
       print *,' '
       print *,'Les tableaux contenant l''evt seront alors '
       print *,' ''nettoyes'' de ces modules indesirables.'
       print *,' '
       print *,'ATTENTION : l''appel doit se faire HORS boucle'
       print *,'sur les fragments/particules de l''evt'
        print *,'************************************************',
     &          '******'
       print *,' '
	       
1	format(1x,i3,2x,i5)
2	format(1x,20(i5))

	close(40)

	return
	end
c---------------------------------------------------------------------------	
	subroutine clean_evt

        Parameter(c=30.,u=931.485)
        
	logical*1 bon(17,24,3)
	common/cebon/bon
	include 'veda_6.incl'
	include 'veda_5.incl'
	include 'veda_11.incl'
	
	integer*4 Ip(traitemax),Ipb(Traitemax)
	integer*4 z0(traitemax),a0(traitemax)
	real*4    z_indra0(traitemax),a_indra0(traitemax)
	real*4    teta_sim0(traitemax),phi_sim0(traitemax)
	real*4    ener0(traitemax)
	integer*4 icou0(traitemax),imod0(traitemax),code0(traitemax)
	integer*4 mt0(traitemax),ecode0(traitemax)
	real*4 de10(traitemax),de20(traitemax),de30(traitemax)
	real*4 de40(traitemax),de50(traitemax),de_mylar0(traitemax)
	real*4 canal0(traitemax,15)
	integer*4 code_idf0(4,Traitemax),code_energie0(4,Traitemax)
       logical*1 bad

        Integer*4 Zbad(traitemax),Abad(traitemax),Codebad(Traitemax)
        integer*4 Icbad(Traitemax),Imbad(traitemax)
        Real*4    Canalbad(traitemax,15),Enerbad(Traitemax)
        Common/CLEANREJ/Mbad,Zbad,Abad,Codebad,enerbad,Canalbad,
     &    Icbad,Imbad

        np=0
        npb=0
	do i=1,npart_traite
          bad=.False.

c --- On enleve les mauvais modules
          
	   if(code(i).eq.0.and..not.bon(icou(i),imod(i),3)) bad=.True.
	   if(code(i).eq.1.and..not.bon(icou(i),imod(i),3)) bad=.True.
	   if(code(i).eq.2.and..not.bon(icou(i),imod(i),3)) bad=.True.
	   if(code(i).eq.3.and..not.bon(icou(i),imod(i),3)) bad=.True.
	   if(code(i).eq.3.and..not.bon(icou(i),imod(i),2)) bad=.True.
	   if(code(i).eq.4.and..not.bon(icou(i),imod(i),2)) bad=.True.
	   if(code(i).eq.4.and..not.bon(icou(i),imod(i),1)) bad=.True.
	   if(code(i).eq.5.and..not.bon(icou(i),imod(i),1)) bad=.True.
	   if(code(i).eq.6.and..not.bon(icou(i),imod(i),1)) bad=.True.
	   if(code(i).eq.6.and..not.bon(icou(i),imod(i),2)) bad=.True.
	   if(code(i).eq.7.and..not.bon(icou(i),imod(i),1)) bad=.True.
	   if(code(i).eq.8.and..not.bon(icou(i),imod(i),1)) bad=.True.
          if(code(i).eq.8.and..not.bon(icou(i),imod(i),2)) bad=.True.

c --- On retire les Phoswichs qui identifient mal
           
          if(icou(i).eq.1.and.z(i).gt.1.25*zproj) bad=.True.

c --- On retire les particules non physiques (pas dans les seuils)

	    am=abs(float(a(i)))
	    if(am.eq.0.) then
	       if(z(i).eq.1) then
	          am=1.007
	       else
	          am=2.*z(i)
	       endif
	    endif
           pp=sqrt(ener(i)*(ener(i)+2.*am*u))
           v=pp*c/(ener(i)+am*u)
           if(code(i).eq.0) v=c
           if(v.gt.c.or.v.lt.c/100.) bad=.True.
	    if(.not.bad) then
	       np=np+1
              ip(np)=i
            else
              npb=npb+1
              ipb(npb)=i
              zbad(npb)=z(i)
              abad(npb)=a(i)
              Icbad(npb)=Icou(i)
              Imbad(npb)=Imod(i)
              Enerbad(npb)=ener(i)
              codebad(npb)=code(i)
              do j=1,15
                canalbad(npb,j)=canal(i,j)
             enddo
	   endif
	enddo
	mbad=npb
	
	do i=1,np
	   z0(i)=z(ip(i))
	   a0(i)=a(ip(i))
	   z_indra0(i)=z_indra(ip(i))
	   a_indra0(i)=a_indra(ip(i))
	   teta_sim0(i)=teta_sim0(ip(i))
	   phi_sim0(i)=phi_sim0(ip(i))
	   icou0(i)=icou(ip(i))
	   imod0(i)=imod(ip(i))
	   ener0(i)=ener(ip(i))
	   code0(i)=code(ip(i))
	   ecode0(i)=ecode(ip(i))
	   de10(i)=de1(ip(i))
	   de20(i)=de2(ip(i))
	   de30(i)=de3(ip(i))
	   de40(i)=de4(ip(i))
	   de50(i)=de5(ip(i))
	   de_mylar0(i)=de_mylar(ip(i))
	   do j=1,15
	      canal0(i,j)=canal(ip(i),j)
	   enddo
	   do j=1,4
	      Code_idf0(j,i)=Code_idf(j,ip(i))
	      Code_energie0(j,i)=Code_energie(j,ip(i))
	    enddo  
	enddo
	
	nr=npart_traite-np
	npart_traite=np
	do i=1,npart_traite
	   z(i)=z0(i)
	   a(i)=a0(i)
	   z_indra(i)=z_indra0(i)
	   a_indra(i)=a_indra0(i)
	   teta_sim(i)=teta_sim0(i)
	   phi_sim(i)=phi_sim0(i)
	   icou(i)=icou0(i)
	   imod(i)=imod0(i)
	   ener(i)=ener0(i)
	   code(i)=code0(i)
	   ecode(i)=ecode0(i)
	   de1(i)=de10(i)
	   de2(i)=de20(i)
	   de3(i)=de30(i)
	   de4(i)=de40(i)
	   de5(i)=de50(i)
	   de_mylar(i)=de_mylar0(i)
	   do j=1,15
	      canal(i,j)=canal0(i,j)
	   enddo
	   do j=1,4
	      Code_idf(j,i)=Code_idf0(j,i)
	      Code_energie(j,i)=Code_energie0(j,i)
	   enddo  
	enddo
	 
	return
	end
c-----------------------------------------------------------------------
       subroutine inipnch (ian,mois,irun, istatus)                       
*------ debut des declarations                                          
        implicit none                                                    
       integer*4         ian, mois, irun, istatus                       
C	                                                                              
       external          pchgg                                          
       integer*4         nc, nm, nm_max, ianm, ln_dir
       real*4            dif                                            
C                                                                        
       integer*4         i, j, k                                        
       integer*4         lur                                            
       character*80      root_dir
C                                                                        
       real*4            valmin_r(17), valmax_r(17)                     
c      real*4            valmin_l(17), valmax_l(17)                     
       integer*4      nurun                                             
       common /runcr/ nurun                                             
*---------------------------------------------------------------------- 
c                     declarations pour le fit                          
*---------------------------------------------------------------------- 
        integer*4     izm_ext, izm_reel                                 
        integer*4     ncou,nmod, nmodu, ru_min, ru_max                       
        integer*4     mmr                                               
        integer*4     bid1, bid2, bid                                   
        real*4        bidr                                              
        logical       logrun                                            
                                                                        
        integer*4     mxnn                                              
        parameter     (mxnn=9)                                          
        real*4        xi(mxnn,10:17,1:24)                               
        real*4        z0i(10:17,1:24), r0i(10:17,1:24)                  
        integer*4     mmi(10:17,1:24)                                   
        integer*4     max_lin                                           
        parameter     (max_lin=20)                                      
        real*4         r_maxi(max_lin,10:17,1:24)                       
        integer*4      izm_reeli(10:17,1:24), izm_exti(10:17,1:24)      
        common/parfit/ xi, z0i, r0i, izm_exti, r_maxi, izm_reeli, mmi   
                                                                        
        real*4        xinfi(10:17,1:24), yinfi(10:17,1:24)              
        real*4        xsupi(10:17,1:24), ysupi(10:17,1:24)              
        common/bragg2/ xinfi, yinfi, xsupi, ysupi                        
*---------------------------------------------------------------------- 
c            declaration pour les piedestaux et les "alpha"             
C   pas de sostraction de piedestal en 2eme campagne
*---------------------------------------------------------------------- 
       character*80    fich_fit                                             
c      character*80    fic2                                             
       integer*4       ncmin_r, ncmax_r, ncmin_l, ncmax_l               
       integer*4       ncal_r, ncxal_r, ncal_l, ncxal_l                 
       integer*4       ncmin_pg, ncmax_pg                               
c                                                                        
       real*4          sig(17,24,2)                                     
       real*4          picr(17,24), pialr(17,24,2)                      
c      real*4          picl(17,24), piall(17,24,2)                      
       real*4          picl, piall                                      
       real*4          pic_pg(17,24), piechgg(2:16,24)                  
                                                                        
       common/ piedcsi / picr, ncmin_r, ncmax_r, picl, ncmin_l, ncmax_l 
       common/ pied_ch / pic_pg, ncmin_pg, ncmax_pg, piechgg            
       common/ alphcsi / pialr, ncal_r, ncxal_r, piall, ncal_l, ncxal_l 
C
        integer*4 run_add ,ni,nrun_NiNi(125),nrun_NiAu(185) ,
     1  nrun_nini_max,nrun_niau_max                         
       logical idlo(10:17,1:24) ,nini,niau,lourds
       common /run_add/ idlo
       character*80 file_add,list_runs
C
	common /Zmax_ID / zzmax
	real*4 zzmax(10:17),zzmax_lourds(10:17),zzmax_UU(10:17),
     m	zzmax_NiNi(10:17) ,zzmax_NiAu(10:17)
	include 'veda_11.incl'
	data nrun_nini_max,nrun_niau_max / 124 , 182 /
         data zzmax_lourds / 9.5,11.5,14.5,9.5,9.5,10.5,9.5,10.5 /
         data zzmax_UU /8.5,10.5,10.5,9.5,7.5,7.5,5.5,5.5 /
         data zzmax_NiNi / 13.5,11.5,10.5,7.5,6.5,6.5,6.5,6.5 /
c         data zzmax_NiAu / 16.5,13.5,15.5,10.5,9.5,8.5,6.5,6.5 /
         data zzmax_NiAu / 9.5,7.5,9.5,6.5,8.5,6.5,6.5,6.5 /

C                                                                         
*------ fin des declarations      /                                       
      istatus = 0                                                       
      nurun = irun                                                      
c                 * Anastasie *
      ln_dir = 0
      call getenvf('THRONG_DIR',root_dir)
      if(root_dir.ne.' ') then
        ln_dir = len(root_dir)          
        do while (root_dir(ln_dir:ln_dir).eq.' ')
          ln_dir = ln_dir-1
        enddo
      endif
c      
      ianm = mod(ian,100)                                                  
********                                                                
* calcul de la valeur min et max de alpha-pied                          
********                                                                
        do nc = 10, 17                                               
          valmin_r(nc) = 8191.                                      
          valmax_r(nc) = 0.                                         
c             valmin_l(nc) = 8191.                                      
c             valmax_l(nc) = 0.                                         
          
          if(nc.ge.10 .and. nc.le.13) then                          
            nm_max = 24                                             
          else if(nc.ge.14 .and. nc.le.15) then                      
            nm_max = 16                                             
          else if(nc.ge.16 .and. nc.le.17) then                      
            nm_max = 8                                              
          endif                                                     
          do nm = 1, nm_max                                         
C                                                                        
            if(pialr(nc,nm,1).ne.0.) then                           
              dif = pialr(nc,nm,2) - picr(nc,nm) + 1.               
              valmin_r(nc) = amin1(valmin_r(nc), dif)               
              valmax_r(nc) = amax1(valmax_r(nc), dif)               
            endif                                                   
                                                                        
c               if(piall(nc,nm,1).ne.0.) then                           
c                 dif = piall(nc,nm,2) - picl(nc,nm) + 1.               
c                 valmin_l(nc) = amin1(valmin_l(nc), dif)               
c                 valmax_l(nc) = amax1(valmax_l(nc), dif)               
c               endif                                                   
                                                                        
          enddo             ! sur les modules                       
C                                                                        
          do nm = 1, nm_max                                         
C                                                                        
            if(pialr(nc,nm,1).ne.0.) then                           
              dif = pialr(nc,nm,2) - picr(nc,nm) + 1.               
              pialr(nc,nm,1) = valmax_r(nc)/dif                     
            endif                                                   
                                                                        
c               if(piall(nc,nm,1).ne.0.) then                           
c                 dif = piall(nc,nm,2) - picl(nc,nm) + 1.               
c                 piall(nc,nm,1) = valmax_l(nc)/dif                     
c               endif                                                   
                                                                        
          enddo             ! sur les modules                       
        enddo                ! sur les couronnes
*
*          ---  PIEDESTAUX CAMPAGNE 94 ---                                    
*
        if(ianm.eq.94 .and. mois.gt.2) then
        do nc = 10, 17
          if(nc.ge.10 .and. nc.le.13) then                          
            nm_max = 24                                             
          else if(nc.ge.14 .and. nc.le.15) then                      
            nm_max = 16                                             
          else if(nc.ge.16 .and. nc.le.17) then                      
            nm_max = 8                                              
          endif                                                     
          do nm = 1, nm_max                                         
            picr(nc,nm) = 1.
            pialr(nc,nm,1) = 1.
            pic_pg(nc,nm) = 1.
          enddo
        enddo
c          ----  AUTRES CAMPAGNES DE MESURES ----                       
      else          ! c.a.d annee.ne.1993 et 94
        istatus = -10                                                   
      endif
C                             
cc  * initialisation 
*** 
	if (istatus.ge.0) then
C
	nini = .false.
	niau = .false.
	lourds = .false.
C
	if (nurun .ge. 1253 ) go to 125
	lourds = .true.
	fich_fit = vedadata(1:long_data)//'pfitlds.dat'
c
       file_add = vedadata(1:long_data)//'runs_add.lds'
C
C	
	do nc = 10,17
	
	zzmax( nc ) = zzmax_lourds(nc)
	 end do
C
	if (nurun .ge. 1170 .and. nurun .le. 1189  ) then 
	do nc = 10,17
	zzmax( nc ) = zzmax_UU(nc)
	 end do
	endif
C
	go to 126	
c
  125	continue	
c lecture des listes de runs Ni + Ni et Ni + Au
	list_runs = vedadata(1:long_data)//'runs.ni'
C
c	print *,' lecture des listes de runs Ni+Ni et Ni+Au :list_runs   
c     1	nrun_max ' 
c         print * ,list_runs 
c	 print * ,'                                                  ' ,
c     2	  nrun_nini_max,nrun_niau_max  
       open(unit=31,file=list_runs,status='old')
	do ni = 1,nrun_nini_max
	read(31,*)nrun_NiNi(ni) 	
C	 write(6,*)nrun_NiNi(ni) 
	end do
c
	do ni = 1,nrun_niau_max
	read(31,*,end=444)nrun_NiAu(ni) 	
C	write(6,*)nrun_NiAu(ni)
	end do
c
  444	continue
C
	close(31)
C
C'est un run Ni + Ni ?
C
	do ni = 1,nrun_nini_max
	if (nurun .eq. nrun_NiNi(ni) ) then 
	nini = .true.  	
	go to 127
	endif
C
        end do                                                            
c
C'est un run Ni + Au ?
C
	do ni = 1,nrun_niau_max
	if (nurun .eq. nrun_NiAu(ni) ) then 
	niau = .true.  	
	endif
        end do                                                            
c
 127	continue	
	if (nini ) then
	fich_fit = vedadata(1:long_data)//'pfcnini.dat'
c
       file_add = vedadata(1:long_data)//'runs_add.NiNi'
C Zmax
	  do nc = 10,17
	  zzmax(nc) = zzmax_NiNi(nc)
	 end do
C	 
	endif
C
	if (niau ) then
	fich_fit = vedadata(1:long_data)//'pfcniau.dat'
c
       file_add = vedadata(1:long_data)//'runs_add.NiAu'
C Zmax
	  do nc = 10,17
	  zzmax(nc) = zzmax_NiAu(nc)
	 end do
C
	endif
C 
C LECTURE DES FICHIERS "RUNS_ADDITIONNABLES"
  126	continue
C
        do ncou = 10,17        ! initialisation du tableau runs_additionnables
         do nmod = 1,24 
            idlo(ncou,nmod) = .false.
         enddo
        enddo  
C
C
C la aussi vedadata
C  lecture du fichier " runs_additionnables "
       print *,'lecture du fichier runs_additionnables'
		  print *,'file_add=',file_add
		 open(unit=20,file=file_add,status='old')
       do i=1,20000
         read(20,*,end=100) run_add,ncou,nmod
         if (run_add.eq.nurun) then
c             write(6,*) 'ligne=',i,' run=',run_add,' cou=',
c     &       ncou,' mod=',nmod
             idlo(ncou,nmod)=.true.
         endif
        enddo
C
100	continue
c      write (6, 234 ) nurun,lourds,nini,niau
 234	format ( 1h ,' n0run      Lourds?     Ni+Ni?     Ni+Au?  '/
     2	 1h ,i7,3l10 )
c     	print * ,' fichier fit ' , fich_fit 
c     	print * , ' fichier runs_add ' , file_add	 	 	
  	close(20)
C  ET les ZZmax
c	write (6 ,619) nurun,(zzmax(nc) , nc = 10,17 )
  619	format(1h , ' ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZmax   ' /
     q	 1h , ' num run    Zmax  cour  10  a  17 ...........  '/
     w	 1h , i6 ,3x, 8f8.1 //) 
C
c-----------------------------------------------------------------------
* lecture du fichier resultat de fit                                    
C
        do k=1,24                                                        
          do j=10,17                                                     
            izm_reeli(j,k) = 0                                           
          enddo                                                          
        enddo                                                            
        do k=1,24                                                        
          do j=10,17                                                     
            mmi(j,k) = 0                                                 
          enddo                                                          
        enddo                                                            
C
       logrun = .false.                                                
C
c	print * , fich_fit                                          
        lur = 22                                                        
        open(unit=lur,file=fich_fit,status='old')                           
C                                                                        
 25      read(lur,'(a1)',end=26)root_dir                                     
c        write(*,'(a1)') root_dir                                           
        read(lur,*,end=26) ncou,nmodu,ru_min,ru_max                     
c        write(6,'(1x,4i5)') ncou,nmodu,ru_min,ru_max                   
                                                                        
        if( ncou.ge.10 .and. ncou.le.17 .and.                           
     &      nmodu.ge.1 .and. nmodu.le. 24) then                         
                                                                        
          if( irun.ge.ru_min .and. irun.le.ru_max ) then                
                                                                        
            read (lur,*,end=26)                                         
     &        izm_reel, izm_ext, z0i(ncou,nmodu), r0i(ncou,nmodu), mmr
            izm_reel = min0(izm_reel,max_lin)                           
            mmi(ncou,nmodu) = mmr                                       
            izm_reeli(ncou,nmodu) = izm_reel                            
            izm_exti(ncou,nmodu)  = izm_ext                             
                                                                        
c            write (*,'(1x,2i3,1x,2f5.2,1x,i3)')                        
c     1        izm_reel, izm_ext, z0i(ncou,nmodu), r0i(ncou,nmodu), mmr 
                                                                        
            if(izm_reel .eq. 0) goto 25                                 
                                                                        
            logrun = .true.                                             
                                                                        
            read(lur,*,end=26) (xi(i,ncou,nmodu),i=1,mmr)               
C             write(*,*) (xi(i,ncou,nmodu),i=1,mmr)                      
                                                                        
            read (lur,*,end=26) (r_maxi(i,ncou,nmodu),i=1,izm_reel)     
c            write(*,*) (r_maxi(i,ncou,nmodu),i=1,izm_reel)             
                                                                        
            if(pialr(ncou,nmodu,1).eq.0.) izm_reeli(ncou,nmodu) = 0     
                                                                        
            read(lur,*,end=26) xinfi(ncou,nmodu), yinfi(ncou,nmodu),    
     &                         xsupi(ncou,nmodu), ysupi(ncou,nmodu)     
                                                                        
c            write(*,*) xinfi(ncou,nmodu), yinfi(ncou,nmodu),           
c     #                         xsupi(ncou,nmodu), ysupi(ncou,nmodu)    
                                                                        
            goto 25                                                     
          else              ! c.a.d irun.lt.ru_min .or. irun.gt.ru_max  
                                                                        
            read (lur,*,end=26) bid1, bid, bidr, bidr, bid2             
            if( bid1 .eq. 0) goto 25                                    
            read(lur,*,end=26) (bidr,i=1,bid2)                          
            read (lur,*,end=26) (bidr,i=1,bid1)                         
            read (lur,*,end=26) (bidr,i=1,4)                            
                                                                        
            goto 25                                                     
          endif                                                         
        endif                                                           
                                                                        
26      close(lur)                                                      
c       call lib$free_lun(lur)                                          
        if (.not. logrun ) then                                         
          istatus = -10                                                
        endif
      endif
c	print *,'  ===> SORTIE INIT ID chio_csi 10-17 RUN N0 ' ,irun 
      return                                                            
       end                                                               
c                                                                       
c---------------------------------------------------------------------- 
* tableau extrait de prog:pied.for (voir mfr) en faisant la             
* correction (valable pour run > 435) sur les chio:                     
* 10-13 <---> 12-13                                                     
* 10-15 <---> 12-15                                                     
c---------------------------------------------------------------------- 
      block data pchgg                                                  
      integer*4       ncmin_pg, ncmax_pg                                
      real*4          pic_pg(17,24)                                     
      real*4          piechgg(2:16,24)                                  
      common/ pied_ch / pic_pg, ncmin_pg, ncmax_pg, piechgg             
                                                                        
      data piechgg/                                                     
     # 323.7,0.,294.8,0.,308.1,0.,535.7,0.,356.5,0.,395.5,473.2,480.5,  
     #                                                    0.   ,485.4,  
     # 14*0.,493.9,                                                     
     # 329.0,0.,288.0,0.,319.6,0.,318.1,0.,349.1,0.,355.8,0.   ,480.5,  
     #                                                    0.   ,489.3,  
     # 11*0.,480.0,2*0.,482.0,                                          
     # 294.4,0.,288.8,0.,314.8,0.,332.5,0.,348.9,0.,342.9,0.   ,481.5,  
     #                                                    0.   ,485.3,  
     # 14*0.,475.5,                                                     
     # 283.9,0.,282.3,0.,314.5,0.,318.4,0.,353.5,0.,349.3,467.3,481.1,  
     #                                                    0.   ,489.3,  
     # 14*0.,481.7,                                                     
     # 285.2,0.,302.3,0.,313.7,0.,348.6,0.,347.6,0.,462.0,0.   ,475.5,  
     #                                                    2*0.,         
     # 11*0.,453.6,3*0.,                                                
     # 288.6,0.,309.3,0.,313.9,0.,344.3,0.,348.5,0.,466.5,0.  ,484.3,   
     #                                                    2*0.,         
     # 15*0.,                                                           
***** pour l'ar                                                         
     # 284.7,0.,307.5,0.,306.4,0.,338.7,0.,453.3,0.,337.9,455.0,473.0,  
***** pour le xe                                                        
c     # 284.7,0.,307.5,0.,306.4,0.,338.7,0.,337.9,0.,453.3,455.0,473.0, 
     #                                                    2*0.,         
     # 15*0.,                                                           
***** pour l'ar                                                         
     # 283.6,0.,304.6,0.,307.4,0.,338.9,0.,457.8,0.,338.1,0.  ,472.2,   
***** pour le xe                                                        
c     # 283.6,0.,304.6,0.,307.4,0.,338.9,0.,338.1,0.,457.8,0.  ,472.2,  
     #                                                    2*0.,         
     # 11*0.,458.0,3*0.,                                                
     # 301.7,0.,309.0,0.,340.4,0.,333.7,0.,364.5,0.,447.0,4*0.,         
     # 15*0.,                                                           
     # 303.5,0.,307.9,0.,331.3,0.,332.8,0.,356.7,0.,441.1,456.6,3*0.,   
     # 15*0.,                                                           
     # 297.2,0.,295.7,0.,333.4,0.,323.7,0.,364.8,0.,452.1,4*0.,         
     # 11*0.,457.7,3*0.,                                                
     # 288.0,0.,299.0,0.,326.0,0.,330.1,0.,349.6,0.,450.2,4*0.,         
     # 15*0./                                                           
      end                                                               
c----------------------------------------------------------------------------
       subroutine Bragg_Chio_csi(nc,nm,irap,ichpg,istat)                   
       implicit none                                                    
       integer*4       nc, nm, istat                                    
       integer*4       irap, ichpg                                      
       real*4          zexp,zzmax(10:17)                                             
c
       external        chio                                             
       real*4          chio                                             
       integer*4       nks                                              
       integer*4       nc_ch, nm_ch                                     
       real*4          prbru, pchpgbru                                  
       real*4          rap, chpg                                        
       logical         cond 
       data nks /7654321/                                               
                                                                        
       real*4          zch, yy1, yy2                                    
       real*4          ran                                              
*---------------------------------------------------------------------- 
c            declaration pour les piedestaux et les "alpha"             
*---------------------------------------------------------------------- 
       integer*4       ncmin_r, ncmax_r, ncmin_l, ncmax_l               
       integer*4       ncal_r, ncxal_r, ncal_l, ncxal_l                 
       integer*4       ncmin_pg, ncmax_pg                               
       real*4          picr(17,24), pialr(17,24,2)                      
c      real*4          picl(17,24), piall(17,24,2)                      
       real*4          picl, piall                                      
       real*4          pic_pg(17,24), piechgg(2:16,24)                  
                                                                        
       common/ piedcsi / picr, ncmin_r, ncmax_r, picl, ncmin_l, ncmax_l 
       common/ pied_ch / pic_pg, ncmin_pg, ncmax_pg, piechgg            
       common/ alphcsi / pialr, ncal_r, ncxal_r, piall, ncal_l, ncxal_l 
                                                                        
*---------------------------------------------------------------------- 
c                     declarations pour le fit                          
c                recuperation des tableaux de l'init                    
*---------------------------------------------------------------------- 
        integer      i                                                  
                                                                        
        integer*4      mxnn                                             
        parameter     (mxnn=9)                                          
        real*4         xi(mxnn,10:17,1:24)                              
        real*4         z0i(10:17,1:24), r0i(10:17,1:24)                 
        integer*4      mmi(10:17,1:24)                                  
        integer*4      max_lin                                          
        parameter     (max_lin=20)                                      
        real*4         r_maxi(max_lin,10:17,1:24)                       
        integer*4      izm_reeli(10:17,1:24), izm_exti(10:17,1:24)      
        common/parfit/ xi, z0i, r0i, izm_exti, r_maxi, izm_reeli, mmi   
                                                                        
        real*4        xinfi(10:17,1:24), yinfi(10:17,1:24)              
        real*4        xsupi(10:17,1:24), ysupi(10:17,1:24)              
        common/bragg2/ xinfi, yinfi, xsupi, ysupi                        
                                                                        
        integer*4       izm_ext, izm_reel                               
        real*4          xinf, yinf, xsup, ysup                          
        real*4          pente, ordal, ybrag                             
*---------------------------------------------------------------------- 
c                     declarations pour le fit                          
c                  pour transmettre a la fonction                       
*---------------------------------------------------------------------- 
       integer*4       mm                                               
       real*4          z0, r0                                           
       real*4          x(mxnn)                                          
       common /chior/  mm, z0, r0, x
C  gestion "runs_additionnables"
C
	logical idlo(10:17,1:24)
	common /run_add/ idlo       
C	
C pour traitement des Zmax
	common /Zmax_ID / zzmax                                         
c                                                                        
*****fin des declarations                                               
*---------------------------------------------------------------------- 
*             correspondance fit <----> fonction theorique              
*---------------------------------------------------------------------- 
       istat = 0                                                        
       zexp = 0.                                                        
       if(nc.lt.10 .or. nc.gt.17 .or. nm.gt.24) then                    
         istat = -7                                                     
         return                                                         
       endif             
C rejet idlo
	if ( .not.idlo(nc,nm) ) then
	Zexp = - 99.
C        istat = - 9
	istat = - 1  !!!!!  on replace le code a une valeur DECODE_PART
	 return
	endif
C                                                      
*---------------------------------------------------------------------- 
*             calcul du module chio                                     
*---------------------------------------------------------------------- 
 111	continue
C 
       nc_ch = nc                                                       
       nm_ch = nm                                                       
       if(nc.ne.13) nc_ch = nc/2*2                                      
       if(nc.eq.13) then                                                
         nm_ch = (nm-1)/3*3+1                                           
       else if(nc.le.15) then                                           
         nm_ch = (nm-1)/2*2+1                                           
       endif                                                            
c                                                                       
       mm       = mmi(nc,nm)                                            
       izm_ext  = izm_exti(nc,nm)                                       
       izm_reel = izm_reeli(nc,nm)                                      
       z0       = z0i(nc,nm)                                            
       r0       = r0i(nc,nm)                                            
                                                                        
       if(izm_reel.eq.0) then                                           
         istat = -1                                                     
         return                                                         
       endif                                                            
       do i = 1, mm                                                     
         x(i) = xi(i,nc,nm)                                             
       enddo                                                            
*---------------------------------------------------------------------- 
*                 passage en coordonnees reduites                       
*---------------------------------------------------------------------- 
*---------------------------------------------------------------------- 
c les lignes si dessous sont commentees pour pouvoir calculer avec le   
c programme  dist.for le spectre  en z a  partir d'un ndim transforme   
c (voir dist.for). au ccpn ils ne sont pas  commentees car on utilise   
c un ndim brut.                                                         
*---------------------------------------------------------------------- 
       prbru    = irap+ran(nks)                                         
       pchpgbru = ichpg+ran(nks)                                        
                                                                        
       rap      = prbru - picr(nc,nm) + 1.                              
       rap      = rap * pialr(nc,nm,1)                                  
       rap      = amax1(rap,0.)                                         
C evenement spurieux si rap < 50. ?
	if (rap .lt. 50. ) rap = 52.
C ??????????????????????????????????
       chpg     = pchpgbru - pic_pg(nc_ch,nm_ch) + 1.                   
       chpg     = amax1(chpg,0.)                                        
                                                                        
c        rap      = irap + ran(nks)     ! commente au ccpn              
c        chpg     = ichpg + ran(nks)    ! commente au ccpn              
*******                                                                 
c  Calcul de la droite limitant le pic de bragg                         
*******                                                                 
       xinf     = xinfi(nc,nm)                                          
       yinf     = yinfi(nc,nm)                                          
       xsup     = xsupi(nc,nm)                                          
       ysup     = ysupi(nc,nm)                                          
                                                                        
       pente    = (ysup - yinf)/(xsup - xinf)                           
       ordal    = ysup - pente*xsup                                     
       ybrag    = pente*rap + ordal                                     
*******                                   
       if(chpg.ge.ybrag)                  istat = 10
	               
       return                                                           
       end                                                              
c======fin de recherche de z                                            
c==================                                                     
c                                                                       
*-----------------------------------------------------------------------
*     call resol(z1, yy1, z2, yy2, y0, fun, rap, tol, z)                
*   entree :                                                            
*      z1      : [r] borne inferieure (fun-y0 < 0)                      
*      yy1     : [r] valeur de la fonction fun pour z1                  
*      z2      : [r] borne superieure (fun-y0 > 0)                      
*      yy2     : [r] valeur de la fonction fun pour z2                  
*      y0      : [r] valeur plancher : annuler fun-y0                   
*     fun      : [r] fonction fun(rap, z) a egaliser a y0               
*                                                (en jouant sur z)      
*     rap      : [r] parametre de fun                                   
*     tol      : [r] tolerance sur la precision de resolution           
*                                                                       
*   retour :                                                            
*     z        : [r] valeur de la solution                              
*-----------------------------------------------------------------------
      subroutine resol(z1, yy1, z2, yy2, y0, fun, rap, tol, z)          
      implicit none                                                     
      real      z1, yy1, z2, yy2, y0, rap, tol, z                       
      real      fun                                                     
                                                                        
      integer   i, it                                                   
      real      x1, y1, x2, y2, xn, yn, xp, yp, y, dist, zp             
                                                                        
      x1   = z1                                                         
      x2   = z2                                                         
      y1   = yy1 - y0                                                   
      y2   = yy2 - y0                                                   
      xn   = x1                                                         
      xp   = x2                                                         
      yn   = y1                                                         
      yp   = y2                                                         
      dist = abs(x2-x1)                                                 
      z    = (y2*x1-y1*x2)/(y2-y1)                                      
      it = 1                                                            
      do while (dist.gt.tol)                                            
c          write(6,'(''  x1, x2 ='',2f10.5,4x,''y1,y2 ='',2f10.5)')     
c     1                   x1, x2, y1, y2                                
        y = fun(rap, z) - y0                                            
c          write(6,'(''         z, y ='',2f10.5)') z, y                 
        if(y.lt.0.) then                                                
          xn = z                                                        
          yn = y                                                        
        else                                                            
          xp = z                                                        
          yp = y                                                        
        endif                                                           
        if(y.eq.0.) dist = 0.                                           
        if(dist.gt.tol) then                                            
          if(y.eq.y1) then                                              
            x1 = z                                                      
            zp = (y2*x1-y1*x2) / (y2-y1)                                
          else if(y.eq.y2) then                                         
            x2 = z                                                      
            zp = (y2*x1-y1*x2) / (y2-y1)                                
          else                                                          
            if(abs(z-x1).lt.abs(x2-z)) then                             
              i = 1                                                     
              zp = (y*x1-y1*z) / (y-y1)                                 
            else                                                        
              i = 2                                                     
              zp = (y2*z-y*x2)/(y2-y)                                   
            endif                                                       
            if(zp.lt.xn .or. zp.gt.xp) then                             
c             write(6,*) '     @@@@@@'                                  
              if(i.eq.1) then                                           
                zp = (y2*z-y*x2)/(y2-y)                                 
              else                                                      
                zp = (y*x1-y1*z)/(y-y1)                                 
              endif                                                     
            endif                                                       
          endif                                                         
          dist = abs(z-zp)                                              
          x1 = xn                                                       
          y1 = yn                                                       
          x2 = xp                                                       
          y2 = yp                                                       
          z = zp                                                        
          it = it+1                                                     
        endif                                                           
      enddo                                                             
c      write(6,'('' nit ='',i3)') it                                    
      return                                                            
      end                                                               
c                                                                       
*----------                                                      -------
*           attention cette routine ne marche qu'apres avoir            
*               fait call inipnch (ian,mois,irun,istatus)               
*----------                                                      -------
*     call chr_ipn(nc,nm,irap,iz,ichpg,ich_inf,ich_sup,istat)           
*            routine calculant la valeur de la chpg a partir            
*          d'une rapide et d'un z donnes dans la carte (r-chpg)         
*   entree:                                                             
*      nc      :  [integer*4]   no de couronne                          
*      nm      :  [integer*4]   no de module                            
*      irap    :  [integer*4]   valeur de la rapide                     
*      iz      :  [integer*4]   valeur de z                             
*                                                                       
*   retour:                                                             
*      ichpg   :  [integer*4]   valeur de la chpg                       
*      ich_inf :  [integer*4]   borne inferieure de la chpg             
*      ich_sup :  [integer*4]   borne superieure de la chpg             
*      istat   :  [integer*4]   code retour                             
*                  istat = 0   ok                                       
*                  istat = 1   ok mais iz  > zmax(trace a la main)      
*                  istat = 2   ok mais iz  > zmax(extrapole)            
*                  istat = 3   ok mais rap > rmax(iz) (dernier point de 
*                                             la rapide trace a la main)
*                  istat = 10  au dessus de la droite limitant bragg    
*                  istat = -1   (carte chio,csi) n'existe pas pour ce ru
*                  istat = -7   cour < 10 ou cour > 17 (pas de chio-csi)
*----------                                                      -------
c                                                                       
       subroutine  chr_ipn(nc,nm,irap,iz,ichpg,ich_inf,ich_sup,istat)   
       implicit   none                                                  
       integer*4       nc, nm, irap, iz                                 
       integer*4       ichpg, ich_inf, ich_sup, istat                   
c                                                                       
       integer*4       nc_ch, nm_ch                                     
       real*4          chio                                             
       real*4          zch, chpg, ch_inf, ch_sup                        
       real*4          prbru, rap, rap0                                 
                                                                        
       integer*4       i                                                
       integer*4      nurun                                             
       common /runcr/ nurun                                             
                                                                        
       real*4          gapz, decal                                      
*---------------------------------------------------------------------- 
c            declaration pour les piedestaux et les "alpha"             
*---------------------------------------------------------------------- 
       integer*4       ncmin_r, ncmax_r, ncmin_l, ncmax_l               
       integer*4       ncal_r, ncxal_r, ncal_l, ncxal_l                 
       integer*4       ncmin_pg, ncmax_pg                               
       real*4          picr(17,24), pialr(17,24,2)                      
c      real*4          picl(17,24), piall(17,24,2)                      
       real*4          picl, piall                                      
       real*4          pic_pg(17,24), piechgg(2:16,24)                  
                                                                        
       common/ piedcsi / picr, ncmin_r, ncmax_r, picl, ncmin_l, ncmax_l 
       common/ pied_ch / pic_pg, ncmin_pg, ncmax_pg, piechgg            
       common/ alphcsi / pialr, ncal_r, ncxal_r, piall, ncal_l, ncxal_l 
                                                                        
*---------------------------------------------------------------------- 
c                     declarations pour le fit                          
c                recuperation des tableaux de l'init                    
*---------------------------------------------------------------------- 
        integer*4      mxnn                                             
        parameter      (mxnn=9)                                         
        real*4         xi(mxnn,10:17,1:24)                              
        real*4         z0i(10:17,1:24), r0i(10:17,1:24)                 
        integer*4      mmi(10:17,1:24)                                  
        integer*4      max_lin                                          
        parameter     (max_lin=20)                                      
        real*4         r_maxi(max_lin,10:17,1:24)                       
        integer*4      izm_reeli(10:17,1:24), izm_exti(10:17,1:24)      
        common/parfit/ xi, z0i, r0i, izm_exti, r_maxi, izm_reeli, mmi   
                                                                        
        real*4         xinfi(10:17,1:24), yinfi(10:17,1:24)             
        real*4         xsupi(10:17,1:24), ysupi(10:17,1:24)             
        common/bragg2/  xinfi, yinfi, xsupi, ysupi                       
                                                                        
        integer*4       izm_ext, izm_reel                               
        real*4          xinf, yinf, xsup, ysup                          
        real*4          pente, ordal, ybrag                             
*---------------------------------------------------------------------- 
c                     declarations pour le fit                          
c                  pour transmettre a la fonction                       
*---------------------------------------------------------------------- 
        real*4          z0, r0                                          
        integer*4      mm                                               
        real*4         x(mxnn)                                          
        common /chior/  mm, z0, r0, x                                   
*****fin des declarations                                               
       istat = 0                                                        
       if(nc.lt.10 .or. nc.gt.17 .or. nm.gt.24) then                    
         istat = -7                                                     
         return                                                         
       endif                                                            
*---------------------------------------------------------------------- 
*             calcul du module chio                                     
*---------------------------------------------------------------------- 
       nc_ch = nc                                                       
       nm_ch = nm                                                       
       if(nc.ne.13) nc_ch = nc/2*2                                      
       if(nc.eq.13) then                                                
         nm_ch = (nm-1)/3*3+1                                           
       else if(nc.le.15) then                                           
         nm_ch = (nm-1)/2*2+1                                           
       endif                                                            
*---------------------------------------------------------------------- 
*             correspondance fit <----> fonction theorique              
*---------------------------------------------------------------------- 
        mm        = mmi(nc,nm)                                          
        izm_ext   = izm_exti(nc,nm)                                     
        izm_reel  = izm_reeli(nc,nm)                                    
        z0        = z0i(nc,nm)                                          
        r0        = r0i(nc,nm)                                          
        if(izm_reel.eq.0) then                                          
          istat = -1                                                    
          return                                                        
        endif                                                           
        do i = 1, mm                                                    
          x(i) = xi(i,nc,nm)                                            
        enddo                                                           
*---------------------------------------------------------------------- 
*             passage en coordonnees reduites pour la rapide            
*---------------------------------------------------------------------- 
       prbru    = irap                                                  
       rap      = prbru - picr(nc,nm) + 1.                              
       rap      = rap * pialr(nc,nm,1)                                  
       rap      = amax1(rap,0.)                                         
*******                                                                 
c  Calcul de la droite limitant le pic de bragg                         
*******                                                                 
       xinf     = xinfi(nc,nm)                                          
       yinf     = yinfi(nc,nm)                                          
       xsup     = xsupi(nc,nm)                                          
       ysup     = ysupi(nc,nm)                                          
                                                                        
       pente    = (ysup - yinf)/(xsup - xinf)                           
       ordal    = ysup - pente*xsup                                     
       ybrag    = pente*rap + ordal                                     
*******                                                                 
c                                                                       
******* debut du programme                                              
c                                                                       
       zch  = iz                                                        
       gapz = 0.5                                                       
c                                                                       
c   gapz : largeur en Z equivalent, servant a l'evaluation de la largeur
c         de ligne de Z en canaux : largeur independante de R et calcule
c                                                                       
       chpg = chio(rap,zch)                                             
       if(iz.le.izm_ext) then                                           
         if(iz.le.izm_reel) then                                        
           if(iz.gt.0) then                                             
             if(rap.gt.r_maxi(iz,nc,nm)) istat = 3                      
           endif                                                        
           if(iz.gt.2 .and. ((nc.eq.10 .or. nc.eq.11) .and.             
     #                                  nurun.ge.545)) then             
               gapz = 0.7                                               
           endif                                                        
         else                                                           
           istat = 1                                                    
           gapz = 0.8                                                   
         endif                                                          
       else                                                             
         istat = 2                                                      
         gapz = 1.                                                      
       endif                                                            
       if(chpg.ge.ybrag) istat = 10                                     
       zch = amax1(zch,1.)                                              
       rap0 = 0.                                                        
       ch_sup = chio(rap0,zch+gapz)                                     
       if(zch.gt.1.5) then                                              
         ch_inf = chio(rap0,zch-gapz)                                   
       else                                                             
         ch_inf = chpg*2-ch_sup                                         
       endif                                                            
       rap = (ch_sup-ch_inf)/2                                          
       ch_inf = chpg-rap                                                
       ch_sup = chpg+rap                                                
*---------------------------------------------------------------------- 
*            passage en coordonnees brutes pour la chio                 
*---------------------------------------------------------------------- 
       decal = pic_pg(nc_ch,nm_ch) - 1.                                 
       chpg = chpg + decal                                              
       chpg = amax1(chpg,0.)                                            
       ichpg = nint(chpg)                                               
       ch_inf = ch_inf + decal                                          
       ch_inf = amax1(ch_inf,0.)                                        
       ich_inf = nint(ch_inf)                                           
       ch_sup = ch_sup + decal                                          
       ch_sup = amax1(ch_sup,0.)                                        
       ich_sup = nint(ch_sup)                                           
       return                                                           
       end                                                              
*---------------------------------------------------------------------- 
      subroutine LecAlph(fich,intg,peak,sigma,ncmin,ncmax)              
      implicit   none                                                   
      integer*4       intg(17,24,*), ncmin, ncmax                       
      real*4          peak (17,24,*), sigma (17,24,*)                   
      character*(*)   fich                                              
c                                                                       
      integer         modules                                           
      parameter       (modules = 400)                                   
      character*80    ndimlu                                            
      integer*4       i, j, k, kp, lu, ifl, nf, nc, nm, np, ncan        
c                                                                       
      lu = 23                                                           
c     call lib$get_lun(lu) 
	print *,' fich dans LecAlph ', fich                                             
      open(unit=lu, file= fich, status= 'old')                          
      read(lu,*) ifl                                                    
c      write(*,*) ifl                                                   
      read(lu,*) nf                                                     
c      write(*,*) nf                                                    
      do i = 1, nf                                                      
          read(lu,'(a1)') ndimlu                                        
c          write(*,'(a)') ndimlu                                        
      enddo                                                             
*                                                                       
      if (ifl.eq.1) then           !  1 : un fichier des pics alpha     
         do k= 1, 2                                                     
           do j= 1, 24                                                  
             do i= 1, 17                                                
               intg(i,j,k) = 0                                          
               peak(i,j,k) = 0.                                         
               sigma(i,j,k) = 0.                                        
             enddo                                                      
           enddo                                                        
         enddo                                                          
         do i = 1, modules                                              
           read(lu,*,end=100) nc, nm, np                                
           if(i.eq.1) ncmin = nc                                        
c           write(*,'(3i4)') nc, nm, np                                 
           if(np.ge.2) then                                             
             do kp = 1, 2                                               
               read(lu,'(i10,f10.2,f7.4)') intg(nc,nm,kp),peak(nc,nm,kp)
     #               ,sigma(nc,nm,kp)                                   
c               write(*,'(i10,f10.2,f7.4)') intg(nc,nm,kp),             
c     #              peak(nc,nm,kp),sigma(nc,nm,kp)                     
             enddo                                                      
           endif                                                        
         enddo                                                          
*                                                                       
      elseif(ifl.eq.2) then        ! 2 : fichier des piedestaux  CH_IO o
         do j= 1, 24                                                    
           do i= 1, 17                                                  
             intg(i,j,1) = 0                                            
             peak(i,j,1) = 0.                                           
             sigma(i,j,1) = 0.                                          
           enddo                                                        
         enddo                                                          
         do i = 1, modules                                              
           read(lu,*,end=100) nc, nm, np                                
           if(i.eq.1) ncmin = nc                                        
c           write(*,'(3i4)') nc, nm, np                                 
           if(np.ge.1) then                                             
              read(lu,'(i10,f10.2,f7.4)') intg(nc,nm,1),peak(nc,nm,1),  
     #                 sigma(nc,nm,1)                                   
c               write(*,'(i10,f10.2,f7.4)') intg(nc,nm,1),peak(nc,nm,1),
c     #                sigma(nc,nm,1)                                   
           endif                                                        
         enddo                                                          
*                                                                       
      elseif(ifl.eq.3) then        ! 3 : fichier des piedestaux CSI     
         do j= 1, 24                                                    
           do i= 1, 17                                                  
             peak(i,j,1) = 0.                                           
           enddo                                                        
         enddo                                                          
        do i = 1, modules                                               
           read(lu,*,end=100) nc, nm, ncan                              
c           write(6,*) nc, nm, ncan                                     
           if(i.eq.1) ncmin = nc                                        
           peak(nc,nm,1) = ncan                                         
        enddo                                                           
      endif                                                             
100   continue                                                          
      write(6,'('' le fichier'',1x,a,/,''    est lu'')') fich           
      close(lu)                                                         
c     call lib$free_lun(lu)                                             
      ncmax = nc                                                        
      return                                                            
      end                                                               
*---------------------------------------------------------------------- 
C                                                                       
C        **************************************                         
C        * ROUTINE DE TIRAGE ALEATOIRE DU VAX *                         
C        **************************************                         
C                                                                       
      FUNCTION RAN(N)                                                   
      IMPLICIT   NONE                                                   
      REAL*4     RAN                                                    
      INTEGER*4  N                                                      
C                                                                       
      INTEGER*4  ID                                                     
      REAL*4     A                                                      
C                                                                       
      DATA ID/Z'80000000'/                                                
      IF(N.GE.0) THEN                                                   
        N = N*69069+1                                                   
      ELSE                                                              
        N = N+ID                                                        
        N = N*69069+1                                                   
        N = N-ID                                                        
      ENDIF                                                             
      A = N                                                             
      RAN = 0.2328306E-09*A                                             
      IF(N.LT.0) RAN = RAN+1.                                           
      RETURN                                                            
      END                                                               
c                                                                       
c                                                                       
*      call rechez(nc,nm,irap,ichpg,zexp,istat)                         
*----------                                                      -------
*           attention cette routine ne marche qu'apres avoir            
*               fait call inipnch (ian,mois,irun,istatus)               
*----------                                                      -------
*            routine de recherche de numero atomique z                  
*                     dans la carte (r-chpg)                            
*   entree:                                                             
*      nc    :  [integer*4]   no de couronne                            
*      nm    :  [integer*4]   no de module                              
*      irap  :  [integer*4]   valeur de la rapide                       
*      ichpg :  [integer*4]   valeur de la chio_pg                      
*                                                                       
*   retour:                                                             
*      zexp  :  [real*4]      charge calculee                           
*                              (peut valoir 0 sans pb avec istat=0)     
*      istat :  [integer*4]   code retour                               
*                  istat = 0   ok                                       
*                  istat = 1   ok mais zexp > zmax(trace a la main)     
*                  istat = 2   ok mais zexp > zmax(extrapole)           
*                  istat = 3   ok mais rap  > rmax(z) (dernier point de 
*                                             la rapide trace a la main)
*                  istat = 10  ok mais au dessus de la droite limitant  
*                              bragg donc zexp = zmin                   
*                  istat = -1  (carte chio,csi) n'existe pas pour ce ru 
*                  istat = -7  cour < 10 ou cour > 17 (pas de chio-csi) 
* nouveaux codes 2eme campagne
*         	            
*                  istat =  -1  : module non_additionnable pour ce run /
*                                 idlo(nc,nm) = .false.    
*                  istat =  -1 : Zexp > ZZmax:Z calcule non IDentifiable.
*----------                                                      -------
c                                                                       
       subroutine rechez(nc,nm,irap,ichpg,zexp,istat)                   
       implicit none                                                    
       integer*4       nc, nm, istat                                    
       integer*4       irap, ichpg                                      
       real*4          zexp,zzmax(10:17)                                             
c
       external        chio                                             
       real*4          chio                                             
       integer*4       nks                                              
       integer*4       nc_ch, nm_ch                                     
       real*4          prbru, pchpgbru                                  
       real*4          rap, chpg                                        
       logical         cond 
       data nks /7654321/                                               
                                                                        
       real*4          zch, yy1, yy2                                    
       real*4          ran                                              
*---------------------------------------------------------------------- 
c            declaration pour les piedestaux et les "alpha"             
*---------------------------------------------------------------------- 
       integer*4       ncmin_r, ncmax_r, ncmin_l, ncmax_l               
       integer*4       ncal_r, ncxal_r, ncal_l, ncxal_l                 
       integer*4       ncmin_pg, ncmax_pg                               
       real*4          picr(17,24), pialr(17,24,2)                      
c      real*4          picl(17,24), piall(17,24,2)                      
       real*4          picl, piall                                      
       real*4          pic_pg(17,24), piechgg(2:16,24)                  
                                                                        
       common/ piedcsi / picr, ncmin_r, ncmax_r, picl, ncmin_l, ncmax_l 
       common/ pied_ch / pic_pg, ncmin_pg, ncmax_pg, piechgg            
       common/ alphcsi / pialr, ncal_r, ncxal_r, piall, ncal_l, ncxal_l 
                                                                        
*---------------------------------------------------------------------- 
c                     declarations pour le fit                          
c                recuperation des tableaux de l'init                    
*---------------------------------------------------------------------- 
        integer      i                                                  
                                                                        
        integer*4      mxnn                                             
        parameter     (mxnn=9)                                          
        real*4         xi(mxnn,10:17,1:24)                              
        real*4         z0i(10:17,1:24), r0i(10:17,1:24)                 
        integer*4      mmi(10:17,1:24)                                  
        integer*4      max_lin                                          
        parameter     (max_lin=20)                                      
        real*4         r_maxi(max_lin,10:17,1:24)                       
        integer*4      izm_reeli(10:17,1:24), izm_exti(10:17,1:24)      
        common/parfit/ xi, z0i, r0i, izm_exti, r_maxi, izm_reeli, mmi   
                                                                        
        real*4        xinfi(10:17,1:24), yinfi(10:17,1:24)              
        real*4        xsupi(10:17,1:24), ysupi(10:17,1:24)              
        common/bragg2/ xinfi, yinfi, xsupi, ysupi                        
                                                                        
        integer*4       izm_ext, izm_reel                               
        real*4          xinf, yinf, xsup, ysup                          
        real*4          pente, ordal, ybrag                             
*---------------------------------------------------------------------- 
c                     declarations pour le fit                          
c                  pour transmettre a la fonction                       
*---------------------------------------------------------------------- 
       integer*4       mm                                               
       real*4          z0, r0                                           
       real*4          x(mxnn)                                          
       common /chior/  mm, z0, r0, x
C  gestion "runs_additionnables"
C
	logical idlo(10:17,1:24)
	common /run_add/ idlo       
C	
C pour traitement des Zmax
	common /Zmax_ID / zzmax                                         
c                                                                        
*****fin des declarations                                               
*---------------------------------------------------------------------- 
*             correspondance fit <----> fonction theorique              
*---------------------------------------------------------------------- 
       istat = 0                                                        
       zexp = 0.                                                        
       if(nc.lt.10 .or. nc.gt.17 .or. nm.gt.24) then                    
         istat = -7                                                     
         return                                                         
       endif             
C rejet idlo
	if ( .not.idlo(nc,nm) ) then
	Zexp = - 99.
C        istat = - 9
	istat = - 1  !!!!!  on replace le code a une valeur DECODE_PART
	 return
	endif
C                                                      
*---------------------------------------------------------------------- 
*             calcul du module chio                                     
*---------------------------------------------------------------------- 
 111	continue
C 
       nc_ch = nc                                                       
       nm_ch = nm                                                       
       if(nc.ne.13) nc_ch = nc/2*2                                      
       if(nc.eq.13) then                                                
         nm_ch = (nm-1)/3*3+1                                           
       else if(nc.le.15) then                                           
         nm_ch = (nm-1)/2*2+1                                           
       endif                                                            
c                                                                       
       mm       = mmi(nc,nm)                                            
       izm_ext  = izm_exti(nc,nm)                                       
       izm_reel = izm_reeli(nc,nm)                                      
       z0       = z0i(nc,nm)                                            
       r0       = r0i(nc,nm)                                            
                                                                        
       if(izm_reel.eq.0) then                                           
         istat = -1                                                     
         return                                                         
       endif                                                            
       do i = 1, mm                                                     
         x(i) = xi(i,nc,nm)                                             
       enddo                                                            
*---------------------------------------------------------------------- 
*                 passage en coordonnees reduites                       
*---------------------------------------------------------------------- 
*---------------------------------------------------------------------- 
c les lignes si dessous sont commentees pour pouvoir calculer avec le   
c programme  dist.for le spectre  en z a  partir d'un ndim transforme   
c (voir dist.for). au ccpn ils ne sont pas  commentees car on utilise   
c un ndim brut.                                                         
*---------------------------------------------------------------------- 
       prbru    = irap+ran(nks)                                         
       pchpgbru = ichpg+ran(nks)                                        
                                                                        
       rap      = prbru - picr(nc,nm) + 1.                              
       rap      = rap * pialr(nc,nm,1)                                  
       rap      = amax1(rap,0.)                                         
C evenement spurieux si rap < 50. ?
	if (rap .lt. 50. ) rap = 52.
C ??????????????????????????????????
       chpg     = pchpgbru - pic_pg(nc_ch,nm_ch) + 1.                   
       chpg     = amax1(chpg,0.)                                        
                                                                        
c        rap      = irap + ran(nks)     ! commente au ccpn              
c        chpg     = ichpg + ran(nks)    ! commente au ccpn              
*******                                                                 
c  Calcul de la droite limitant le pic de bragg                         
*******                                                                 
       xinf     = xinfi(nc,nm)                                          
       yinf     = yinfi(nc,nm)                                          
       xsup     = xsupi(nc,nm)                                          
       ysup     = ysupi(nc,nm)                                          
                                                                        
       pente    = (ysup - yinf)/(xsup - xinf)                           
       ordal    = ysup - pente*xsup                                     
       ybrag    = pente*rap + ordal                                     
*******                                   
C	                              
***** debut de recherche de z                                           
c                                                                       
         istat  = 0                                                     
         zch    = -0.5                                                  
         yy2    = 0.                                                    
         cond   = .true.                                                
         i = 0                                                          
         dowhile(cond)                                                  
            i = i+1                                                     
            zch  = zch + 1.                                             
            yy1 = yy2                                                   
            yy2 = chio(rap,zch)                                         
            cond = chpg.gt.yy2                                          
C
C
         enddo                                                          
C                                                                       
         if(i.eq.1) then                                                
            zexp = 0.                                                   
         else                                                           
            call resol(zch-1.,yy1, zch,yy2, chpg, chio,rap, 0.01,zexp)
C            
            i = nint(zexp)                                              
            if(i.gt.izm_reel)                  istat = 1                
            if(i.gt.izm_ext)                   istat = 2                
            if(istat.eq.0 .and. i.gt.0) then                            
              if(rap.gt.r_maxi(i,nc,nm))       istat = 3                
            endif                                                       
            if(chpg.ge.ybrag)                  istat = 10               
         endif  
c
c --- Modification 001-98 OL/JLL 26-02-98
c --- Motif : Prise en compte des Z extrapoles
c ---                                                                
C limitation Zmax
C ===============
c	if (Zexp .gt. zzmax(nc) ) then
c	 Zexp = - 1.
C	 istat = -10
c	 istat = -1    !!!!! valeur DECODE_PART
c	 endif
C
c --- Fin de la modification 001-98
c	
       return                                                           
       end 
c                                                                       
      function      chio(xx, zd)                                        
      implicit   none                                                   
      real*4        chio, xx, zd                                        
*-----------------------------------------------------------------------
c            Declaration pour la fonction theorique                     
*-----------------------------------------------------------------------
        integer*4      mxnn                                             
        parameter      (mxnn=9)                                         
        integer*4      mm                                               
        real*4          z0, r0                                          
        real*4         x(mxnn)                                          
        common /chior/  mm, z0, r0, x                                   
c                                                                       
        real*4         a, a1, b, dn, den, xxc, zz, zc, y0, d0           
*-----------------------------------------------------------------------

      xxc = xx-r0                                                       
      zz = zd-z0                                                        
      zz = amax1(zz,0.)   
C                                                    
      zc = sqrt(zz)                                                     
      y0 = (x(2)*zz+x(1))*zz                                            
      d0 = (x(4)*zz+x(3))*zc                                            
      a =  (x(6)*zz+x(5))*zz**2                                         
      b =  x(8)*zz+x(7)                                                 
      a1 = 0.                                                           
      if(y0.ne.0.) a1 = (a+d0)/y0                                       
      dn = y0+a*xxc                                                     
      den = (b*xxc+a1)*xxc+1.  
C                                               
      chio = dn/den+x(mm)                                               
      return                                                            
      end                                                               
c----------------------------------------------------------------------
	Subroutine INI_TABAC_CSI_2_9
	
	REal*4 a11(17,24),a22(17,24),a44(17,24)
	REal*4 az11(17,24),az22(17,24),az44(17,24)
        
        Integer*4 aa1(17,24),aa2(17,24),aa4(17,24)
        REal*4 a1,a2
	Common/tabac1/a11,a22,a44,az11,az22,az44
	Integer*4 xc,xm
	Integer*4 Imodmax(17),Ipas(17)
        DIMENSION pied_R(17,24),pied_L(17,24) 	
        COMMON /PIEton/numsel,pied_R,pied_L
       
	Data Imodmax/12,24,24,24,24,24,24,24,24,24,24,24,24,16,16,8,8/
	Data Ipas/1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1/
	include 'veda_11.incl'
	character*120 file_tabac
        character*120 file_z1
	REal*4 Pied_run(18,24,18),centr_gene(18,24,18)
	Integer*4 erreur,numerun
	
c --- Appel des piedestaux CsI R/L pour le run 1361

        Numrun=1361
        call veda_findp(Numrun,pied_run,centr_gene,erreur) 
	 
	 Do ic=2,17
	    Do im=1,24
	       Pied_r(Ic,im)=Pied_run(Ic,Im,7)
	       Pied_l(Ic,im)=Pied_run(Ic,Im,8)
	    enddo
	 end do
	 
        file_tabac=vedadata(1:long_data)//'par_tabac_csi2_9.dat'
        file_z1=vedadata(1:long_data)//'par1_tabac_csi2_9.dat'


	ifi=LONGUEUR_KALI(file_tabac)
	ifi1=LONGUEUR_KALI(file_z1)
	
	Print *,' '
	Print *,' -----------------------------------------------------'
	Print *,'| Initialisation des nouvelles calibrations CsI 2-9   |'
	print *,'|                                                     |'
	Print *,'|        **** parametrisation Tabacaru ****           |'
	print *,'|                                                     |'
	Print *,'| Y. Larochelle / D. Theriault (U. Laval)  18/10/2000 |'
	Print *,' -----------------------------------------------------'
	Print *,' '
	Print *,'Ouverture des fichier de parametres : '
	Print *,' ===> ',file_tabac(1:ifi)
	Print *,' ===> ',file_z1(1:ifi1)
	print *,' '


c       PLACER LES NOMS DES FICHIERS DE PARAMETRES ICI

	open (unit=27, file=file_tabac, status='old',form='formatted')
       open (unit=28, file=file_z1, status='old',form='formatted')

	do ic=1,17
	   do im=1,24
	      a11(ic,im)=-1.
	      a22(ic,im)=-1.
	      a44(ic,im)=-1.
              az11(ic,im)=-1.
              az22(ic,im)=-1.
              az44(ic,im)=-1.
	       	
	   enddo
	enddo
	
	Ios=0
1	format(2I4,3f10.4)

c  LECTURE DES PARAMETRES TABACARU (CHARGE>1)

4       read(27,1,end=17) xc,xm,a1,a2,a4
	   if(xc.ge.1.and.xc.le.9) then
	      a11(xc,xm)=a1
	      a22(xc,xm)=a2
             a44(xc,xm)=a4
	   end if

        goto 4

17     close(unit=27)

c  LECTURE DES PARAMETRES TABACARU (CHARGE=1)

18    read(28,1,end=19) xc,xm,a1,a2,a4

	   if(xc.ge.1.and.xc.le.9) then
	      az11(xc,xm)=a1
	      az22(xc,xm)=a2
             az44(xc,xm)=a4
	   end if

        goto 18

19     close(unit=28)

	end
c------------------------------------------------------------------------
	Real*4 function tabacaru_2_9(ic,im,hl,iz,ia,iblag)

	REal*4 a11(17,24),a22(17,24),a44(17,24)
	REal*4 az11(17,24),az22(17,24),az44(17,24)

	Common/tabac1/a11,a22,a44,az11,az22,az44

	real*4 hl,terme,t1,t2,T,Ed
	real*4 c,c0,M,baz2,az2,fx,fpx
        real*4 fermi,a1t,a2t,a3t,a4t
        real*4 bi,bi2,bi3,bi4,bi5
	integer*4 iz,ia,iblag,ic,im
	logical*1 cond
	
	iblag=0
	
        niter=0
        eps=1.e-4
       tabacaru_2_9=0.
       	
       IF(hl.le.0.)THEN
        iblag= 5
        RETURN
       END IF
       
       if(ic.lt.2.or.ic.gt.17) then
         iblag=5
         return
       endif
       
       IF (ia.eq.8.and.iz.eq.4) THEN            ! Be8 = 2* He4
          az2=abs(ia*iz*iz/8.)
          fbe = 2.
       ELSE                                       ! tout le reste
          az2=abs(ia*iz*iz)
          fbe = 1.
       END IF
      
     
CCCCCCCCCC
C====================>>>>>>>>> Ce TEST N'EST PAS A LA BONNE PLACE
c	if(a1t.eq.-1.) then
c	   ecsi=0.
c	   iblag=15
c	   return
c	endif

       IF(iz.eq.1) THEN
  
	a1t=az11(ic,im)
	a2t=az22(ic,im)
	a4t=az44(ic,im)

       ELSE 
	a1t=a11(ic,im)
	a2t=a22(ic,im)
	a4t=a44(ic,im)
       ENDIF
c
C  TEST SUR a1t ( = -1  pour le  cour/mod 2  1 ) 
	if(a1t.eq.-1.) then
	   ecsi=0.
	   iblag=15
	   return
	endif
	
       IF(a2t.lt. 1.e-7)THEN
        x = (hl)/a1t
       ELSE
        x = 2000.
        argx = 1.+ x/(a2t*az2)
        cond=.true.
        
        DO while (cond)
          niter=niter+1

	baz2 = a2t*az2
       	T=8.0*ia
	Ed=6.0*ia
	fermi = (1.+exp((-x+Ed)/T))
	c = a4t/fermi
	c0 = a4t/(1.+ exp(Ed/T))	
	t1 = (x + baz2)/baz2
	t2 = (x + baz2)/(baz2+Ed)
	M = -1. * a1t*c0*baz2*log(baz2/(baz2+Ed))
	fx = -hl+fbe*a1t*(x-baz2*log(t1)+
     &	c*baz2*log(t2)) + M
	fpx = a1t*fbe*(1-((a2t*az2)/(x+a2t*az2))+
     &	a4t*a2t*az2*(1/(fermi*(x+a2t*az2))
     &  + (log(t2)*(fermi-1.))/(T*fermi*fermi)))	
          dx = - fx/fpx
          x = x + dx
          argx = 1.+ x /(a2t*az2)
          cond = abs(dx/x).ge.eps.and.argx.gt.0..and.niter.le.50
        END DO
        
        IF (argx.le.0.) THEN
          iblag=13
           RETURN
        ELSE IF(niter.gt.50) THEN
          iblag=14
          RETURN
        ENDIF
        
       ENDIF
      
          tabacaru_2_9=fbe*x
	   
          return
          end
c----------------------------------------------------------------------
	SUBROUTINE LUMIERE(ic,im,icsir,icsil,hl,hr,iblag)

	INTEGER*4 ic,iblag,im,inp
	REAL*4    rp,to,hl,rcsir,rcsil,ecsi,c1,c2,tau,hr,icsir,icsil
       
	REal*4 a11(17,24),a22(17,24),a44(17,24)
	REal*4 az11(17,24),az22(17,24),az44(17,24)

	Common/tabac1/a11,a22,a44,az11,az22,az44
	
        DIMENSION pied_R(17,24),pied_L(17,24) 	
        COMMON /PIEton/numsel,pied_R,pied_L
        DATA        p0/400./, p1/900./, eps/1.e-4/, pre/0.4318/
        LOGICAL     cond,condi
        Integer*4 Iref(17)
	 Data Iref/6,6,6,6,6,6,6,6,6,6,6,7,6,6,6,6,6/
	 
	iblag=1  ! code de retour
	hl=0.
	 
	IF(ic.ge.11.and.ic.le.16) tau=60. ! cste de temps de montee
	IF(ic.lt.11.or.ic.gt.16)  tau=20.
	
c*** rapport des capacites des integrateurs R et L sur la carte
c*** VXI des Csi

	 c1=1.5
	 
        IF (ic.gt.3) THEN
         c2=2.2
        ELSE
         c2=3.3
        END IF
        
c*** signaux bruts corriges du piedestal (fichiers Dapnia)
            
c            PRINT * , ' icsir avant ',icsir     

	     icsir=icsir-pied_R(ic,im)
	     icsil=icsil-pied_L(ic,im)-3
	     rcsir=icsir
c	     rcsil=float(icsil)
             rcsil=icsil
	
c*** quelques precautions
	
        IF(rcsil.le.0.or.rcsir.le.-5.) THEN
        iblag=15
        RETURN	
	END IF
	
	
	IF(rcsir.le.0.) rcsir=0.1
	
	rp=(rcsil/rcsir)*(c1/c2)
	
c*** recherche de la quantite de lumiere totale	


 	niter=0
     
        IF(rp.ge.1.0.and.rcsir.le.10.)THEN
        
        x = 600.
        x2 = exp(-1590./x)
        x3 = exp(-3090./x)
        bx = x2 - x3
        
        ELSE
        
        x=p0+p1*rp
        x = x - 100.
        at=1.-exp(-390./tau)
        at = tau*at
        condi=.true.
        iblag=4
        
        DO while (condi)
          niter = niter + 1
          xx=x*x
          x1 = exp(-390./x)
          x2 = exp(-1590./x)
          x3 = exp(-3090./x)
          ax = 1.-x1
          ax = ax-at/x
          bx = x2 - x3
          fx=bx/ax-rp
          apx = -390.*x1
          apx = (apx+at)/xx
          bpx = 1590.*x2-3090.*x3
          bpx = bpx/xx
          fpx=(bpx*ax-apx*bx)/ax**2
          dx = - fx/fpx
          x=x+dx
          condi=abs(dx/x).ge.eps.and.niter.le.50
        END DO
        
          IF(niter.gt.50.or.x.le.tau) THEN
          iblag=12
          RETURN
          ENDIF
        
        ENDIF
        
c*** calcul de h sans correction de gain PM
       
        to=x
        hl=rcsil*(x-tau)/(x*bx)
        
c --- Recalage sur le module de reference

	 hr=hl*a11(ic,iref(ic))/a11(Ic,Im)
c      PRINT * , ' hl ds lumiere ',hl     

        return
        
        end
c-------------------------------------------------------------------
c Fonction longueur
c=============================================
      function longueur_kali(chaine)
      integer*4 longueur
      integer*4 lc
      character*(*) chaine
c
      lc=len(chaine)
      do while(chaine(lc:lc).eq.' '.and.lc.gt.0)
       lc=lc-1
      end do
      longueur_kali=lc
c
      return
      end
C=======================================================================
      FUNCTION AMASS(Z)                                                 
C=======================================================================
c    MASSE D UN FRAGMENT EN FONCTION DE SA CHARGE                       
c    EXTRAPOLATION A PARTIR DE LA TABLE PERODIQUE DES ELEMENTS          
C=======================================================================
      ZZ=nint(z)
      AMASS=nint(1.867*ZZ+.016*ZZ**2-1.07E-4*ZZ**3)                              
      IF(ZZ.le.3.)AMASS=2.*ZZ                                           
      IF(ZZ.LE.1.)AMASS=ZZ                                              
      RETURN                                                            
      END                                                               
C=======================================================================
c--------------------------------------------------------------         

c --- Lot de routines pour la lecture des piedestaux                    
 
c-----------------------------------------------------------------------
c     sous-programme pour trouver les piedestaux par type et run        
c      Liste des type de parametres :                                   
c       PHOS_R = 1                                                      
c       PHOS_L = 2                                                      
c       CI_GG = 3                                                       
c       CI_PG = 4                                                       
c       SI_GG = 5                                                       
c       SI_PG = 6                                                       
c       CSI_R = 7                                                       
c       CSCSI_L = 8                                                     
c       SI75_PG = 9                                                     
c       SI75_GG =10                                                     
c       SISILI_PG =11                                                   
c       SILI_GG =12                                                     
C                                                                       
C        LES COMMENTAIRES SONT ECRITS SUR L'UNITE NUM_WRITE             
C                                                                       
C       L'UNITE DE LECTURE DES FICHIERS EST 26                          
c                                                                       
c       Explication des Erreurs :                                       
c         Erreur = 0 OK                                                 
c         Erreur = -10 Le run REF n'existe pas                          
c         Erreur = -11 Erreur d'ouverture du fichier de REF             
c         Erreur = -12 Le Run REF pas trouve dans le fichier ad hoc     
c         Erreur = -20 Le run RUN n'existe pas                          
c         Erreur = -21 Erreur d'ouverture du fichier de RUN             
c         Erreur = -22 Le Run RUN pas trouve dans le fichier ad hoc     
C                                                                       
c       Le fichier REF n'est lu qu'une fois au premier appel            
c         Modification 23.03.94 A. Chbihi :                             
c        *pied_run(18,24,12) au lieu de pied_run(17,24,12)              
c-----------------------------------------------------------------------
      subroutine veda_findp(run,pied_run,centr_gene,erreur) 
                  
      implicit none                                                     
      include  'veda_11.incl'
                                                                        
c     Parameter Num_Fich=20                                             
                                                                        
      real*4 pied_run(18,24,18)                                         
                                                                        
      real*4 centre_pin,centr_gene(18,24,18)                            
                                                                        
      integer*4 Run,ir,it                                               
      integer*4 i,type_min,type_max,num_write,erreur                    
      integer*4 cour,mod,type,cour_min,cour_max                         
      Integer*2 itab(4)
      character*20 nomrun                                              
      Character*4  crun
      Real*4    xrun,base,xx,base2
 
      logical test,lecture_ref                                          
                                                                        
      data lecture_ref/.false./                    
      integer*4 Longueur_kali
      external longueur_kali                     
                                                                        
c-----------------------------------------------------------------------
c          Lecture du fichier                                           
c-----------------------------------------------------------------------
                                                                        
c------------- Recherche du nom du fichier                              
 
      xrun=float(run)
      
20    continue

      Erreur=0                                                          
      do i=0,3
         base=10.**i
         base2=10.**(i+1)
         xx=int(xrun/base)-10.*int(xrun/base2)
         ir=nint(xx)
         itab(i+1)=ir
      end do
      crun=' '
      do i=1,4
         it=itab(5-i)
         crun(i:i)=CHAR(it)
      end do
      write(crun,'(4I1)') (itab(5-i),i=1,4)
      nomrun='run'//crun//'g.data'                                   
 
1     format(1x,'Lecture du fichier Piedestaux :',a6)
2     format(1x,i2,':',a1)
 
c------------ Lecture du fichier                                        
                                                                        
      call veda_gene(nomrun,run,centr_gene,centre_pin,                  
     # pied_run,erreur)                                                 

       if(erreur.eq.-2) then
          Print *,'On scanne le run suivant ...'
          xrun=xrun+1.
          if(xrun.gt.1600.) then
             print *,'Aucun fichier piedestal pour le run ',xrun
             go to 15
          endif
          go to 20
       endif
                                                                        
c      lecture_ref=.true.                                               
c-----------------------------------------------------------------------
c          Fin de la Lecture du Run Reference                           
c-----------------------------------------------------------------------
15    continue                                                          

c --- Recuperation du nom du bon fichier piedestaux

      nompied=nomrun
      long_pied=LONGUEUR_KALI(nomrun)
       
      RETURN                                                            
      END                                                               
c ---------------------------------------------------------------------
      subroutine veda_gene(nomfich,Run0,centr_gene,centre_pin,          
     # pied_run,erreur)                                                 
                                                                        
                                                                        
      implicit none                                                     
      include  'veda_11.incl'
      Character*80  nomfil
                                                                        
      integer*4 cour,mod,type,run,run0,i,label,j,irc,icode              
      integer*4 pied,num_write,erreur,nl                                   
      real*4    centr_gene(18,24,18),pied_run(18,24,18)                 
      real*4 centre_pin,sigma,skew,somme,centre,xpied                   
                                                                        
      character*(*) nomfich                                             
      character*90 text                                                 
      character*10 nom                                                  
      Integer*4 runlu,nbllu                                             
      logical Trouve,trouve_pied,last                                   
                                                                        
      erreur=0                                                          
      num_write=6                                                       
c-------------------------------------------------------------------    
c      Ouverture du fichier                                             
c-------------------------------------------------------------------    
      write(num_write,'(/,'' Lecture du fichier : '',a20)')nomfich      
 
      nomfil='/afs/in2p3.fr/home/throng/indra/veda2/gene/'//nomfich
      open (unit=60,file=nomfil,status='OLD',iostat=icode)

      if(icode.ne.0) then
         Print *,'fichier ',nomfich,' inexistant ...'
         trouve=.False.
         go to 11
      endif         
                                                                              
      Trouve=.False.                                                    
      Do j = 1,1       ! Debut d'un nouveau Run                         
         read(60,12)text                                                
      End do                                                            
12    format(a82)
 
c --- Lecture de l'entete d'un run                                      
                                                                        
98    Continue                                                          
                                                                        
c --- Inutile de continuer a lire si le run RUN0 est trouve !           
                                                                        
      if(trouve)goto 97                                                 
                                                                        
      Do j = 1,10                                                       
         read(60,12,end=97)text                                         
      End do                                                            
      centre_pin=0.                                                     
      nl=0
                                                                               
c --- Ligne suivante SVP...                                             
                                                                        
10    Continue                                                          
                                                                        
c --- Lecture de la ligne avec le format 100                            
                                                                        
      read(60,100,end=97,err=98)                                        
     # run,label,cour,mod,type,nom,somme,centre,sigma,skew,xpied,skew   
c     print *,cour,mod,xpied,skew
      nl=nl+1
                                                                        
100   format(1x,2I5,3I3,1X,A10,F8.0,F9.2,2F9.3,f8.2,f4.0)               
                                                                        
c --- Changement de run                                                 
                                                                        
      If (run.eq.0.and.label.eq.0) Go to 98                             
                                                                        
c --- On continue a lire s'il ne s'agit pas du bon run...               
                                                                        
      if (run.gt.run0) go to 11                                         
      if (run.ne.run0) Go to 10                                         
      go to 13                                                          
                                                                        
c --- Si on ne trouve le run, prendre le suivant...                     
                                                                        
11    continue                                                          
                                                                        
      Print *,'Le run ',run0,' n''existe pas dans la base de donnees'   
      print *,'Par consequent, on prend le run (suivant):',run          
      run0=run                                                          
                                                                        
c --- Remplissage des tableaux Piedestaux/GENE                          
                                                                        
13    continue                                                          
                                                                        
      trouve=.True.                                                     
      centr_gene(cour,mod,type)=centre                                  
      pied_run(cour,mod,type)=xpied                                     
      Go to 10                                                          
                                                                        
c --- Fin de lecture du fichier Piedestaux/GENE                         
                                                                        
97    continue                                                          
      close(unit=60)                                                    
                                                                        
c --- Messages d'information                                            
                                                                        
      if(trouve)then                                                    
         write(num_write,'('' Le Run '',i4,''  a ete trouve'')')run0  
         print *,'Nombre de lignes lues :',nl
         if(nl.lt.1280) then
             Print *,'Le fichier n''a pas la longueur nominale !'
             Print *,'Les piedestaux ne doivent pas tous etre charges '
             Print *,'Par consequent, on passe au run suivant ...'
             erreur=-2
             return
         endif   
         erreur=0                                                       
         return                                                         
      else                                                              
                                                                        
c --- Le run n'a pas ete trouve...                                      
                                                                        
         write(num_write,'('' Le Run '',i4,''  n a pas ete trouve'')     
     &   ')run0                                                         
         erreur=-2                                                      
         return                                                         
      endif                                                             
                                                                        
c --- Erreur de lecture !                                               
                                                                        
95    erreur=-1                                                         
      return                                                            
      end                                                               
c--------------------------------------------------------------------------	
	subroutine chio_kali(ic,im,zz,csir,csil,cigg,cipg,irc)
	
	real*4 zz,csir,csil,cigg,cipg
	integer*4 ic,im,irc,ill,Nrun
	
	ill=ill+1
	Nrun=-9999
	
	if (ill.eq.1) call INI_GET_CHIO(Nrun,irc)
	
	irc=-1
	
	call GET_CHIO(Ic,im,zz,csir,csil,cigg,cipg,irc)
     
        return
	end
c--------------------------------------------------------------------------	
	subroutine ini_chio_e
	
	include 'veda_11.incl'
	character*120 fich,ligne
	integer*4 ic,im,it
	
	real*4 base_indra(17,24,2,7)
	common/babase/base_indra
c ---                                                                   
8      format(6(/),a10,5(/),a10)
10     format(6(/),i4,4(/))                                              
14     format(17x,2i3,i4,1x,4(e13.6),f8.3,1x,f8.3,2x,f5.3)               
18     format(13(/))                                                     
22     format(3x,i4,7x,i4,5x,i2,4x,i2,3x,i1,3x,f4.2)                     
c ---                                                                   
       fich=vedadata(1:long_data)//'chio_E.dat'
       open(36,file=fich,status='old',err=100)                           
       fich=vedadata(1:long_data)//'corrphys.vedasac'
C a verifier le corrphys.vedasac pour les chios malades
       open(37,file=fich,status='old',err=100)                           
	
       icod=0
       nbparam=2100
       do i=1,13
         read(36,'(a)') ligne
       end do
       read(37,18)                                                       
c ---
       nl=0
       do i=1,nbparam                                                    
          read(36,14,end=31)ic,im,it,                              
     &        (base_indra(ic,im,it,k),k=1,6)                
          base_indra(ic,im,it,7)=1.
          nl=nl+1
c         write(6,14)icour,imod,ityp,                                    
c     &   (base_indra(icour,imod,ityp,k),k=1,7)              
        enddo                                                             
                                                                        
31      Print *,nl,' parametres Chio lus ...'
	
        close(36)                                                         
        close(37)  
	
	call REMPLIT_BASE_LPC
       
	return

100	print *,'Dans ini_chio_e : ereur a l''ouverture !'
	stop
	
	end
c-----------------------------------------------------------------------
	real*4 function e_chio_kali(ic,im,cigg,cipg)
	
	real*4 can,can_volt
	
	include 'id_chiosi_lpc.incl'
	real*4 base_indra(17,24,2,7)
	common/babase/base_indra
	
	e_chio_kali=0.
	
	ill=ill+1
	if(ill.eq.1) call ini_chio_e()
	
        call find_mod_plage(ic,im,icg,img)
	
	if(cigg.lt.3800.) then
 	  can=mini_base(ic,im,1,6)*cigg+mini_base(ic,im,1,5)
	else
	  can=cipg	   
	endif
	
        can_volt=0.                                                       
        can_volt=base_indra(icg,img,2,1)*can**2+base_indra(icg,img,2,2)
     &           *can+base_indra(icg,img,2,3)          
      
        E_chio_kali=can_volt*base_indra(icg,img,2,4)                     
	
	return
	end
c=============================================                          
c================================================================       
c Sous routine permettant de trouver le module minimum a partir         
c d'un numero de module                                                 
c================================================================       
      subroutine find_mod_plage(ic,im,ic0,im0)                          
 
      include 'veda_1.incl'                                             
      include 'veda_2.incl'                                             
 
      if(ic.eq.1) then
        im0=im
        ic0=ic
        return
      end if
      if(ic.eq.2.and.(mod(im,2).eq.0)) then                             
       im0=0                                                            
       ic0=0                                                            
       return                                                           
      endif                                                             
      ic0=0                                                             
      im0=0                                                             
      ic0=ic-mod(ic,dcc(ic))                                            
      im0=(im-1)/dmm(ic)                                                
      im0=im0*dmm(ic)+1                                                 
      return                                                            
      end                                                               
c------------------------------------------------------------------     
c-----------------------------------------------------------------------
c
c                       ID ChIo - Si Version 2.2
c
c
c	Fichier servant a l'identification ChIo-Si systemes Lourds (Ta/U)
c	ATTENTION : Valable uniquement pour 2e campagne !
c	
c      Derniere mise a jour : le 1/12/97 par O. LOPEZ (V 2.2)
c
c-----------------------------------------------------------------------
c	Historique des versions :
c
c
c		1.0 - (10/05/97)  : Mise en place du fichier id_chiosi_lpc.f 
c                                 dans la zone $THRONG_DIR/veda2/for2.
c		                    
c		1.1 - (23/07/97)  : Modification de l'include associe et 
c		                    correction de certains codes de retour.
c		
c		1.2 - (25/07/97)  : Correction d'un bug pour certaines
c		                    couronnes (parametres mal charges ...)
c		                    
c		1.3 - (02/09/97)  : Modification des parametres de fit pour
c				      les couronnes 6-9.
c				      
c		2.0 - (28/09/97)  : Mise en place des bornes (rebroussement
c		                    et Bragg).
c		                    
c		2.1 - (15/11/97)  : correction d'un bug sur la determination
c		                    des zones de rebroussement et Bragg.
c		                    
c		2.2 - (20/11/97)  : mise en place de la fonction GET_CHIO
c				      permettant le calcul de la contribution
c				      ChIo correspondant a une particule en arret
c				      dans le CsI.
c				      
c-----------------------------------------------------------------------
c Description des differentes routines :
c
c      - INI_CHIO_SI_LPC : routine d'initialisation de l'ID Chio-Si
c      Elle doit etre appelee en DEBUT de traitement d'un run par :
c
c      Call INI_CHIO_SI_LPC(Nrun,Imois,Iannee,Irc)
c
c      ou :
c
c      - Nrun (Integer*4) : Numero du run courant
c      - Imois (Integer*4) : Numero du mois du run (fourni par veda_5.in
c      - Iannee (Integer*4) : Annee du run (fourni par veda_5.incl)
c      
c      * Irc (Integer*4) : Code de retour fourni par la routine.
c
c      Avec :
c
c      Irc =  0 : Initialisation OK.
c      Irc = -1 : Initialisation non correcte (manque d'un ou plusieurs
c      fichiers de parametres).
c
c-----------------------------------------------------------------------
c      - ID_CHIOSI_LPC : Routine d'identification ChIo-SI systeme LOURDs
c      uniquement (faisceaux de Tanatale et d'Uranium).
c      Elle doit etre appelee evt/evt par :
c
c      Call ID_CHIOSI_LPC(Ic,Im,Chg,Chp,Cht,Sig,Sip,Sit,Csr,Csl,zed,
c     &  Chio,Si,Icode)
c
c      ou :
c
c      - Ic, Im (Integer*4) : numeros de couronne / module
c      - Chg, Chp, Cht (Real*4) : signaux ChIo GG, PG et MT respectiveme
c      - Sig, Sip, Sit (Real*4) : signaux Sil. GG, PG et MT respectiveme
c      - Csr, Csl (Real*4) : signaux CsI rapide et lent respectivement.
c
c      * Zed (Real*4) : Charge mesuree et retournee par la routine
c      * Chio (Real*4) : signal ChIo RECALE par couronne.
c      * Si   (Real*4) : signal Sil. RECALE par couronne.
c      * Icode (Integer*4) : Code de retour de l'identification.
c
c      Avec :
c
c      Charge retournee quand :
c      
c      Icode = 0  : Identification correcte (DZ<=1)
c      Icode = 1  : Identification correcte mais charge estimee (DZ=2)
c      Icode = 2  : Identification correcte mais Charge extrapolee (DZ=2
c      Icode = 3 : Identification dans le pic de Bragg (grosse incertit
c      sur la charge mesuree, DZ=1-20!).
c      
c      Pas de charge retournee quand :
c      
c      Icode = 14 : Rebroussement dans le Silicium; pas d'identification
c      possible. Il faut regarder dans la carte Si_CsI.
c      Icode = 15 : Parametre d'entree non valide (couronne /module).
c
c-----------------------------------------------------------------------
c      - GET_CHIO : routine de calcul
c	de la contribution ChIo a partir du signal CsIR et du Z de la 
c	particule. Cette routine est appelee par l'algorithme de coherence.
c	
c	Call GET_CHIO(Ic,Im,Z,Csir,Csil,Chiogg,Chiopg,Irc)
c	
c	ou on donne en entree :
c	
c	- Ic    (I*4) : Numero de couronne
c	- Im    (I*4) : Numero de module
c	- Z     (R*4) : Charge de la particule
c	- CsiR  (R*4) : signal CsI rapide en canaux
c	- CsiL  (R*4) : signal CsI lent en canaux
c	
c	En sortie, on obtient :
c	
c	- Chiogg (R*4) : signal ChIo grand gain en canaux bruts.
c	- Chiopg (R*4) : signal ChIo petit gain en canaux bruts.
c	- Irc    (I*4) : Code de retour
c	
c			= 0  : Estimation correcte.
c			= -1 : Z non pris en compte par l'estimation.
c			= -2 : Pas de parametres estimes pour ce module.
c			
c----------------------------------------------------------------------------			
c      En cas de probleme, contacter  :
c
c      O. Lopez  (LPC Caen) : lopezo@in2p3.fr    /    Tel : 02-31-45-29-62
c---------------------------------------------------------------------------
	Subroutine INI_CHIO_SI_LPC(Nrun,Imois,Iannee,Irc)
	
	if(nrun.lt.1000) then
	   Print *,'INI_CHIO_SI_LPC :'
	   Print *,'Numero de run invalide (<1000) !!'
	   Print *,'L''initialisation a echoue ...'
	   Print *,'Aucune ID ChIo-Si n''est donc possible !'
	   Irc=-1
	endif
	
	Print *,'     '
	print *,'         ---------------------------------'
	Print *,'        |  Initialisation IDentification  |'
	Print *,'        |    ChIo - Si Couronne 2 a 9     |'
	Print *,'        |               -oOo-             |'
       Print *,'        | Version 2.2     1 Decembre 1997 |'
	Print *,'        |                                 |'
	Print *,'        | Auteurs :                       |'
	Print *,'        |                                 |'
	Print *,'        | D. Cussol, J. Colin, O. Lopez   |'
	Print *,'        |           (LPC Caen)            |'
	print *,'         ---------------------------------'
	Print *,'     '
	
	Irc=0
	Print *,'INI_CHIO_SI_LPC : Init. Base de donnees recalage'
       Call REMPLIT_BASE_LPC

	Print *,'INI_CHIO_SI_LPC : Init. parametres ID'
       Call INI_COEFF(Nrun,Irc)
      
	Print *,'INI_CHIO_SI_LPC : Init. zones ID'
       Call GET_BORNES_LPC
	
	if(irc.ne.0) then
	   Print *,'INI_CHIO_SI_LPC : ******* ERREUR *******'
	   Print *,'L''initialisation a echoue ...'
	   Print *,'Aucune ID ChIo-Si n''est donc possible !'
	   Irc=-1
	else
	   Print *,'INI_CHIO_SI_LPC : Initialisation correcte'
	endif
	
	Print *,'INI_CHIO_SI_LPC : Init. coherence ChIo'
	Call INI_GET_CHIO(Nrun,Irc)
	
	Return
	End
c---------------------------------------------------------------------------
	Subroutine INI_GET_CHIO(Nrun,Irc)
	
	include 'id_chiosi_lpc.incl'
	Character*80 Nom
	Real*4 par(14)
	
	Irc=0
       
       Nom='/afs/in2p3.fr/home/throng/indra/veda2/data/'
     &     //'coherence_chio.data'
       Long=LONGUEUR(Nom)
       
	Open(unit=lun,file=nom(1:long),form='FORMATTED',Status='OLD')
	
	Ios=0
	Do while(ios.eq.0)
          Read(Lun,1,Err=10,Iostat=ios) ic,im,iz0,iz1,(par(i),i=1,14)
          if(ic.gt.0) then
            if(im.eq.0) then
	      Do im=1,24
                 Z_chiocsi_min(Ic,im)=iz0
                 Z_chiocsi_max(Ic,im)=iz1
	         do i=1,14
	            Coeff_chiocsi(ic,im,i)=par(i)
	         enddo
               End do
             else
               if(im.ge.1.and.im.le.24) then
                  do i=1,14
	              Coeff_chiocsi(ic,im,i)=par(i)
	           enddo
                  Z_chiocsi_min(Ic,im)=iz0
                  Z_chiocsi_max(Ic,im)=iz1
               endif
             endif
	   endif
	end do
	
	close(unit=lun)

c --- Deuxieme fichier (pour Z compris entre 1 et 10 )
        
       Nom='/afs/in2p3.fr/home/throng/indra/veda2/data/'
     &   //'coherence_chio_2.data'
       Long=LONGUEUR(Nom)
       
	Open(unit=lun,file=nom(1:long),form='FORMATTED',Status='OLD')
	
	Ios=0
	Do while(ios.eq.0)
          Read(Lun,1,Err=10,Iostat=ios) ic,im,iz0,iz1,(par(i),i=1,14)
          if(ic.gt.0) then
            if(im.eq.0) then
	      Do im=1,24
                Z_chiocsi_min_2(Ic,im)=iz0
                Z_chiocsi_max_2(Ic,im)=iz1
	         do i=1,14
                   Coeff_chiocsi_2(ic,im,i)=par(i)
	         enddo
               End do
             else
               if(im.ge.1.and.im.le.24) then
                  do i=1,14
                    Coeff_chiocsi_2(ic,im,i)=par(i)
	           enddo
                   Z_chiocsi_min_2(Ic,im)=iz0
                   Z_chiocsi_max_2(Ic,im)=iz1
               endif
             endif
	   endif
	end do
	
	close(unit=lun)
1	Format(1x,4(i4,1x),14(e15.6))

	Return
	
10     Print *,'Erreur a la lecture du fichier ',nom
	Irc=-1
	
	End
	
c---------------------------------------------------------------------------	 
	Subroutine GET_Bornes_LPC
	
	Character*80 Nom
	Logical*1    EoF
	Dimension xb(10),yb(10)
	Include 'id_chiosi_lpc.incl'
	
        Nom='/afs/in2p3.fr/home/throng/indra/veda2/data/'
     &     //'bornes_chiosi_lpc.data'
	
	Inom=LONGUEUR(Nom)
	
	Print *,'Lecture des bornes (Bragg-rebroussement) ...'
	Print *,'Ouverture du fichier ',nom(1:inom)
	
	OPEN(unit=lun,file=nom(1:inom),form='FORMATTED',status='OLD')
	Do while(.not.EOF)
	   Read(lun,1,end=10) Ic,Ityp,Np,(xb(i),yb(i),i=1,10)
	   if(ic.ge.2.and.ic.le.9) then
	      Do i=1,Np
	         Xbornes(Ic,Ityp,i)=xb(i)
	         Ybornes(Ic,Ityp,i)=yb(i)
	      end do
	      Npoints(Ic,Ityp)=Np
	   else
	      EOF=.True.
	   endif
	end do
	
10	Continue
       Close(unit=lun)
1      Format(1x,i2,i2,i3,':',24(f6.0))
     
       Print *,'Chargement des bornes effectue ...'
       
       Return
       End
c---------------------------------------------------------------------------
	Integer*4 Function IBORNES(x,y,Ic)
	
	Include 'id_chiosi_lpc.incl'
	Dimension Np(3),Ylim_reb(3)
       Real*8 X_bragg
       Common/BRAGGET/x_bragg
       	
	IBornes=0
	
c --- Est-on dans la zone de Bragg ?
       
       Np(1)=Npoints(ic,1)
       if(np(1).lt.2) then
          IBornes=-1
          Return
       endif
       
       xmax=0.
       Do i=1,Np(1)
          if(xbornes(ic,1,i).gt.xmax) xmax=xbornes(Ic,1,i)
       end do
       
       if(x.le.xbornes(ic,1,1)) then
          a=ybornes(ic,1,1)/xbornes(ic,1,1)
          ylim_bragg=a*x
          x_bragg=abs(y/a)
          index=0
       elseif(x.gt.xmax) then
          ylim_bragg=-99999.
          index=np(1)+1
          x_bragg=abs(xmax)
       else
          index=1
          Do while(x.gt.xbornes(ic,1,index))
             index=index+1
          end do
          i=index-1
          a=(ybornes(ic,1,i)-ybornes(ic,1,i+1))/
     &      (xbornes(ic,1,i)-xbornes(ic,1,i+1))
          b=ybornes(ic,1,i)-a*xbornes(ic,1,i)
          ylim_bragg=a*x+b
          if(a.ne.0.) then
             x_bragg=abs((y-b)/a)
          else
             x_bragg=0.
          endif
       endif
       
       if(y.gt.ylim_bragg.and.x.le.xmax) then
          IBornes=1
          Return
       endif
       
c --- Est-0n dans la zone de rebroussement ?

      Np(2)=Npoints(Ic,2)
      Np(3)=Npoints(Ic,3)
      
      if(np(3).lt.2.or.np(2).lt.2) then
         IBornes=-1
         Return
      endif
      Ylim_reb(2)=0.
      Ylim_reb(3)=0.
      Do ityp=2,3
         if(x.lt.xbornes(Ic,Ityp,1)) then
            a=ybornes(ic,Ityp,1)/xbornes(ic,Ityp,1)
            ylim_reb(Ityp)=a*x
          elseif(x.gt.xbornes(ic,Ityp,np(Ityp))) then
             a=(ybornes(ic,Ityp,np(ityp))-ybornes(ic,Ityp,np(ityp)-1))
     &        /(xbornes(ic,Ityp,np(ityp))-xbornes(ic,Ityp,np(ityp)-1))
             b=ybornes(ic,Ityp,np(ityp))-a*xbornes(ic,Ityp,np(ityp))
             ylim_reb(Ityp)=a*x+b
          else
             index=1
             Do while(x.gt.xbornes(ic,ityp,index))
                index=index+1
             end do
             i=index-1
             a=(ybornes(ic,Ityp,i)-ybornes(ic,ityp,i+1))/
     &         (xbornes(ic,Ityp,i)-xbornes(ic,Ityp,i+1))
             b=ybornes(ic,Ityp,i)-a*xbornes(ic,Ityp,i)
             ylim_reb(Ityp)=a*x+b
          endif
       end do
       
      if(ylim_reb(2).lt.ylim_reb(3)) then
         yt=ylim_reb(2)
         ylim_reb(2)=ylim_reb(3)
         ylim_reb(3)=yt
      endif
      
       if(y.le.ylim_reb(3)) then
          IBornes=3
       elseif(y.gt.ylim_reb(3).and.y.le.ylim_reb(2)) then
          IBornes=2
       elseif(y.gt.ylim_reb(2)) then
          IBornes=0
       else
          IBornes=-1
       endif
       
       Return     
       End
c---------------------------------------------------------------------------	
	Subroutine REMPLIT_BASE_LPC
	
	Include 'id_chiosi_lpc.incl'
	
        Character*100 fic1,fic2,label
	Logical*1    EoF
	Integer*4 Il1,Il2

c --- RAZ de la base de donnees

      Do ic=2,9
         Do im=1,24
            Do ityp=1,9
               do Ifonc=1,6
                  Mini_base(Ic,Im,Ityp,Ifonc)=0.
               end do
            end do
         end do
      End do
	
c --- Definition du repertoire de recalage	
	
      Path='/afs/in2p3.fr/home/throng/indra/veda2/data/'
       Ip=LONGUEUR(Path)
       
c --- Definitions du fichier de recalage ...

c	fic1=path(1:ip)//'coef_recal_chiosi.data'
	fic1=path(1:ip)//'coef_recal_chiosi_lpc.data'
c	fic1=path(1:ip)//'coef_recal_chiosi_elas.essai'

c --- Definitions du fichier de conversion GG --> PG (ChIo/Si)

	fic2=path(1:ip)//'conv_ggpg_chiosi.data'
	
c --- Ouverture du fichier recalage ChIo, Si, CsIr

	il1=LONGUEUR(Fic1)
	
	OPEN(unit=Lun,file=fic1(1:il1),form='FORMATTED',STATUS='OLD')
	Print *,'Lecture recalage ChIo_PG/Si_PG/CsI_r :'
	Print *,' -->',fic1(1:il1)
	
	EOF=.False.
	Do while(.not.EOF)
	   Read(lun,1,end=10) label,a1,a2,a3,a4
	   Call DECODE_LABEL(label,ic,im,ityp)
	   if(ityp.eq.2) then
	      if(ic.ge.3.and.ic.le.9) then
	         Mini_base(Ic,Im,ityp,1)=a1
	         Mini_base(Ic,Im,ityp,2)=a2
	         Mini_base(Ic,Im,ityp,3)=a3
	         Mini_base(Ic,Im,ityp,4)=a4
	         Mini_base(Ic+1,Im,ityp,1)=a1
	         Mini_base(Ic+1,Im,ityp,2)=a2
	         Mini_base(Ic+1,Im,ityp,3)=a3
	         Mini_base(Ic+1,Im,ityp,4)=a4
	         Mini_base(Ic,Im+1,ityp,1)=a1
	         Mini_base(Ic,Im+1,ityp,2)=a2
	         Mini_base(Ic,Im+1,ityp,3)=a3
	         Mini_base(Ic,Im+1,ityp,4)=a4
	         Mini_base(Ic+1,Im+1,ityp,1)=a1
	         Mini_base(Ic+1,Im+1,ityp,2)=a2
	         Mini_base(Ic+1,Im+1,ityp,3)=a3
	         Mini_base(Ic+1,Im+1,ityp,4)=a4
	      else
	         Mini_base(Ic,Im,ityp,1)=a1
	         Mini_base(Ic,Im,ityp,2)=a2
	         Mini_base(Ic,Im,ityp,3)=a3
	         Mini_base(Ic,Im,ityp,4)=a4
	         Mini_base(Ic+1,Im,ityp,1)=a1
	         Mini_base(Ic+1,Im,ityp,2)=a2
	         Mini_base(Ic+1,Im,ityp,3)=a3
	         Mini_base(Ic+1,Im,ityp,4)=a4
	         Mini_base(Ic+1,Im+1,ityp,1)=a1
	         Mini_base(Ic+1,Im+1,ityp,2)=a2
	         Mini_base(Ic+1,Im+1,ityp,3)=a3
	         Mini_base(Ic+1,Im+1,ityp,4)=a4
	      endif
	   else
	      Mini_base(Ic,Im,ityp,1)=a1
	      Mini_base(Ic,Im,ityp,2)=a2
	      Mini_base(Ic,Im,ityp,3)=a3
	      Mini_base(Ic,Im,ityp,4)=a4
	   endif
	end do
	
10     close(unit=lun)
	
3      Format(1x,3(i2),1x,6(g10.4))
       
c --- Ouverture du fichier de conversion GG -> PG (ChIo/Si)

	il2=LONGUEUR(Fic2)
	
	OPEN(unit=lun,file=fic2(1:il2),form='FORMATTED',STATUS='OLD')
	Print *,'Lecture conversion GG --> PG pour ChIo/Si :'
	Print *,' --> ',fic2(1:il2)
	
	EOF=.False.
	Do while(.not.EOF)
	   Read(lun,2,end=40) label,a5,a6
	   Call DECODE_LABEL(label,ic,im,ityp)
	   if(ityp.eq.1) then
	      ic1=ic+1
              im1=im+1
              if(ic.ge.2.and.ic.le.9) then
	         if(ic.eq.2) then
	         Mini_base(Ic,Im,ityp,5)=a5
	         Mini_base(Ic,Im,ityp,6)=a6
	         Mini_base(Ic1,Im,ityp,5)=a5
	         Mini_base(Ic1,Im,ityp,6)=a6
	         Mini_base(Ic1,Im1,ityp,5)=a5
	         Mini_base(Ic1,Im1,ityp,6)=a6
                 elseif(mod(ic,2).eq.0) then
	         Mini_base(Ic,Im,ityp,5)=a5
	         Mini_base(Ic,Im,ityp,6)=a6
	         Mini_base(Ic,Im1,ityp,5)=a5
	         Mini_base(Ic,Im1,ityp,6)=a6
	         Mini_base(Ic1,Im,ityp,5)=a5
	         Mini_base(Ic1,Im,ityp,6)=a6
	         Mini_base(Ic1,Im1,ityp,5)=a5
	         Mini_base(Ic1,Im1,ityp,6)=a6
                 endif
              else
	         Mini_base(Ic,Im,ityp,5)=a5
	         Mini_base(Ic,Im,ityp,6)=a6
              endif
	   elseif(ityp.eq.4) then
	      Mini_base(Ic,Im,ityp,5)=a5
	      Mini_base(Ic,Im,ityp,6)=a6
	   endif
	end do
	
40     close(unit=lun)

1      Format(a6,f12.4,4x,g12.4,f12.4,4x,f12.4)
2      Format(a6,1x,2(g15.8))

       Return
       End

c--------------------------------------------------------------------
	Subroutine DECODE_LABEL(label,Ic,Im,Ityp)
	 
	 Character*(*) label
	   
	 Ii=0
	 Do i=2,6
	    Ich=ICHAR(label(i:i))
	    Ii=Ii+10**(6-i)*(Ich-48)
	 end do
	 Ityp=Ii/10000
	 Ii2=Ii-Ityp*10000
	 Ic=Ii2/100
	 Im=mod(ii2,100)
                                                             
	 Return
	 End
c-----------------------------------------------------------------------
	function RECALE(Signalgg,signalpg,Ic,Im,Itype)
		
       include 'id_chiosi_lpc.incl'
       	
c --- conversion GG --> PG si besoin ...

	if(itype.eq.1.or.itype.eq.4) then
	   s1=mini_base(ic,im,itype,6)*signalgg+mini_base(ic,im,itype,5)
	   itype1=itype+1
	   if(mini_base(ic,im,itype,6).eq.0.) then
	      s1=signalpg
	      itype1=itype+1
	   endif
	else
	   s1=signalpg
	   itype1=itype
	endif
	
c --- recalage

       pr=mini_base(ic,im,itype1,3)
       ps=mini_base(ic,im,itype1,4)
       a=mini_base(ic,im,itype1,1)
       b=mini_base(ic,im,itype1,2)
       
       recale=pr+a*(s1-ps)+b*(s1-ps)**2
       
        return
       end
c---------------------------------------------------------------------------
	Subroutine INI_COEFF(nrun,irc)
	
	Real*4       Par(14)
       Character*80 fic
       logical*1 EOF
       include 'id_chiosi_lpc.incl'
       include 'veda_11.incl'
       
c --- RAZ de la base de donnees

       Irc=0
       Do i=1,14
          Do j=2,9
             Do k=1,3
                Xval(i,j,k)=-100000.
             End do
          end do
       end do
       
       fic=vedadata(1:long_data)//nom_param
     
       ific=LONGUEUR(fic)
       Print *,'Ouverture du fichier de parametres :',fic(1:ific)
       OPEN(Unit=Lun,file=fic(1:ific),form='FORMATTED',
     &      STATUS='OLD')
     
       nl=0
       EOF=.False.
       Do while(.not.EOF)
          Read(lun,1,end=20) Ilabel,Nbpar2,Izmi,Izma,
     &         Izmi0,Izma0,(par(i),i=1,14)
          Ic=ilabel/100
          Ityp=mod(ilabel,100)
          Izmin(Ic)=Izmi
          Izmax(Ic)=Izma
          Izmin0(Ic)=Izmi0
          Izmax0(Ic)=Izma0
          
          Do i=1,nbpar2
             Xval(i,Ic,Ityp)=par(i)
          end do
          nl=nl+1
       end do
       
20     Close(unit=Lun)

       Print *,nl,' lignes lues dans le fichier ',fic(1:ific)
       if(nl.lt.1) then
          Irc=-1
          Print *,'********* Erreur dans INI_COEFF *****************'
          Print *,'Probleme de lecture dans le fichier ',fic(1:ific)
          Print *,'Verifiez le format employe pour les lignes'
          Print *,'Voici le VRAI format :'
          Print *,'Format(1x,6(i6),'' : '',14(g16.8))'
          Print *,'AUCUNS coefficients charges !!'
          Print *,'********* Erreur dans INI_COEFF *****************'
       endif
             
1      Format(1x,6(i6),' : ',14(g16.8))
       
       Return
       end
c-------------------------------------------------------------------    
c Fonction longueur                                                     
c=============================================                          
      function longueur(chaine)                                         
      integer*4 longueur                                                
      integer*4 lc                                                      
      character*(*) chaine                                              
c                                                                       
      lc=len(chaine)                                                    
      do while(chaine(lc:lc).eq.' '.and.lc.gt.0)                        
       lc=lc-1                                                          
      end do                                                            
      longueur=lc                                                       
c                                                                       
      return                                                            
      end  
c--------------------------------------------------------------------
      Subroutine id_chiosi_lpc(ic,im,chg,chp,chio_mt,
     &    sig, sip, si_mt, csr, csl,zed2,chio,si,icod2)
     
      Integer*4 ic,im,icod2
      Real*4    chg,chp,chio_mt,sig,sip,si_mt,csr,csl,zed2,recale
      Real*4    chio,si
      Include 'id_chiosi_lpc.incl'
      Real*8 Par(Nbpar)
      
      External recale
      
      z0min=0.
      z0max=150.
      
      zed2=-999.
      icod2=0
      
      do i=1,nbpar
         par(i)=xval(i,ic,1)
      end do
      
      if(par(1).eq.-100000.) then
         icod2=15
         zed=-999.
         return
      endif
      
c --- Conversion des signaux GG et PG ==> recalage !

      chiogg=chg
      chiopg=chp
      sigg=sig
      sipg=sip
      
      if(chiogg.lt.3800.) then
         chio=RECALE(chiogg,chiopg,Ic,Im,1)
      else
         chio=RECALE(chiogg,chiopg,Ic,Im,2)
      endif
      if(sigg.lt.3800.) then
         si=RECALE(sigg,sipg,Ic,Im,4)
      else
         si=RECALE(sipg,sipg,Ic,Im,5)
      endif
      
      xx=si
      yy=chio   
      
c --- On verifie les bornes ...

      Izone=IBORNES(si,chio,ic)

c --- Affectation du code d'identification

      izmax0(Ic)=20.
      izmax(ic)=35.
      
      zz=0.
      call INVERSION_CHIOSI(xx,yy,par,nbpar,zz,z0min,z0max)
      iz=nint(zz)
      if(iz.ge.izmin0(Ic).and.iz.le.izmax0(Ic)) then
         zed2=zz
         icod2=0
      elseif(iz.gt.izmax0(Ic).and.iz.le.izmax(Ic)) then
         zed2=zz
         icod2=1
      elseif(iz.gt.izmax(Ic).and.iz.lt.z0max) then
         zed2=zz
         icod2=3
      elseif(iz.ge.z0max) then
         zed2=-999.
         Icod2=14
      else
         zed2=-999
         Icod2=14
      endif  
      
c --- Definition des zones dans la carte ChIo - Si recales 
c
c     Izone = 0 : Zone bonne pour l'identification
c           = 1 : Pic de bragg (code = 13)
c           = 2 : Zone de rebroussement (DZ =1-2)
c           = 3 : Zone en dessous du rebroussement (non physique) 
c     
      if(Izone.eq.1) then
         Icod2=13
      elseif(Izone.eq.2) then
         Icod2=2
      elseif(Izone.eq.3.or.Izone.eq.-1) then
         Icod2=14
         Zed2=-999.
      endif

      if(zed2.lt.0..or.zed2.gt.100.) then
        Icod2=14
        zed2=-1.
      endif
      
      Return
      End 
c-----------------------------------------------------------------------------
      Real*8 function funfit(x,zz,nbpar,xval)

      implicit double precision(a-h,o-z)
      common/BRAGGET/x_bragg
      
      Real*8  xval(*)

      a=xval(1)*zz**3+xval(2)*zz**2+xval(3)*zz+xval(4)
      b=xval(5)*zz**3+xval(6)*zz**2+xval(7)*zz+xval(8)
      c=xval(9)*zz**3+xval(10)*zz**2+xval(11)*zz+xval(12)

      if(nbpar.eq.12) then
         funfit=a+b/(x+c)
      else
         d=xval(13)*zz**4
         e=xval(14)*zz**4
         funfit=a+b/(x+c)+d/(x*x+e)
      end if
      
c --- Cas special des points en dessous du pic de Bragg ...
      
      if(x_bragg.eq.0.) x_bragg=60.
      if(x.lt.x_bragg.and.x_bragg.gt.0.) then
         y_bragg=funfit_rec(x_bragg,zz,nbpar,xval)
         a=y_bragg/x_bragg
         funfit=a*x
      endif
      
      return
      end
c-----------------------------------------------------------------------------
      Real*8 function funfit_rec(x,zz,nbpar,xval)

      implicit double precision(a-h,o-z)
      Real*8  xval(*)

      a=xval(1)*zz**3+xval(2)*zz**2+xval(3)*zz+xval(4)
      b=xval(5)*zz**3+xval(6)*zz**2+xval(7)*zz+xval(8)
      c=xval(9)*zz**3+xval(10)*zz**2+xval(11)*zz+xval(12)

      if(nbpar.eq.12) then
         funfit_rec=a+b/(x+c)
      else
         d=xval(13)*zz**4
         e=xval(14)*zz**4
         funfit_rec=a+b/(x+c)+d/(x*x+e)
      end if
      
      return
      end

c=====================================================================
c Routine d'inversion de la fonction d'ajustage
c Entree X,Y --> Couple d'entree
c        par --> tableau des parametres
c        nbpar-> dimension du tableau
c Sortie Z   --> Valeur de la solution
c=====================================================================
      subroutine inversion_chiosi(x,y,xval,nbpar,z,z0min,z0max)

      implicit double precision(a-h,o-z)
      Real*8 Xval(*),Funfit
      Integer*4 Nbpar
      external funfit

      i=0
      eps=1.e-3
      ycal=-1.
      bouclemax=50
      zmin=z0min
      zmax=z0min+5.
      if((z0max-z0min).gt.5) then
        ycal=funfit(x,zmax,nbpar,xval)
        if(ycal.gt.100000.) ycal=0.
        do while((ycal.lt.y)
     1                      .and.(zmax.lt.z0max))
           zmin=zmax
           zmax=zmax+5.
           ycal=funfit(x,zmax,nbpar,xval)
           if(ycal.gt.100000.) ycal=0.
        enddo
      endif
      if(zmax.gt.z0max) zmax=z0max
      
      do while((abs(zmax-zmin).gt.eps).and.(i.lt.bouclemax))
       z=(zmin+zmax)/2.
       ycal=funfit(x,z,nbpar,xval)
       if(ycal.gt.y) then
        zmax=z
       else
        zmin=z
       endif
       i=i+1
      enddo
      if(i.ge.bouclemax) then
       z=z0max
      endif

      return
      end
C======================================================================
      subroutine GET_CHIO(Ic,Im,Z,Csir,Csil,Chiogg,Chiopg,Irc)

      include 'id_chiosi_lpc.incl'
      Real*8 Csrr,Par(14),Csrr1,Csrr2
      Real*4 Csir,Chiogg,Chiopg,Z,Chior,Csil
      Real*8 funfit_rec,funfit
      
      Chiogg=0.
      Chiopg=0.
      Irc=0
      
      csrr=RECALE(Csil,csir,ic,im,7)
      Zz=z

c --- Cas "special" Z legers (<10)
      
      if(z.lt.10.) then
      
         Do i=1,14
            Par(i)=Coeff_chiocsi_2(Ic,im,i)
         End do
         Zmin=float(Z_chiocsi_min_2(Ic,im))
         Zmax=float(Z_chiocsi_max_2(Ic,im))
         
c --- Cas normal 
         
      else
      
         Do i=1,14
            Par(i)=Coeff_chiocsi(Ic,im,i)
         End do
         Zmin=float(Z_chiocsi_min(Ic,im))
         Zmax=float(Z_chiocsi_max(Ic,im))
         
      endif
      
      if(Par(1).eq.0.) then
         Irc=-1
         Return
       End if
	Zmin=max(Zmin,3.)
       if(z.lt.zmin) then
         Irc=-2
         Return
       endif
       
       if(z.ge.1.1*zmax) then
         Irc=-2
         Return
       endif
       
       if(z.gt.zmax.and.z.lt.1.1*zmax) then
         Irc=1
       endif
      
c --- Dans le cas "basse energie CsI", on prend l'extrapolation lineaire 

       if(Csrr.lt.100.) then
         Csrr0=Csrr
         Csrr1=100.
         chior1=funfit_rec(Csrr1,zz,nbpar,Par)
         Csrr2=110.
         chior2=funfit_rec(Csrr2,zz,nbpar,Par)
         a=(chior2-chior1)/(Csrr2-Csrr1)
         b=chior1-Csrr1*a
         chior=a*csrr0+b
         
c --- Sinon, calcul normal 

       else
         chior=funfit(Csrr,zz,nbpar,Par)
       endif
      
      Itype=2          
      pr=mini_base(ic,im,itype,3)
      ps=mini_base(ic,im,itype,4)
      a=mini_base(ic,im,itype,1)
      b=mini_base(ic,im,itype,2)
      
      chiopg=(chior-pr)/a+ps
      chiogg=(chiopg-mini_base(ic,im,1,5))/mini_base(ic,im,1,6)
      if(chiogg.gt.4095.) chiogg=4095.
      
      if(chiogg.lt.0.) then
         chiogg=0.
      endif
      if(chiopg.lt.0.) then
         chiopg=0.
      endif
      
      Return
      End	
c--------------------------------------------------------------------------
****************************************************************************
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
****************************************************************************
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
      Subroutine emaxcsi                                                
c     Cette routine initialise les energies max arretees par les CsI    
      Real*4 emax_rap(2:17,2,4)      ! emax_rap(cour,iz,im)             
      Common/emaxenr/emax_rap                                           
c 
CCCouronne 1 :  Phoswichs
		ic  = 1 
         emax_rap(ic,1,1) = 237.                                        
         emax_rap(ic,1,2) = 313.                                        
         emax_rap(ic,1,3) = 370.                                        
         emax_rap(ic,2,3) = 846.                                        
         emax_rap(ic,2,4) = 948.                                        
      Do ic = 2,5                                                       
         emax_rap(ic,1,1) = 237.                                        
         emax_rap(ic,1,2) = 313.                                        
         emax_rap(ic,1,3) = 370.                                        
         emax_rap(ic,2,3) = 846.                                        
         emax_rap(ic,2,4) = 948.                                        
      End do                                                            
      Do ic = 6,7                                                       
         emax_rap(ic,1,1) = 192.                                        
         emax_rap(ic,1,2) = 255.                                        
         emax_rap(ic,1,3) = 300.                                        
         emax_rap(ic,2,3) = 683.                                        
         emax_rap(ic,2,4) = 766.                                        
      End do                                                            
      Do ic = 8,9                                                       
         emax_rap(ic,1,1) = 183.                                        
         emax_rap(ic,1,2) = 244.                                        
         emax_rap(ic,1,3) = 288.                                        
         emax_rap(ic,2,3) = 654.                                        
         emax_rap(ic,2,4) = 733.                                        
      End do                                                            
      Do ic = 10,11                                                     
         emax_rap(ic,1,1) = 166.                                        
         emax_rap(ic,1,2) = 221.                                        
         emax_rap(ic,1,3) = 262.                                        
         emax_rap(ic,2,3) = 591.                                        
         emax_rap(ic,2,4) = 664.                                        
      End do                                                            
      Do ic = 12,12                                                     
         emax_rap(ic,1,1) = 126.                                        
         emax_rap(ic,1,2) = 169.                                        
         emax_rap(ic,1,3) = 199.                                        
         emax_rap(ic,2,3) = 448.                                        
         emax_rap(ic,2,4) = 504.                                        
      End do                                                            
      Do ic = 13,13                                                     
         emax_rap(ic,1,1) = 144.                                        
         emax_rap(ic,1,2) = 192.                                        
         emax_rap(ic,1,3) = 228.                                        
         emax_rap(ic,2,3) = 512.                                        
         emax_rap(ic,2,4) = 577.                                        
      End do                                                            
      Do ic = 14,17                                                     
         emax_rap(ic,1,1) = 129.                                        
         emax_rap(ic,1,2) = 173.                                        
         emax_rap(ic,1,3) = 205.                                        
         emax_rap(ic,2,3) = 460.                                        
         emax_rap(ic,2,4) = 517.                                        
      End do                                                            
      Return                                                            
      End                                                               
                                                                        
c=======================================================================

      include 'vedaloss2_E.f'

	

        
	
	                                                                        
	                                                             
