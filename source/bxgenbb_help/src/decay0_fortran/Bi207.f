c Bi207.f 
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
	subroutine Bi207(tcnuc,tdnuc)
c Scheme of Bi207+Pb207m decay (Nucl. Data Sheets 70(1993)315)
c with NNDC corrections of 10.10.2000 and 20.04.2005.
c To describe the deexcitation processes in atomic shell of Pb207,
c the information of PC Nuclear Data File retrieval program and data
c base (last updated on 16-Aug-1994) was used.
c Gammas, beta+, e+e- pairs, K, L and M conversion electrons, K, L and M
c X-rays and K and L Auger electrons are emitted.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 7.07.1995, 22.10.1995, 11.09.2005, 3.10.2005.
	common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),
     +                  ptime(100)
	thnuc=1.0382166E+09
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	EbindK=0.088
	EbindL=0.015
	EbindM=0.003
	cg=1.
	npe570=0
	npe1064=0
	npg570=0
	npg1064=0
	pdecay=100.*rnd1(d)
	if(pdecay.le.99.988) go to 1  ! 99.988 EC
	                     go to 2  !  0.012 b+
c-----------------------------------------------------------------------
c EC to Pb207 or Pb207m
1	pec=pdecay
	if(pec.le. 7.027) go to 2340  !  7.027%
	if(pec.le.90.992) go to 1633  ! 83.965%
	                  go to  570  !  8.996%
c-----------------------------------------------------------------------
c b+ to Pb207
2	call beta(0.807,-82.,0.,0.,t)
	go to 57000
c-----------------------------------------------------------------------
2340	if(rnd1(d).le.0.70) then
	   call PbAtShell(88,0.,0.,t)
	else
	   call PbAtShell(15,0.,0.,t)
	end if
23400	thlev=0.
	p=100.*rnd1(d)
	if(p.le.98.13) go to 23401
	               go to 23402
23401	Egamma=1.770
	cK=3.51e-3
	cL=0.49e-3
	cM=0.13e-3
	cp=2.5e-4
	p=rnd1(d)*(cg+cK+cL+cM+cp)
	if(p.le.cg) then
	   call gamma(Egamma,tclev,thlev,tdlev)
	else if(p.le.cg+cK) then
	   call electron(Egamma-EbindK,tclev,thlev,tdlev)
	   call PbAtShell(88,0.,0.,tdlev)
	else if(p.le.cg+cK+cL) then
	   call electron(Egamma-EbindL,tclev,thlev,tdlev)
	   call PbAtShell(15,0.,0.,tdlev)
	else if(p.le.cg+cK+cL+cM) then
	   call electron(Egamma-EbindM,tclev,thlev,tdlev)
	   call PbAtShell(3,0.,0.,tdlev)
	else
	   call pair(Egamma-1.022,tclev,thlev,tdlev)
	end if
	go to 57000
23402	Egamma=1.442
	cK=2.7e-3
	cL=0.4e-3
	cM=0.1e-3
	cp=0.2e-4
	p=rnd1(d)*(cg+cK+cL+cM+cp)
	if(p.le.cg) then
	   call gamma(Egamma,tclev,thlev,tdlev)
	else if(p.le.cg+cK) then
	   call electron(Egamma-EbindK,tclev,thlev,tdlev)
	   call PbAtShell(88,0.,0.,tdlev)
	else if(p.le.cg+cK+cL) then
	   call electron(Egamma-EbindL,tclev,thlev,tdlev)
	   call PbAtShell(15,0.,0.,tdlev)
	else if(p.le.cg+cK+cL+cM) then
	   call electron(Egamma-EbindM,tclev,thlev,tdlev)
	   call PbAtShell(3,0.,0.,tdlev)
	else
	   call pair(Egamma-1.022,tclev,thlev,tdlev)
	end if
	go to 89800
c-----------------------------------------------------------------------
1633	p=rnd1(d)
	if(p.le.0.733) then              ! 73.3% EC from K shell
	   call PbAtShell(88,0.,0.,t)
	elseif(p.le.0.931) then          ! 19.8% EC from L shell
	   call PbAtShell(15,0.,0.,t)    
	else                             !  6.9% EC from M shell
	   call PbAtShell(3,0.,0.,t)     
	end if
16330	thlev=0.80
	Egamma=1.064
	cK=9.42e-2
	cL=2.47e-2
	cM=0.73e-2
	p=rnd1(d)*(cg+cK+cL+cM)
	if(p.le.cg) then
	   call gamma(Egamma,tclev,thlev,tdlev)
	   npg1064=npfull
	else if(p.le.cg+cK) then
	   call electron(Egamma-EbindK,tclev,thlev,tdlev)
	   npe1064=npfull
	   call PbAtShell(88,0.,0.,tdlev)
	else if(p.le.cg+cK+cL) then
	   call electron(Egamma-EbindL,tclev,thlev,tdlev)
	   npe1064=npfull
	   call PbAtShell(15,0.,0.,tdlev)
	else
	   call electron(Egamma-EbindM,tclev,thlev,tdlev)
	   npe1064=npfull
	   call PbAtShell(3,0.,0.,tdlev)
	end if
	go to 57000
c-----------------------------------------------------------------------
89800	thlev=0.
	p=100.*rnd1(d)
	if(p.le.99.245) go to 89801
	                go to 89802
89801	Egamma=0.898
	cK=2.01e-2
	cL=0.34e-2
	p=rnd1(d)*(cg+cK+cL)
	if(p.le.cg) then
	   call gamma(Egamma,tclev,thlev,tdlev)
	elseif(p.le.cg+cK) then
	   call electron(Egamma-EbindK,tclev,thlev,tdlev)
	   call PbAtShell(88,0.,0.,tdlev)
	else
	   call electron(Egamma-EbindL,tclev,thlev,tdlev)
	   call PbAtShell(15,0.,0.,tdlev)
	end if
	return
89802	Egamma=0.328
	cK=0.2850
	cL=0.0486
	cM=0.0151
	p=rnd1(d)*(cg+cK+cL+cM)
	if(p.le.cg) then
	   call gamma(Egamma,tclev,thlev,tdlev)
	else if(p.le.cg+cK) then
	   call electron(Egamma-EbindK,tclev,thlev,tdlev)
	   call PbAtShell(88,0.,0.,tdlev)
	else if(p.le.cg+cK+cL) then
	   call electron(Egamma-EbindL,tclev,thlev,tdlev)
	   call PbAtShell(15,0.,0.,tdlev)
	else
	   call electron(Egamma-EbindM,tclev,thlev,tdlev)
	   call PbAtShell(3,0.,0.,tdlev)
	end if
	go to 57000
c-----------------------------------------------------------------------
570	p=rnd1(d)
	if(p.le.0.7965) then
	   call PbAtShell(88,0.,0.,t)
	elseif(p.le.0.9466) then
	   call PbAtShell(15,0.,0.,t)
	else
	   call PbAtShell(3,0.,0.,t)
	end if
57000	thlev=130.5e-12
	Egamma=0.570
	cK=1.55e-2
	cL=0.45e-2
	cM=0.15e-2
	p=rnd1(d)*(cg+cK+cL+cM)
	if(p.le.cg) then
	   call gamma(Egamma,tclev,thlev,tdlev)
	   npg570=npfull
	else if(p.le.cg+cK) then
	   call electron(Egamma-EbindK,tclev,thlev,tdlev)
	   npe570=npfull
	   call PbAtShell(88,0.,0.,tdlev)
	else if(p.le.cg+cK+cL) then
	   call electron(Egamma-EbindL,tclev,thlev,tdlev)
	   npe570=npfull
	   call PbAtShell(15,0.,0.,tdlev)
	else
	   call electron(Egamma-EbindM,tclev,thlev,tdlev)
	   npe570=npfull
	   call PbAtShell(3,0.,0.,tdlev)
	end if
c-----------------------------------------------------------------------
c Angular correlation between gammas and conversion electrons of 1064 and 
c 570 keV; values of angular coefficients are taken from P.Kleinheinz
c et al., Nucl. Phys. A 93(1967)63. For K, L and M electrons the same 
c coefficients are used.
c Thanks to V.Vasilyev for correcting formula in previous DECAY0 version
c for case of two conversion electrons emitted.
	if    (npg1064.ne.0 .and. npg570.ne.0) then
	   a2=0.231
	   a4=-0.023
	   np1064=npg1064
	   np570=npg570
	elseif(npe1064.ne.0 .and. npg570.ne.0) then
	   a2=0.223
	   a4=-0.020
	   np1064=npe1064
	   np570=npg570
	elseif(npg1064.ne.0 .and. npe570.ne.0) then
	   a2=0.275
	   a4=-0.012
	   np1064=npg1064
	   np570=npe570
	elseif(npe1064.ne.0 .and. npe570.ne.0) then
	   a2=0.271
	   a4=-0.010
	   np1064=npe1064
	   np570=npe570
	else
	   return
	endif
	p1064=sqrt(pmoment(1,np1064)**2+pmoment(2,np1064)**2+
     +             pmoment(3,np1064)**2)
	p570=sqrt(pmoment(1,np570)**2+pmoment(2,np570)**2+
     +            pmoment(3,np570)**2)
	twopi=6.2831853
3	phi1=twopi*rnd1(d)
	ctet1=1.-2.*rnd1(d)
	stet1=sqrt(1.-ctet1*ctet1)
	phi2=twopi*rnd1(d)
	ctet2=1.-2.*rnd1(d)
	stet2=sqrt(1.-ctet2*ctet2)
	ctet=ctet1*ctet2+stet1*stet2*cos(phi1-phi2)
	p2=(3.*ctet**2-1.)/2.
	p4=(35.*ctet**4-30.*ctet**2+3.)/8.
	if(rnd1(d)*(1.+abs(a2)+abs(a4)).gt.1.+a2*p2+a4*p4) go to 3
	pmoment(1,np1064)=p1064*stet1*cos(phi1)
	pmoment(2,np1064)=p1064*stet1*sin(phi1)
	pmoment(3,np1064)=p1064*ctet1
	pmoment(1,np570)=p570*stet2*cos(phi2)
	pmoment(2,np570)=p570*stet2*sin(phi2)
	pmoment(3,np570)=p570*ctet2
	return
c-----------------------------------------------------------------------
	end
c
c end of Bi207.f
c
c Local Variables: --
c mode: fortran --
c End: --
