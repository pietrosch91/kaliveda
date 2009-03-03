c
c                  ***********************************
c                  *     Fit of E-DE functional      *
c                  *  Laurent TASSAN-GOT  17/03/2001 *
c                  ***********************************
c
c   Three basic routines are available :
c  globede  : user-friend routine in which only the data have to be supplied.
c             The routine estimates good initial values, reasonnable limits,
c             automatically from the data and manages the whole fit procedure
c   fitede  : more specific but flexible routine in which initial values
c             have to be supplied and also limits of the parameters and 
c             scaling factors. This routine performs a fit in the given 
c             conditions, it is called several times by globede. It can be 
c             invoked directly by the user in case of specific difficult cases,
c             otherwise globede is much simpler and easier to handle.
c    fede   : function which returns the value of the functional (energy loss)
c             for a given Z, A and energy (or light). Subroutines globede or
c             fitede should have been called first to load the parameters.
c
c  At least 1 (Z,A) pair must contain 2 points.
c
c---------------------------------------------------------------------
c
c                    ******************
c                    *  Automatic fit *
c                    ******************
c
c      call globede(npts,zd,ad,xd,yd,ixt,ih,par,istate,irc)
c
c INPUT :
c   npts : [I] number of data points really used in zd, ad, xd, yd
c    zd  : [R] array of Z's (npts values)
c    ad  : [R] array of A's (npts values)
c    xd  : [R] array of E's (npts values)
c    yd  : [R] array of DE's (npts values)
c   ixt  : [I] =0->basic functional       <>0->extended functional
c       * For the basic formula :
c         yy = ((g*E)**(mu+1)+lambda**(mu+1)*Z**2*A**mu)**(1/(mu+1))-g*E + pdy
c       * For the extended formula :      
c         yy = ((g*E)**(mu+nu+1)+(lambda*Z**alpha*A**beta)**(mu+nu+1)+
c                  xi*A**mu*(g*E)**nu)**(1/(mu+mu+1))-g*E + pdy
c
c       *  If ih=0  no non-linear light response : E=xx-pdx
c       *  If ih<>0 non-linear light response included :
c           E = sqrt(h**2+2*rho*h*(1+log(1+h/rho)))
c          rho=eta*Z**2*A    and   h=xx-pdx
c    ih  : [I]  =0->no non-linear light response
c              <>0->non-linear light response included
c OUTPUT :
c    par : [R] array of parameters resulting from the fit, according to the 
c              ixt, ih combination the numbers and orders of parameters are :
c              ixt=0  ih=0   5 parameters: lambda, mu, g, pdx, pdy
c              ixt=0  ih<>0  6 parameters: lambda, mu, g, pdx, pdy, eta
c              ixt<>0 ih=0   9 parameters: lambda, alpha, beta, mu, nu, 
c                                         xi, g, pdx, pdy
c              ixt<>0 ih<>0 10 parameters: lambda, alpha, beta, mu, nu, 
c                                         xi, g, pdx, pdy, eta
c  istate : [I] array of status of parameters
c                 =0 -> free parameter
c                 =1 -> parameter constrained by the lower limit
c                 =2 -> parameter constrained by the upper limit
c                 =3 -> constant parameter (bl(i)=bu(i)=par(i))
c    irc  : [I] return code of the fit :
c              0 -> convergence reached 
c              1 -> convergence reached, but not well marked minimum
c              2 -> too many iterations, convergence not reached
c             -1 -> no identification line with at least 2 points
c             -2 -> too few data points
c             -3 -> addressing problem between Fortran and C
c
c
c                    ******************
c                    *  Specific fit  *
c                    ******************
c
c      call fitede(npts,zd,ad,xd,yd,ixt,ih,sc,bl,bu,par,istate,irc)
c
c INPUT :
c   npts : [I] number of data points really used in zd, ad, xd, yd
c    zd  : [R] array of Z's (npts values)
c    ad  : [R] array of A's (npts values)
c    xd  : [R] array of E's (npts values)
c    yd  : [R] array of DE's (npts values)
c   ixt  : [I] =0->basic functional       <>0->extended functional
c       * For the basic formula :
c         yy = ((g*E)**(mu+1)+lambda**(mu+1)*Z**2*A**mu)**(1/(mu+1))-g*E + pdy
c       * For the extended formula :      
c         yy = ((g*E)**(mu+nu+1)+(lambda*Z**alpha*A**beta)**(mu+nu+1)+
c                  xi*Z**2*A**mu*(g*E)**nu)**(1/(mu+mu+1))-g*E + pdy
c
c       *  If ih=0  no non-linear light response : E=xx-pdx
c       *  If ih<>0 non-linear light response included :
c           E = sqrt(h**2+2*rho*h*(1+log(1+h/rho)))
c          rho=eta*Z**2*A    and   h=xx-pdx
c    ih  : [I]  =0->no non-linear light response
c              <>0->non-linear light response included
c    sc  : [R] array of size equal to the number of parameters : scaling
c              factors of parameters. If sc(1)=0. the initial values
c              of parameters will be taken as scaling factors, this is
c              recommended if no initial value is null.
c     bl : [R] array of the lower limits of parameters
c     bu : [R] array of the upper limits of parameters
c              if bl(i)=bu(i)=par(i) the parameter i will be constrained to
c              stay fixed along the fit procedure, for example pdx and pdy
c              can be fixed in this way, if in the data A is taken dependent
c              of Z, beta should be constant, and if only one Z is present
c              alpha should be constant.      
c    par : [R] array of initial values of parameters, according to the 
c              ixt, ih combination the numbers and orders of parameters are :
c              ixt=0  ih=0   5 parameters: lambda, mu, g, pdx, pdy
c              ixt=0  ih<>0  6 parameters: lambda, mu, g, pdx, pdy, eta
c              ixt<>0 ih=0   9 parameters: lambda, alpha, beta, mu, nu, 
c                                         xi, g, pdx, pdy
c              ixt<>0 ih<>0 10 parameters: lambda, alpha, beta, mu, nu, 
c                                         xi, g, pdx, pdy, eta
c OUTPUT :
c    par  : [R] array of final values of parameters resulting from the fit
c  istate : [I] array of status of parameters
c                 =0 -> free parameter
c                 =1 -> parameter constrained by the lower limit
c                 =2 -> parameter constrained by the upper limit
c                 =3 -> constant parameter (bl(i)=bu(i)=par(i))
c    irc  : [I] return code of the fit :
c              0 -> convergence reached 
c              1 -> convergence reached, but not well marked minimum
c              2 -> too many iterations, convergence not reached
c             11 -> initial values inconsistent with limits :
c                     bu < bl   or    par < bl   or   par > bu
c             -3 -> addressing problem between Fortran and C
c
c                    *************************************
c                    *  DE value given by the functional *
c                    *************************************
c
c       de = fede(z,a,e)
c  INPUT :
c     z  : [R] Z of the particle      
c     a  : [R] A of the particle      
c     e  : [R] energy or light of the particle      
c  OUTPUT :
c   fede : [R] value of the energy loss
c
c     This call is only valid after having called  fitede or globede
c---------------------------------------------------------------------
c
c
c                    ******************
c                    *  Automatic fit *
c                    ******************
c
      subroutine globede(npts,zd,ad,xd,yd,ixt,ih,par,istate,irc)
      implicit     none
      integer      npts, ixt, ih, irc
      integer      istate(*)
      real         zd(*),ad(*),xd(*),yd(*),par(*)
c
      integer      MXNN
      parameter    (MXNN=10)
      real         bl(MXNN), bu(MXNN), scl(MXNN)
      integer      nl, npn, nz, na
      integer      i, j, k, ixmin, ixmax, iymin, iymax, ixm
      real         xmin, xmax, ymin, ymax, xm, xx, frng, frngp, zz, aa
      real         zmax, pdx, pdy, gg, rl

      irc = -2
      if (npts.lt.10) return
      frng = 1.e-4
      frngp = 0.005
c                Line counting
      nl = 1
      npn = 0
      do while (npn.lt.npts)
        i = 1
        do while (zd(i).lt.0.)
          i = i+1
        enddo
        nl = nl+1
        zz = zd(i)
        aa = ad(i)
        do i = 1,npts
          if (zd(i).eq.zz .and. ad(i).eq.aa) then
            zd(i) = -zd(i)
            npn = npn+1
          endif
        enddo
      enddo
      do i = 1,npts
        zd(i) = -zd(i)
      enddo
c                Counting of the number of Z'S
      nz = 1
      npn = 0
      do while (npn.lt.npts)
        i = 1
        do while (zd(i).lt.0.)
          i = i+1
        enddo
        nz = nz+1
        zz = zd(i)
        do i = 1,npts
          if (zd(i).eq.zz) then
            zd(i) = -zd(i)
            npn = npn+1
          endif
        enddo
      enddo
      do i = 1,npts
        zd(i) = -zd(i)
      enddo
c                Counting of the number of A/Z
      na = 0
      npn = 0
      do while (npn.lt.npts)
        i = 1
        do while (zd(i).lt.0.)
          i = i+1
        enddo
        na = na+1
        zz = ad(i)/zd(i)
        do i = 1,npts
          if (zd(i).gt.0.) then
            if (abs(ad(i)/zd(i)/zz-1.).lt.1.e-3) then
              zd(i) = -zd(i)
              npn = npn+1
            endif
          endif
        enddo
      enddo
      do i = 1,npts
        zd(i) = -zd(i)
      enddo
c      write(6,'(3(a,i3,3x))') 'nl=',nl,'nZ=',nz,'nA=',na
c               Definition of the outer rectangle
      zmax = 0.
      xmin = 1.e20
      xmax = -xmin
      ymin = xmin
      ymax = -ymin
      do i = 1, npts
        xx = zd(i)**2*ad(i)
        zmax = max(zmax,xx)
        xx = xd(i)
        if (xx.lt.xmin) then
          xmin = xx
          ixmin = i
        endif
        if (xx.gt.xmax) then
          xmax = xx
          ixmax = i
        endif
        xx = yd(i)
        if (xx.lt.ymin) then
          ymin = xx
          iymin = i
        endif
        if (xx.gt.ymax) then
          ymax = xx
          iymax = i
        endif
      enddo
      pdx = xmin-(xmax-xmin)*frng
      pdy = ymin-(ymax-ymin)*frng
c              Looking for the most left side segment
      ixm = ixmin
      npn = 0
      k = 0
      do while (npn.lt.npts .and. k.eq.0)
        zz = zd(ixm)
        aa = ad(ixm)
        zd(ixm) = -zz
        npn = npn+1
        xm = 1.e20
        do i = 1,npts
          if (zd(i).eq.zz .and. ad(i).eq.aa) then
            zd(i) = -zd(i)
            npn = npn+1
            xx = xd(i)
            if (xx.lt.xm) then
              xm = xx
              k = i
            endif
          endif
        enddo
        if (k.eq.0) then
          ixm = 0
          do i = 1,npts
            if (zd(i).gt.0.) then
              xx = xd(i)
              if (xx.lt.xm) then
                xm = xx
                ixm = i
              endif
            endif
          enddo
        endif
      enddo
      do i = 1,npts
        if (zd(i).lt.0.) zd(i) = -zd(i)
      enddo
      irc = -1
      if (k.eq.0) return
      gg = -(yd(k)-yd(ixm))/(xd(k)-xd(ixm))
      if (gg.lt.0.) gg = -gg
      rl = (yd(ixmin)-pdy)/zd(ixmin)/sqrt(ad(ixmin))
c      write(6,'(x,3(a,i3,3x),2(a,f7.3,3x))') 'nl=',nl,'nZ=',nz,
c     &  'nA/Z=',na,'gg=',gg,'rl=',rl
c      write(6,'(x,2(a,f7.3,3x))') 'pdx=',pdx,'pdy=',pdy
c
c       Fit with the basic functional with  pdx and eta  kept constant
c           in order to get good starting values of rl, pdy, gg
      par(1) = rl
      par(2) = 1.
      par(3) = gg
      par(4) = pdx
      par(5) = pdy
c      write(6,'(a,5(2x,f8.2))') 'par0=',(par(i),i=1,5)
      do i = 1,5
        scl(i) = abs(par(i))
      enddo
      scl(4) = (xmax-xmin)*frngp
      scl(5) = (ymax-ymin)*frngp
      bl(1) = par(1)/1000.
      bu(1) = par(1)*1000.
      bl(2) = 0.4
      bu(2) = 2.
      bl(3) = par(3)/1000.
      bu(3) = par(3)*1000.
      bl(4) = par(4)
      bu(4) = par(4)
      bl(5) = par(5)-(ymax-ymin)*0.5
      bu(5) = par(5)+(ymax-ymin)*0.5
      call fitede(npts,zd,ad,xd,yd,0,0,scl,bl,bu,par,istate,irc)
      if (irc.ne.0 .and. irc.ne.1) return
      rl = par(1)
      gg = par(3)
      pdy = par(5)
c                 Fit with basic functional
      if (ixt.eq.0) then
        do i = 1,5
          scl(i) = abs(par(i))
        enddo
        bl(1) = par(1)/5.
        bu(1) = par(1)*5.
        bl(3) = par(3)/5.
        bu(3) = par(3)*5.
        bl(4) = par(4)-(xmax-xmin)*0.5
        if (ih.ne.0) then
          par(6) = (xmax-xmin)*frngp/zmax
          scl(6) = par(6)
          bl(6) = par(6)*1.e-8
          bu(6) = par(6)*1000.
        endif
        call fitede(npts,zd,ad,xd,yd,0,ih,scl,bl,bu,par,istate,irc)
c                 Fit with extended functional
      else
        do i = 5,3,-1
          j = i+4
          par(j) = par(i)
          scl(j) = scl(i)
          bl(j) = bl(i)
          bu(j) = bu(i)
        enddo
        par(4) = par(2)
        scl(4) = par(4)
        bl(4) = bl(2)
        bu(4) = bu(2)
        scl(1) = par(1)
        bl(1) = par(1)/10.
        bu(1) = par(1)*10.
        par(2) = 1.
        scl(2) = par(2)
        bl(2) = 0.25
        bu(2) = 4.
        if (nz.le.1) then
          bl(2) = par(2)
          bu(2) = par(2)
        endif
        par(3) = 0.5
        scl(3) = par(3)
        bl(3) = 0.01
        bu(3) = 1.
        if (na.le.1) then
          bl(3) = par(3)
          bu(3) = par(3)
        endif
        par(5) = 1.+par(4)
        scl(5) = par(5)
        bl(5) = 0.1
        bu(5) = 4.
        scl(7) = par(7)
        bl(7) = par(7)/10.
        bu(7) = par(7)*10.
        i = ixmax
        zz = zd(i)
        aa = ad(i)
        xx = xd(i)
        xm = yd(i)
        xx = (xx-par(8))*par(7)
        xm = (xm-par(9)+xx)**(par(4)+par(5)+1.)
        xm = xm-(par(1)*zz**par(2)*aa**par(3))**(par(4)+par(5)+1.)
        xm = xm-xx**(par(4)+par(5)+1.)
        xm = xm/(zz*zz*aa**par(4)*xx**par(5))
        if (xm.lt.0) xm = -xm
        par(6) = xm
        scl(6) = par(6)
        bl(6) = par(6)*1.e-8
        bu(6) = par(6)*1.e8
        call fitede(npts,zd,ad,xd,yd,ixt,0,scl,bl,bu,par,istate,irc)
        if (irc.ne.0 .and. irc.ne.1) return
        do i = 1,7
          scl(i) = par(i)
        enddo
        bl(8) = par(8)-(xmax-xmin)*0.5
        if (ih.ne.0) then
          par(10) = (xmax-xmin)*frngp/zmax
          scl(10) = par(10)
          bl(10) = par(10)*1.e-8
          bu(10) = par(10)*1000.
        endif
        call fitede(npts,zd,ad,xd,yd,ixt,ih,scl,bl,bu,par,istate,irc)
      endif
      return
      end
c
c                    **************************
c                    *  Print monitor routine *
c                    **************************
c
      subroutine monit(n,x,f,g,h,istate,istat,nit,iw,liw,w,lw)
      implicit   none
      integer    n, istat, nit, liw, lw
      integer    istate(n), iw(liw)
      real*8     f, x(n), g(n), h(*), w(lw)
c
      integer    i
c
      write(6,103) nit, f, istat
      write(6,100) (x(i), i=1,n)
      write(6,101) (g(i), i=1,n)
      write(6,102) (istate(i), i=1,n)
  100 format('     X =',5e14.6)
  101 format(('     g =',5e10.3))
  102 format(('   ist =',5(4x,i3,3x)))
  103 format(' --- N =',i4,3x,'F =',e11.4,3x,'status=',i3,'  ----')
      return
      end
C
C                ***********************************
C                *    ELIMINATION DE GAUSS         *
C                *   POUR RESOLUTION OU INVERSION  *
C                *  PIVOTAGE SUR TOUTE LA MATRICE  *
C                ***********************************
C
C   Elimination de Gauss et calcul du determinant. Etape necessaire a la
C   resolution d'un systeme lineaire ou inversion de matrice.
C
C      call gel8(n, m, eps, a, jp, det, icod)
c
C ENTREE
C   n    : [I]   taille effective de la matrice carree a
C   m    : [I]   premiere dimension de la matrice a (nbre max de lignes)
C   eps  : [R*8] seuil de non-singularite : en simple precision ~10**(-5)
C                en double precision ~10**(-14)
C   a    : [R*8] matrice a(ligne,colonne) de dimension a(m,*)
C RETOUR
C   a    : [R*8] matrice contenant le resultat de l'elimination de Gauss
C  jp    : [I]   jp(2,n) vecteur de permutation des lignes et colonnes
C  det   : [R*8] determinant de la matrice
C  icod  : [I]   code de retour :
C                0 -> OK
C                1 -> singularite probable (depend de eps)
C                2 -> pivot nul, elimination impossible (matrice singuliere)
C
      subroutine gel8(n, m, eps, a, jp, det, icod)
      implicit   none
      integer        n, m, icod
      integer        jp(2,*)
      real*8         det
      real*8         a(m,*)
c               Modules de nombres
      real*8         eps
c
      integer        i, j, k, ic, ic0, il0, k1, n1
      real*8         piv, api, cor
c               Modules de nombres
      real*8         pivs, pivl, aa
      real*8         abs
c
      icod = 2
      ic0 = 0
      if(n.gt.1) then
c         ---  Recherche du pivot initial ---
        n1 = n-1
        det = 0.
        pivs = 0.
        il0 = -1
        do j = 1,n
          do i = 1,n
            pivl = abs(a(i,j))
            if(pivl.gt.pivs) then
              pivs = pivl
              il0 = i
              ic0 = j
            endif
          enddo
        enddo
        if(il0.lt.0) return
        icod = 0
c         ---  Boucle sur les lignes pour elimination ---
        det = 1.
        do k = 1,n1
          k1 = k+1
          jp(1,k) = il0
          jp(2,k) = ic0
c         ---  Permutation eventuelle de lignes ---
          if(il0.ne.k) then
            do i = k,n
              piv = a(il0,i)
              a(il0,i) = a(k,i)
              a(k,i) = piv
            enddo
          endif
          det = -det
c         ---  Permutation eventuelle de colonnes ---
          if(ic0.ne.k) then
            do i = 1,n
              piv = a(i,ic0)
              a(i,ic0) = a(i,k)
              a(i,k) = piv
            enddo
          endif
          det = -det
c          --- Elimination de Gauss et recherche pivot suivant ---
          piv = a(k,k)
          det = det*piv
          pivs = 0.
          il0 = -1
          do i = k1,n
            pivl = 0.
            api = -a(i,k)/piv
            ic = -1
            do j = k1,n
c                     ** transformation **
              cor = a(k,j)*api
              a(i,j) = a(i,j)+cor
c                     ** recherche pivot **
              aa = abs(a(i,j))
              if(aa.gt.pivl) then
                if(aa.gt.abs(cor)*eps) ic = j
                pivl = aa
              endif
            enddo
            a(i,k) = api
            if(ic.lt.0) then
              icod = 1
            else if(pivl.gt.pivs) then
              pivs = pivl
              ic0 = ic
              il0 = i
            endif
          enddo
          if(il0.lt.0) then
            do j = k1,n
              do i = k1,n
                pivl = abs(a(i,j))
                if(pivl.gt.pivs) then
                  pivs = pivl
                  il0 = i
                  ic0 = j
                endif
              enddo
            enddo
            if(il0.lt.0) then
              icod = 2
              det = 0.
              return
            endif
          endif
        enddo
        det = det*a(n,n)
c               --- Matrice a 1 element ---
      else
        det = a(1,1)
        if(det.ne.0.) icod = 0
      endif
      return
      end
C
C                ***********************************
C                *  RESOLUTION DE SYSTEME LINEAIRE *
C                ***********************************
C
C                 Resolution du systeme  A.X = B
C   Il faut appeler au prealable la routine gel8 pour effectuer l'elimination
C                 de Gauss sur la matrice A.
C
C      call res8(n, m, a, jp, x)
c
C ENTREE
C   n    : [I]   taille effective de la matrice carree a
C   m    : [I]   premiere dimension de la matrice a (nbre max de lignes)
C   a    : [R*8] matrice a(ligne,colonne) de dimension a(m,*) issue de la
C                routine gel8 (elimination de Gauss)
C  jp    : [I]   jp(2,n) vecteur de permutation des lignes et colonnes
C                issu de la routine gel8 (elimination de Gauss)
C  x     : [R*8] vecteur B(n) du second membre de l'equation
C RETOUR
C  x     : [R*8] vecteur X(n) solution de l'equation
C
      subroutine res8(n, m, a, jp, x)
      implicit     none
      integer        n, m
      integer        jp(2,*)
      real*8         a(m,*), x(*)
c
      integer        i, k, k1, n1
      real*8         api
c
      if(n.gt.1) then
        n1 = n-1
c         ---  Boucle sur les lignes pour elimination ---
        do k = 1,n1
          k1 = k+1
c         ---  Permutation eventuelle de lignes ---
          i = jp(1,k)
          if(i.ne.k) then
            api = x(i)
            x(i) = x(k)
            x(k) = api
          endif
c          --- Elimination de Gauss ---
          api = x(k)
          do i = k1,n
            x(i) = x(i)+api*a(i,k)
          enddo
        enddo
c             --- Substitution inverse ---
        do k = n,2,-1
          k1 = k-1
          api = x(k)/a(k,k)
          x(k) = api
          do i = 1,k1
            x(i) = x(i)-a(i,k)*api
          enddo
        enddo
        x(1) = x(1)/a(1,1)
c         ---  Restauration ordre des inconnues ---
        do k = n1,1,-1
          i = jp(2,k)
          if(i.ne.k) then
            api = x(i)
            x(i) = x(k)
            x(k) = api
          endif
        enddo
c               --- Matrice a 1 element ---
      else
        x(1) = x(1)/a(1,1)
      endif
      return
      end
C
C                *************************
C                *  INVERSION DE MATRICE *
C                *************************
C
C   Il faut appeler au prealable la routine gel8 pour effectuer l'elimination
C                 de Gauss sur la matrice A.
C
C      call inv8(n, m, jp, a)
c
C ENTREE
C   n    : [I]   taille effective de la matrice carree a
C   m    : [I]   premiere dimension de la matrice a (nbre max de lignes)
C  jp    : [I]   jp(2,n) vecteur de permutation des lignes et colonnes
C                issu de la routine gel8 (elimination de Gauss)
C   a    : [R*8] matrice a(ligne,colonne) de dimension a(m,*) issue de la
C                routine gel8 (elimination de Gauss)
C RETOUR
C   a    : [R*8] matrice a(ligne,colonne) contenant l'inverse de la matrice
C                initiale
C
      subroutine inv8(n, m, jp, a)
      implicit none
      integer        n, m
      integer        jp(2,*)
      real*8         a(m,*)
c
      integer        i, il, k, k1, k2, n1
      real*8         api, piv
c
      if(n.gt.1) then
        n1 = n-1
c        if(n.gt.2) then
c        --- Permutation eventuelle de lignes ---
          do k = 2,n1
            k2 = jp(1,k)
            if(k2.ne.k) then
              k1 = k-1
              do i = 1,k1
                api = a(k2,i)
                a(k2,i) = a(k,i)
                a(k,i) = api
              enddo
            endif
          enddo
c        --- Produit des Li ---
          do k = 2,n1
            k1 = k-1
            k2 = k+1
            do i = 1,k1
              api = a(k,i)
              do il = k2,n
                a(il,i) = a(il,i)+a(il,k)*api
              enddo
            enddo
          enddo
c        endif
c        --- Produit des Ui ---
        do k = n,1,-1
          k1 = k-1
          k2 = k+1
          piv = 1./a(k,k)
          a(k,k) = piv
c         if(k.gt.1) then
            do i = 1,k1
              api = -a(i,k)*piv
c             if(k.lt.n) then
                do il = k2,n
                  a(i,il) = a(i,il)+a(k,il)*api
                enddo
c             endif
              a(i,k) = api
            enddo
c         endif
c         if(k.lt.n) then
            do i = k2,n
              a(k,i) = a(k,i)*piv
            enddo
c         endif
        enddo
c        --- Produit  UL ---
        do k = 1,n
          do i = 1,n
            api = 0.
            k1 = k
            if(i.ge.k) then
              k1 = i+1
              api = a(k,i)
            endif
c           if(k1.le.n) then
              do k2 = k1,n
                api = api+a(k,k2)*a(k2,i)
              enddo
c           endif
            a(k,i) = api
          enddo
        enddo
c         ---  Permutation eventuelle de lignes ---
        do k = n1,1,-1
          k2 = jp(2,k)
          if(k2.ne.k) then
            do i = 1,n
              piv = a(k2,i)
              a(k2,i) = a(k,i)
              a(k,i) = piv
            enddo
          endif
c         ---  Permutation eventuelle de colonnes ---
          k2 = jp(1,k)
          if(k2.ne.k) then
            do i = 1,n
              piv = a(i,k2)
              a(i,k2) = a(i,k)
              a(i,k) = piv
            enddo
          endif
        enddo
c               --- Matrice a 1 element ---
      else
        a(1,1) = 1./a(1,1)
      endif
      return
      end
c
c                *********************************
c                * MINIMISATION SOUS CONTRAINTES *
c                *********************************
c
c      Il faudra que l'utilisateur fournisse une routine calculant
c   la fonction, le gradient et la matrice hessienne.
c
c      call mini8(n,funct,monit,iprint,mxcall,tolg,
c     &  ibound,bl,bu,dx,x,f,g,h,istate,iw,liw,w,lw,ifail)
c
c ENTREE :
c   n     : [I] nombre de variables
c   funct : [E] fonction utilisateur retournant la valeur de la fonction, son
c               gradient et derivees secondes.
c               Dans le cas ou on veut minimiser un chi2, on peut fournir
c               simplement la routine de librairie : mini8chi, qui appelle la
c               routine utilisateur miniuser.      
c           subroutine funct(iflag,n,x,f,g,h,iw,liw,w,lw)
c           ENTREE :
c           iflag : [I] >0 -> la matrice hessienne doit etre calculee 
c             n   : [I]   nombre de variables
c             x   : [R*8] tableau des variables             n valeurs
c           RETOUR :
c           iflag : [I]    inchange -> on continue
c                          <0       -> on arrete et on sort avec ifail=-1
c             f   : [R*8] valeur de la fonction
c             g   : [R*8] tableau du gradient               n valeurs
c             h   : [R*8] tableau de la matrice hessienne : a calculer si
c                         iflag>0     n*(n+1)/2 valeurs
c                     triangle superieur range par colonnes -> ordre :      
c                     (l,c) -> (1,1) (1,2) (2,2) (1,3) (2,3) (3,3) (1,4) ...
c            iw,liw,w,lw : valeurs transmises a mini8, les tableaux permettent
c                    la transmission d'informations utilisateur (au-dela des
c                    tailles liw et lw) entre funct et le programme
c  monit : [E] fonction utilisateur d'affichage
c              Pour simplifier on peut passer la routine standard de librairie
c              qui s'appelle mini8monit
c          subroutine monit(n,x,f,g,h,istate,istat,nit,iw,liw,w,lw)
c           ENTREE :
c             n   : [I]   nombre de variables
c             x   : [R*8] tableau des variables             n valeurs
c             f   : [R*8] valeur de la fonction
c             g   : [R*8] tableau du gradient               n valeurs
c             h   : [R*8] tableau de la matrice hessienne : n*(n+1)/2 valeurs
c                         triangle superieur range par colonnes -> ordre :
c                        (l,c) -> (1,1) (1,2) (2,2) (1,3) (2,3) (3,3) (1,4) ...
c           istate: [I]   tableau des etats des variables          n valeurs
c                         0->variable libre  1->variable en contrainte par bl
c                         2->variable en contrainte par bu
c                         3->variable en contrainte par bl et bu
c            istat: [I]  Etat de la minimisation
c                         1 : approximation parabomique OK
c                         2 : approximation parabolique mais on suit le 
c                             gradient (angle proche de 90 deg)
c                         3 : matrice non definie positive, on suit le gradient
c                        -1 : comme 1 mais le pas a ete trop grand
c                        -2 : comme 2 mais le pas a ete trop grand
c                        -3 : comme 3 mais le pas a ete trop grand
c             nit : [I]   numero de l'iteration       
c            iw,liw,w,lw : valeurs transmises a mini8
c  iprint : [I]   la routine monit sera appelee toutes les iprint iterations
c                 pas d'appel si iprint=0
c  mxcall : [I]   nombre max d'iterations, si mxcall est atteint on sort avec
c                 ifail = 2
c   tolg  : [R*8] tolerance d'arret sur le module du gradient : on arrete si la
c                 variation absolue (si tolg>0) ou relative au gradient initial
c                 (si tolg<0) est inferieure a abs(tolg).
c                 Si tolg=0 : pas de test d'arret sur le module du gradient.
c  ibound : [I]   indicateur de contraintes 
c                 0->pas de contrainte
c                 1->contraintes min (bl) et max (bu)
c                 2->contraintes sur deplacement entre 2 iterations (dx)
c                 3->contraintes sur deplacement entre 2 iterations (dx)
c                      et min (bl) et max (bu)
c    bl   : [R*8] tableau des bornes inferieures  (ibound=1 ou 3)  n valeurs
c    bu   : [R*8] tableau des bornes superieures  (ibound=1 ou 3)  n valeurs
c    dx   : [R*8] tableau des deplacements maximaux permis entre 2 iterations
c                 (ibound=2 ou 3)   n valeurs
c                 si composante >0 : represente ecart max en absolu
c                 si composante <0 : represente ecart max en relatif
c                 si composante =0 : pas de test d'ecart sur cette composante
c     x   : [R*8] tableau des variables             n valeurs
c    iw   : [I]   tableau de travail de taille liw, au-dela de liw des infos 
c                 peuvent etre transmises, ou bien zone de travail utilisateur
c    liw  : [I]   taille de iw, on doit avoir liw >= n*2
c     w   : [R*8] tableau de travail de taille lw, au-dela de lw des infos 
c                 peuvent etre transmises, ou bien zone de travail utilisateur
c    lw   : [I]   taille de w, on doit avoir lw >= n*(n+1)
c                 si on utilise mini8drv il faudra lw >= n*(n+1)+1
c                 si on utilise mini8chi il faudra lw >= n*(n+1), mais il faut
c                     dimensionner w a au moins lw+n         
c RETOUR :
c     x   : [R*8] tableau des variables             n valeurs
c     f   : [R*8] valeur de la fonction
c     g   : [R*8] tableau du gradient               n valeurs
c     h   : [R*8] tableau de la matrice hessienne : n*(n+1)/2 valeurs
c                 triangle superieur range par colonnes -> ordre :      
c                 (l,c) -> (1,1) (1,2) (2,2) (1,3) (2,3) (3,3) (1,4) ...
c   istate: [I]   tableau des etats des variables          n valeurs
c                 0->variable libre  1->variable en contrainte par bl
c                 2->variable en contrainte par bu
c                 3->variable en contrainte par bl et bu
c   ifail : [I] code de retour :
c               -1 -> arret par l'utilisateur dans funct
c                0 -> Ok
c                1 -> minimum peu marque
c                2 -> nombre d'iterations >= mxcall      
c               10 -> liw ou lw trop petits
c               11 -> incoherence dans les bornes : bu < bl ou x < bl ou x > bu
c
      subroutine mini8(n,iprint,mxcall,tolg,
     &  ibound,bl,bu,dx,x,f,g,h,istate,iw,liw,w,lw,ifail)
      implicit none
      integer     n, iprint, mxcall, lw, liw, ifail, ibound
      integer     iw(liw), istate(n)
      real*8      tolg, f, bl(n), bu(n), dx(n), x(n), g(n)
      real*8      h(*), w(lw)

      integer     i, ii, j, jj, k, kk, nlib, nfix, nsat, isat
      integer     nit, nit0, n2, nh, ndx, ndx1, istat
      real*8      gn, gnn, gni, fp, sc, scc, pp, tol
      logical     loop
      real*8      dabs
      integer     iabs
c
c  Organisation de iw :
c       - vecteur permutation                        n*2
c  Organisation de w  :
c       - matrice hessienne inversee                 n*n
c       - vecteur dx                                 n
c
      tol = 1.d-12
      n2 = n*n
      nh = n*(n+1)/2
      ndx = n2
      ndx1 = ndx+1
      ifail = 10
      if (liw.lt.n*2 .or. lw.lt.(ndx+n)) return
c                Etat par rapport aux contraintes
      nfix = 0
      nsat = 0
      ifail = 11
      do i = 1,n
        istate(i) = 0
      enddo
      if (ibound.eq.1 .or. ibound.eq.3) then
        do i = 1,n
          if (bl(i).gt.bu(i) .or. x(i).lt.bl(i) .or. x(i).gt.bu(i))
     &      return
          if (x(i).eq.bl(i)) istate(i) = 1
          if (x(i).eq.bu(i)) istate(i) = istate(i)+2
          if (istate(i).eq.3) nfix = nfix+1
          if (istate(i).eq.1 .or. istate(i).eq.2) nsat = nsat+1
        enddo
      endif
c              ***   Boucle sur les iterations ***
      fp = 0.
      gn = 0.
      gni = 0.
      ifail = 0
      istat = 0
      nit0 = 0
      nit = 0
      loop = .true.
      do while (nit.lt.mxcall .and. loop)
        nit = nit+1
        i = 1
c               *  Appel fonction utilisateur *
        call mini8chi(i,n,x,f,g,h,iw,liw,w,lw)
        if (i.lt.0) then
          ifail = -1
          return
         endif
c ---------------------------------------------------
c        write(6,*) ' ------------------------------------------'
c        write(6,'(3(3x,a,i4))') 'n=',n,'nfix=',nfix,'nsat=',nsat
c        write(6,'(x,a,20f14.6)') 'x=',(x(i),i=1,n)
c        write(6,'(x,a,f14.6)') 'f=',f
c        write(6,'(x,a,20e14.6)') 'g=',(g(i),i=1,n)
c        write(6,'(x,a,20(i8,6x))') 'ist=',(istate(i),i=1,n)
c        write(6,'(x,a,380e14.6)') 'h=',(h(i),i=1,nh)
c ---------------------------------------------------
c                  **  La fonction a decru **
        if (nit.eq.1 .or. fp.ge.f) then
          fp = f
c       Changement signe de istate pour variables de-saturables 
c                 (pour garder en memoire)
          if (nsat.ne.0) then
            do i = 1,n
              if (istate(i).eq.1 .or. istate(i).eq.2)
     &          istate(i) = -istate(i)
            enddo
          endif
c             Boucle sur les contraintes saturables immediatement
c     et resolution parabolique tant que des contraintes initiales s'ajoutent
          isat = 0
          do while (isat.le.nsat .and. loop)
            nlib = n-nfix-isat
c     Recopie gradient + calcul norme gradient (variables libres) 
            gn = 0.
            k = ndx
            do i = 1,n
              if (istate(i).le.0) then
                k = k+1
                gn = gn+g(i)*g(i)
                w(k) = -g(i)
              endif
            enddo
c                 Recopie matrice hessienne pour inversion
            k = 0
            do j = 1,n
              kk = j
              jj = (j-1)*n
              do i = 1,j
                k = k+1
                jj = jj+1
                w(jj) = h(k)
                w(kk) = h(k)
                kk = kk+n
              enddo
            enddo
c         On suppose la matrice hessienne rangee en colonnes :
c                l'indice de ligne varie le plus vite
c ---------------------------------------------------
c            write(6,'(x,a,i3)') 'isat =',isat
c            if (isat.eq.0) then
c              write(6,'(x,a,i3,4x,a,i3)') 
c     &          'Matrice hessienne initiale + gradient: n=',n,
c     &          'nlib=',nlib
c              do i = 1,n
c                write(6,'(21(x,e14.6))') (w(i+(j-1)*n),j=1,n),-g(i)
c              enddo
c            endif
c ---------------------------------------------------
c                 Suppression lignes et colonnes sur contraintes
            if (nlib.ne.n) then
c                          colonne
              kk = n
              do j = n,1,-1
                if (istate(j).gt.0) then
                  if (j.lt.kk) then
                    jj = (j-1)*n
                    k = (kk-j)*n
                    do i = 1,k
                      jj = jj+1
                      w(jj) = w(jj+n)
                    enddo
                  endif
                  kk = kk-1
                endif
              enddo
c                             ligne
              jj = n
              kk = kk*n
              do j = n,1,-1
                if (istate(j).gt.0) then
                  k = (nlib-1)*jj+j+1
                  do ii = nlib,1,-1
                    do i = k,kk
                      w(i-1) = w(i)
                    enddo
                    kk = kk-1
                    k = k-jj
                  enddo
                  jj = jj-1
                endif
              enddo
c ---------------------------------------------------
c              write(6,*)
c     &          'Matrice hessienne apres contraintes + gradient :'
c              do i = 1,nlib
c                write(6,'(21(x,e14.6))') (w(i+(j-1)*nlib),j=1,nlib),
c     &            w(ndx+i)
c              enddo
c ---------------------------------------------------
            endif
            call gel8(nlib,nlib,tol,w,iw,sc,i)
c ---------------------------------------------------
c            write(6,'(x,a,i3,5x,a,e14.7)') 'Resolution : irc=',i,
c     &        'det=',sc
c ---------------------------------------------------
c                 Resolution pour le deplacement
            loop = .false.
            istat = 3
            if (i.eq.0) then
              call res8(nlib,nlib,w,iw,w(ndx1))
              istat = 1
c        Injection de 0 dans le deplacement pour les directions contraintes
              if (nlib.ne.n) then
                kk = ndx+nlib
                k = ndx+nlib+1
                do j = n,1,-1
                  if (istate(j).gt.0) then
                    if (k.le.kk) then
                      do i = kk,k,-1
                        w(i+1) = w(i)
                      enddo
                    endif
                    kk = kk+1
                    w(k) = 0.
                  else
                    k = k-1
                  endif
                enddo
              endif
c          Test des contraintes en attente
              loop = .true.
              if (isat.lt.nsat) then
                i = 0
                do while (i.lt.n .and. loop)
                  i = i+1
                  if (istate(i).eq.-1) then
                    loop = (w(ndx+i).gt.0.)
                  else if (istate(i).eq.-2) then
                    loop = (w(ndx+i).lt.0.)
                  endif
                enddo
                if (.not.loop) istate(i) = -istate(i)
              endif
              loop = .not.loop
c ---------------------------------------------------
c              if (loop) write(6,'(x,a,i2,3x,a,20e12.5)') 'i =',
c     &          i,'dl =',(w(k),k=ndx1,ndx+n)
c ---------------------------------------------------
              if (loop) isat = isat+1
            endif
          enddo
c     Tableau istate :
c      0 : pas de contrainte
c      3 : contrainte fixe
c    1,2 : contrainte initiale saturee par la resolution
c  -1,-2 : contrainte initiale de-saturee par la resolution
c ---------------------------------------------------
c          write(6,'(3(a,i3,3x))')
c     &      ' Fin sur contraintes en attente : nsat=',nsat,
c     &      'isat=',isat,'istat=',istat
c          write(6,'(x,a,20(i8,6x))') 'ist=',(istate(i),i=1,n)
c ---------------------------------------------------
c             Calcule de  G.dl
          if (istat.eq.1) then
            sc = 0.
            k = ndx
            do i = 1,n
              k = k+1
              sc = sc+g(i)*w(k)
            enddo
            istat = 3
            if (sc.lt.0.) istat = 2
            if (sc.lt.-dabs(f*tolg)) istat = 1
c                  Sauvegarde deplacement 
c   pour recuperation eventuelle apres calcul deplacement gradient
            gnn = gn
            scc = sc
            do i = 1,n
              w(i) = w(ndx+i)
            enddo
c ---------------------------------------------------
c            write(6,'(x,a,i3,4x,a,e12.3)') 'istat=',istat,'sc=',sc
c            write (6,'(x,a,20e12.5)') 'dxav=',(w(ndx+i),i=1,n)
c ---------------------------------------------------
          endif
c      On tente la descente suivant le gradient
          if (istat.gt.1) then
c                 Recopie gradient et calcul de tG.G
            gn = 0.
            k = ndx
            do i = 1,n
              k = k+1
              w(k) = g(i)
              if (istate(i).eq.3) then
                w(k) = 0.
              else if (istate(i).eq.1) then
                if (w(k).ge.0.) then
                  istate(i) = 11
                  w(k) = 0.
                endif
              else if (istate(i).eq.-1) then
                if (w(k).ge.0.) then
                  istate(i) = -11
                  w(k) = 0.
                endif
              else if (istate(i).eq.2) then
                if (w(k).le.0.) then
                  istate(i) = 12
                  w(k) = 0.
                endif
              else if (istate(i).eq.-2) then
                if (w(k).le.0.) then
                  istate(i) = -12
                  w(k) = 0.
                endif
              endif
              gn = gn+w(k)**2
            enddo
c     Tableau istate :
c        0 : pas de contrainte
c        3 : contrainte fixe
c     1,2  : contrainte initiale saturee par la resolution
c    -1,-2 : contrainte initiale desaturee par la resolution
c  -11,-12 : contrainte initiale desaturee par resolution, saturee par gradient
c    11,12 : contrainte initiale saturee par resolution, saturee par gradient
c ---------------------------------------------------
c            write(6,'(x,a,20(i8,6x))') 'ist=',(istate(i),i=1,n)
c ---------------------------------------------------
c                   Produit tG.H.G
            pp = 0.
            jj = 1
            do j = 1,n
              sc = 0.
              kk = jj
              do i = 1,n
                sc = sc+h(kk)*w(ndx+i)
                if (i.lt.j) then
                  kk = kk+1
                else
                  kk = kk+i
                endif
              enddo
              pp = pp+sc*w(ndx+j)
              jj = jj+j
            enddo
c ---------------------------------------------------
c            write(6,'(2(4x,a,e14.5))') 'tGG=',gn,'tGHG=',pp
c ---------------------------------------------------
            if (pp.eq.0.) pp = 1.
c                  Calcul deplacement suivant gradient
            pp = gn/pp
            pp = dabs(pp)
            sc = 0.
            k = ndx
            do i = 1,n
              k = k+1
              w(k) = -pp*w(k)
              sc = sc+w(k)*g(i)
            enddo
c                 Suivi parabolique plus performant
            if (istat.eq.2 .and. scc.le.sc) then
              istat = 1
              do i = 1,n
                w(ndx+i) = w(i)
              enddo
              gn = gnn
            endif
c ---------------------------------------------------
c            write(6,'(2(4x,a,e14.5),3x,a,i2)') 'sc =',sc,
c     &        'scc =',scc,'-> istat =',istat
c ---------------------------------------------------
          endif
c   Actualisation du tableau istate, etat actuel :
c        0 : pas de contrainte
c        3 : contrainte fixe
c     1,2  : contrainte initiale saturee par la resolution
c    -1,-2 : contrainte initiale desaturee par la resolution
c  -11,-12 : contrainte initiale desaturee par resolution, saturee par gradient
c    11,12 : contrainte initiale saturee par resolution, saturee par gradient
          if (nit.eq.1) gni = gn
c                On suit le gradient  (istate=0,3,-11,-12,11,12)
          if (istat.gt.1) then
            nit0 = 0
            nsat = 0
            do i = 1,n
              if (istate(i).lt.0) istate(i) = -istate(i)
              if (istate(i).gt.10) then
                nsat = nsat+1
                istate(i) = istate(i)-10
              endif
            enddo
c                    On va au minimum de la parabole 
c                (istate peut prendre toutes les valeurs)
          else
            nit0 = nit0+1
            nsat = 0
            do i = 1,n
              if (istate(i).lt.0) then
                istate(i) = 0
              else if (istate(i).ne.0 .and. istate(i).ne.3) then
                nsat = nsat+1
                if (istate(i).gt.10) istate(i) = istate(i)-10
              endif
            enddo
          endif
c         **  La fonction remonte -> 1/2 pas **
        else
c ---------------------------------------------------
c          write(6,'(x,a)')
c     &      'On est alle trop loin : division du pas par 2'
c ---------------------------------------------------
          istat = -iabs(istat)
          nit0 = 0
          do i = 1,n
            k = ndx+i
            x(i) = x(i)-w(k)
            if (w(k).ne.0. .and. istate(i).ne.0) then
              istate(i) = 0
              nsat = nsat-1
            endif
            w(k) = w(k)/2.
          enddo
        endif
c                   **  Affichage **
        if (iprint.ne.0) then
          if (mod(nit-1,iprint).eq.0)
     &      call monit(n,x,f,g,h,istate,istat,nit,iw,liw,w,lw)
        endif
c   **  Test nouvelles contraintes -> limitation deplacement **
        if (istat.gt.0 .and. (ibound.eq.1 .or.ibound.eq.3)) then
          sc = 1.
          j = 0
          k = ndx
          do i = 1,n
            k = k+1
            if (istate(i).eq.0 .and. w(k).ne.0.) then
              pp = bu(i)
              if (w(k).lt.0.) pp = bl(i)
              pp = (pp-x(i))/w(k)
              if (pp.lt.sc) then
                sc = pp
                j = i
              endif
            endif
          enddo
          if (j.ne.0) then
c ---------------------------------------------------
c            write (6,'(x,a,i3)') 'Avant limitation sur contrainte',j
c            write (6,'(x,a,20e14.6)') 'dx=',(w(ndx+i),i=1,n)
c ---------------------------------------------------
            istate(j) = 1
            if (w(ndx+j).gt.0.) istate(j) = 2
            k = ndx
            do i = 1,n
              k = k+1
              w(k) = w(k)*sc
            enddo
            nsat = nsat+1
          endif
        endif
c   **  limitation deplacement sur limitation ecart  **
        if (istat.gt.0 .and. (ibound.eq.2.or.ibound.eq.3)) then
          sc = 0.
          k = ndx
          do i = 1,n
            k = k+1
            if (dx(i).ne.0.) then
              pp = dabs(w(k)/dx(i))
              if (dx(i).lt.0. .and. x(i).ne.0.) pp = pp/dabs(x(i))
              if (pp.gt.sc) sc = pp
            endif
          enddo
          if (sc.gt.1.) then
c ---------------------------------------------------
c            write (6,'(x,a)') 'Avant limitation sur ecart'
c            write (6,'(x,a,20e14.6)') 'dx=',(w(ndx+i),i=1,n)
c ---------------------------------------------------
            k = ndx
            do i = 1,n
              k = k+1
              if (w(k).ne.0. .and. istate(i).ne.0) then
                istate(i) = 0
                nsat = nsat-1
              endif
              w(k) = w(k)/sc
            enddo
          endif
        endif
c                  **  On fait le deplacement **
        do i = 1,n
          x(i) = x(i)+w(ndx+i)
        enddo
c ---------------------------------------------------
c        write (6,'(x,a,e14.6)') 'sqrt(gn)=',dsqrt(gn)
c        write (6,'(x,a,20e14.6)') 'dx=',(w(ndx+i),i=1,n)
c ---------------------------------------------------
c                  **   Tests d'arret **
        loop = .true.
        if (nit.gt.1 .and. istat.ge.1 .and. istat.le.2) then
          i = 0
          loop = .false.
          if (tolg.ne.0.) then
            sc = gn
            if (tolg.lt.0.) sc = sc/gni
            loop = loop .or. sc.gt.tolg**2
            i = i+1
          endif
          loop = loop .or. i.eq.0
          loop = loop .and. (n-nfix-nsat).gt.0
        endif
c           * Test deplacement/valeur pour pertinence precision *
        if (loop) then
          sc = 0.
          do i = 1,n
            if (istate(i).eq.0) then
              pp = 1.
              if (x(i).ne.0.) pp = dabs(w(ndx+i)/x(i))
              if (pp.gt.sc) sc = pp
            endif
          enddo
          if (sc.lt.tol) then
            loop = .false.
            ifail = 1
          endif
        endif
        if (loop) then
          sc = 0.
          do i = 1,n
            pp = dabs(w(ndx+i)*g(i))
            if (pp.gt.sc) sc = pp
          enddo
          pp = tol*dabs(f)
          if (sc.lt.pp) then
            loop = .false.
            ifail = 1
          endif
        endif
      enddo
      if (nit.ge.mxcall) ifail = 2
      return
      end
c
c                *************************************
c                * AFFICHAGE A UNE ETAPE D'ITERATION *
c                *************************************
c
c    Meme interface que fonction monit de mini8
c
      subroutine mini8monit(n,x,f,g,h,istate,istat,nit,iw,liw,w,lw)
      implicit   none
      integer    n, istat, nit, liw, lw
      integer    istate(n), iw(liw)
      real*8     f, x(n), g(n), h(*), w(lw)
c
      integer    i
c
      write(6,103) nit, f, istat
      write(6,100) (x(i), i=1,n)
      write(6,101) (g(i), i=1,n)
      write(6,102) (istate(i), i=1,n)
  100 format('     X =',5e14.6)
  101 format(('     g =',5e10.3))
  102 format(('   ist =',5(4x,i3,3x)))
  103 format(' --- N =',i4,3x,'F =',e11.4,3x,'status=',i3,'  ----')
      return
      end
c
c       *************************************************************
c       * INVERSION MATRICE HESSIENNE SUR VARIABLES NON CONTRAINTES *
c       *************************************************************
c
c    Inversion valable seulement si ifail=0 en sortie de mini8
c    Les lignes et colonnes des variables contraintes seront remplies de 0
c
c      call mini8invh(n,istate,h,iw,liw,w,lw)
c
c  ENTREE :
c     n   : [I]   nombre de variables
c   istate: [I]   tableau des etats des variables          n valeurs
c                 0->variable libre  1->variable en contrainte par bl
c                 2->variable en contrainte par bu
c                 3->variable en contrainte par bl et bu
c     h   : [R*8] tableau de la matrice hessienne : n*(n+1)/2 valeurs
c                 triangle superieur range par colonnes -> ordre :      
c                 (l,c) -> (1,1) (1,2) (2,2) (1,3) (2,3) (3,3) (1,4) ...
c  iw,liw,w,lw : valeurs transmises a mini8, il faut lw >= n*n et liw >= n*2
c  RETOUR :
c     h   : [R*8] matrice hessienne inversee : n*(n+1)/2 valeurs
c
      subroutine mini8invh(n,istate,h,iw,liw,w,lw)
      implicit none
      integer    n, liw, lw
      integer    istate(n), iw(liw)
      real*8     h(*), w(lw)

      integer    i, ii, j, jj, k, kk, nlib
      real*8     tol, vv

      if (lw.lt.n*n .or. liw.lt.n*2) return
      tol = 1.e-10
c                 Comptage des contraintes
      nlib = n
      do i = 1,n
        if (istate(i).gt.0) nlib = nlib-1
      enddo
c                 Recopie matrice hessienne pour inversion
      k = 0
      do j = 1,n
        kk = j
        jj = (j-1)*n
        do i = 1,j
          k = k+1
          jj = jj+1
          w(jj) = h(k)
          w(kk) = h(k)
          kk = kk+n
        enddo
      enddo
c                 Suppression lignes et colonnes sur contraintes
      if (nlib.ne.n) then
c                          colonne
        kk = n
        do j = n,1,-1
          if (istate(j).gt.0) then
            if (j.lt.kk) then
              jj = (j-1)*n
              k = (kk-j)*n
              do i = 1,k
                jj = jj+1
                w(jj) = w(jj+n)
              enddo
            endif
            kk = kk-1
          endif
        enddo
c                             ligne
        jj = n
        kk = kk*n
        do j = n,1,-1
          if (istate(j).gt.0) then
            k = (nlib-1)*jj+j+1
            do ii = nlib,1,-1
              do i = k,kk
                w(i-1) = w(i)
              enddo
              kk = kk-1
              k = k-jj
            enddo
            jj = jj-1
          endif
        enddo
      endif
c                Inversion      
      call gel8(nlib,nlib,tol,w,iw,vv,i)
      call inv8(nlib,nlib,iw,w)
c                 Introduction lignes et colonnes de 0 pour contraintes
      if (nlib.ne.n) then
c                             ligne
        jj = nlib
        kk = nlib*nlib
        do j = 1,n
          if (istate(j).gt.0) then
            k = j
            do ii = 1,nlib
              do i = kk,k,-1
                w(i+1) = w(i)
              enddo
              w(k) = 0.
              kk = kk+1
              k = k+jj+1
            enddo
            jj = jj+1
          endif
        enddo
c                          colonne
        kk = nlib
        do j = 1,n
          if (istate(j).gt.0) then
            jj = (j-1)*n
            if (j.le.kk) then
              k = (kk-j+1)*n
              do i = k,1,-1
                ii = jj+i
                w(ii+n) = w(ii)
              enddo
            endif
            k = jj+n
            jj = jj+1
            do i = jj,k
              w(i) = 0.
            enddo
            kk = kk+1
          endif
        enddo
      endif
c                 Recopie matrice hessienne pour inversee
      k = 0
      do j = 1,n
        jj = (j-1)*n
        do i = 1,j
          k = k+1
          jj = jj+1
          h(k) = w(jj)
        enddo
      enddo
      return
      end
c
c                ******************************
c                * TEST DE CALCUL DE DERIVEES *
c                ******************************
c
c      call mini8drv(n,funct,x,dx,g,h,iw,liw,w,lw,iout)
c
c ENTREE :
c   n   : [I]   nombre de variables, si n<0 la valeur absolue sera consideree
c               et seul le gradient sera verifie
c   x   : [R*8] tableau des variables                                n valeurs
c   dx  : [R*8] tableau des differences finies sur variables         n valeurs
c  iout : [I]   unite logique d'impression (6 pour la sortie standard)
c TABLEAUX DE TRAVAIL : 
c   g   : [R*8] tableau du gradient               n valeurs
c   h   : [R*8] tableau de la matrice hessienne : n*(n+1)/2 valeurs
c  iw,liw,w,lw : valeurs transmises a mini8, il faut lw >= n*(n+1)+1
C
      subroutine mini8drv(n,x,dx,g,h,iw,liw,w,lw,iout)
      implicit    none
      integer     n, liw, lw, iout
      integer     iw(liw)
      real*8      x(n), dx(n), g(n), h(*), w(lw)
c
      integer     i, idec, j, k, nn, ifl
      real*8      f, ddx, ddx2, df, ddf, fp, fm, hh
c
c      Occupation du tableau de travail w :
c       - n valeurs pour premier calcul du gradient
c       - n valeurs pour deuxieme calcul du gradient
c       - n*(n+1)/2 valeurs pour  calcul de matrice hessienne
c
      nn = n
      if (nn.lt.0) nn = -n
      if (lw.lt.nn*(nn+5)/2) then
        write(iout,'(x,a,i4,a,i4)') 'lw trop petit :',lw,
     &    ' < ',nn*(nn+5)/2
        return
      endif
      ifl = 1
      call mini8chi(ifl,nn,x,f,g,h,iw,liw,w,lw)
      ifl = 0
      do i = 1,nn
        ddx = dx(i)
        ddx2 = ddx*2.
        x(i) = x(i)-ddx
        call mini8chi(ifl,nn,x,fm,w,w(nn*2+1),iw,liw,w,lw)
        x(i) = x(i)+ddx2
        call mini8chi(ifl,nn,x,fp,w(nn+1),w(nn*2+1),iw,liw,w,lw)
        x(i) = x(i)-ddx
        df = (fp-fm)/ddx2
        ddf = 1.e20
        if(df.ne.0.) then
          ddf = (df-g(i))/df
        else if (g(i).eq.0.) then
          ddf = 0.
        endif
        write(iout,'(/,a,i3,4(4x,a,e12.4))') ' ----- Variable',i,
     &    'dx =',dx(i),'df/dx =',df,'g =',g(i),'Delta =',ddf
        idec = i*(i-1)/2
        k = i
        if (n.lt.0) k = 0
        do j = 1,k
          hh = h(idec+j)
          df = (w(nn+j)-w(j))/ddx2
          ddf = 1.e20
          if(df.ne.0.) then
            ddf = (df-hh)/df
          else if (hh.eq.0.) then
            ddf = 0.
          endif
          write(iout,'(5x,a,i3,3(4x,a,e12.4))') 'j =',j,
     &      'dg/dx =',df,'H =',hh,'Delta =',ddf
        enddo
      enddo
      return
      end
c
c                **********************************
c                * CALCUL DE CHI2 ET SES DERIVEES *
c                **********************************
c
c      call mini8chi(iflag, n, x, f, g, h, iw, liw, w, lw)
c    Meme interface que fonction funct de mini8
c    On utilise dans w la suite de la zone de travail de mini8, comme zone
c  de travail pour chi2 : gradient fonction utilisateur   :  n valeurs
c  Il faudra donc dimensionner w a au moins lw+n
c
c    mini8chi calcule une somme de fi**2 et appelle pour chaque point la
c    fonction utilisateur :
c       subroutine miniuser(ip,x,df,g,iwf)
c        ENTREE :
c          ip   : [I]   numero du point utilisateur (commence a 1)
c           x   : [R*8] tableau des variables             n valeurs
c        RETOUR :
c          df   : [R*8] valeur de l'ecart pour ce point
c           g   : [R*8] gradient de la fonction utilisateur         n valeurs
c          iwf  : [I]   >=0 -> facteur de ponderation (0 pour eliminer le point)
c                       <0  -> plus de points : iteration terminee
c
      subroutine mini8chi(ifl, n, x, f, g, h, iw, liw, w, lw)
      implicit    none
      integer      ifl, n, liw, lw
      integer      iw(*)
      real*8       f, x(n), g(n), h(*), w(*)
c
      integer      i, k, kk, nh, idg, idg1, inp, ip, iwf
      real*8       df, gf, wf

      nh = n*(n+1)/2
      idg = lw
      idg1 = idg+1
c     initialisation f,g et h
      f = 0.
      do i = 1,n
        g(i) = 0.
      enddo
      do i = 1,nh
        h(i) = 0.
      enddo
c          ---- Boucle sur les points (les cellules)------
      inp = 0
      ip = 1
c
      call miniuser(ip,x,df,w(idg1),iwf)
      do while (iwf.ge.0)
c     --- Debut de boucle ---
        if (iwf.gt.0) then
          inp = inp+iwf
          wf = iwf
          f = f+wf*df**2
          do i = 1,n
            g(i) = g(i)+wf*df*w(idg+i)
          enddo
          kk = 0
          do k = 1,n
            gf = w(idg+k)
            do i = 1,k
              kk = kk+1
              h(kk) = h(kk)+wf*gf*w(idg+i)
            enddo
          enddo
        endif
        ip = ip+1
        call miniuser(ip,x,df,w(idg1),iwf)
      enddo
c       --- Fin de boucle ---
      do i = 1,n
        g(i) = g(i)*2.
      enddo
      do i = 1,nh
        h(i) = h(i)*2.
      enddo
      return
      end
