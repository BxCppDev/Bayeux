c Se76low.f 
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
	subroutine Se76low(levelkeV)
c Subroutine describes the deexcitation process in Se76 nucleus
c after 2b-decay of Ge76 to ground and excited 0+ and 2+ levels
c of Se76 ("Table of Isotopes", 7th ed., 1978).
c Call  : call Se76low(levelkeV)
c Input : levelkeV - energy of Se76 level (integer in keV) occupied
c                    initially; following levels can be occupied:
c                    0+(gs) -    0 keV,
c                    2+(1)  -  559 keV,
c                    0+(1)  - 1122 keV,
c                    2+(2)  - 1216 keV.
c Output: common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 19.07.1993, 22.10.1995.
c Luciano Pandola, 25.10.2006: angular correlation of 563 and 559 keV gamma
c quanta if 1122 keV level is populated.
	common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),
     +                  ptime(100)
	npg563=0
	npg559=0
	tclev=0.
	if(levelkev.eq.1216) go to  1216
	if(levelkev.eq.1122) go to  1122
	if(levelkev.eq. 559) go to   559
	if(levelkev.eq.   0) go to 10000
	                     go to 20000
c-----------------------------------------------------------------------
1216	thlev=3.4e-12
	p=100.*rnd1(d)
	if(p.le.36.) go to 12161
	             go to 12162
12161	call nucltransK(1.216,0.013,4.3e-4,0.1e-4,tclev,thlev,tdlev)
	return
12162	call nucltransK(0.657,0.013,2.1e-3,0.,tclev,thlev,tdlev)
	go to 559
c-----------------------------------------------------------------------
1122	thlev=11.e-12
c	call nucltransK(0.563,0.013,2.0e-3,0.,tclev,thlev,tdlev)
	Egamma=0.563
	EbindK=0.013
	cg=1.
	cK=2.0e-3
	p=rnd1(d)*(cg+cK)
	if(p.le.cg) then
	   call gamma(Egamma,tclev,thlev,tdlev)
	   npg563=npfull
	else 
	   call electron(Egamma-EbindK,tclev,thlev,tdlev)
	   call gamma(EbindK,0.,0.,tdlev)
	end if
	go to 559
c-----------------------------------------------------------------------
559	thlev=12.3e-12
c	call nucltransK(0.559,0.013,2.0e-3,0.,tclev,thlev,tdlev)
	Egamma=0.559
	EbindK=0.013
	cg=1.
	cK=2.0e-3
	p=rnd1(d)*(cg+cK)
	if(p.le.cg) then
	   call gamma(Egamma,tclev,thlev,tdlev)
	   npg559=npfull
	else 
	   call electron(Egamma-EbindK,tclev,thlev,tdlev)
	   call gamma(EbindK,0.,0.,tdlev)
	end if
c Angular correlation between gammas 559 and 563 keV, L.Pandola + VIT
	if(npg559.ne.0.and.npg563.ne.0) then 
	   p559=sqrt(pmoment(1,npg559)**2+pmoment(2,npg559)**2+
     +               pmoment(3,npg559)**2)
	   p563=sqrt(pmoment(1,npg563)**2+pmoment(2,npg563)**2+
     +               pmoment(3,npg563)**2)
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
	   pmoment(1,npg559)=p559*stet1*cos(phi1)
	   pmoment(2,npg559)=p559*stet1*sin(phi1)
	   pmoment(3,npg559)=p559*ctet1
	   pmoment(1,npg563)=p563*stet2*cos(phi2)
	   pmoment(2,npg563)=p563*stet2*sin(phi2)
	   pmoment(3,npg563)=p563*ctet2
	endif
	return
c-----------------------------------------------------------------------
10000	return
c-----------------------------------------------------------------------
20000	print *,'Se76: wrong level [keV] ',levelkev
c-----------------------------------------------------------------------
	return
	end
c
c end of Se76low.f
c
c Local Variables: --
c mode: fortran --
c End: --
