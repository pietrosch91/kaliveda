c ---------------------------------------------------------------------

      subroutine INIT_CALIB_C1017 (numrun)
 
c ---------------------------------------------------------------------
c     Routine appelant toutes les routines d'initialisation pour
c     la nouvelle calibration et identifications des cour. 10-17
c
c   29/10/98 JLC  (routines mises au point par Nicolas le Neindre) 
c ---------------------------------------------------------------------

      implicit  none                                                    
                                                                        
      integer*4 numrun,iwf 
                                                       
      include 'veda_1.incl'                                             
      include 'veda_2.incl'                                             
      include 'veda_5.incl' 
      include 'veda_rel.incl'                                        
                                
      iwf=i_out		                          
      write(iwf,'('' '')')
      write(iwf,'('' --------------------------- '')')
      write(iwf,'('' NEW CALIBRATION Rings 10-17: RUN = '',i4)') numrun
      write(iwf,'('' --------------------------- '')')
      
      write(*,'('' --------------------------- '')')
      write(*,'('' NEW CALIBRATION Rings 10-17: RUN = '',i4)') numrun
      write(*,'('' --------------------------- '')')
                                                                       
       call INI_PIED_CSI(Numrun)
       
       call INI_TABAC
       
       call INIT_ID_STD
       
       return
       end
       
       
       
cc-----------------------------------------------------------------------

      SUBROUTINE NEW_CALIBRATION_10_17(ic,im,csir,csil,codi
     & ,z_old,a_old,iz_old,ia_old
     & ,E_old,E_CsI_old,dechio
     & ,z_new,a_new,iz_new,ia_new
     & ,E_new,E_CsI_new
     & ,new_ident,code_mev)
                                                  
      include 'veda_rel.incl'                                        
      include 'veda_5.incl'                                           
      include 'veda_6.incl'                                           
      include 'veda_11.incl' 

      real*4 a11(17,24),a22(17,24)
      real*4 xh,xhr,ZZ,AA
      integer*4 ib,irc

      real*4 E_old,E_CsI_old,dechio
      real*4 E_new,E_CsI_new
      real*4 Z_OLD,A_OLD
      real*4 Z_NEW,A_NEW
      integer*4 IZ_OLD,IA_OLD
      integer*4 IZ_NEW,IA_NEW
      integer*4 codi,code_mev,new_ident
      integer*4 ic,im,csir,csil

      Common/tabac1/a11,a22
      
      z_new=z_old
      a_new=a_old
      iz_new=iz_old
      ia_new=ia_old
      xh=0.
      xhr=0.
      E_new=E_old
      E_CsI_new=E_CsI_old
      new_ident=0
      code_mev=0
    
      
c--CORRECTION DE L ENERGIE CSI POUR COUR 10-17 TABACARU --------
c  cas particulier ou on a pas pu fitter 3 modules
        if(ic.eq.8.and.im.eq.7)go to 26
        if(ic.eq.12.and.im.eq.24)go to 26
        if(ic.eq.16.and.im.eq.2)go to 26
c-----        
        if(ic.ge.10.and.ic.le.17)then
	  if(codi.eq.3) go to 26  ! les etalons  
          call LUMIERE(ic,im,csir,csil,xh,ib)
	  if(xh.le.0.)go to 26

c -- LIMITE D IDENTIFICATION EXPERIMENTALE
           iz_old=abs(iz_old)
	   ia_old=abs(ia_old)
	   if(ic.eq.10.and.iz_old.le.12)go to 27
	   if(ic.eq.11.and.iz_old.le.10)go to 27
	   if(ic.eq.12.and.iz_old.le.6)go to 27
	   if(ic.eq.13.and.iz_old.le.3)go to 27
	   if(ic.eq.14.and.iz_old.le.3)go to 27
	   if(ic.eq.15.and.iz_old.le.2)go to 27
	   if(ic.eq.16.and.iz_old.le.2)go to 27
	   if(ic.eq.17.and.iz_old.le.2)go to 27

c -- NOUVELLE IDENTIFICATION EN Z	    

c -- ON SE RECALE SUR UN MODULE DE REFERENCE (LE NUMERO 1)
	  xhr=xh*a11(ic,1)/a11(ic,im)

c -- ATTENTION IL FAUT RENTRER LE MODULE 0	  
	  call identification_z(ic,0,xhr,dechio,ZZ,AA,irc)
	  if(irc.eq.0.or.irc.eq.1)then 
            z_new = ZZ
	    a_new = AA
	    iz_new = nint(ZZ)
	    ia_new = nint(abs(AA))
	  else
	    print*,'ATTENTION dans identification_z IRC different 0,1' 
	    print*,'IRC=',irc   
	  end if
	  new_ident=1 

 27       continue  ! on garde l ancienne identification
 
c -- NOUVELLE CALIBRATION EN ENERGIE
          iz_new = abs(iz_new)
	  ia_new = abs(ia_new)
c
c --- Modif OL (04/04/2000)
c	  if(new_ident.eq.1)print *,xh,ic,im,iz_new,ia_new,ib	
c    
          E_CsI_new = tabacaru(xh,ic,im,iz_new,ia_new,ib)
	   E_new = E_old - E_CsI_old + E_CsI_new
c
c --- Modif OL (04/04/2000)
c	  if(new_ident.eq.1)print *,E_old,E_csi_old,E_csi_new
c
          code_mev=1
c ---	  	  
        end if ! cour 10-17
	
 26    continue  ! on ne fait rien 
             
      return                                                            
      end                                                               
c-----------------------------------------------------------------------
      SUBROUTINE NOUVELLE_CALIBRATION_IDENTIFICATION_C10_17                                            
      include 'veda_rel.incl'                                        
      include 'veda_5.incl'                                           
      include 'veda_6.incl'                                           
      include 'veda_11.incl' 

      real*4 ecsi,xh,xhr,dechio,Z_NEW,A_NEW
      real*4 a11(17,24),a22(17,24)
      integer*4 iz_old,ia_old,code_old,new_ident
      integer*4 ic,im,iz,ia,icsir,icsil

      Common/tabac1/a11,a22
      
      ic=0
      im=0
      icsir=0
      icsil=0
      iz=0
      ia=0
      iz_old=0
      ia_old=0
      code_old=0
      ecsi=0.
      dechio=0.
      z_new=0.
      a_new=0.
      new_ident=0
      new_cali=0
    
      DO I = 1,NPART_TRAITE
        IF(A(I).LT.0) A(I) = -1*A(I) 
      
c--CORRECTION DE L ENERGIE CSI POUR COUR 10-17 TABACARU --------
c  cas particulier ou on a pas pu fitter 3 modules
        if(icou(i).eq.8.and.imod(i).eq.7)go to 26
        if(icou(i).eq.12.and.imod(i).eq.24)go to 26
        if(icou(i).eq.16.and.imod(i).eq.2)go to 26
c-----        
        if(icou(i).ge.10.and.icou(i).le.17)then
          ic=icou(i)
          im=imod(i)
          icsir=canal(i,7)
          icsil=canal(i,8) 
          code_old=code(i)
	  iz_old = z(i)
	  ia_old = a(i)
	  if(code_old.eq.3) go to 26  ! les etalons  
          dechio=de1(i)
          call LUMIERE(ic,im,icsir,icsil,xh,ib)
	  if(xh.le.0.)go to 26

c -- LIMITE D IDENTIFICATION EXPERIMENTALE
	  new_ident=0
	   if(ic.eq.10.and.z(i).le.12)go to 27
	   if(ic.eq.11.and.z(i).le.10)go to 27
	   if(ic.eq.12.and.z(i).le.6)go to 27
	   if(ic.eq.13.and.z(i).le.3)go to 27
	   if(ic.eq.14.and.z(i).le.3)go to 27
	   if(ic.eq.15.and.z(i).le.2)go to 27
	   if(ic.eq.16.and.z(i).le.2)go to 27
	   if(ic.eq.17.and.z(i).le.2)go to 27

c -- NOUVELLE IDENTIFICATION EN Z	    

c -- ON SE RECALE SUR UN MODULE DE REFERENCE (LE NUMERO 1)
	  xhr=xh*a11(ic,1)/a11(ic,im)

c -- ATTENTION IL FAUT RENTRER LE MODULE 0	  
	  call identification_z(ic,0,xhr,dechio,Z_NEW,A_NEW,irc)
	  if(irc.eq.0.or.irc.eq.1)then 
            z_indra(i) = Z_NEW
	    a_indra(i) = A_NEW
	    z(i) = nint(Z_NEW)
	    a(i) = nint(A_NEW)
	  else
	    print*,'ATTENTION dans identification_z IRC different 0,1' 
	    print*,'IRC=',irc   
	  end if
	  new_ident=1
	  new_cali=1 

 27       continue  ! on garde l ancienne identification
 
c -- NOUVELLE CALIBRATION EN ENERGIE	    
          iz=z(i)
          ia=nint(abs(float(a(i))))
          ecsi=tabacaru(xh,ic,im,iz,ia,ib)
c          ener_old=ener(i)
	  ener(i)=ener(i)-de3(i)+ecsi
	  de3(i)=ecsi
c ---	  	  
        end if ! cour 10-17
	
 26    continue  ! on ne fait rien 
             
      END DO ! i=1 a NPART_TRAITE
      
      return                                                            
      end                                                               
c-----------------------------------------------------------------------
	function tabacaru(hl,ic,im,iz,ia,iblag)
	
	Real*4 a11(17,24),a22(17,24)
	Common/tabac1/a11,a22
	real*4 hl
	integer*4 iz,ia,iblag
	logical*1 cond
	
        tabacaru=0.
	iblag=0
        niter=0
        eps=1.e-4
       
       IF(hl.le.0.)THEN
        iblag= 5
        RETURN
       END IF
       
       if(ic.lt.2.or.ic.gt.17) then
         iblag=5
         return
       endif
       
       IF (ia.eq.8.and.iz.eq.4) THEN            ! Be8 = 2* He4
c  ATTENTION LA FONCTION AMASS DONNE A=8 POUR Z=4
c  MAIS C EST UN VRAI Be (en fait un 9Be)
c  CE CAS EST LE PLUC COURRANT
c  A NE PAS CONFONDRE AVEC UN VRAI 8Be QUI CASSE EN DEUX ALPHAS 
cc        az2=ia*iz*iz/8. ! pour un vrai 8Be
cc        fbe = 2.        ! pour un vrai 8Be
          az2=ia*iz*iz
          fbe = 1.
       ELSE                                       ! tout le reste
          az2=ia*iz*iz
          fbe = 1.
       END IF
      
	a1t=a11(ic,im)
	if(a1t.eq.-1.) then
	   ecsi=0.
	   iblag=15
	   return
	endif
	
	a2t=a22(Ic,im)
	a3t=1.
	a4t=0.27
	
	E0=a3t*float(ia)
	
       IF(a2t.lt. 1.e-7)THEN
        x = (hl)/a1t
       ELSE
        x = 2000.
        argx = 1.+ x/(a2t*az2)
        cond=.true.
        
        DO while (cond)
          niter=niter+1

	
	a4e=a4t
	if (x.lt.E0) a4e=0.
	 
          fx =  - hl + fbe*a1t * (x - a2t* az2 * log(argx)+a2t*a4e*az2
     &       *log((x+a2t*az2)/(E0+a2t*az2)))
     
          fpx =fbe*a1t * (x+a2t*az2*a4e)/(x+a2t*az2)
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
      
       tabacaru=fbe*x
          
       return
       end
c----------------------------------------------------------------------
	SUBROUTINE LUMIERE(ic,im,icsir,icsil,hl,iblag)

	INTEGER*4 ic,iblag,im,inp
	REAL*4    rp,to,hl,rcsir,rcsil,ecsi,c1,c2,tau
       
        DIMENSION pied_R(17,24),pied_L(17,24) 	
        COMMON /PIEton/numsel,pied_R,pied_L
        DATA        p0/400./, p1/900./, eps/1.e-4/, pre/0.4318/
        LOGICAL     cond,condi
        
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
            
	     icsir=icsir-pied_R(ic,im)
	     icsil=icsil-pied_L(ic,im)-3
	     rcsir=float(icsir)
	     rcsil=float(icsil)
	
c*** quelques precautions
	
        IF(rcsil.le.0.or.rcsir.le.-5.) THEN
        iblag=15
        RETURN	
	END IF
	
	
	IF(rcsir.le.0.) rcsir=0.1
	
	rp=(rcsil/rcsir)*(c1/c2)
	
c*** recherche de la quantite de lumiere totale	


c        print *,ic,im,icsir,icsil,rp
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
        
        return
        
        end
c--------------------------------------------------------------
	Subroutine IDENTIFICATION_Z(Ic,Im,x,y,z,a,Irc)

	Common/IDENZ1/par(17,3,0:24,14),Zmax(17,3,0:24),
     &  Zmin(17,3,0:24),Ijeu(17)	
	Real*8 Xval(14),Z2,X2,Y2,Z0min,Z0max
        Real*4 Z_Ijeu(3)
	
	Irc=3
	z=-10000.
	do ik=1,3
	   Z_Ijeu(ik)=0.
	end do
	If(Ic.lt.1.or.Ic.gt.17) Then
	   Z=0.
	   Irc=-1
	endif
	If(Im.lt.0.or.Im.gt.24) Then
	   Z=0.
	   Irc=-1
	endif
	
	DO ind=1,Ijeu(ic)  ! boucle sur le nb de jeux par cour
	  Nbpar=14
	  Zzmin=Zmin(Ic,ind,im)
	  Zzmax=Zmax(Ic,ind,im)
	
	  Do i=1,nbpar
	     Xval(i)=Par(Ic,ind,Im,i)
	  end do
		
	  X2=x
	  Y2=y
	  if(Xval(1).eq.-1.) then
	     Irc=-2
	     z=0.
	     return
	  endif
c ----------------------------------------	
	  Z0min=0.
	  Z0max=100.
	  call zinversion(x2,y2,Xval,nbpar,z2,z0min,z0max)
	  z_ijeu(ind)=z2
          if(z_ijeu(ind).ge.Zzmin.and.z_ijeu(ind).le.Zzmax)then
	    z=z2
	    Irc=0
	  end if  
        END DO !  ind=1,Ijeu(ic)

        if(z_ijeu(1).gt.Zmax(ic,1,im).and.
     &     z_ijeu(2).lt.Zmin(ic,2,im).and.Irc.eq.3)then
          z=Zmax(ic,1,im)
	  Irc=0
	endif
        if(z_ijeu(2).gt.Zmax(ic,2,im).and.
     &     z_ijeu(3).lt.Zmin(ic,3,im).and.Irc.eq.3)then
          z=Zmax(ic,2,im)
	  Irc=0
	endif
	if(z2.gt.Zmax(ic,Ijeu(ic),im).and.Irc.eq.3) then
	   z=z_ijeu(Ijeu(ic)) ! Z extrapole
	   Irc=1
	endif
	if(z_ijeu(1).lt.Zmin(ic,1,im).and.Irc.eq.3) then
	   z=z_ijeu(1) ! Z extrapole
	   Irc=1
	endif
c ----------------------------------------	
	A=Amass(z)  
    
	REturn
	End
c--------------------------------------------------------------
      function zfunfit(x,zz,nbpar,xval)

      implicit double precision(a-h,o-z)

      dimension  xval(*)


      a=xval(1)*zz**3+xval(2)*zz**2+xval(3)*zz+xval(4)
      b=xval(5)*zz**3+xval(6)*zz**2+xval(7)*zz+xval(8)
      c=xval(9)*zz**3+xval(10)*zz**2+xval(11)*zz+xval(12)

      if(nbpar.eq.12) then
         zfunfit=a+b/(x+c)
      else
         d=xval(13)*zz**4
         e=xval(14)*zz**4
         zfunfit=a+b/(x+c)+d/(x*x+e)
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
      subroutine zinversion(x,y,para,nbpar,z,z0min,z0max)

      implicit double precision(a-h,o-z)
      external zfunfit
      dimension Para(14)
      
      i=0
      eps=1.e-2
      ycal=-1.
      bouclemax=50
      zmin=z0min
      zmax=z0min+5.
      if((z0max-z0min).gt.5) then
        ycal=zfunfit(x,zmax,nbpar,para)
        if(ycal.gt.100000.) ycal=0.
        do while((ycal.lt.y)
     1                      .and.(zmax.lt.z0max))
           zmin=zmax
           zmax=zmax+5.
           ycal=zfunfit(x,zmax,nbpar,para)
           if(ycal.gt.100000.) ycal=0.
        enddo
      endif
      if(zmax.gt.z0max) zmax=z0max
      do while((abs(zmax-zmin).gt.eps).and.(i.lt.bouclemax))
       z=(zmin+zmax)/2.
       ycal=zfunfit(x,z,nbpar,para)
       if(ycal.gt.y) then
        zmax=z
       else
        zmin=z
       endif
       i=i+1
      enddo
      if(i.ge.bouclemax) then
       z=z0max+110.
      endif

      return
      end
C=======================================================================
      FUNCTION AMASS(Z)                                                 
C=======================================================================
c    MASSE D UN FRAGMENT EN FONCTION DE SA CHARGE                       
c    EXTRAPOLATION A PARTIR DE LA TABLE PERODIQUE DES ELEMENTS          
C=======================================================================
      AMASS=1.867*Z+.016*Z**2-1.07E-4*Z**3                              
      IF(Z.LT.2.)AMASS=2.*Z                                             
      RETURN                                                            
      END                                                               
c-------------------------------------------------------------------    
c Fonction longueur : calcule la longueur effective d'une chaine de cara
c-------------------------------------------------------------------    
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
c-----------------------------------------------------------------------
	Subroutine INI_TABAC
	
	include 'veda_11.incl'
	
	REal*4 a11(17,24),a22(17,24)
	Common/tabac1/a11,a22
	
	character*120 file_tabac
	
        file_tabac=vedafil(1:long_path)//'tabac.data'
c        file_tabac='/home/crash19/indra/run_camp93/tabac.data'
c	file_tabac='/afs/in2p3.fr/home/l/leneindr/calib/'
c     &            //'dst1/tabac.data'
	ifi=LONGUEUR(file_tabac)
	
	Print *,' -----------------------------------------------------'
	Print *,'| Initialisation des nouvelles calibrations CsI 10-17 |'
	print *,'|                                                     |'
	Print *,'|        **** parametrisation Tabacaru ****           |'
	print *,'|                                                     |'
	Print *,' -----------------------------------------------------'
	Print *,' '
	Print *,'Ouverture du fichier de parametres : '
	Print *,file_tabac(1:ifi)
	print *,' '
	open(unit=27,file=file_tabac(1:ifi),status='OLD')
	
	do ic=1,17
	   do im=1,24
	      a11(ic,im)=-1.
	      a22(ic,im)=-1.
	   enddo
	enddo
	
	Ios=0
	Do while(ios.eq.0)
	   read(27,1,iostat=ios) xc,xm,a1,a2,a3
	   ic=nint(xc)
	   im=nint(xm)
	   if(ic.ge.1.and.ic.le.17) then
	      a11(ic,im)=a1
	      a22(ic,im)=a2
	   endif
	end do
	
1	format(1x,f3.0,1x,f3.0,3(f10.4))

	close(unit=27)
	
	Print *,'Fin INIT Calibration Csi 10-17'
	print *,' '
	
	return
	end
c-------------------------------------------------------------------
	Subroutine INIT_ID_STD
	
	include 'veda_11.incl'
	
	character*120 file_par,file_bragg,file_reb,path
	
	Common/IDENZ1/par(17,3,0:24,14),Zmax(17,3,0:24),
     &  Zmin(17,3,0:24),Ijeu(17)
	Integer*4 Iclu(17),nbjeux
	Real*4 P(14)

        path=vedafil(1:long_path)
c	path='/afs/in2p3.fr/home/l/leneindr/calib/dst1/'
c	path='/home/crash19/indra/run_camp93/'
	ipa=LONGUEUR(Path)
	file_par=path(1:ipa)//'coeffit_c10_17.data_formatte'
	
	Print *,' '
	Print *,'     -----------------------------------------'
	Print *,'    |        Identification standard          |'
	Print *,'    |                                         |'
	Print *,'    |     Fonctionnelle a 12/14 parametres    |'
	Print *,'    |                                         |'
	Print *,'    |                                         |'
	Print *,'    | Version 2.0             29 Juillet 1998 |'
	Print *,'     -----------------------------------------'
	Print *,' '   
	Do ic=1,17
	  Do nbjeux=1,3  ! nb de jeux de para par cour pour les fonctionnelles
	    Do im=0,24
	      Zmin(Ic,nbjeux,Im)=-1.
	      Zmax(Ic,nbjeux,Im)=-1.
	      do it=1,14
	         Par(Ic,nbjeux,Im,It)=-1.
	      End do
	    End do
	  End do  
	End do
	
	ifi=LONGUEUR(File_par)
	
	Print *,'Ouverture du fichier de parametres : ',file_par(1:ifi)
	OPEN(21,file=file_par(1:ifi),status='OLD',err=10)
	
	IOs=0
	Nline=0
	Ncour=0
	Do i=1,17
	   Iclu(i)=0
	   Ijeu(i)=0
	End do
	
	Do while(ios.eq.0)
           Ic=0
	   REad(21,1,iostat=ios) Ic,Im,Izmin,Izmax,(p(i),i=1,14)
	   Nline=Nline+1
	   if(ic.ge.1.and.ic.le.17.and.Im.ge.0.and.Im.le.24) then
	      Iclu(ic)=Iclu(ic)+1
	      Ijeu(ic)=Ijeu(ic)+1
	      Do i=1,14
	         Par(Ic,Ijeu(ic),Im,i)=p(i)
	      End do
	      Zmin(Ic,Ijeu(ic),Im)=Izmin
	      Zmax(Ic,Ijeu(ic),Im)=Izmax
	   endif
	   
	end do
	
	close(21)
	
	Print *,'Initialisation effectuee ...'

	Print *,' '
	Print *,'------------------------------------------------------'	
	Print *,'Statistiques de lecture pour les parametres ID :'
	Iclutot=0
	Do i=1,17
	   Iclutot=Iclutot+Iclu(i)
	   if(iclu(i).gt.0) then
	      Print *,'  Couronne ',i,'  ==> ',iclu(i),' modules lus'
	   endif
	end do
	Print *,' '
	Print *,'Soit ',iclutot,' modules au total ...'
	Print *,'------------------------------------------------------'	
	Print *,' '
	Do i=1,17
	   if(ijeu(i).gt.0) then
	      Print *,'Couronne ',i,' ==> ',ijeu(i),'jeux de param lus'
	   endif
	End do   
	
1	format(1x,4(i4,1x),14(e15.6))
	
	Return
	
10	Print *,'***** ERREUR dans INIT_ID_STD ******'
	Print *,'Erreur a l''ouverture du fichier ',file_par(1:ifi)
	Print *,'Verifiez qu''il existe bien avant de relancer'
	Irc=-1
	REturn
	
20	Print *,'***** ERREUR dans INIT_ID_STD ******'
	Print *,'Erreur de lecture dans le fichier ',file_par(1:ifi)
	Print *,'A la ligne numero ',nline
	
	Return
	End  
c----------------------------------------------------------------
       Subroutine INI_PIED_CSI(Numrun)
	
       DIMENSION pied_R(17,24),pied_L(17,24) 	
       COMMON /PIEton/numsel,pied_R,pied_L
	
       integer*4 pied_run(18,24,18),n1,n2,n3,erreur                     
       real*4 centre_pin,centr_gene(18,24,18)  

        call veda_findp(Numrun,pied_run,centr_gene,erreur) 
	
	do ic=2,17
	   Do im=1,24
	      pied_R(Ic,Im)=pied_run(Ic,Im,7)
	      pied_L(Ic,Im)=pied_run(Ic,Im,8)
	   Enddo
	End do
	          
       return
       end
c-----------------------------------------------------------------------
      subroutine veda_findp(run,pied_run,centr_gene,erreur) 

      implicit none                                                     
                                                                        
c     Parameter Num_Fich=20                                             
                                                                        
      integer*4 pied_run(18,24,18),n1,n2,n3                             
      real*4 centre_pin,centr_gene(18,24,18)                            
                                                                        
      integer*4 run                                                     
      integer*4 i,type_min,type_max,num_write,erreur                    
      integer*4 cour,mod,type,cour_min,cour_max                         
                                                                        
                                                                        
      character*20 nomrun                                               
      character*3  nn                                                   
      character*7  nrun                                                 
      logical test,lecture_ref    
c corrections dictees par John Frankland le 9/05/06 est faite par A.Chbihi 
c le numero de run "run" changeait dans une des routines X_FFIL ou veda_gene. On l'a remplace 
c par une variable tempon run_tmp. 
	Integer*4 run_tmp                                     
                                                                        
      data lecture_ref/.false./                                         
                                                                        
         WRITE(6,*)'JE SUIS DANS FIND_PIED'                             
      Erreur=0                                                          
c-----------------------------------------------------------------------
c          Lecture du fichier                                           
c-----------------------------------------------------------------------
                                                                        
c------------- Recherche du nom du fichier (routine ds GAMMA FORTRAN)   
 
	run_tmp = run
      call X_FFIL('run','vax',run_tmp,nomrun)
      
                                                                        
c     nomrun='RUN'                                                      
c     n1=run/100                                                        
c     n2=(run-n1*100)/10                                                
c     n3=run-n1*100-n2*10                                               
c     nn(1:1)=char(240+n1)                                              
c     nn(2:2)=char(240+n2)                                              
c     nn(3:3)=char(240+n3)                                              
c                                                                       
c     nrun='/RUN'//nn                                                   
c     nomrun=nrun//' VAX *'                                             
                                                                        
1     format(1x,'Nom du fichier Piedestal a lire :',a6)                 
2     format(1x,'Nom du fichier Piedestal a lire :',a20)                
                                                                        
c------------ Lecture du fichier                                        
                                                                        
      print *,' Fichier piedestal = ',nomrun                            
                                    
                                                                        
      call veda_gene(nomrun,run_tmp,centr_gene,centre_pin,                  
     # pied_run,erreur)                                                 
                                                                        
c      lecture_ref=.true.                                               
c-----------------------------------------------------------------------
c          Fin de la Lecture du Run Reference                           
c-----------------------------------------------------------------------
15    continue                                                          
      RETURN                                                            
      END                                                               
c---------------------------------------------------------------------- 
      subroutine veda_gene(nomfich,Run0,centr_gene,centre_pin,          
     # pied_run,erreur)                                                 
                                                                        
                                                                        
      implicit none                                                     
      include 'veda_11.incl'
                                                                        
      integer*4 cour,mod,type,run,run0,i,label,j                        
      integer*4 pied_run(18,24,18),pied,num_write,erreur                
      real*4    centr_gene(18,24,18)                                    
      real*4    centre_pin,sigma,skew,somme,centre                      
      integer*4 long_fil
                                                                        
      character*(*) nomfich                                             
      character*80 text,nomfil                                          
      character*10 nom                                                  
      Integer*4 runlu,nbllu                                             
      logical Trouve,trouve_pied,last                                   
                                                                        
      erreur=0                                                          
      num_write=6                                                       
c-------------------------------------------------------------------    
c      Ouverture du fichier                                             
c-------------------------------------------------------------------    
      write(num_write,'(/,'' Lecture du fichier :'',a)')nomfich       
      nomfil=vedafil(1:long_path)//nomfich
c      nomfil='/home/crash19/indra/run_camp93/'//nomfich
                                                                        
c      open (unit=60,file=nomfil,action='READ',err=95)                  
      open (unit=60,file=nomfil,err=95) 
      print *,' Open File (veda_gene): ',nomfil                  
                                                                        
      Trouve=.False.                                                    
c     Do j = 1,1       ! Debut d'un nouveau Run                         
c        read(60,'(a)')text                                             
c     End do                                                            
                                                                        
                                                                        
c --- Lecture de l'entete d'un run                                      
                                                                        
98    Continue                                                          
                                                                        
c --- Inutile de continuer a lire si le run RUN0 est trouve !           
                                                                        
      if(trouve)goto 97                                                 
                                                                        
      Do j = 1,9                                                        
         read(60,'(a)',end=97)text                                      
c        write(6,'(a,i5)')text,j                                        
      End do                                                            
      centre_pin=0.                                                     
                                                                        
c --- Ligne suivante SVP...                                             
                                                                        
10    Continue                                                          
                                                                        
c --- Lecture de la ligne avec le format 100                            
                                                                        
      read(60,100,end=97,err=98)                                        
     # run,label,cour,mod,type,nom,somme,centre,sigma,skew,pied         
                                                                        
100   format(1x,2I5,3I3,1X,A10,F8.0,F9.2,2F9.3,I5)                      
                                                                        
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
      pied_run(cour,mod,type)=pied                                      
      Go to 10                                                          
                                                                        
c --- Fin de lecture du fichier Piedestaux/GENE                         
                                                                        
97    continue                                                          
      close(unit=60)                                                    
                                                                        
c --- Messages d'information                                            
                                                                        
      if(run.eq.run0) trouve=.True.                                     
                                                                        
      if(trouve)then                                                    
         write(num_write,'('' Le Run'',i4,''  a ete trouve'')')run0     
         erreur=0                                                       
         return                                                         
      else                                                              
                                                                        
c --- Le run n'a pas ete trouve...                                      
                                                                        
         write(num_write,'('' Le Run'',i4,''  n a pas ete trouve'')     
     &   ')run0                                                         
         erreur=-2                                                      
         return                                                         
      endif                                                             
                                                                        
c --- Erreur de lecture !                                               
                                                                        
95    erreur=-1                                                         
      return                                                            
      end                                                               
c -------------------------------------------------------------------
c         ******************************************
c         * RECHERCHE DE FICHIER PAR NUMERO DE RUN *
c         ******************************************
c
c   Les fichiers sont de la forme : racnameNURUN ftype, et si le fichier
c    correspondant a un numero de run n'est pas trouve, on cherche sur
c    les runs voisins.
c      call ffil(racname,ftype,nurun,fil)
c  ENTREE :
c   racname : [C*(*)] racine generique des noms de fichiers
c   ftype   : [C*(*)] file type
c   nurun   : [I*4]   numero de run
c  RETOUR :
c   nurun   : [I*4]   numero de run en retour (-1 si pas trouve)
c   file    : [C*(*)] pathname du fichier trouve
c
c
      subroutine x_ffil(racname,ftype,nurun,fil)
      implicit  none
      include   'veda_11.incl'
      character*80  namefil
      character*(*) racname, ftype, fil
      integer*4     nurun
c
      logical       exist
      integer       is, i1, i2, nn, nt, np, nur
      character*5   num
c
      nn = len(racname)
 
      do while (nn.ge.1 .and. racname(nn:nn).eq.' ')
        nn = nn-1
      enddo
      nt = len(ftype)
      do while (nt.ge.1 .and. ftype(nt:nt).eq.' ')
        nt = nt-1
      enddo
      exist = .false.
      is = 0
      np = 0
      do while ((.not.exist).and.np.le.20)
        num = ' '
        nur = nurun + is
        write(num,'(i5)') nur
        i1 = 1
        do while(num(i1:i1).eq.' ')
          i1 = i1 + 1
        enddo
        i2 = 5
        do while(num(i2:i2).eq.' ')
          i2 = i2 - 1
        enddo
        fil = racname(1:nn)//num(i1:i2)//'.'//ftype
        namefil=vedafil(1:long_path)//fil
c        namefil='/home/crash19/indra/run_camp93/'//fil
 
        inquire(file=namefil,exist=exist)  
        if(is.lt.0) then
          is = -is
        else
          is = -(is+1)
        endif
        np = np+1
      enddo
      if(exist) then
        nurun = nur
      else
        nurun = -1
        fil = ' '
      endif
      return
      end
