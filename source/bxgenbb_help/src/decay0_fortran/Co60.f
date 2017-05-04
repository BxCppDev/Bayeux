c Co60.f
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
	subroutine Co60(tcnuc,tdnuc)
c Scheme of Co60 decay ("Table of Isotopes", 7th ed., 1978).
c Four-figured labels correspond to energies of 60Ni excited
c levels in keV.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 3.08.1992, 22.10.1995.
c Updated to NDS 100(2003)347, VIT, 16.10.2006;
c angular correlation of 1173 and 1333 keV gammas, L.Pandola + VIT, 18.10.2006;
c 2nd forbidden unique shape for beta decay to 1333 keV level, VIT, 27.10.2006.
        common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),
     +                  ptime(100)
	thnuc=0.166344192e+09
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	npg1173=0
	npg1333=0
	pbeta=100.*rnd1(d)
        !call genbb_trace_tag(10002,'pbeta'//CHAR(0),pbeta)
	if(pbeta.le.99.880) go to 2506   ! 99.880%
	                    go to 1333   !  0.120%
c-----------------------------------------------------------------------
2506	call beta(0.318,28.,0.,0.,t)
	thlev=0.3e-12
	p=100.*rnd1(d)
        !call genbb_trace_tag(10002,'p1'//CHAR(0),p)
	if(p.le. 0.000002) go to 25061   !  0.000002%
	if(p.le.99.992449) go to 25062   ! 99.992447%
	                   go to 25063   !  0.007551%
25061	call nucltransK(2.506,0.008,8.6e-5,0.,tclev,thlev,tdlev)
	return
c25062	call nucltransK(1.173,0.008,1.7e-4,6.2e-6,tclev,thlev,tdlev)
c longer code for angular correlation of gammas 1173 and 1333 keV
25062	Egamma=1.173
	EbindK=0.008
	cg=1.
	cK=1.7e-4
	cp=6.2e-6
	p=rnd1(d)*(cg+cK+cp)
        !call genbb_trace_tag(10002,'p2'//CHAR(0),p)
	if(p.le.cg) then
	   call gamma(Egamma,tclev,thlev,tdlev)
	   npg1173=npfull
	else if(p.le.cg+cK) then
	   call electron(Egamma-EbindK,tclev,thlev,tdlev)
	   call gamma(EbindK,0.,0.,tdlev)
	else
	   call pair(Egamma-1.022,tclev,thlev,tdlev)
	end if
	go to 13330
25063	call nucltransK(0.347,0.008,5.5e-3,0.,tclev,thlev,tdlev)
	go to 2159
c-----------------------------------------------------------------------
2159	thlev=0.
	p=100.*rnd1(d)
        !call genbb_trace_tag(10002,'p3'//CHAR(0),p)
	if(p.le.13.64) go to 21591      ! 13.64%
	               go to 21592      ! 86.36%
21591	call nucltransK(2.159,0.008,4.9e-5,3.9e-4,tclev,thlev,tdlev)
	return
21592	call nucltransK(0.826,0.008,3.3e-4,0.,tclev,thlev,tdlev)
	go to 13330
c-----------------------------------------------------------------------
c1333	call beta(1.491,28.,0.,0.,t)
c change to the 2nd forbidden unique shape in accordance with
c H.Daniel, RMP 40(1968)659 and J.L.Wolfson, Can. J. Phys. 34(1956)256
c cf(e) = pel**4 + c1*pel**2*pnu**2 + c2*pnu**4,
1333	call beta2(1.491,28.,0.,0.,t,2,3.333333,1.,0.,0.)
13330	thlev=0.9e-12
c	call nucltransK(1.333,0.008,1.3e-4,3.4e-5,tclev,thlev,tdlev)
c longer code for angular correlation of gammas 1173 and 1333 keV
	Egamma=1.333
	EbindK=0.008
	cg=1.
	cK=1.3e-4
	cp=3.4e-5
	p=rnd1(d)*(cg+cK+cp)
        !call genbb_trace_tag(10002,'p4'//CHAR(0),p)
	if(p.le.cg) then
	   call gamma(Egamma,tclev,thlev,tdlev)
	   npg1333=npfull
	else if(p.le.cg+cK) then
	   call electron(Egamma-EbindK,tclev,thlev,tdlev)
	   call gamma(EbindK,0.,0.,tdlev)
	else
	   call pair(Egamma-1.022,tclev,thlev,tdlev)
	end if
c-----------------------------------------------------------------------
c Angular correlation between gammas 1173 and 1333 keV, L.Pandola + VIT
	if(npg1333.ne.0.and.npg1173.ne.0) then
	   p1333=sqrt(pmoment(1,npg1333)**2+pmoment(2,npg1333)**2+
     +                pmoment(3,npg1333)**2)
	   p1173=sqrt(pmoment(1,npg1173)**2+pmoment(2,npg1173)**2+
     +                pmoment(3,npg1173)**2)
c           call genbb_trace_msg(10002,
c     +   	'gammas 1173 and 1333 correlation'//CHAR(0))
c Coefficients in formula 1+a2*ctet**2+a4*ctet**4 are from:
c R.D.Evans, "The Atomic Nucleus", Krieger Publ. Comp., 1985, p. 240 (4(2)2(2)0 cascade).
c They correspond to coefficients in formula 1+a2*p2+a4*p4, a2=0.1020, a4=0.0091 in
c K.Siegbahn, "Alpha-, Beta- and Gamma-Ray Spectroscopy", North-Holland Publ. Comp., 1968, p. 1033.
	   a2=1./8.
	   a4=1./24.
	   twopi=6.2831853
1	   phi1=twopi*rnd1(d)
	   ctet1=1.-2.*rnd1(d)
	   stet1=sqrt(1.-ctet1*ctet1)
	   phi2=twopi*rnd1(d)
	   ctet2=1.-2.*rnd1(d)
	   stet2=sqrt(1.-ctet2*ctet2)
	   ctet=ctet1*ctet2+stet1*stet2*cos(phi1-phi2)
	   pcor=rnd1(d)
           !call genbb_trace_tag(10002,'pcor'//CHAR(0),pcor)
	   if(pcor*(1.+abs(a2)+abs(a4)).gt.1.+a2*ctet**2+a4*ctet**4)
     +        go to 1
	   pmoment(1,npg1333)=p1333*stet1*cos(phi1)
	   pmoment(2,npg1333)=p1333*stet1*sin(phi1)
	   pmoment(3,npg1333)=p1333*ctet1
	   pmoment(1,npg1173)=p1173*stet2*cos(phi2)
	   pmoment(2,npg1173)=p1173*stet2*sin(phi2)
	   pmoment(3,npg1173)=p1173*ctet2
	endif
c-----------------------------------------------------------------------
	return
	end
c
c end of Co60.f
c
c Local Variables: --
c mode: fortran --
c End: --
