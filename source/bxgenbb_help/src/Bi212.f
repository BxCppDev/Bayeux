c Bi212.f 
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
	subroutine Bi212(tcnuc,tdnuc)
c Scheme of Bi212 decay ("Table of Isotopes", 7th ed., 1978).
c Two-, three- and four-figured labels correspond to energies of
c 208Tl or 212Po excited levels in keV. Beta-alfa decays to 208Pb
c are not considered (p=0.014%).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 4.08.1992, 22.10.1995.
	thnuc=3636.
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pdecay=rnd1(d)
	if(pdecay.le.0.36) go to 10000   !  36% alfa to 208Tl
	                   go to 20000   !  64% beta to 212Po
c-----------------------------------------------------------------------
10000	palfa=100.*rnd1(d)
	if(palfa.le. 1.10) go to   493   !   1.10%
	if(palfa.le. 1.25) go to   473   !   0.15%
	if(palfa.le. 2.92) go to   328   !   1.67%
	if(palfa.le.72.80) go to    40   !  69.88%
	                   go to 10001   !  27.20%
c-----------------------------------------------------------------------
493	call alpha(5.607,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 5.) go to 49301   !  5%
	if(p.le.99.) go to 49302   ! 94%
	             go to 49303   !  1%
49301	call nucltransK(0.493,0.086,2.8e-2,0.,tclev,thlev,tdlev)
	return
49302	call nucltransK(0.453,0.086,0.18,0.,tclev,thlev,tdlev)
	go to 40000
49303	call nucltransK(0.164,0.086,0.75,0.,tclev,thlev,tdlev)
	go to 32800
c-----------------------------------------------------------------------
473	call alpha(5.626,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le.68.) go to 47301   ! 68%
	if(p.le.87.) go to 47302   ! 19%
	             go to 47303   ! 13%
47301	call nucltransK(0.474,0.086,0.14,0.,tclev,thlev,tdlev)
	return
47302	call nucltransK(0.434,0.086,0.14,0.,tclev,thlev,tdlev)
	go to 40000
47303	call nucltransK(0.145,0.086,2.8,0.,tclev,thlev,tdlev)
	go to 32800
c-----------------------------------------------------------------------
328	call alpha(5.769,0.,0.,t)
32800	thlev=0.
	p=100.*rnd1(d)
	if(p.le.29.) go to 32801   ! 29%
	             go to 32802   ! 71%
32801	call nucltransK(0.328,0.086,0.33,0.,tclev,thlev,tdlev)
	return
32802	call nucltransK(0.288,0.086,0.53,0.,tclev,thlev,tdlev)
	go to 40000
c-----------------------------------------------------------------------
40	call alpha(6.051,0.,0.,t)
40000	thlev=6.e-12
	call nucltransK(0.040,0.015,22.55,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10001	call alpha(6.090,0.,0.,t)
	return
c-----------------------------------------------------------------------
20000	pbeta=64.*rnd1(d)
	if(pbeta.le. 0.660) go to  1806  !  0.660%
	if(pbeta.le. 0.687) go to  1801  !  0.027%
	if(pbeta.le. 0.937) go to  1680  !  0.250%
	if(pbeta.le. 2.837) go to  1621  !  1.900%
	if(pbeta.le. 4.337) go to  1513  !  1.500%
	if(pbeta.le. 8.737) go to   727  !  4.400%
	                    go to 20001  ! 55.263%
c-----------------------------------------------------------------------
1806	call beta(0.440,84.,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le.17.) go to 18061   ! 17%
	             go to 18062   ! 83%
18061	call nucltransK(1.806,0.093,2.6e-2,1.7e-4,tclev,thlev,tdlev)
	return
18062	call nucltransK(1.079,0.093,2.0e-2,0.,tclev,thlev,tdlev)
	go to 72700
c-----------------------------------------------------------------------
1801	call beta(0.445,84.,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le.35.) go to 18011   ! 35%
	             go to 18012   ! 65%
18011	call electron(1.708,tclev,thlev,tdlev)       ! only ec
	call gamma(0.093,0.,0.,tdlev)
	return
18012	call nucltransK(1.074,0.093,7.0e-3,0.,tclev,thlev,tdlev)
	go to 72700
c-----------------------------------------------------------------------
1680	call beta(0.566,84.,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le.28.) go to 16801   ! 28%
	             go to 16802   ! 72%
16801	call nucltransK(1.680,0.093,2.8e-3,1.0e-4,tclev,thlev,tdlev)
	return
16802	call nucltransK(0.952,0.093,4.5e-2,0.,tclev,thlev,tdlev)
	go to 72700
c-----------------------------------------------------------------------
1621	call beta(0.625,84.,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le.80.) go to 16211   ! 80%
	             go to 16212   ! 20%
16211	call nucltransK(1.621,0.093,7.0e-3,1.2e-4,tclev,thlev,tdlev)
	return
16212	call nucltransK(0.893,0.093,4.5e-2,0.,tclev,thlev,tdlev)
	go to 72700
c-----------------------------------------------------------------------
1513	call beta(0.733,84.,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le.22.) go to 15131   ! 22%
	             go to 15132   ! 78%
15131	call nucltransK(1.513,0.093,3.5e-3,0.7e-4,tclev,thlev,tdlev)
	return
15132	call nucltransK(0.786,0.093,4.1e-2,0.,tclev,thlev,tdlev)
	go to 72700
c-----------------------------------------------------------------------
727	call beta(1.519,84.,0.,0.,t)
72700	thlev=0.
	call nucltransK(0.727,0.093,1.7e-2,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
20001	call beta(2.246,84.,0.,0.,t)
	return
c-----------------------------------------------------------------------
	end
c
c end of Bi212.f
c
c Local Variables: --
c mode: fortran --
c End: --
