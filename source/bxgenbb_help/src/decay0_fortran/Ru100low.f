c Ru100low.f 
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
	subroutine Ru100low(levelkeV)
c Subroutine describes the deexcitation process in Ru100 nucleus
c after 2b-decay of Mo100 to ground and excited 0+ and 2+ levels
c of Ru100 ("Table of Isotopes", 7th ed., 1978).
c Call  : call Ru100low(levelkeV)
c Input : levelkeV - energy of Ru100 level (integer in keV) occupied
c                    initially; following levels can be occupied:
c                    0+(gs) -    0 keV,
c                    2+(1)  -  540 keV,
c                    0+(1)  - 1130 keV,
c                    2+(2)  - 1362 keV,
c                    0+(2)  - 1741 keV.
c Output: common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 28.06.1993, 22.10.1995.
c VIT, 19.05.2009: angular correlation of 591 and 540 keV gamma quanta
c if 1130 keV level is populated.
	common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),
     +                  ptime(100)
	npg591=0
	npg540=0
	tclev=0.
	if(levelkev.eq.1741) go to  1741
	if(levelkev.eq.1362) go to  1362
	if(levelkev.eq.1130) go to  1130
	if(levelkev.eq. 540) go to   540
	if(levelkev.eq.   0) go to 10000
	                     go to 20000
c-----------------------------------------------------------------------
1741	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.05) go to 17411
	if(p.le.59.00) go to 17412
	if(p.le.59.03) go to 17413
	               go to 17414
17411	call electron(1.741-0.022,tclev,thlev,tdlev)    ! only ec
	call gamma(0.022,0.,0.,tdlev)
	return
17412	call nucltransK(1.201,0.022,6.2e-4,0.1e-4,tclev,thlev,tdlev)
	go to 540
17413	call electron(0.611-0.022,tclev,thlev,tdlev)    ! only ec
	call gamma(0.022,0.,0.,tdlev)
	go to 1130
17414	call nucltransK(0.379,0.022,1.3e-2,0.,tclev,thlev,tdlev)
	go to 1362
c-----------------------------------------------------------------------
1362	thlev=1.2e-12
	p=100.*rnd1(d)
	if(p.le.43.) go to 13621
	             go to 13622
13621	call nucltransK(1.362,0.022,4.2e-4,0.2e-4,tclev,thlev,tdlev)
	return
13622	call nucltransK(0.822,0.022,1.7e-3,0.,tclev,thlev,tdlev)
	go to 540
c-----------------------------------------------------------------------
1130	thlev=0.
	p=100.*rnd1(d)
	if(p.le.0.02) go to 11301
	              go to 11302
11301	call electron(1.130-0.022,tclev,thlev,tdlev) ! only ec
	call gamma(0.022,0.,0.,tdlev)
	return
c11302	call nucltransK(0.591,0.022,3.3e-3,0.,tclev,thlev,tdlev)
11302	Egamma=0.591
	EbindK=0.022
	cg=1.
	cK=3.3e-3
	p=rnd1(d)*(cg+cK)
	if(p.le.cg) then
	   call gamma(Egamma,tclev,thlev,tdlev)
	   npg591=npfull
	else 
	   call electron(Egamma-EbindK,tclev,thlev,tdlev)
	   call gamma(EbindK,0.,0.,tdlev)
	end if
	go to 540
c-----------------------------------------------------------------------
540	thlev=11.e-12
c	call nucltransK(0.540,0.022,4.4e-3,0.,tclev,thlev,tdlev)
	Egamma=0.540
	EbindK=0.022
	cg=1.
	cK=4.4e-3
	p=rnd1(d)*(cg+cK)
	if(p.le.cg) then
	   call gamma(Egamma,tclev,thlev,tdlev)
	   npg540=npfull
	else 
	   call electron(Egamma-EbindK,tclev,thlev,tdlev)
	   call gamma(EbindK,0.,0.,tdlev)
	end if
c Angular correlation between gammas 591 and 540 keV
	if(npg591.ne.0.and.npg540.ne.0) then 
	   p591=sqrt(pmoment(1,npg591)**2+pmoment(2,npg591)**2+
     +               pmoment(3,npg591)**2)
	   p540=sqrt(pmoment(1,npg540)**2+pmoment(2,npg540)**2+
     +               pmoment(3,npg540)**2)
c Coefficients in formula 1+a2*ctet**2+a4*ctet**4 are from:
c R.D.Evans, "The Atomic Nucleus", Krieger Publ. Comp., 1985, p. 240, 
c 0(2)2(2)0 cascade.
	   a2=-3.0
	   a4=4.0
	   twopi=6.2831853
1	   phi1=twopi*rnd1(d)
	   ctet1=1.-2.*rnd1(d)
	   stet1=sqrt(1.-ctet1*ctet1)
	   phi2=twopi*rnd1(d)
	   ctet2=1.-2.*rnd1(d)
	   stet2=sqrt(1.-ctet2*ctet2)
	   ctet=ctet1*ctet2+stet1*stet2*cos(phi1-phi2)
	   if(rnd1(d)*(1.+abs(a2)+abs(a4)).gt.1.+a2*ctet**2+a4*ctet**4) 
     +        go to 1 
	   pmoment(1,npg591)=p591*stet1*cos(phi1)
	   pmoment(2,npg591)=p591*stet1*sin(phi1)
	   pmoment(3,npg591)=p591*ctet1
	   pmoment(1,npg540)=p540*stet2*cos(phi2)
	   pmoment(2,npg540)=p540*stet2*sin(phi2)
	   pmoment(3,npg540)=p540*ctet2
	endif
	return
c-----------------------------------------------------------------------
10000	return
c-----------------------------------------------------------------------
20000	print *,'Ru100: wrong level [keV] ',levelkev
c-----------------------------------------------------------------------
	return
	end
c
c end of Ru100low.f
c
c Local Variables: --
c mode: fortran --
c End: --
