c Cs136.f 
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
	subroutine Cs136(tcnuc,tdnuc)
c Model for scheme of Cs136 decay ("Table of Isotopes", 8th ed., 
c 1996 and Nucl. Data Sheets 95(2002)837).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 11.09.2002.
	thnuc=1.137024e+6
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le. 2.025) go to 2374 !  2.025%
	if(pbeta.le. 2.233) go to 2357 !  0.208%
	if(pbeta.le.72.009) go to 2207 ! 69.776%
	if(pbeta.le.82.431) go to 2140 ! 10.422%
	if(pbeta.le.87.097) go to 2054 !  4.666%
	                    go to 1867 ! 12.903%
c-----------------------------------------------------------------------
2374	call beta(0.174,56.,0.,0.,t)
23740	thlev=0.
	p=100.*rnd1(d)
	if(p.le.48.02) go to 23741   ! 48.02%
	if(p.le.73.36) go to 23742   ! 25.34%
	if(p.le.77.36) go to 23743   !  4.00%
	               go to 23744   ! 22.64%
23741	call nucltransK(0.507,0.037,1.1e-2,0.,tclev,thlev,tdlev)
	go to 18670
23742	call nucltransK(0.320,0.037,3.5e-2,0.,tclev,thlev,tdlev)
	go to 20540
23743	call nucltransK(0.234,0.037,2.0e-2,0.,tclev,thlev,tdlev)
	go to 21400
23744	call nucltransK(0.167,0.037,2.5e-1,0.,tclev,thlev,tdlev)
	go to 22070
c-----------------------------------------------------------------------
2357	call beta(0.191,56.,0.,0.,t)
23570	thlev=0.
	p=100.*rnd1(d)
	if(p.le.47.12) go to 23571   ! 47.12%
	if(p.le.85.24) go to 23572   ! 38.12%
	               go to 23573   ! 14.76%
23571	call nucltransK(1.538,0.037,6.0e-4,0.2e-4,tclev,thlev,tdlev)
	go to 81900
23572	call nucltransK(0.490,0.037,1.2e-2,0.,tclev,thlev,tdlev)
	go to 18670
23573	call nucltransK(0.303,0.037,4.5e-2,0.,tclev,thlev,tdlev)
	go to 20540
c-----------------------------------------------------------------------
2207	call beta(0.341,56.,0.,0.,t)
22070	thlev=3.11e-9
	p=100.*rnd1(d)
	if(p.le.61.84) go to 22071   ! 61.84%
	if(p.le.76.78) go to 22072   ! 14.94%
	if(p.le.88.48) go to 22073   ! 11.70%
	               go to 22074   ! 11.52%
22071	call nucltransK(0.341,0.037,3.0e-2,0.,tclev,thlev,tdlev)
	go to 18670
22072	call nucltransK(0.176,0.037,5.0e-2,0.,tclev,thlev,tdlev)
	go to 20310
22073	call nucltransK(0.153,0.037,4.3e-1,0.,tclev,thlev,tdlev)
	go to 20540
22074	call nucltransK(0.067,0.037,6.9e-1,0.,tclev,thlev,tdlev)
	go to 21400
c-----------------------------------------------------------------------
2140	call beta(0.408,56.,0.,0.,t)
21400	thlev=1.6e-9
	p=100.*rnd1(d)
	if(p.le. 0.27) go to 21401   !  0.27%
	if(p.le.60.13) go to 21402   ! 59.86%
	if(p.le.62.88) go to 21403   !  2.75%
	               go to 21404   ! 37.12%
21401	call nucltransK(1.321,0.037,1.7e-3,0.1e-4,tclev,thlev,tdlev)
	go to 81900
21402	call nucltransK(0.273,0.037,1.6e-2,0.,tclev,thlev,tdlev)
	go to 18670
21403	call nucltransK(0.109,0.037,1.47,0.,tclev,thlev,tdlev)
	go to 20310
21404	call nucltransK(0.086,0.037,3.5e-1,0.,tclev,thlev,tdlev)
	go to 20540
c-----------------------------------------------------------------------
2054	call beta(0.494,56.,0.,0.,t)
20540	thlev=0.
	p=100.*rnd1(d)
	if(p.le.97.90) go to 20541   ! 97.90%
	               go to 20542   !  2.10%
20541	call nucltransK(1.235,0.037,1.0e-3,0.1e-4,tclev,thlev,tdlev)
	go to 81900
20542	call nucltransK(0.187,0.037,1.9e-1,0.,tclev,thlev,tdlev)
	go to 18670
c-----------------------------------------------------------------------
20310	thlev=0.3084
	call nucltransK(0.164,0.037,2.26,0.,tclev,thlev,tdlev)
	go to 18670
c-----------------------------------------------------------------------
1867	call beta(0.681,56.,0.,0.,t)
18670	thlev=0.
	call nucltransK(1.048,0.037,1.5e-3,0.,tclev,thlev,tdlev)
	go to 81900
c-----------------------------------------------------------------------
81900	thlev=1.930e-12
	call nucltransK(0.819,0.037,2.7e-3,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
	end
c
c end of Cs136.f
c
c Local Variables: --
c mode: fortran --
c End: --
