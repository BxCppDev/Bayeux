c*****
 	real*4 function fermi(Z,E)
	implicit none
	real*4 Z,E
	real*4 zz,ee
	real*4 fermi_wrap
	zz=Z
	ee=E
 	fermi=fermi_wrap(zz,ee)
 	return
 	end
c*****
c*****
c***** problem with 
c*****
	real*4 function fermi_cern(Z,E)
c Function fermi calculates the traditional function of Fermi in
c theory of beta decay to take into account the Coulomb correction
c to the shape of electron/positron energy spectrum.
c Call  : corr=fermi(Z,E)
c Input : Z - atomic number of daughter nuclei (>0 for e-, <0 for e+);
c         E - kinetic energy of particle (MeV; E>50 eV).
c Output: corr - value of correction factor (without normalization -
c                constant factors are removed - for MC simulation).
c V.I.Tretyak, 15.07.1992.
        real*4 pi,emass
	real*4 lnr,p,y,g
        real*4 zz,ee
	complex carg,algama
        !print *, 'devel: fermi_cern: pi=', pi
        !print *, 'devel: fermi_cern: emass=', emass
        pi=acos(-1.)
        emass=0.511
	zz=Z
	ee=E
	if(ee.lt.50.e-6) ee=50.e-6
	alfaz=zz/137.036
	w=ee/emass+1.
	p=sqrt(w*w-1.)
	y=alfaz*w/p
	g=sqrt(1.-alfaz**2)
	carg=cmplx(g,y)
        lnr=algama(carg)
        r=exp(lnr)
	fermi_cern=p**(2.*g-2.)*exp(pi*y+2.*alog(r))
	return
	end
c*****
	real*4 function fermi_cern_2(Z,E)
        real*4 pi,emass
        real*4 zz,ee
        real alpha,aZ,g0,nu,w,k,r0,a,R,res,lnr,rr,lnden,den,hbarc
        complex carg,algama
        pi=acos(-1.)
        emass=0.511
	zz=Z
	ee=E
	if(ee.lt.50.e-6) ee=50.e-6
        !print *, 'devel: fermi_cern: pi=', pi
        !print *, 'devel: fermi_cern: emass=', emass
        !print *, 'devel: fermi_cern: E=', E
        alpha=1./137.036
	aZ=alpha*zz
        g0=sqrt(1.-aZ*aZ)
        !print *, 'devel: fermi_cern: g0=', g0
        w=ee/emass + 1.
        !print *, 'devel: fermi_cern: w=', w
        k=sqrt(w*w-1.0)
        !print *, 'devel: fermi_cern: k=', k
        nu=zz*ee*alpha/k
        !print *, 'devel: fermi_cern: nu=', nu
        a = 0.0075 * zz**2  + 1.9 * zz + 0.25
        !print *, 'devel: fermi_cern: a=', a
        r0=1.2 
        hbarc = 197.3269631
        R = r0 * exp (alog (a) / 3.) * emass / hbarc
        !print *, 'devel: fermi_cern: R=', R
	carg=cmplx(g0,nu)
        !print *, 'devel: fermi_cern: carg=', carg
        lnr=algama(carg)
        !print *, 'devel: fermi_cern: lnr=', lnr
        rr=exp(lnr)
        !print *, 'devel: fermi_cern: rr=', rr
        !den=gamma(2*g0+1)
	carg=cmplx(2*g0+1,0.0)
        lnden=algama(carg)
        den=exp(lnden)
        !print *, 'devel: fermi_cern: den=', den
        res = 2.*(1.+g0)*exp(2.*(g0-1.)*alog(2*k*R))*exp (pi*nu) 
        res = res * (rr**2) / (den**2)
        fermi_cern_2 = res
	return
	end
c

c -- end
