c bb.f 
c
c This file was extracted from the 'decay0' program by V.I. Tretyak
c
c Copyright 1995-2011 V.I. Tretyak
c
c This program is free software; you can redistribute it and/or modify
c it under the terms of the GNU General Public License as published by
c the Free Software Foundation; either version 3 of the License, or (at
c your option) any later version.
c 
c This program is distributed in the hope that it will be useful, but
c WITHOUT ANY WARRANTY; without even the implied warranty of
c MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
c General Public License for more details.
c 
c You should have received a copy of the GNU General Public License
c along with this program; if not, write to the Free Software
c Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
c 
c
	subroutine bb(modebb,Qbb,Edlevel,EK,Zdbb,Adbb,istartbb)

c Subroutine for sampling the energies and angles of electrons in various
c modes of double beta decay without Primakoff-Rosen approximation.
c
c Call  : common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100)
c         common/const/pi,emass
c         common/enrange/ebb1,ebb2,toallevents
c         call bb(modebb,Qbb,Edlevel,EK,Zdbb,Adbb,istartbb)
c Input : modebb    - mode of double beta (- or +) decay:
c                     1  - 0nubb with neutrino mass, 0+ -> 0+,      2n;
c                     2  - 0nubb with rhc-lambda,    0+ -> 0+,      2n;
c                     3  - 0nubb with rhc-lambda,    0+ -> 0+, 2+,  N*;
c                     4  - 2nubb,                    0+ -> 0+,      2n;
c                     5  - 0nuM1bb (Majoron, SI=1),  0+ -> 0+,      2n;
c                          [SI - spectral index]
c                     6  - 0nuM3bb (Majoron, SI=3),  0+ -> 0+,      2n;
c                     7  - 0nubb with rhc-lambda,    0+ -> 2+,      2n;
c                     8  - 2nubb,                    0+ -> 2+,      2n, N*;
c                     9  - 0nuKb+                    0+ -> 0+, 2+;
c                     10 - 2nuKb+                    0+ -> 0+, 2+;
c                     11 - 0nu2K                     0+ -> 0+, 2+;
c                     12 - 2nu2K                     0+ -> 0+, 2+;
c                     13 - 0nuM7bb (Majoron, SI=7)   0+ -> 0+,      2n;
c                     14 - 0nuM2bb (Majoron, SI=2)   0+ -> 0+,      2n;
c                     15 - 2nubb with bosonic nu's   0+ -> 0+;
c                     16 - 2nubb with bosonic nu's   0+ -> 2+;
c                     17 - onubb with rhc-eta        0+ -> 0+       simplified expression;
c                     18 - onubb with rhc-eta        0+ -> 0+       with specific NMEs;
c         Qbb       - energy release in double beta process: difference
c                     between masses of parent and daughter atoms (MeV);
c         Edlevel   - energy of level of daughter nucleus to which transition
c                     occured;
c         EK        - binding energy of electron on K shell of parent atom (MeV)
c                     (for modebb=10 and 11);
c         Zdbb      - atomic number of daughter nucleus (Z>0 for b-b-
c                     and Z<0 for b+b+ and eb+ processes);
c         Adbb      - mass number of daughter nucleus
c         istartbb  - must be =0 for first call of bb for a given mode;
c         pi,emass  - 3.1415927 and electron rest mass (MeV);
c         ebb1,ebb2 - for modes with continuous distribution of sum of
c                     e-/e+ energies (4,5,6,8,10 and 13), left and right
c                     limits for E sum in which events will be generated;
c Output: tevst               - time of event's start (sec) (as supposed,
c                               =0 here);
c         npfull              - current number of particle in event;
c         npgeant(npfull)     - GEANT number for particle identification
c                               (1 - gamma, 2 - e+, 3 - e-);
c         pmoment(1-3,npfull) - x,y,z components of particle momentum
c                               (MeV/c);
c         ptime(npfull)       - time shift from previous time to calculate
c                               when particle was emitted (sec)
c                               (as supposed, =0 here);
c         toallevents         - coefficient to calculate the corresponding
c                               number of bb events in full range of energies:
c                               full_number=generated_number*toallevents
c                               (for modes 4,5,6,8,10 and 13).
c
c V.I.Tretyak, 26.11.1995; 17.02.2009.

	double precision dens,denf,dummy
c --- 2012-07-18 FM: explicit real*8 dgmlt1
	double precision dgmlt1
	character chhelp*4,chfile*256
	external dshelp1
c --- 2012-07-18 FM: declare external dgmlt1
	external dgmlt1
	external fe12_mod4,  fe12_mod5,  fe12_mod6,  fe12_mod8,
     +           fe12_mod13, fe12_mod14, fe12_mod15, fe12_mod16,
     +           fe2_mod4,   fe2_mod5,   fe2_mod6,   fe2_mod8,
     +           fe2_mod13,  fe2_mod14,  fe2_mod15,  fe2_mod16,
     +           fe1_mod10
	dimension spthe1(4300),spthe2(4300)
        save spthe1,spmax
	common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),
     +                  ptime(100)
        common/const/pi,emass,datamass(50)
	common/enrange/ebb1,ebb2,toallevents,lhelp,chhelp
	common/denrange/dens,denf,mode
	common/helpbb/Zd,Ad,e0,e1
	common/eta_nme/chi_GTw,chi_Fw,chip_GT,chip_F,chip_T,
     +                 chip_P,chip_R
	twopi=2.*pi
	Zd=Zdbb
	Ad=Adbb
	if(Zdbb.ge.0.)                   e0=Qbb-Edlevel
	if(Zdbb.lt.0.)                   e0=Qbb-Edlevel-4.*emass
	if(modebb.eq. 9.or.modebb.eq.10) e0=Qbb-Edlevel-EK-2.*emass
	if(modebb.eq.11.or.modebb.eq.12) e0=Qbb-Edlevel-2.*EK
	if(modebb.eq.9) then
c fixed energies of e+ and X-ray; no angular correlation
	   call particle(2,e0,e0,0.,pi,0.,twopi,0.,0.,t)
	   call particle(1,EK,EK,0.,pi,0.,twopi,0.,0.,t)
	   return
	endif
	if(modebb.eq.11) then
c one gamma and two X-rays with fixed energies; no angular correlation
	   call particle(1,e0,e0,0.,pi,0.,twopi,0.,0.,t)
	   call particle(1,EK,EK,0.,pi,0.,twopi,0.,0.,t)
	   call particle(1,EK,EK,0.,pi,0.,twopi,0.,0.,t)
	   return
	endif
	if(modebb.eq.12) then
c fixed energies of two X-rays; no angular correlation
	   call particle(1,EK,EK,0.,pi,0.,twopi,0.,0.,t)
	   call particle(1,EK,EK,0.,pi,0.,twopi,0.,0.,t)
	   return
	endif
	if(istartbb.ne.0) go to 1
c calculate the theoretical energy spectrum of first particle with step
c of 1 keV and find its maximum
	!print *,'wait, please: calculation of theoretical spectrum'
	if(ebb1.lt.0.) ebb1=0.
	if(ebb2.gt.e0) ebb2=e0
	spmax=-1.
	b2amin=+1.e20 ! coefficients in angular correlation 
	b2amax=-1.e20 ! for eta-h term of 2b0nu
	relerr=1.e-4
	do i=1,int(e0*1000.)
	   e1=i/1000.
	   e1h=e1
	   spthe1(i)=0.
	   if(modebb.eq.1)  spthe1(i)=fe1_mod1(e1h)
	   if(modebb.eq.2)  spthe1(i)=fe1_mod2(e1h)
	   if(modebb.eq.3)  spthe1(i)=fe1_mod3(e1h)
	   elow=amax1(1.e-4,ebb1-e1+1.e-4)
	   ehigh=amax1(1.e-4,ebb2-e1+1.e-4)
c	   print *,'e1,elow,ehigh=',e1,elow,ehigh
	   if(modebb.eq.4.and.e1.lt.e0)  spthe1(i)=
cc     +      gauss(fe12_mod4,1.e-4,e0-e1+1.e-4,relerr)
c correction for restricted energy sum (ebb1<=e1+e2<=ebb2)
     +        gauss(fe12_mod4,elow,ehigh,relerr)
	   if(modebb.eq.5.and.e1.lt.e0)  spthe1(i)=
     +        gauss(fe12_mod5,elow,ehigh,relerr)
	   if(modebb.eq.6.and.e1.lt.e0)  spthe1(i)=
     +        gauss(fe12_mod6,elow,ehigh,relerr)
	   if(modebb.eq.7)  spthe1(i)=fe1_mod7(e1h)
	   if(modebb.eq.8.and.e1.lt.e0)  spthe1(i)=
     +        gauss(fe12_mod8,elow,ehigh,relerr)
	   if(modebb.eq.10) spthe1(i)=fe1_mod10(e1h)
	   if(modebb.eq.13.and.e1.lt.e0)  spthe1(i)=
     +        gauss(fe12_mod13,elow,ehigh,relerr)
	   if(modebb.eq.14.and.e1.lt.e0)  spthe1(i)=
     +        gauss(fe12_mod14,elow,ehigh,relerr)
	   if(modebb.eq.15.and.e1.lt.e0)  spthe1(i)=
     +        gauss(fe12_mod15,elow,ehigh,relerr)
	   if(modebb.eq.16.and.e1.lt.e0)  spthe1(i)=
     +        gauss(fe12_mod16,elow,ehigh,relerr)
	   if(modebb.eq.17)  spthe1(i)=fe1_mod17(e1h)
	   if(modebb.eq.18)  spthe1(i)=fe1_mod18(e1h)
	   if(spthe1(i).gt.spmax) spmax=spthe1(i)
c	   print *,'spthe1=',spthe1(i)
	enddo
	do i=int(e0*1000.)+1,4300
	   spthe1(i)=0.
	enddo
c	open(unit=33,file='th-e1-spectrum.dat')
c	do i=1,4300
c	   write(33,*) i,spthe1(i)
c	enddo
	toallevents=1.
	if(modebb.eq. 4.or.modebb.eq. 5.or.modebb.eq. 6.or.
     +     modebb.eq. 8.or.modebb.eq.13.or.modebb.eq.14) then
	   mode=modebb
	   dens=0.d0
	   denf=e0
	   !dummy=dgmlt1(dshelp1,0.d0,denf,8,8,d_el)
	   !print *,'DEVEL:dummy(r1)=',dummy
	   r1=dgmlt1(dshelp1,0.d0,denf,8,8,d_el)
	   !print *, 'DEVEL: r1=',r1
	   dens=ebb1
	   denf=ebb2
	   !dummy=dgmlt1(dshelp1,0.d0,denf,8,8,d_el)
	   !print *,'DEVEL:dummy(r2)=',dummy
	   r2=dgmlt1(dshelp1,0.d0,denf,8,8,d_el)
	   !print *,'DEVEL: r2=',r2
	   toallevents=r1/r2
	elseif(modebb.eq.10) then
	   toallevents=gauss(fe1_mod10,1.e-4,e0+1.e-4,relerr)
     +                /gauss(fe1_mod10,ebb1+1.e-4,ebb2+1.e-4,relerr)
	endif
	!print *, 'DEVEL: toallevents=',toallevents
	istartbb=1
	!print *,'starting the generation'
c sampling the energies: first e-/e+
c1	e1=e0*rnd1(d)
1	if(modebb.ne.10) e1=ebb2*rnd1(d)
	if(modebb.eq.10) e1=ebb1+(ebb2-ebb1)*rnd1(d)
	k=nint(e1*1000.)
	if(k.lt.1) k=1
	if(spmax*rnd1(d).gt.spthe1(k)) go to 1
c second e-/e+ or X-ray
	if    (modebb.eq.1.or.modebb.eq.2.or.modebb.eq.3.or.
     +       modebb.eq.7.or.modebb.eq.17.or.modebb.eq.18) then
c modes with no emission of other particles beside of two e-/e+:
c energy of second e-/e+ is calculated
	   e2=e0-e1
	elseif(modebb.eq. 4.or.modebb.eq. 5.or.modebb.eq. 6.or.
     +         modebb.eq. 8.or.modebb.eq.13.or.modebb.eq.14.or.
     +         modebb.eq.15.or.modebb.eq.16) then
c something else is emitted - energy of second e-/e+ is random
	re2s=amax1(0.,ebb1-e1)
	re2f=ebb2-e1

cc	   if(modebb.eq.4)
cc     +        call tgold(0.,e0-e1,fe2_mod4,1.e-3,2,e2max,f2max)
c	   if(modebb.eq.4)
c     +        call tgold(re2s,re2f,fe2_mod4,1.e-3,2,e2max,f2max)
c	   if(modebb.eq.5)
c     +        call tgold(re2s,re2f,fe2_mod5,1.e-3,2,e2max,f2max)
c	   if(modebb.eq.6)
c     +        call tgold(re2s,re2f,fe2_mod6,1.e-3,2,e2max,f2max)
c	   if(modebb.eq.13)
c     +        call tgold(re2s,re2f,fe2_mod13,1.e-3,2,e2max,f2max)
c	   if(modebb.eq.14)
c     +        call tgold(re2s,re2f,fe2_mod14,1.e-3,2,e2max,f2max)
c	   if(modebb.eq.8)
c     +        call tgold(re2s,re2f,fe2_mod8,1.e-3,2,e2max,f2max)
c	   if(modebb.eq.15)
c     +        call tgold(re2s,re2f,fe2_mod15,1.e-3,2,e2max,f2max)
c	   if(modebb.eq.16)
c     +        call tgold(re2s,re2f,fe2_mod16,1.e-3,2,e2max,f2max)
cc	print *,'e1,re2s,re2f=',e1,re2s,re2f
cc	print *,'tgold: e2max,f2max=',e2max,f2max

	f2max=-1.
	ke2s=max0(1,int(re2s*1000.))
	ke2f=int(re2f*1000.)
	do ke2=ke2s,ke2f
	   e2=ke2/1000.
	   if(modebb.eq. 4) spthe2(ke2)=fe2_mod4(e2)
	   if(modebb.eq. 5) spthe2(ke2)=fe2_mod5(e2)
	   if(modebb.eq. 6) spthe2(ke2)=fe2_mod6(e2)
	   if(modebb.eq. 8) spthe2(ke2)=fe2_mod8(e2)
	   if(modebb.eq.13) spthe2(ke2)=fe2_mod13(e2)
	   if(modebb.eq.14) spthe2(ke2)=fe2_mod14(e2)
	   if(modebb.eq.15) spthe2(ke2)=fe2_mod15(e2)
	   if(modebb.eq.16) spthe2(ke2)=fe2_mod16(e2)
	   if(spthe2(ke2).gt.f2max) f2max=spthe2(ke2)
	enddo
c	print *,'real f2max=',f2max
c	pause

c2	   e2=(e0-e1)*rnd1(d)
2	   e2=re2s+(re2f-re2s)*rnd1(d)
	   if(modebb.eq.4) fe2=fe2_mod4(e2)
	   if(modebb.eq.5) fe2=fe2_mod5(e2)
	   if(modebb.eq.6) fe2=fe2_mod6(e2)
	   if(modebb.eq.13) fe2=fe2_mod13(e2)
	   if(modebb.eq.14) fe2=fe2_mod14(e2)
	   if(modebb.eq.8) fe2=fe2_mod8(e2)
	   if(modebb.eq.15) fe2=fe2_mod15(e2)
	   if(modebb.eq.16) fe2=fe2_mod16(e2)
	   if(f2max*rnd1(d).gt.fe2) go to 2
c	   allevents=allevents+1.
c	   if(e1+e2.lt.ebb1.or.e1+e2.gt.ebb2) go to 1
cc	   if(rnd1(d).lt.0.5) then
cc	      eh=e1
cc	      e1=e2
cc	      e2=eh
cc	   endif
	elseif(modebb.eq.10) then
c energy of X-ray is fixed; no angular correlation
c	   allevents=allevents+1.
c	   if(e1.lt.ebb1.or.e1.gt.ebb2) go to 1
	   call particle(2,e1,e1,0.,pi,0.,twopi,0.,0.,t)
	   call particle(1,EK,EK,0.,pi,0.,twopi,0.,0.,t)
	   return
	endif
	p1=sqrt(e1*(e1+2.*emass))
	p2=sqrt(e2*(e2+2.*emass))
	b1=p1/(e1+emass)
	b2=p2/(e2+emass)
c sampling the angles with angular correlation
	a=1.
	b=-b1*b2
	c=0.
	if(modebb.eq.2) b=b1*b2
	if(modebb.eq.3) then
	   w1=e1+emass
	   w2=e2+emass
	   a=3.*(w1*w2+emass**2)*(p1**2+p2**2)
	   b=-p1*p2*((w1+w2)**2+4.*(w1*w2+emass**2))
	   c=2.*p1**2*p2**2
	endif
	if(modebb.eq.7) then
	   w1=e1+emass
	   w2=e2+emass
	   a=5.*(w1*w2+emass**2)*(p1**2+p2**2)-p1**2*p2**2
	   b=-p1*p2*(10.*(w1*w2+emass**2)+p1**2+p2**2)
	   c=3.*p1**2*p2**2
	endif
	if(modebb.eq.8) b=b1*b2/3.
	if(modebb.eq.15) then
	   a=9.*(e0-e1-e2)**2+21.*(e2-e1)**2
	   b=-b1*b2*(9.*(e0-e1-e2)**2-7.*(e2-e1)**2)
	endif
	if(modebb.eq.16) b=b1*b2/3.
	if(modebb.eq.17) b=b1*b2
	if(modebb.eq.18) then
	   et0=e0/emass+1. ! total energies in the units of electron mass
	   et1=e1/emass+1.
	   et2=e2/emass+1.
	   a1=(et1*et2-1.)*(et1-et2)**2/(2.*et1*et2)
	   a2=-2.*(et1-et2)**2/(9.*et1*et2)
	   a3=2.*(et1*et2-1.)/(81.*et1*et2)
	   r=3.107526e-3*Adbb**(1./3.)
	   rksi=3./137.036*Zdbb+r*et0
	   a4=8.*(et1*et2+1.)/(r**2*et1*et2)
	   a5=-8.*(rksi*(et1*et2+1.)-2.*r*et0)/(3.*r**2*et1*et2)
	   a6=2.*((rksi**2+4.*r**2)*(et1*et2+1.)-4.*rksi*r*et0)/
     +      (9.*r**2*et1*et2)
	   chi_1plus =chip_GT+3.*chip_F-6.*chip_T
	   chi_1minus=chip_GT-3.*chip_F-6.*chip_T
	   chi_2plus =chi_GTw+chi_Fw-chi_1minus/9.
	   a_eta=a1*chi_2plus**2+a2*chi_2plus*chi_1minus+a3*chi_1minus**2
     +        +a4*chip_R**2+a5*chip_R*chip_P+a6*chip_P**2
	   b_eta=(et1-et2)**2*chi_2plus**2/2.-4.*chi_1minus**2/81.
     +        +8.*(rksi*chip_P/6.-chip_R)**2/r**2-8.*chip_P**2/9.
	   if(a_eta.ne.0.) then
	      b=b_eta/a_eta*b1*b2
	   else
	      print *,'a_eta=0'
	   endif
	endif
	romaxt=a+abs(b)+c
3	phi1=twopi*rnd1(d)
	ctet1=1.-2.*rnd1(d)
	stet1=sqrt(1.-ctet1*ctet1)
	phi2=twopi*rnd1(d)
	ctet2=1.-2.*rnd1(d)
	stet2=sqrt(1.-ctet2*ctet2)
	ctet=ctet1*ctet2+stet1*stet2*cos(phi1-phi2)
	if(romaxt*rnd1(d).gt.a+b*ctet+c*ctet**2) go to 3
	npfull=npfull+1
	if(Zdbb.ge.0.) npgeant(npfull)=3
	if(Zdbb.lt.0.) npgeant(npfull)=2
	pmoment(1,npfull)=p1*stet1*cos(phi1)
	pmoment(2,npfull)=p1*stet1*sin(phi1)
	pmoment(3,npfull)=p1*ctet1
	ptime(npfull)=0.
	npfull=npfull+1
	if(Zdbb.ge.0.) npgeant(npfull)=3
	if(Zdbb.lt.0.) npgeant(npfull)=2
	pmoment(1,npfull)=p2*stet2*cos(phi2)
	pmoment(2,npfull)=p2*stet2*sin(phi2)
	pmoment(3,npfull)=p2*ctet2
	ptime(npfull)=0.
	return
	end
c
c end of bb.f
c
c Local Variables: --
c mode: fortran --
c End: --
