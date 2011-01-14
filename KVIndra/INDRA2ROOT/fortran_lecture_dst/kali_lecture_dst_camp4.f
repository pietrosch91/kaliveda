c************************************************************************
c	Eric Bonnet 04/2010
c  Derniere compilation sous SL5 64 bits
c  g77 -w -I/afs/in2p3.fr/home/throng/indra/veda4/for/include fortran.f -c

c --- Version JLC 06/02/04

c $Id: vishnou_sub.f,v 1.1 2001/05/18 10:05:28 plagnol Exp $
c***********************************************************************
c   Check Ring number 1
c   Check GSI parameters (entete)
c                                                                       
c  KALI : ou (K)artouches (A) (L)ire pour (I)ndra ...                   
c  
c
c Version speciale pour la campagne GSI : elle litun fichier sur disque 
c 10/03/2003
c  D.Cussol: Version speciale pour la campagne GSI : elle copie les fichiers 
c            HPSS sur le repertoire courant.
c  D.Cussol: 27/09/2007  Version speciale pour la campagne GSI : elle copie les fichiers 
c            HPSS sur le repertoire courant. (il y a redite on sait....)
c***********************************************************************
c-------------------------------------------------------------------- 
      program Kali_4                                                  
                                                                        
      logical   end_of_file,end_of_program
      integer*2 ibuff(8192),itab(8),ktab(100) 
      Character*1  char1                                    
      character*8  typebloc
      character*80 filein,fileout,namefil
      character*132 fic_run
      real*4       time                                                 
      Character*6   cart_dst,cartou
      Character*8   datj,hj     
      Integer*4     ifin,iq,jq
      integer*4    lun,irc,numruni,new_run
      Integer*4  iascii(256)
      integer*4  nblocecr_tot,nevtecr_tot
      logical    linux
      Character*70  critere
      integer*4     long_critere
      
      Character*6  op_system
      common /SYST/linux
      logical*1  time_max
      
      character*160 commande ! ajout D.CUSSOL 08/03/2003
      character*80 hpssdir ! ajout D.CUSSOL 08/03/2003
      common /TestBizarrz/ibizarre  
      Common /RED1/new_run,long_critere,critere                         
      Common /ASKI/iascii
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
      Logical*1 lfiltre,lsimulation                                            

      call INITC 
      
      linux=.false.  ! AIX by default
      call GETENVF ('SYSTEM',op_system)
      write(*,'(/,'' Operating system = '',a)')op_system
      if(op_system(1:5).eq.'Linux')then
         linux=.true.
      endif
      call VAR_ENVIRONNEMENT  
             
      namefil=nomjob(1:long_job)//'.sortie'

      print*,'INFO namefil=',namefil,' iout=',iout
      open (i_out,file=namefil) ! JLC 9/02/04
      
      call DATIMH (datj,hj)                                             
      write(6,'(//,'' Date debut : '',a8,''  '',a8)')datj,hj 
      call INI_ASCII                                                    
      
      call INI_HBOOK

      lfiltre=.false.
      lsimulation=.false.                                               
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

      lun=1
		
		end_of_file=.False.                                            
      nbloclus=0                                                     
      nevtlus=0                                                      
      nb_scaler=0 
      nkbyta=0                                                       
      nkbyte=0 
      end_of_file=.false. 
      fin_lect_run=.false.  
                   
c --- Lecture du nom du run a lire
		filein=nomjob(1:long_job)//'.dst'
		call OPEN_FILE(filein,lun)

		end_of_file=.False.                                            
      
		if(lun.eq.-100) then
      	print *,'Probleme a l''ouverture du fichier : ',filein(1:8)
      	print *,'On passe au fichier suivant...'
      	end_of_file=.True.
      endif ! if(ios.ne.0) then

		do while(.not.end_of_file)                                        
                                                                        
			call READ_BUFFER_1 (lun,nbloclus,ibuff,ios)

	    
c --- Swap de ibuff (LINUX seulement)

			if(linux)then
				do i=1,8192
					ibuf=ibuff(i)   
					ibuf1=jbyt(ibuf,1,8)                                             
					ibuf2=jbyt(ibuf,9,8)                                             
					ibuff(i)=ibuf1*256+ibuf2  ! buffer swappe                                   
				end do
			endif  

			if(ios.lt.0) then
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
                        
				if(typebloc.eq.'VEDADST3'.or.typebloc.eq.'VEDADST4'.
     &      	  or.typebloc.eq.'VEDADST5'.or.typebloc.eq.'VEDADST6'
     &      	 .or.typebloc.eq.'VEDARED1' )then
            
					if(typebloc.eq.'VEDARED1')then
						dst_reduite=.true.
						write(6,104)
104					format(/,' ** TRAITEMENT des bandes DST <reduite> ** ')
					endif
 
					call DEC_BLOC_HEAD (ibuff,iversion,irc)

					if(irc.ne.0) then                                           
						write(6,*) ' Erreur lecture du fichier : ',filein
						write(6,*) ' Erreur lecture du fichier : ',filein
						end_of_program=.true.
						goto 999
					end if 
            
            	print *,' *** RUN = ',numerun
	            
					call INI_RUN 
                                                                        
				elseif(typebloc.eq.'BLOCDATA') then

					if(iversion.eq.3)then 
						print *,' 1rst campaign !!! Incompatible version 3 !'
		   stop3 
c                call DEC_BLOC_DST_3  (ibuff)
					elseif(iversion.eq.4)then
						print *,' 1rst campaign !!! Incompatible version 4 !'
		   stop4 
c                call DEC_BLOC_DST_4  (ibuff)
					elseif(iversion.eq.5)then
						print *,' 1rst campaign !!! Incompatible version 5 !'
		   stop5 
c                   call DEC_BLOC_DST  (ibuff)
					elseif(iversion.eq.6)then
						call DEC_BLOC_DST_6  (ibuff)
						
						if (ibizarre.eq.1) goto 779
					
					else
						STOP 'BLOCDATA  impossible a lire'
					endif      
               
               if(fin_lect_run)then
						end_of_file=.true.
               endif   
            	if(fin_lect_gen)then
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
     &      (ibuff,echelle,nb_echelle_a_imprimer,Iwt)
            	call PRINT_SCALER(i_out)
             
        		elseif(typebloc.eq.'ETAT_IND') then
 
					Print *,'Bloc ETAT INDRA non decode au bloc #',nbloclus
	     			Irc=0 
            	if(irc.ne.0) then                                           
            		write(6,*) ' Erreur a la lecture du bloc ETAT_IND '  
            		write(6,*) ' Erreur a la lecture du bloc ETAT_IND '      
            	end if  
				else
					write(6,25) nbloclus,typebloc
25          format(1x,' Type du bloc #',i6,' : ',a8,' inconnu !')
					do i=1,8                                                    
						itab(i)=ibuff(i)
					end do
					Print *,(itab(i),i=1,8)
				end if
                                                                        
c --- Pointeur de bloc                                                  
                                                                       
         	iloop=mod(nbloclus,250)                                        
         	iwoop=mod(nblocecr,250) 
                                                                        
         	if(iloop.eq.0.and.nbloclus.ne.1) then 
           		write(6,*) 'Nbloc lus =',Nbloclus,'    Nevt lus =',Nevtlus 
				end if
      
			endif  
      
		end do 

 779  continue
c --- Fin de la lecture, qq rappels...                                  

		write(6,*) ' '                                              
      write(6,'(1x,a,/,2x,a)') 'Fin de lecture du fichier DST : ',
     &																  filein(1:8)    
      write(6,*) 'Nombre de blocs lus  	  :',nbloclus  			
      write(6,*) 'Nombre de blocs SCALER    :',nb_scaler
      write(6,*) 'Nombre d''evts lus			:',nevtlus  			
      write(6,*) 'Numero du dernier evt lu  :',num_evt_brut 		
      write(6,*)'-----------------------------------------------'
      																							  
      write(6,*) ' ' 																 
      call FIN_RUN																	 
      write(6,*) ' ' 
      if(time_max)then
      	 write(*,*)' Fin du traitement car Time_max = ',time_max
      	 end_of_program=.true.
      endif 
                                                                        
c --- Fermeture                                                         
                                                                        
999   write(6,*) ' '                                                
      write(6,*) ' ********    Fin du traitement ****************'
      write(6,*) ' -- Cumul du nb de blocs lus : ',nbloc_tot    
      write(6,*) ' -- Cumul du nb d''evts   lus : ',nevt_tot    
      write(6,*) ' '                                                
      write(6,*) ' '                                                    
      write(6,*) ' -- Cumul du nb de blocs lus : ',nbloc_tot        
      write(6,*) ' -- Cumul du nb d''evts   lus : ',nevt_tot        
      write(6,*) ' '                                                    
                                                                         
200   call CLOSE_HB                                                     
                                                                        
      write(6,*) ' '                                                
      write(6,*) ' ---- FIN DE LECTURE ----'                        
      call DATIMH (datj,hj)                                             
      write(6,'(//,'' Date fin  : '',a8,''  '',a8)')datj,hj         
                                                                        
      end                                                               
                                                                        
c---------------------------------------------------------------------- 
c --- Routine de decodage des blocs evts   (VEDA  iversion=6 for GSI)           
c---------------------------------------------------------------------- 
      subroutine DEC_BLOC_DST_6(ibuff)                                    
                                                                        
      integer*2 ibuff(8192)
      integer*4 ivalz(300),jvalz,long2
      integer*2 nevt_low,nevt_high
      integer*4 code16,code4(4),ien
      logical   evt_a_lire,prt,dump_eric_2
      integer*2 kbyt(2)
                                                                        
      Real*4       tab_pw(12),hit_pw(12)
      common /TABPW/tab_pw,hit_pw      
      common /TestBizarrz/ibizarre      

      include 'veda_rel.incl'                                           
      include 'veda_wri.incl'
      include 'veda_5.incl'                                             
      include 'veda_6.incl'                                             

      include 'veda_gsi_1.incl'
      
      common /VEDADUMP/ivalz
      logical    linux
      common /SYST/linux
      Equivalence (ien,kbyt)      
      data dump_eric_2/.false./
      ipt=9                 ! a verifier 
      evt_a_lire=.True.                                                 
      prt=.False.  
                                                                        
		ibizarre=0                                                             
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

			if(ipt.gt.8192)evt_a_lire=.false.   ! on W.Mueller's advice

         ilong=ibuff(ipt)                                               
         nkbyte=nkbyte+ilong*2                                          
         if(dump_eric_2)	write(*,*)' DEC_BLOC_DST_6 : ilong =',ilong
                                                                        
c --- Fin de lecture du bloc courant                                    
                                                                        
         if(ilong.eq.0.or.ipt.ge.8192) then   ! JLC 09/02/04                                         
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
            npart_new_coherency=ibuff(ipt+26)   ! a verifier
            npart_modif_coherency=ibuff(ipt+27)   ! a verifier
            if(dump_eric_2)then
					write(*,*)
     &  'Nouveau Apr 20 :: DEC_BLOC_DST_6 : npart_traite =',npart_traite
              	write(*,*)
     &   ' npart_coherency new,modif=',npart_new_coherency,
     &                                 npart_modif_coherency
              	write(*,*)' header'
              	write(*,'(10i6)')(ibuff(ipt+k),k=0,29)
              	write(*,*)'the events'
              	k=30
              	do i=1,999
						long2=ibuff(ipt+k)
						if(long2.lt.4.or.long2.gt.26)stop 'Eric'
						write(*,*)' Particle : i,long2,k,code=',i,long2,k
c                write(*,'(10i6)')(ibuff(ipt+k+j-1),j=1,long2)
						k=k+long2
						if(k.gt.(ilong-2))goto 1467
					enddo
1467        	continue
				endif
c --- Reconstitution de NEVT...                                         
 
				iflag=0
				if(nevt_high.lt.0) iflag=1                                  
				if(iflag.eq.0) then
					num_evt_brut=nevt_low*2**16+nevt_high                    
				else
					num_evt_brut=nevt_low*2**16+nevt_high+2**16              
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

 				stop 'DEC_BLOC_DST 2'
				return
            
				endif  
                                                                        
c            if(ilong.gt.1200)then
c               print *,' -------------------------------------------'
c               print *,' ATTENTION EVENEMENT BIZARRE ...'
c               print *,' Longueur evt un peu grande: = ',ilong,' > 1200'
c               print *,' evt = ',nevtlus,' evt brut = ',num_evt_brut
c               print *,' npart_traite, Ztot , Mult.(code>=2) = ',
c     &                   npart_traite,iztot,mcha
c               print *,' *** Danger s''ils sont trop nombreux ! ***'
c               print *,' -------------------------------------------'
c            endif  
c            write(*,*)' DEC_BLOC_DST_6 : raw_event_number,nevt =',num_evt_brut,nevt
				do k=1,ilong
					ievt(k)=ibuff(ipt+k-1)
c            if(k.lt.30)write(*,*)' DEC_BLOC_DST_6 : index,ievt =',k,ievt(k)
				enddo
				do k=ilong+1,wmax
					ievt(k)=0
				enddo   
c            if(nevt.gt.10)stop' Demande dans DEC_BLOC_DST_6'
                                                                        
c --- Positionnement du pointeur de lecture                             
c     l'entete contient la longueur de l'evenement ibuff(ipt)
c     le premier parametre de GSI correspond a IBUFF(ipt+8)
c     le dernier parametre de GSI correspond a IBUFF(ipt+i_entete-1)
				do i=8,i_entete-1
					entete_gsi(i-7)=ibuff(ipt+i)
				enddo
				ipt=ipt+i_entete
                                                                        
c --- Decodage caracteristiques des particules...                       
                                                                        
				do i=1,npart_traite+npart_modif_coherency                                         
                                                                        
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
            		code_idf(k,i)=0
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

          		a_sec(i) = 0.
          		z_sec(i) = 0.
                                                                        
c --- Si Gamma ( Code = 0 )                                             
                                                                        
    				if(ilongevt.eq.i_long1) then                             
                                                                        
       				z(i)=0                                                
            		a(i)=0                                                
            		z_indra(i)=0.                                         
        				a_indra(i)=0.                                         
             		code_part(i)=0 
           			code16=code_part(i)                                   
           			call DECODE_PART(icou(i),code16,code4)                
             		code(i)=code4(1)                                      
             		do k=1,4
            			code_idf(k,i)=code4(k)                                
             		enddo
 
             		de3(i)=ibuff(ipt+2)                                 
            		call mtcombo_unpack(ibuff(ipt+3),mt(i),mtsig(i))
                                                                        
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
            		if(dump_eric_2) then
							write(*,*)' particule : num,code=',i,code(i)
            		endif
						do k=1,4
             			code_idf(k,i)=code4(k)
           			enddo
c   Decodage de zcombo -acombo                
						if((icou(i).ge.1.and.icou(i).le.9) .and.
     +       	(code(i).eq.2.or.code(i).eq.3)) then
            			call zcombo_unpack(ibuff(ipt+2),r_zpri,r_zsec)
              			z_indra(i) = r_zpri
                		z(i)       = nint(z_indra(i))
               		z_sec(i)   = r_zsec
              			call acombo_unpack(ibuff(ipt+3),r_apri,r_asec)
                		a_indra(i) = r_apri
              			a(i)       = nint(a_indra(i))
               		a_sec(i)   = r_asec
            		endif

             		if(icou(i).ge.10.and.icou(i).le.17)then
              			if(code(i).eq.2.or.code(i).eq.4) then
               	   	call zcombo_unpack(ibuff(ipt+2),r_zpri,r_zsec)
               	    	z_indra(i) = r_zpri
               	    	z(i)       = nint(z_indra(i))
               	    	z_sec(i)   = r_zsec
               	  	else if (code(i).eq.3) then ! etalon
               	    	call zcombo_unpack(ibuff(ipt+2),r_zpri,r_zsec)  
               	    	z_indra(i) = r_zpri
               	   	z(i)       = nint(z_indra(i))
               	    	z_sec(i)   = r_zsec
               	    	call acombo_unpack(ibuff(ipt+3),r_apri,r_asec)
               	   	a_indra(i) = r_apri
               	     	a(i)       = nint(a_indra(i))
               	   	a_sec(i)   = r_asec
               	 	endif
            		endif
c -----
						code_cali(i)=ibuff(ipt+5)
                 
						code16=code_cali(i)                                   
						code_energie(1,i)=jbyt(code16,1,4)                    
						code_energie(2,i)=jbyt(code16,5,4)                    
						code_energie(3,i)=jbyt(code16,9,4)                    
						code_energie(4,i)=jbyt(code16,13,4)                   
						ecode(i)=code_energie(1,i)                            
                                                                        
						call mtcombo_unpack(ibuff(ipt+6),mt(i),mtsig(i))
                                                                         
c --- Cas des couronnes 1 a 9   Ring 1 as others                               
               	                                                         
						if(ilongevt.eq.i_long3) then                      
               	                                                         
							de_mylar(i)=(ibuff(ipt+7))/100.                     
							de1(i)=(ibuff(ipt+8))/10.                           
                                                                        
							if(.not.linux)then
		      				kbyt(1)=ibuff(ipt+9)
		      				kbyt(2)=ibuff(ipt+10)
							else
		      				kbyt(1)=ibuff(ipt+10)
		      				kbyt(2)=ibuff(ipt+9)
							endif
		    
c		    ien=ibuff(ipt+9)*(2**16)+ibuff(ipt+10)
		      				de2(i)=float(ien)
		      				de2(i)=de2(i)/10.
                                                                        
							if(.not.linux)then
		      				kbyt(1)=ibuff(ipt+11)
		      				kbyt(2)=ibuff(ipt+12)
							else
		      				kbyt(1)=ibuff(ipt+12)
		      				kbyt(2)=ibuff(ipt+11)
							endif
c		    ien=ibuff(ipt+11)*(2**16)+ibuff(ipt+12)
		      			de3(i)=float(ien)
		      			de3(i)=de3(i)/10.
                    
		      			canal(i,1)=ibuff(ipt+13)  !  canal  ChIo GG
		      			canal(i,2)=ibuff(ipt+14)  !  canal  ChIo PG
		      			canal(i,4)=ibuff(ipt+15)  !  canal  SI GG
		      			canal(i,5)=ibuff(ipt+16)  !  canal  SI PG
		      			canal(i,7)=ibuff(ipt+17)  !  canal  CsI R
		      			canal(i,8)=ibuff(ipt+18)  !  canal  CsI L
                    
c --- Cas des couronnes 10 a 17  (sans Etalons)                         
                                                                        
		      		elseif(ilongevt.eq.i_long4) then                      
                                                                        
		      			de_mylar(i)=(ibuff(ipt+7))/100.                     
		      			de1(i)=(ibuff(ipt+8))/10.                           
                                                                        
							if(.not.linux)then
              				kbyt(1)=ibuff(ipt+9)
                  	   kbyt(2)=ibuff(ipt+10)
							else
                  	   kbyt(1)=ibuff(ipt+10)
                  	   kbyt(2)=ibuff(ipt+9)
							endif
c 		    ien=ibuff(ipt+9)*(2**16)+ibuff(ipt+10)
                  	de3(i)=float(ien)
              			de3(i)=de3(i)/10.
                    
              			canal(i,1)=ibuff(ipt+11)  !  canal  ChIo GG
              			canal(i,2)=ibuff(ipt+12)  !  canal  ChIo PG
              			canal(i,7)=ibuff(ipt+13)  !  canal  CsI R
              			canal(i,8)=ibuff(ipt+14)  !  canal  CsI L           
                    
		      
		                                                          
c --- Cas des couronnes 10 a 17  (avec Etalons)                         
                                                                        
		      		elseif(ilongevt.eq.i_long5) then                      
c                   write(*,*)' DEC_BLOC_DST_6 : Evt Etalon,nevt =',nevt
                     
							de_mylar(i)=(ibuff(ipt+7))/100.                     
                    	de1(i)=(ibuff(ipt+8))/10.                           
                    	de4(i)=(ibuff(ipt+9))/10.                           
                                                                        
                    	if(.not.linux)then
                    		kbyt(1)=ibuff(ipt+10)
								kbyt(2)=ibuff(ipt+11)
                    	else
								kbyt(1)=ibuff(ipt+11)
								kbyt(2)=ibuff(ipt+10)
                    	endif
c   		    ien=ibuff(ipt+10)*(2**16)+ibuff(ipt+11)
                    	de5(i)=float(ien)
                    	de5(i)=de5(i)/10.
                                                                        
                    	if(.not.linux)then
								kbyt(1)=ibuff(ipt+12)
								kbyt(2)=ibuff(ipt+13)
                    	else
								kbyt(1)=ibuff(ipt+13)
								kbyt(2)=ibuff(ipt+12)
                    	endif
c 		    ien=ibuff(ipt+12)*(2**16)+ibuff(ipt+13)
                    	de3(i)=float(ien)
                    	de3(i)=de3(i)/10.
                    
                    	canal(i,1)=ibuff(ipt+14)  !  canal  ChIo GG
                    	canal(i,2)=ibuff(ipt+15)  !  canal  ChIo PG
                    	canal(i,7)=ibuff(ipt+16)  !  canal  CsI R
                    	canal(i,8)=ibuff(ipt+17)  !  canal  CsI L
                    	canal(i,10)=ibuff(ipt+18)  !  Canal Si75 GG 
                    	canal(i,11)=ibuff(ipt+19)  !  Canal Si75 PG
                    	canal(i,13)=ibuff(ipt+20)  !  Canal SiLi GG
                    	canal(i,14)=ibuff(ipt+21)  !  Canal SiLi PG
                    	canal(i,3)=ibuff(ipt+22)  !  canal  ChIo temps
                    	canal(i,12)=ibuff(ipt+23)  !  canal  Si75 temps
                    	canal(i,15)=ibuff(ipt+24)  !  canal  SiLi temps
                    	canal(i,9)=ibuff(ipt+25)  !  canal  Csi temps
                                                                        
						else                                                  
                                                                        
                    	write(6,*) 'i_long1=',i_long1
                    	write(6,*) 'i_long2=',i_long2
                    	write(6,*) 'i_long3=',i_long3
                    	write(6,*) 'i_long4=',i_long4
                    	write(6,*) 'i_long5=',i_long5
                    	write(6,*) 'Bloc=',nbloclus,' Evt:',nevt,'bizarre!'
                    	write(6,*) 'En effet, ilongevt =',ilongevt,' !?'	 
                    	write(6,*) 'On passe au bloc suivant !!!?'
                    	
							ibizarre=1
                    	return
!							evt_a_lire=.false.
!							end_of_file=.true.
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
					
					if(num_evt_brut.eq.-159)then
						write(*,*)' npart,ic,im,z,a =',i,icou(i),imod(i),z(i),a(i)
            	endif
         	end do

		    
c --- Routine utilisateurs pour le traitement evt/evt 
                  
         	copy_event=.false.    
                                                  
         	call TRAITEMENT
            
         	if(fin_lect_run)evt_a_lire=.false.
				if(fin_lect_gen)evt_a_lire=.false.
                                                                        
101			format(1x,5(i6,2x))                                         
                                                                        
			end if                                                         
                                                                        
		end do    ! do while on evt_a_lire
                                                                        
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
      
      if(Zproj.eq.0.) then
           Zproj=54.
	    aproj=129.
	    Zcib=50.
	    Acib=119.
	    Esura=50.
	    Numerun=4625
      endif
	              
      write(6,'(/,'' *** Lecture Run numero '',i4)')numerun         
      write(6,1) ijour,imois,iannee                                 
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
        write(6,4) day,hour                                         
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
        write(6,7)kjour,kmois,kanne,kheure,kminut,ksecon
7       format(' Fichier DST cree le     : ',i2,'/',i2,'/19',i2,' a ',
     &                                       i2,'h.',i2,'m.',i2,'s.')   
        do l=1,8
        ibuff(ipt+l)=iascii(ibuff(ipt+l))
        enddo
                              
      endif
      
      write(6,2) zproj,aproj,esura                                  
2     format(/,1x,'Projectile ==>  Z=',f4.0,'   A=',f4.0,               
     &         '   E/A=',f5.1)                                          
      write(6,3) zcib,acib                                          
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
      if(iversion.eq.6)then
        write(*,*) '   - Cour. 1 a 9                :',i_long3
      else
        write(*,*) '   - Phoswich                   :',i_long2
        write(*,*) '   - Cour. 2 a 9                :',i_long3
      endif
      write(*,*) '   - Cour 10 a 17 (sans si_etal):',i_long4        
      write(*,*) '   - Cour 10 a 17 (avec si_etal):',i_long5        
      write(*,*) ' '                                                
      write(*,*) 'Longueur des blocs :',numbytes,' octets'          
      write(*,6) type                                               
6     format(1x,'Type des mots      : ',a5)                             
                                                                        
      return                                                            
      end  
       
                                                                        
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
c   Routine de decodeage des codes d'identification 16 bits DST       
c            Pour INDRA a GSI
c  Elle fournit sous forme d'un tableau de 4 entiers*4 les differents   
c  codes ORIGINAUX d'identification ainsi que le code general defini    
c  dans le fichier VEDA_6 INCL sur le disque INDRA 200 .                
c-------------------------------------------------------------------    
      Subroutine DECODE_PART(jcou,icode,code4)

      Integer*4   code4(4)
      Integer*4   icode,jcou

      code4(1)=jbyt(icode,1,4)
      code4(2)=jbyt(icode,5,4)
      code4(3)=jbyt(icode,9,4)
      code4(4)=jbyt(icode,13,4)

      return
      end
c---------------------------------------------------------------------- 
c     Routine de decodeage des codes d'identification 16 bits DST       
c                            Vieille Routine
c  Elle fournit sous forme d'un tableau de 4 entiers*4 les differents   
c  codes ORIGINAUX d'identification ainsi que le code general defini    
c  dans le fichier VEDA_6 INCL sur le disque INDRA 200 .                
c-------------------------------------------------------------------    
      Subroutine DECODE_PART_old (jcou,icode,code4)                         
                                                                        
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
 
      logical  linux
      common /SYST/linux
      Common       ibuf2,ibuf1
      Equivalence  (ibuf4,ibuf2)

      do i=1,4096
      k=2*i-1
      if(.not.linux)then
        ibuf1=buffer(k)
        ibuf2=buffer(k+1)
      else	
        ibuf2=buffer(k)
        ibuf1=buffer(k+1)
      endif	
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
                                                                        
      i_entete=30   ! Nombre de mots pour entete EVT (normally read from data                    
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
c
c For format of packed charge see zcombo_pack in vedaid_4.f
c
c  i_pack:  I   integer*2 with packed charge value
c  r_zpri:  O   real*4 with primary mass
c  r_zsec   O   real*4 with secondary mass
c

      subroutine zcombo_unpack(i_pack,r_zpri,r_zsec)
      implicit none
c
      integer*2  i_pack
      real*4     r_zpri,r_zsec
c
      integer*4  i_in,i_zpri,i_zsec
c
      i_in = i_pack
      if (i_in .lt. 0) i_in = i_in + 65536
c
      if (i_pack .lt. 12100) then
        r_zpri = 0.01 * i_pack
        r_zsec = 0.
      else
        i_in   = i_pack - 12100
        i_zsec = i_in / 140
        i_zpri = i_in - 140*i_zsec
        r_zpri = 0.10 * i_zpri
        r_zsec = 0.10 * i_zsec
      endif
      return
      end
c
c-------------------------------------------------------------------    
c
c For format of packed mass see acombo_pack in vedaid_4.f
c
c  i_pack:  I   integer*2 with packed mass value
c  r_apri:  O   real*4 with primary mass
c  r_asec   O   real*4 with secondary mass
c

      subroutine acombo_unpack(i_pack,r_apri,r_asec)
      implicit none
c
      integer*2  i_pack
      real*4     r_apri,r_asec
c
      integer*4  i_in,i_apri,i_asec
c
      i_in = i_pack
      if (i_in .lt. 0) i_in = i_in + 65536
c
      if (i_in .lt. 50000) then
        i_asec = i_in / 320
        i_apri = i_in - 320*i_asec
        r_apri = 0.05 * i_apri
        r_asec = 0.10 * i_asec
      else
        r_apri = 0.05 * (i_in - 57000)
        r_asec = 0.
      endif
      return
      end
c-------------------------------------------------------------------    
c
c for format of packed mt see vedaecr_4.f
c
      subroutine mtcombo_unpack(i_pack,i_mt,i_mtsig)
      implicit none
c
      integer*2  i_pack
      integer*4  i_mt,i_mtsig
c
      integer*4  i_in
      integer*4  i_tag
c
      i_in = i_pack
c
      if (i_pack .lt. 0) then
        i_mt    = i_pack
        i_mtsig = 0
      else
        i_tag = i_pack / 1024
        i_mt  = iand(i_in,1023)
        if (i_tag .eq. 1) then
          i_mtsig = 3
        else if (i_tag .eq. 2) then
          i_mtsig = 6
        else if (i_tag .eq. 3) then
          i_mtsig = 9
        else if (i_tag .eq. 4) then
          i_mtsig = 12
        else if (i_tag .eq. 5) then
          i_mtsig = 15
        else
          i_mtsig = 0
        endif
      endif

      return
      end
      
c -----------------------------------------------------------------------------
 
      Subroutine READ_BUFFER_1(lun,nbloc,buffer,istatus)                 
                                                                  
      Parameter (lr=8192)
      integer*2  buffer(lr)
      integer*2  ibuff(8196)
      integer*4  lun,irc,nwrec,nwtak,istatus

c      nwrec=4096
      nwrec=4098
      nwtak=nwrec
      istatus=0

c --- lecture d'un bloc                                           
                                                                    
c      call cfget (lun,0,nwrec,nwtak,buffer,irc)
      call cfget (lun,0,nwrec,nwtak,ibuff,irc)
      
      if(irc.eq.0)then
	 do k=1,8192
	 buffer(k)=ibuff(k+2)
	 enddo
         nbloc=nbloc+1
	 
c	 if(nbloc.eq.2)then
c            write(*,'(10(2x,z4))') (ibuff(l),l=1,8196)
c	    print *
c            write(*,'(10(2x,z4))') (buffer(l),l=1,8192)
c	 endif   
   
      elseif(irc.eq.-1)then
          istatus=irc  ! fin de fichier detecte
          
      else
          print *,' !!!! Error cfget : irc = ',irc
          stop 'READ_BUFFER_1'    
      endif   
      
      return                                                    
      end
                                                                  
c -----------------------------------------------------------------------------
 
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
      
c -----------------------------------------------------------------------------
C++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

