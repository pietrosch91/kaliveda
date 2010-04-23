c$Id: faire_arbre.f,v 1.9 2006/10/19 14:32:43 franklan Exp $                                                                   
c---------------------------------------------------------------------  
c     Ici se trouvent reunies les diverses routines personnelles a    | 
c     l'utilisateur. Celui trouvera donc la routine INI_HBOOK,        | 
c     appelee en tout debut de programme et servant a initialiser     | 
c     les fichiers type Hbook par exemple. La routine INI_RUN est     | 
c     appelee a chaque fois que commence la lecture d'un run DST.     | 
c     La routine TRAITEMENT est appelee a CHAQUE evenement et permet  | 
c     de realiser l'analyse des donnees recuperees via les COMMONs    | 
c     definis dans VEDA_6 INCL ( cf fichier VEDA_6 INCL ).            | 
c     La routine FIN_RUN est appelee a la fin de la lecture d'un      | 
c     run DST et la routine CLOSE_HB est appelee en toute fin de      | 
c     lecture.                                                        | 
c                                                                     | 
c                                                                     | 
c Version du 12/12/94                                                 | 
c --------------------------------------------------------------------  
c                                                                       
      subroutine INI_HBOOK                                              
c                                                                       
c --- Hmemory donne la taille memoire en octets utilise par HBOOK...    
c --- Sa valeur peut etre diminuee ou augmentee suivant le nombre       
c --- de Hbooks crees ...                                               
c                                                                       
      include 'veda_rel.incl'                                           
	character*1 campaign
                                                                        
	call getenvf('CAMPAGNE', campaign)
c Campaign-dependent corrections etc.
       if( campaign .eq. '1' ) then
c identification/calibration ChIo-CsI correction Tabacaru/LeNeindre
	 	new_calib_c1017 = .true.
		print*,'Will correct CsI calibration rings 10-17'
	endif
	                                                                 
c --- Fin de la routine                                                 
                                                                        
      return                                                            
      end                                                               
                                                                        
c---------------------------------------------------------------------  
c --- Routine appelee evt par evt, les caracteristiques de l'evt        
c --- sont dans veda_6 INCL                                             
c---------------------------------------------------------------------  
c --- Compteurs disponibles sur la lecture des cartouches disponibles   
c --- via VEDA_REL INCL :                                               
c                                                                       
c   - Nbloclus  : Nbre de blocs lus dans le fichier courant             
c   - Nbloc_tot : Nbre TOTAL de blocs lus (cumul de Nbloclus)           
c   - Nevtlus   : Nbre d'evenements lus dans le fichier courant         
c   - Nevt_tot  : Nbre total d'evenement lus (cumul de Nevtlus)         
c                                                                       
c---------------------------------------------------------------------  
c --- Variables logiques disponibles concernant la lecture :            
c                                                                       
c   - Fin_lect_run : valeur a .False. pour la lecture des evenements    
c                     du fichier courant et a .True. lorsqu'on ne       
c                     veut plus lire d'evt dans le run DST courant.     
c                                                                       
c   - Fin_lect_gen : valeur a .False. pour la lecture des evenements    
c                     de tous les fichiers courant et a .True. lors-    
c                     qu'on ne veut plus lire du tout d'evt .           
c                                                                       
c   Ces 2 variables sont a .False. lorsqu'on demarre la lecture et      
c   peuvent etre passees a .True. lorsqu'on desire ne plus lire         
c   d'evenements soit dans le run courant ( on reprend alors la         
c   lecture des evts au run suivant s'il existe) soit pour le           
c   job ( on arrete alors proprement le batch ).                        
c                                                                       
c---------------------------------------------------------------------  
      subroutine TRAITEMENT                                             
                                                                        
      include 'veda_rel.incl'                                           
      include 'veda_5.incl'                                             
      include 'veda_6.incl'                                             
c      include 'anin_1.incl'                                             
c      include 'anin_3.incl'                                             
c      include 'simingeo.incl'                                           
      common /rootwrite/ necrit, nfiles     
	character*60 filename                               
      integer*4  indice(500)
      integer*4  imult,icans

c ---- Inserez ici le traitement evt par evt                            
c      call initevt()
      pi=3.1415927                                                      
      
      if(npart_traite.gt.0) then
       necrit=necrit+1
       write(88,*) num_evt_brut
	 write(88,*) npart_traite
       do j=1,npart_traite
        
        	write(88,*) a(j)
		write(88,*) z(j)
		write(88,*) z_indra(j)
		write(88,*) a_indra(j)
		write(88,*) ener(j)
		write(88,*) icou(j)
		write(88,*) imod(j)
		write(88,*) code(j)
		write(88,*) ecode(j)
		write(88,*) de_mylar(j)
		write(88,*) de1(j)
		write(88,*) de2(j)
		write(88,*) de3(j)
		write(88,*) de4(j)
		write(88,*) de5(j)
		write(88,*) mt(j)
		icans=0
	 	do kk=1,15
                  if(canal(j,kk).ne.0) then
                    icans=icans+1
                  endif
		enddo
		write(88,*) icans
	 	do kk=1,15
                  if(canal(j,kk).ne.0) then
                    write(88,*) kk,canal(j,kk)
                  endif
		enddo
       enddo    
c --- open new text file for writing events every 50 000 events
      if(mod(necrit, 50000).eq.0) then
		nfiles = nfiles+1
		close(88)
		if(nfiles.lt.10) then
			write(filename,'(a,i1,a)') 'arbre_root_', nfiles, '.txt'
		else
			write(filename,'(a,i2,a)') 'arbre_root_', nfiles, '.txt'
		endif
		print *,'Opening file for writing : ', filename
      	open(unit=88,file=filename,status='new')
	endif
		                                                                        
      endif !if(imult > 0) then
                                                                        
                                                                        
      return                                                            
      end                                                               
                                                                        
c-------------------------------------------------------------------    
c --- Routine appelee en debut de lecture d'un run                      
c-------------------------------------------------------------------    
      subroutine INI_RUN()                                              
                                                                        
      include 'veda_rel.incl'                                           
      include 'veda_5.incl'                                             
      include 'veda_6.incl'                                             
c      include 'anin_1.incl'                                             
      common /rootwrite/ necrit, nfiles
	  character*60 filename                                 
                                                                        
c --- Faites ici les initialisations de runs                            
      print *,'Debut du traitement du run ',numerun,'...'               
      	  
c --- open first text file for writing events
	nfiles = 1
	write(filename,'(a,i1,a)') 'arbre_root_', nfiles, '.txt'
	print *,'Opening file for writing : ', filename
      open(unit=88,file=filename,status='new')                                                                        
c --- write number of run at beginning of text file
	  write(88,*) numerun
	  
      necrit=0
      
      xmnuc=931.5                                                       
      ptot=sqrt((esura+xmnuc)**2-xmnuc**2)*aproj                        
      ztot=zcib+zproj                                                   
      vproj=ptot/(aproj*(esura+xmnuc))                                  
      zvtot=zproj*vproj                                                 
      vcm=aproj*vproj/(aproj+acib)                                      
      ecm=aproj*acib*esura/(aproj+acib)**2  
      iap=aproj                            
      izp=zproj                            
      iac=acib                            
      izc=zcib                            
      print 10,esura,projectile,aproj,zproj,cible,acib,zcib             
      print 20,ptot,vproj,zvtot                                         
      print 30,vcm,ecm
      

      return                                                            
10    format(1x,'E/A=',f10.2,/,1x,'Projectile :',a3,2(1x,f4.0),/,       
     1                         1x,'Cible      :',a3,2(1x,f4.0))         
20    format(1x,'Ptot  :',f15.3,'  Vproj/C :',f6.4,'    ZVproj :',      
     1                                             f15.3)               
30    format(1x,'Vcm/C :',f6.4,'    Ecm disp :',f10.4,' MeV/A.',/)      
40    format(2(1x,i3),3(1x,f10.2),' 0 0. 0 0 0 ',6(1x,'0.'),' 0')
      end                                                               
                                                                        
c-------------------------------------------------------------------    
c --- Routine appelee en fin de lecture d'un run                        
c-------------------------------------------------------------------    
      subroutine FIN_RUN()
                                                                        
      include 'veda_rel.incl'                                           
      include 'veda_5.incl'                                             
      include 'veda_6.incl'                                             
      common /ttime/ ttot,tmin,tmax                                     
      common /rootwrite/ necrit , nfiles                                    
                                                                        
      print *,'Fin de traitement du run ',numerun,'...'                 
      print *,'Evenements traites       :',nevt,'/',necrit
      print *,'Taux d''aceptes           :',necrit*100./nevt                        
      print *,'Temps moyen de traitement:',ttot/float(nevt)             
      print *,'Temps min                :',tmin                         
      print *,'Temps max                :',tmax                         
      print *,'Number of files written :', nfiles                                                       
      print *,' '                                                       
c write total number of events at end of last file
	write(88,*) 'TOTAL=',necrit                                                                        
      close(88)
	  
      return                                                            
      end                                                               

c-------------------------------------------------------------------    
c --- Routine appelee en fin de lecture d'une tache                     
c-------------------------------------------------------------------    
      subroutine close_hb                                               
                                                                        
      include 'veda_rel.incl'                                          
      include 'veda_5.incl'                                            
      include 'veda_6.incl'                                            

c --- Inserez ici les fermetures de fichiers (Hbooks, ... )           
                                                                        
                                                                             
      return                                                          
      end                                                              
