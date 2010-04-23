c******************************************************************************
c*                                                                            *
c*   Ce fichier contient toutes les routines necessaires a la correction      *
c*   de l'energie silicium ecrite sur DST                                     *  
c*                                                                            *
c 25/6/97 : M-F Rivet , J-L Charvet                                           *                                         *
c******************************************************************************
c 
c    La routine de correction est : COR_CAN (ipart)
c
c    Elle fait appel a des routines qui appartiennent a VEDA et qui ont ete 
c    recopiees dans ce fichier
c                                                                           
c   Subroutines de VEDALPC.F
c
c           INICALF (numerun,imois,iannee,irc)
c	    ALPHA2(Ic,Im,Sigg,Sipg,CsIR,Esi,Ecsi,Irc)
c  function AJUSTE (x,xval,nbpar,z)                                  
c  function POLYN (xval,nbpar,id,ideg,x)                             
c           STOCK_PARAM(ic,im,its,nbparf,tabl,irc)                 
c           ENRFCSI(Ic,Im,Iz,Ia,Sigg,Sipg,CsiR,Ia1,Esi,Ecsi,Irc)
c
c   Subroutines de VEDAORSAY.F
c
c           INITSI(iannee,imois,nurun,icode)
c           DEFDION(NC ,NM ,Cg ,Cp ,R ,Z ,E ,DEF, ISTATUS)
c
c******************************************************************************

      subroutine cor_can(ipart)
      
c******************************************************************************
c*     *** Sous-programmes pour corriger l'energie deposee dans le silicium, a cause
**  de l'inversion de la correction de piedestal dans defdion. Campagne 1993
***    M.F. Rivet juin 1997.
*
*  La correction est effectue si la variation de piedestal est superieure
*  a une valeur correction_piedestal_SiPG transmise par veda_rel.incl
* 
*  correction_piedestal_SiPG=1 par defaut : correction systematique
*  mais peut-etre modifie (periode de test par MFR ou JLC - 10/7/97) 
*  dans INIRUN par exemple
*----------------------------------------------------------------------------
** Dans le programme appelant (kali.f), il doit y avoir des lignes du type:
**
**  Lecture du fichier d'etalonnage arcal1.vedasac
**      
**      if(canal(i,4) .gt. 3800) then
**        call cor_can(i)
*       endif

c  Si-CsI (INI_SI_CSI), la routine d'initialisation de la calibration   
c  des Siliciums (INITSI) et la routine d'initialisation de la routine  
c  de calcul de pertes d'energie (LOSS_INIT). Tous les arguments doi-   
c  inicalf
      
      implicit none

      include      'veda_6.incl'
      include      'veda_rel.incl'
      include      'veda_wri.incl'

      integer*2    kbyt(2),code16
      integer*4    ien,code4(4)
      integer*4    ipart
      integer*4    icod, j, k, ico
      integer*4    nsito,nsito1,nsico                                        
      real*4       cancor
      real         enert,dde1,dde2,dde3
      common/cor97/cancor                 ! juin 1997
      common /CORESIL/nsito,nsito1,nsico

      integer*4    runinf(11,6), runsup(11,6)
      data runinf/101,216,336,353,570,688,770,4*0,
     &            101,206,213,219,231,250,343,359,570,688,770,
     &            101,155,339,570,688,770,5*0,
     &            101,336,570,655,688,770,5*0,
     &            101,331,570,688,780,6*0,
     &            101,570,688,8*0/
      data runsup/215,335,352,401,683,765,804,4*0,
     &            205,212,218,230,249,342,358,401,683,765,805,
     &            150,338,401,683,765,804,5*0,
     &            335,401,654,683,765,804,5*0,
     &            330,401,683,779,804,6*0,
     &            401,683,804,8*0/
      real*4      delta(11,6)
      data delta /0. ,1.0,2.5,1.5,3.0,-1.0,2.0,4*0.,
     &            0. ,1 .0,2.0,1.0,0. ,1.0,2.0,1.0,8.0,3.0,8.0,
     &            0. ,2.0,3.0,1.0,-4.0,0.,5*0.,
     &            0. ,1.0,3.0,1.0,0. ,1.0,5*0.,
     &            0. ,1.0,2.0,1.0,2.0,6*0.,
     &            0. ,1.0,0. ,8*0./
*
      real*4      zz, esi, ecsi, e, cg, sigg, def
      integer*4   irc, ia1, istatus
c      Equivalence (ien,kbyt)

c ** test lecture
c      if(nevtlus.eq.1)then
c      write (6,'('' Run = '',i6)')numerun
c      do icod = 1,6
c        do j=1,11
c          write(6,'('' codeur'',i2,'' runs'',i4,'' a'',i4,''delta_c'',
c     &        f6.1)') icod,runinf(j,icod),runsup(j,icod),delta(j,icod)
c        enddo
c      enddo
c      endif 
****
***  correction du canal petit gain du Si
      icod = 1 + ((icou(ipart)-2) * 24 + imod(ipart) - 1) / 32
      cancor = 0.
      do j = 1,11
        if(numerun.ge.runinf(j,icod) .and.
     &                 numerun.le.runsup(j,icod))then
           cancor = delta(j,icod)
        endif
      enddo
      
* correction suivant la valeur de cancor 

      if(abs(cancor) .gt. correction_piedestal_SiPG-0.5) then
      
** arret dans le silicium

        if(code(ipart).ge.4.and.code(ipart).le.8) then

        nsico=nsico+1.
        zz=z(ipart)
        cg = canal (ipart,4)
        call DEFDION(icou(ipart), imod(ipart), cg,canal (ipart,5) ,
     &               canal(ipart,7), zz, e, def, ico)
          if((e.lt.-1.or.ico.eq.3).and.nevtlus.le.10000)then
               write(*,'('' E (DEFDION)<-1.: evt,ic,im,ener,ico='',
     &                   i6,2i3,f7.1,i3)')
     &                   nevtlus,icou(ipart),imod(ipart),e,ico
          endif
        if(ico.eq.15)e=0.
        de2(ipart) = e
           ien=nint(de2(ipart)*10.) 
	   kbyt(1)=ien/(2**16)
	   kbyt(2)=ien-(kbyt(1)*2**16)                                       
           ievt(jbuff(ipart)+9)=kbyt(1)                                
           ievt(jbuff(ipart)+10)=kbyt(2)
	                                  
        code_energie(3,ipart)=ico
        do k=1,4
        code4(k)=code_energie(k,ipart)
        enddo
        call CODAGE (code4,code16)
        ievt(jbuff(ipart)+5)=code16 
        
       endif

** transmission: appel ss-prog. Olivier Lopez
c  cas ou z>2 calibration CsI avec Silicium

       if(code(ipart).le.3.or.
     &          code(ipart).eq.9.or.code(ipart).eq.10)then !!!! CsI
        
         if(z(ipart).le.2)return
         
         nsico=nsico+1.
         sigg = canal (ipart,4)
         call ENRFCSI(icou(ipart), Imod(ipart), z(ipart), a(ipart),
     &      sigg, canal(ipart,5), canal(ipart,7), Ia1, Esi, Ecsi, Ico)
     
            if(ecsi.lt.0.and.ico.le.12)then
                 if(ico.eq.1)ico=10
                 if(nevtlus.le.10000)then
                   write(*,'('' ENRFCSI < 0 : evt,ic,im,iz,ener,ico='',
     &                   i6,2i3,i4,f12.2,i3)')
     &             nevtlus,icou(ipart),imod(ipart),z(ipart),ecsi,ico
                 endif
            endif
            if(ico.ge.13)ecsi=0.
            de2(ipart) = esi
            ien=nint(de2(ipart)*10.)                                        
	    kbyt(1)=ien/(2**16)
	    kbyt(2)=ien-(kbyt(1)*2**16)                                       
            ievt(jbuff(ipart)+9)=kbyt(1)                                
            ievt(jbuff(ipart)+10)=kbyt(2)
	                                     
            de3(ipart) = ecsi
            ien=nint(de3(ipart)*10.)                                        
	    kbyt(1)=ien/(2**16)
	    kbyt(2)=ien-(kbyt(1)*2**16)                                       
            ievt(jbuff(ipart)+11)=kbyt(1)                                
            ievt(jbuff(ipart)+12)=kbyt(2)
	                                     
            code_energie(2,ipart)=ico
            do k=1,4
            code4(k)=code_energie(k,ipart)
            enddo
            call CODAGE (code4,code16)
            ievt(jbuff(ipart)+5)=code16                               
            
        endif   

      endif

      return
      end


ccc-----------------------------------------------------------------------------
                                                                            
      subroutine inicalf(numerun,imois,iannee,irc)  
                          
c--------------------------------------------------------------------   
c  Cette routine initialise la routine de calibration des fragments     
c  dans les modules CsI de la couronne 2 a 9. Elle utilise les          
c  routines d'initialisation des fonctionnelles d'identification        
c  Si-CsI (INI_SI_CSI), la routine d'initialisation de la calibration   
c  des Siliciums (INITSI) et la routine d'initialisation de la routine  
c  de calcul de pertes d'energie (LOSS_INIT). Tous les arguments doi-   
c  vent etre du type Integer*4. Les arguments demandes par la routine   
c  sont :                                                               
c                                                                       
c     - Numerun  : le numero du run courant.                            
c     - Imois    : le mois de la campagne ( fourni dans VEDA_5 INCL)    
c     - Iannee   : l'annee de la campagne (   -     -     -     -  )    
c                                                                       
c  En sortie, on recupere :                                             
c                                                                       
c     - Irc     : code de retour dont la valeur peut etre               
c                                                                       
c                 => = 0 , Initialisation correcte.                     
c                    = -1, Initialisation incorrecte Si_CsI             
c                          Les valeurs d'energie CsI retournees         
c                          correspondront aux valeurs silicium          
c                          vrais et non pas aux centroides vrais.       
c                                                                       
c-------------------------------------------------------------------    
                                                                        
      implicit none 
      include 'veda_1.incl'                                             
      include 'veda_2.incl'                                             
      include 'veda_3.incl'                                             
      include 'veda_11.incl'
                                                                        
      include 'vedalpc1.incl'                                           
      include 'vedalpc2.incl'                                           
      include 'vedalpc4.incl'                                           
                                                                        
      integer*4 numerun,imois,iannee,irc,iniflag,ifais                  
      logical   vazy                                                    
      character*80 line,namefil,file                                        
      common/cafrag29/iniflag,ifais                                     
                                                                        
c --- Definitions des variables pour fichier calibrations
                                            
       Logical*1    Eof,ok_newcalib
	Real*4 Calxfra(2:9,24,14,3)
	Common/CALFRA2/Calxfra
      integer*4  init2,irc2,ibad,iline,it,ityp,imod
      DATA init2/0/
      real*4      p1,p2,p3
      
      
      irc=0                                                             
      iniflag=0                                                         

      Print *,'----------------------- INICALF ---------------------'

      Print *,' INICALF -> Init. Calibration Fragment CsI (2-9)'
      if(init2.gt.1) then                                                                  
         print *,' INICALF -> Initialisation deja effectuee !!'
         return     
      end if                                                            
      init2=init2+1                                               
                                                                        
c --- Initialisation de la routine de perte d'energie                   
                                                                        
      call LOSS_INIT(5,irc2)                                            
      if(irc2.ne.0) then                                                
         iniflag=2                                                      
         irc=-3                                                         
      end if                                                            
                                                                        
      if(numerun.gt.559) then                                           
        ifais=2                                                         
      else                                                              
        ifais=1                                                         
      end if                                                            
                                                                        
	Print *,'Appel a l''init calibration Fragments CsI cour. 2 a 9'
       Print *,' ********** Mise a jour le 24 Mai 1996 *************'
       Print *,'------------------- INICALF  ------------------------'
       
	Eof=.False.
	ibad=0
	iline=0
	
c --- Choix du fichier suivant le nuemro de run (Argon / Xenon-Gadolinium)

	if(numerun.ge.101.and.numerun.le.558) then
	
	   File=vedafil(1:long_path)//'calimf_argon.data'
          
       elseif(numerun.ge.559.and.numerun.le.764) then
       
	   File=vedafil(1:long_path)//'calimf_xenon.data'
          
       elseif(numerun.ge.770.and.numerun.le.804) then
       
	   File=vedafil(1:long_path)//'calimf_xenon.data'
          
       else
       
       Print *,'Le run ',numerun,' n''est pas disponible ...'
       Print *,'Verifiez son numero ou contacter O. LOPEZ'
       Print *,'pour plus de details'
       Print *,'------------------- INICALFRA -----------------------'
       Irc=-1
        
       end if

c --- Ouverture du fichier de calibration considere ...

	Open(80,file=file,status='OLD',form='FORMATTED')
	
c --- Initialisation necessaire de la base de parametres ...

	do ic=2,9
	   do im=1,24
	      do it=1,14
	         calxfra(ic,im,it,1)=-1000.
	         calxfra(ic,im,it,3)=-1000.
	         calxfra(ic,im,it,3)=-1000.
	      end do
	   end do
	end do

c --- Boucle de lecture du fichier ...
	
       do while(.not.eof)
       
          read(80,1,end=11) imod,ityp,p1,p2,p3
          iline=iline+1
          if(imod.gt.0) then
             if(ityp.ge.-1.and.ityp.le.12) then
                it=ityp+2
                ic=imod/100
                im=mod(imod,100)
                if(ic.ge.2.and.ic.le.9.and.im.ge.1.and.im.le.24) then
                   calxfra(ic,im,it,1)=p1
                   calxfra(ic,im,it,2)=p2
                   calxfra(ic,im,it,3)=p3
                else
                   ibad=ibad+1
                end if
             end if
          else
             ibad=ibad+1
          endif
          
21        continue

       end do
       
11     continue

       close(unit=80)

c --- Impression des commentaires sur la lecture du fichier ...

       ibad=ibad-1
       if(ibad.lt.0) ibad=0
       
       Print *,iline,' lignes lues dans le fichier ',file
       Print *,ibad,' lignes mal decodees...'
       
       if(ibad.gt.0) then
       
          irc=-1
          print *,'Probleme possible pour l''initialisation ...'
          print *,'Verifiez le fichier ',file
          
       else
       
c          Print *,'Fichier ',file(1:ifile),' correctement lu'
           Print *,'Fichier correctement lu'
         irc=0
          
       end if           
                   
       Print *,'------------------- INICALFRA -----------------------'

1 	Format(1x,i5,1x,i3,2x,3(f12.2,1x))
       
	return
	end                 
                   

c----------------------------------------------------------------------------

	Subroutine ALPHA2(Ic,Im,Sigg,Sipg,CsIR,Esi,Ecsi,Irc)
	
       include 'vedalpc1.incl'                                           
       include 'vedalpc2.incl'                                           
       include 'vedalpc4.incl'                                           
                                                                        
       include 'vedasac1.incl'   
       include 'veda_1.incl'                                             
       include 'veda_2.incl'                                             
       include 'veda_3.incl'                                             
	       
       real*4  epaisseur(24,2:9) 
                                              
       Real*8 De,Eres,Az,Am,Einc,Eps,Ep,amasr
       Real*8 Eres1,Ein1
       Real*4 Sigg,Sipg,CsiR,Esi,Ecsi,def1
       
c --- Epaisseurs des diodes Silicium                                    
                                                                        
       data epaisseur/302.,0.,300.,0.,305.,0.,304.,0.,298.,0.,296.,      
     &  0.,299.,0.,296.,0.,299.,0.,300.,0.,299.,0.,299.,0.,              
     &  302.,302.,300.,300.,305.,305.,304.,304.,298.,298.,296.,296.,     
     &  299.,299.,296.,296.,299.,299.,300.,300.,299.,299.,299.,299.,     
     &  301.,301.,301.,301.,301.,301.,300.,300.,300.,300.,302.,302.,     
     &  295.,295.,298.,298.,299.,299.,296.,296.,297.,297.,297.,297.,     
     &  301.,301.,301.,301.,301.,301.,300.,300.,300.,300.,302.,302.,     
     &  295.,295.,298.,298.,299.,299.,296.,296.,297.,297.,297.,297.,     
     &  299.,299.,302.,302.,303.,303.,303.,303.,300.,300.,306.,306.,     
     &  306.,306.,303.,303.,307.,307.,300.,300.,306.,306.,303.,303.,     
     &  299.,299.,302.,302.,303.,303.,303.,303.,300.,300.,306.,306.,     
     &  306.,306.,303.,303.,307.,307.,300.,300.,306.,306.,303.,303.,     
     &  303.,303.,301.,301.,303.,303.,299.,299.,303.,303.,298.,298.,     
     &  305.,305.,303.,303.,302.,302.,303.,303.,302.,302.,306.,306.,     
     &  303.,303.,301.,301.,303.,303.,299.,299.,303.,303.,298.,298.,     
     &  305.,305.,303.,303.,302.,302.,303.,303.,302.,302.,306.,306./     
                                                                        
c --- Valeurs par defaut

       Irc=15
       Esi=0.
       Ecsi=0.
       Iter=50000
       Eps=1.e-5
       
c --- Epaisseur Silicium  :                                             
                                                                        
      ep=epaisseur(im,ic)                                          
      if(ep.le.0.) then                                                 
         ep=300.                                                        
      end if     
      
      amasr=0.                                                       
      call SET_THICKNESS(ep,amasr,1)   
                                             
c --- On calcule le signal Silicium pour un alpha de signal moitie       
                                                                        
        sigpied=Base_indra(ic,Im,4,I_pied)                        
        sippied=Base_indra(ic,Im,5,I_pied)                        
        sigg2=(sigg-sigpied)/2.+sigpied                              
        sipg2=(sipg-sippied)/2.+sippied    
                                  
        z2=2.         
        iz2=2
        ia2=4                                                 
                                                                        
c --- Calibration des alphas ...

        esi2=0.
        call DEFDION(Ic,Im,sigg2,sipg2,csir,z2,esi2,def1,istat)
c   DEF1   : defaut d'ionisation calcule (modif. 31/5/96)
          
        if(esi2.le.0.) then                              
           ecsi=-100000.                                                    
           irc=13                                                     
           return                                                     
        end if                                                        
                                                                        
c --- Calcul de la perte d'energie pour un Beryllium stable (A=9)       
                                                                        
        az=4.                                                          
        am=9.                         
         
c --- La contribution du Beryllium est multipliee par 4
                                          
        de=esi2*3.75                                                    
        esi=esi2*3.75                                                    
                                                                        
        call DELTA(de,az,am,1,einc,eres,Iter,ein1,eres1,Iter1,eps,Icode) 
            
c --- Correction isotopique                                             
                                                                        
        rap=8./am                                               
        eres=eres*rap  
        ecsi=eres  
        
c --- Estimation de la perte d'energie si pas de valeur ...

        if(icode.gt.1) then
        
           p1=1.224
           p2=4572.9
           p3=60.46
           eres=p2/(esi-p1)-p3
           if(eres.lt.0..or.eres.gt.3500.) then
            
c --- Estimation grossiere  de la vraie perte Silicium ...

              p1=1.88
              p2=8104.
              p3=191.
              esi=p1+p2/(csir+p3) 
               
c --- On recommence pour l'energie CsI ...

              p1=1.224
              p2=4572.9
              p3=60.46
              eres=p2/(esi-p1)-p3
               
c --- On arrete les frais car y'en a marre !!!

              if(eres.lt.0..or.eres.gt.3500.) then
                          
                 ecsi=-100000.
                 irc=14
                 return
                  
              end if
               
           endif
           irc=1
           ecsi=eres                                                   

      end if                                                            
          
      return                                                      
      End 

C====================================================================== 
      FUNCTION ajuste (x,xval,nbpar,z)                                  
C---------------------------------------------------------------------- 
      implicit real*4 (a-h,o-z)                                         
      dimension xval(nbpar)                                             
c                                                                       
      a2 = polyn(xval,nbpar,1,3,z)                                      
      a3 = polyn(xval,nbpar,5,3,z)                                      
      a4 = polyn(xval,nbpar,9,3,z)                                      
                                                                        
c --- Cas de l'argon (12 parametres)                                    
                                                                        
      if(nbpar.eq.12) then                                              
                                                                        
        ajuste =  a2 + a3/(a4+x)                                        
                                                                        
      else                                                              
                                                                        
c --- Cas du Xenon/Gd (14 parametres)                                   
                                                                        
        a5=xval(13)*z**4                                                
        a6=xval(14)*z**4                                                
        ajuste = a2 + a3/(a4+x) + a5/(a6+x*x)                           
                                                                        
      end if                                                            
C                                                                       
      return                                                            
      end 
       
C====================================================================== 
      FUNCTION polyn (xval,nbpar,id,ideg,x)                             
C---------------------------------------------------------------------- 
      implicit real*4 (a-h,o-z)                                         
      dimension xval(nbpar)                                             
c                                                                       
      pol=xval(id)                                                      
c     if(abs(x).lt.1.e-30) x=0.                                         
      do i=1,ideg                                                       
       pol=pol*x+xval(id+i)                                             
      enddo                                                             
      polyn=pol                                                         
C                                                                       
      return                                                            
      end  
                                                                   
C====================================================================== 
                                                                   
C============================================================================= 

      subroutine STOCK_PARAM(ic,im,its,nbparf,tabl,irc)                 
                                                                        
      include 'vedalpc1.incl'                                           
      include 'vedalpc2.incl'                                           
      include 'vedalpc3.incl'                                           
      real*4 tabl(nbparfitmax)                                          
      integer*4 i,surv,nbparf,irc                                       
                                                                        
      surv=0                                                            
      nbparf=int(parfit(ic,im,its,0))                                   
      do i=1,nbparf                                                     
       tabl(i)=parfit(ic,im,its,i)                                      
      if(tabl(i).gt.0.0000001.or.tabl(i).lt.-.00000001)surv=surv+1      
      enddo                                                             
      if(surv.gt.0) then                                                
         irc=0                                                          
      else                                                              
         irc=1                                                          
      end if                                                            
                                                                        
      return                                                            
      end                                                               

c-----------------------------------------------------------------------------

	Subroutine ENRFCSI(Ic,Im,Iz,Ia,Sigg,Sipg,CsiR,Ia1,Esi,Ecsi,Irc)
	                   
       include 'vedalpc1.incl'                                          
       include 'vedalpc2.incl'                                          
       include 'vedalpc3.incl'                                          
       include 'vedalpc4.incl'                                          
                                                                        
       include 'vedasac1.incl'   
       include 'veda_1.incl'                                            
       include 'veda_2.incl'                                            
       include 'veda_3.incl'                                            
       real*4  epaisseur(24,2:9) 
       Integer*4  Iloop 
       Logical*1 Use_sicsi                                         
                                              
       Real*8 De,Eres,Az,Am,Einc,Eps,Ep,amasr
       Real*8 Eres2,Einc2
       Real*4 Sigg,Sipg,CsiR,Esi,Ecsi
       
       real*4 tabl1(nbparfitmax)                                        
       real*4 tabl2(nbparfitmax)                                        

	Real*4 Calxfra(2:9,24,14,3)
	Common/CALFRA2/Calxfra

c --- Epaisseurs des diodes Silicium                                    
                                                                        
       data epaisseur/302.,0.,300.,0.,305.,0.,304.,0.,298.,0.,296.,     
     &  0.,299.,0.,296.,0.,299.,0.,300.,0.,299.,0.,299.,0.,             
     &  302.,302.,300.,300.,305.,305.,304.,304.,298.,298.,296.,296.,    
     &  299.,299.,296.,296.,299.,299.,300.,300.,299.,299.,299.,299.,    
     &  301.,301.,301.,301.,301.,301.,300.,300.,300.,300.,302.,302.,    
     &  295.,295.,298.,298.,299.,299.,296.,296.,297.,297.,297.,297.,    
     &  301.,301.,301.,301.,301.,301.,300.,300.,300.,300.,302.,302.,    
     &  295.,295.,298.,298.,299.,299.,296.,296.,297.,297.,297.,297.,    
     &  299.,299.,302.,302.,303.,303.,303.,303.,300.,300.,306.,306.,    
     &  306.,306.,303.,303.,307.,307.,300.,300.,306.,306.,303.,303.,    
     &  299.,299.,302.,302.,303.,303.,303.,303.,300.,300.,306.,306.,    
     &  306.,306.,303.,303.,307.,307.,300.,300.,306.,306.,303.,303.,    
     &  303.,303.,301.,301.,303.,303.,299.,299.,303.,303.,298.,298.,    
     &  305.,305.,303.,303.,302.,302.,303.,303.,302.,302.,306.,306.,    
     &  303.,303.,301.,301.,303.,303.,299.,299.,303.,303.,298.,298.,    
     &  305.,305.,303.,303.,302.,302.,303.,303.,302.,302.,306.,306./    
                                                                        
       common/cafrag29/iniflag,ifais                                    

c --- Valeurs par defaut

       Irc=15
       Esi=0.
       Ecsi=0.
       Iter=50000
       Iter2=50000
       Eps=1.e-5
       Use_sicsi=.True.
       
c --- On retire tous les appels non corrects ...
       
       if(ic.lt.2.or.ic.gt.9) then
          return
       endif
       if(im.lt.1.or.im.gt.24) then
          return
       endif
       if(Iz.lt.3) then
          return
       endif
       if(abs(Ia).lt.6) then
        return
       endif
       if(CsiR.lt.5..or.CsiR.gt.4095) then
          return
       end if
       
c --- Epaisseur Silicium  :                                             
                                                                        
      ep=epaisseur(im,ic)                                          
      if(ep.le.0.) then                                                 
         ep=300.                                                        
      end if     
      
      amasr=0.                                                       
      call SET_THICKNESS(ep,amasr,1)                                          

c --- On convertit la particule en type compatible fichier "PATCH"

       ia1=abs(ia)
       it=0
       if(iz.eq.3.and.ia1.eq.6) then
          it=1
       elseif(iz.eq.3.and.ia1.eq.7) then
          it=2
       elseif(iz.eq.3.and.ia1.eq.8) then
          it=3
       elseif(iz.eq.4.and.ia1.eq.7) then
          it=4
       elseif(iz.eq.4.and.ia1.eq.9) then
          it=5
       elseif(iz.eq.4.and.ia1.eq.10) then
          it=6
       elseif(iz.ge.5) then
          it=iz+2
       endif
       
c --- Cas du Beryllium 8 traite independamment ...

	if(iz.eq.4.and.ia.eq.8) then
	
	   Call ALPHA2(Ic,Im,Sigg,Sipg,CsIR,Esi,Ecsi,Irc)
	   if(ecsi.lt.0.) then
	      Irc=14
	      Return
	   end if
	   Irc=1
	   Return
	End if
	       
c --- Cas des calibrations "PATCHs", lecture des parametres Esi - CsIR
          
       if(it.ge.1.and.it.le.6) then
   
          p1=Calxfra(Ic,Im,It,1)
          p2=Calxfra(Ic,Im,It,2)
          p3=Calxfra(Ic,Im,It,3)
          
       else
       
          p1=-1000.
          p2=-1000.
          p3=-1000.
           
       end if

c --- Si la calibration "PATCH" est disponible ...
       
       If(p1.ne.-1000.) then
       
          Esi=p1+p2/(csir+p3)
          
       else
       
          z=float(iz)
          
c --- Utilisation des lignes de crete de l'identification Si-CsI
          
          if(use_sicsi) then
          
             z=float(iz)
             call STOCK_PARAM(Ic,Im,1,nparf1,tabl1,irc1)           
             call STOCK_PARAM(Ic,Im,2,nparf2,tabl2,irc1) 
             if(irc1.eq.0.and.csir.gt.100.) then           
                siggr=ajuste(csir,tabl1,nparf1,z)                       
                sipgr=ajuste(csir,tabl2,nparf2,z)
             else
                siggr=sigg+(1.-2.*rndm(xx))/2.
                sipgr=sipg+(1.-2.*rndm(xx))/2.
             end if
             
c --- Sinon, on prend les signaux vrais ...

          else                         

             siggr=sigg+(1.-2.*rndm(xx))/2.
             sipgr=sipg+(1.-2.*rndm(xx))/2.
             
          end if

          if(siggr.gt.4090.or.siggr.lt.-1.) then
             siggr=4095.
          endif
                    
          istat=0
          esil=0.
          call DEFDION(ic,im,siggr,sipgr,csir,z,esil,def,istat)      
          if(istat.gt.2.or.esil.le.0.) then   
             ecsi=0.                                                  
             irc=13                                                   
             return                                                   
          end if  
          esi=esil
          
       end if
         
       if(esi.lt.0.) then
          ecsi=-100000.
          Irc=13
          Return
       end if
       
       ecsi=0.
       de=esi
       az=dfloat(iz)
       am=dfloat(abs(ia))
       Eres=0.   
       
       call DELTA(de,az,am,1,einc,eres,Iter,einc2,eres2,
     &            iter2,eps,Icode) 
     
       if(icode.eq.3) then                                        
          irc=14                                                   
          return                                                   
       elseif(icode.eq.2) then 
           eps=1.e-3                                   
           call DELTA(de,az,am,1,einc,eres,Iter,einc2,eres2,
     &               iter2,eps,Icode) 
          if(icode.eq.1) then 
             Irc=3
             Ecsi=0.
             return
          elseif(icode.gt.2) then
             Irc=14
             Ecsi=0.
             Return
          elseif(icode.eq.2) then
             Irc=2
             Return
          else
             Irc=1
             Return
          endif   
       elseif(icode.eq.1) then                                    
          irc=3                                                    
          ecsi=0.                                                  
          return                                                   
       else                                                       
          ecsi=eres                                                
          if(ecsi.lt.0.) then
             irc=14
             return
          endif
       endif
       
       Irc=1
       
       return
       end

c-----------------------------------------------------------------------------       
c ================ ENERSI FORTRAN ======================================
c
c
      Subroutine   initsi(iannee,imois,nurun,icode)
c            icode : code en retour =0 lecture effectuee
c                                   =1 date non valide
      implicit     none
      
      include      'veda_11.incl'
      Character*80  namefil
      integer*4    iannee ,imois ,nurun ,icode
      integer*4    ian, i ,i1 ,i2 ,is ,iu ,nr1 ,np ,ipied
      integer*4    nr ,nc ,nm ,ntyp ,lab
      real*4       som ,centr ,sig ,sk
      real*4       p(9,24,0:2) ,g(9,24,0:2) ,dedv(9,24) ,cmr
      real*4       x2 ,x1 ,x0, x3
      character    fich*80 ,ligne*8  ,num*4 ,nom*20
      logical      fin, exist
      common/etlaziz/    g, p, dedv
c ------------------------------------------------
      ian = mod(iannee,100)
      if(ian.eq.93.and.(imois.eq.3.or.imois.eq.4)) then
        nr = nurun
        icode = 0
ccc
C   -------------------      LECTURE FICHIER ETALONNAGE    -------------
ccc
      do nm = 1,24
        do nc = 2,9
          dedv(nc,nm) = -1.
        enddo
      enddo
      iu=25
c      fich = 'dskp:[project.indra.manip.alpha]gene.nalpas'
      FICH = 'arcal1.vedasac'
c      open(unit=iu,file=fich,status='old',readonly)
      namefil=vedafil(1:long_path)//fich
      OPEN(UNIT=IU,FILE=namefil,STATUS='OLD')
      do i = 1, 8
         read(iu,'(a)') ligne
      enddo
      fin = .false.
      dowhile(.not.fin)
        read(iu,'(i5,1x,a10,1x,2i3,i4,1x,4e13.6)',end=300,err=300)
     1                 np,nom,nc,nm,ntyp,x2,x1,x0,x3
          if(x3.eq.1.) x3 = -1.
          IF(NTYP .EQ. 5)THEN
            DEDV(NC,NM) = X3
            P(NC,NM,0) = X0
            P(NC,NM,1) = X1
            P(NC,NM,2) = X2
          ELSEIF(NTYP .EQ. 4)THEN
            G(NC,NM,0) = X0
            G(NC,NM,1) = X1
            G(NC,NM,2) = X2
          endif
c      write(*,'(i4,1x,3i4,3x,a12,3e14.6)')
c     1                               np,nc,nm,ntyp,nom,x2,x1,x0
c      write(*,*) dedv(nc,nm)
      enddo
c --- Modif JLC du 1/12/94 qui prend en compte la correction de
c     gain du Silicium 6-13
      if(nurun.ge.113.and.nurun.le.302)then
           dedv(6,13)=1903.43
      endif
c --- Fin Modif 1/12/94
 
300      close(iu)
      else
         icode = 1
      endif
      return
      end

ccc-----------------------------------------------------------------------------
*** juillet 1997:  adaptation du programme defdion 1993 pour corrections
*** dans les bandes DST
*** correction delta dans le bon sens!!!
c
*** Ce sous-programme est modifie le 31 mai 1996, pour calculer un
*** defaut d'ionisation, que la particule s'arrete ou non dans le Si.
*** Le defaut d'ionisation calcule est renvoye en argument, pour tests
*** eventuels.   MFR
      SUBROUTINE DEFDION(NC ,NM ,Cg ,Cp ,R ,Z ,E ,DEF, ISTATUS)
*
CE PROGRAMME CALCULE L'ENERGIE DEPOSEE DANS LE SILICIUM ET              aut de
CALCULE LE DEFAUT DE collection pour les ions de Z > 10.
c  Entree :
c          nc,nm : numeros de couronne et du module
c          Z     : charge de l'ion
c          Cg,Cp : canaux grand gain et petit gain du codeur Si
c          R     : Canal composante rapide du CsI
*      
c  RETOUR :
c          E     : energie vraie de l'ion (calcul du defaut d'ionisation)
C                  (0.5 % D'ERREUR DE CALCUL)
c          DEF   : defaut d'ionisation calcule
C          STATUS: CODE DE RETOUR  =1 : PAS DE DEFAUT D'IONISATION
C                                  =2 : OK DEFAUT D'IONISATION CALCULE
C                                  =3 : PAS DE CONVERGENCE EN ENERGIE
C                                  =4 : ENERGIE NEGATIVE POUR UNE PART.
C                                        SANS DEF. D'IONISATION
c                                  =15: probleme detecteur
*------------------------------------------------------------------------
      implicit  none
      real*4    d(2,3,2:9) ,zl(3,2:9) ,ESI ,Z ,E ,E0 ,a ,b ,eps ,phd
      INTEGER*4 NG(24,2:9), NR, ISTATUS, N, ITOUR
      logical   conv
      real*4    cg ,cp ,r ,delta
      common/cor97/delta                  ! juin 1997
*      real*4    ref(9,24) ,gene(9,24)    ! juin 1997
      real*4    g(9,24,0:2), p(9,24,0:2) ,dedv(9,24)
      common/etlaziz/    g, p, dedv
*      common /inisio2/ nr,gene,ref       ! juin 1997
      integer*4     nc,nm
      real*4        cpp
      real*4        DEF                     !!! 31/05/96
      
      real*4        gg_pg,rndm,xxx

c******
cc detecteurs defectueux
c???      data det/4.04,4.09,4.10,5.09,5.10,6.01,6.02,6.23,7.01,7.02,8.02/???
c*****
cc  Z limite a partir duquel on calcul le defaut d'ionisation pour chaque groupe
      data ZL/24*10./
c****** Les groupes
      data ng/1,1,1,1,1,1,1,1,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,
     #        1,1,1,1,1,1,1,1,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,
     #        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,1,1,
     #        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,1,1,
     #        1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,
     #        1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,
     #        1,1,1,1,1,1,3,3,2,2,2,2,2,2,3,3,2,2,2,2,3,3,3,3,
     #        1,1,1,1,1,1,3,3,2,2,2,2,2,2,3,3,2,2,2,2,3,3,3,3/
******
cc PHD = 10**b * E**a
cc b = d(1,groupe,couronne) * (100./Z) + d(2,groupe,couronne)
      datad/-0.6197,1.0095,-0.4988,0.8968,0.,0.,
     #      -0.6197,1.0095,-0.4988,0.8968,0.,0.,
     #      -0.4139,0.8478,-0.3733,0.8975,0.,0.,
     #      -0.4139,0.8478,-0.3733,0.8975,0.,0.,
     #      -0.3448,0.8329,-0.3186,0.9390,0.,0.,
     #      -0.3448,0.8329,-0.3186,0.9390,0.,0.,
     #      -0.5639,1.1463,-0.3395,0.9471,-0.3365,1.0505,
     #      -0.5639,1.1463,-0.3395,0.9471,-0.3365,1.0505/
c******
      ISTATUS = 15
      def = 0.        !  13 juin 1996
      e = 0.          !  13 juin 1996
      if(dedv(nc,nm).lt.0.) return
      n = ng(nm,nc)
c
c --- modification JLC 23/11/96 : tirage aleatoire du passage GG-PG:3700->3800
c      gg_pg=3700.+(rndm(xxx)*100.)
      gg_pg=3800.
      if(cg.le.gg_pg) then                 ! etalonnage en grand gain
         esi = (g(nc,nm,0) + CG*( g(nc,nm,1)+g(nc,nm,2)*CG))*dedv(nc,nm)
      else                                 ! etalonnage en peit gain
*         delta = gene(nc,nm) - ref(nc,nm)
         cpp = cp - delta
         esi = (p(nc,nm,0)+ cpp * (p(nc,nm,1) + p(nc,nm,2) * cpp))
     &           * dedv(nc,nm)
      endif
      e = esi
*
      if(Z.le.Zl(n,nc)) then    ! Limite du calcul du PHD en Z
            ISTATUS = 1
            IF(ESI.LE.0.)ISTATUS=4
            return
c****
cc   peut etre il faut tester sur les detecteurs defectueux aussi
c****
      elseif(esi.le.0.) then
            ISTATUS = 15
            return
      else
c  -----------   calcul du defaut d'ionisation  -------------------
              b = d(1,n,nc)*(100./z) + d(2,n,nc)
              a = 0.0223*(z**2/1000.) + 0.5682
C  **** ERREUR DE 0.5%
             eps = 0.5E-2
c ------      boucle    de    convergence   -------------
             b = 10**b
                 conv = .true.
                 ITOUR=0
c             if(nc.eq.6.and.nm.eq.23) write(*,'(a,3f12.4,2f8.5)')
c    1         'cg,cp,e,a,b=',cg,cp,e,a,b
             dowhile(conv)
                 ITOUR=ITOUR+1
c         if(nc.eq.6.and.nm.eq.23) write(*,'(a,2f12.4)') 'e,phd=',e,phd
                 e0 = e
                 phd = b*(e**a)
                 e = esi + phd
                 conv = (abs(E-E0)/E0).ge.eps
                 IF(ITOUR.EQ.20)THEN
                         ISTATUS=3
                         E=ESI
                         RETURN
                 ENDIF
             enddo
             ISTATUS = 2
      endif
      def = e - esi             !!! 31/05/96
      return
      end


c-----------------------------------------------------------------------

      subroutine Codage (code4,code)                                         
 
      implicit   none 
      integer*4  ic
      data ic/0/                                                  
  
      Integer*4   code4(4),int4,j
      integer*2   cod(2),code
 
c      Equivalence (cod,int4)  ! attention au swap entre AIX et Linux
c                       AIX :   A(8 octets)B(8 octets) = AB(16 octets)
c                     Linux :   A(8 octets)B(8 octets) = BA(16 octets)
           
      ic=ic+1
      int4=0
      do j=4,1,-1
      int4=int4+16**(j-1)*code4(j)
      enddo
      cod(1)=int4/(2**16)
      cod(2)=int4-(cod(1)*(2**16))
      code=cod(2)
      
c      if(ic.lt.500)then
c       write(7,'(z8,2(1x,z4),2x,4i8)')
c     & int4,cod(1),cod(2),int4,cod(1),cod(2),code
c      endif 

      return
      end
      
c ---------------------------------------------------------------------------   
