c Eu147.f 
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
	subroutine Eu147(tcnuc,tdnuc)
c Scheme of 147Eu decay ("Table of Isotopes", 7th ed., 1978).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 10.03.1996.
	thnuc=1.9008e6
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pdecay=100.*rnd1(d)
	if(pdecay.le. 0.002) then
	   call alpha(2.908,0.,0.,t)
	   return
	endif
	if(pdecay.le. 0.202) go to  1646
	if(pdecay.le. 0.502) go to  1549
	if(pdecay.le. 0.552) go to  1471
	if(pdecay.le. 1.552) go to  1453
	if(pdecay.le. 2.052) go to  1318
	if(pdecay.le. 2.252) go to  1304
	if(pdecay.le.11.352) go to  1077
	if(pdecay.le.11.582) go to  1063
	if(pdecay.le.17.102) go to  1054
	if(pdecay.le.17.182) go to  1007
	if(pdecay.le.35.382) go to   799
	if(pdecay.le.57.592) go to   197
	if(pdecay.le.76.792) go to   121
	                     go to 10000
c-----------------------------------------------------------------------
1646	call gamma(0.049,0.,0.,t)
	thlev=0.
	call nucltransK(1.449,0.047,5.0e-4,0.8e-4,tclev,thlev,tdlev)
	go to 19700
c-----------------------------------------------------------------------
1549	call gamma(0.049,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le.31.) go to 15491
	             go to 15492
15491	call nucltransK(1.427,0.047,5.0e-4,0.7e-4,tclev,thlev,tdlev)
	go to 12100
15492	call nucltransK(0.750,0.047,1.8e-3,0.,tclev,thlev,tdlev)
	go to 79900
c-----------------------------------------------------------------------
1471	call gamma(0.049,0.,0.,t)
	thlev=0.
	call nucltransK(1.350,0.047,5.5e-4,0.6e-4,tclev,thlev,tdlev)
	go to 12100
c-----------------------------------------------------------------------
1453	call gamma(0.049,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le.24.) go to 14531
	             go to 14532
14531	call nucltransK(1.332,0.047,5.5e-4,0.5e-4,tclev,thlev,tdlev)
	go to 12100
14532	call nucltransK(1.256,0.047,6.0e-4,0.4e-4,tclev,thlev,tdlev)
	go to 19700
c-----------------------------------------------------------------------
1318	call gamma(0.049,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le.22.8) go to 13181
	if(p.le.68.3) go to 13182
	              go to 13183
13181	call nucltransK(1.318,0.047,5.5e-4,0.5e-4,tclev,thlev,tdlev)
	return
13182	call nucltransK(1.197,0.047,6.5e-4,0.3e-4,tclev,thlev,tdlev)
	go to 12100
13183	call nucltransK(1.120,0.047,7.5e-4,0.2e-4,tclev,thlev,tdlev)
	go to 19700
c-----------------------------------------------------------------------
1304	call gamma(0.049,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le.15.) go to 13041
	             go to 13042
13041	call nucltransK(1.107,0.047,7.5e-4,0.1e-4,tclev,thlev,tdlev)
	go to 19700
13042	call nucltransK(0.505,0.047,4.0e-3,0.,tclev,thlev,tdlev)
	go to 79900
c-----------------------------------------------------------------------
1077	call gamma(0.049,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le.62.5) go to 10771
	if(p.le.98.5) go to 10772
	              go to 10773
10771	call nucltransK(1.077,0.047,4.3e-3,0.1e-4,tclev,thlev,tdlev)
	return
10772	call nucltransK(0.956,0.047,5.5e-3,0.,tclev,thlev,tdlev)
	go to 12100
10773	call nucltransK(0.880,0.047,9.5e-3,0.,tclev,thlev,tdlev)
	go to 19700
c-----------------------------------------------------------------------
1063	call gamma(0.049,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le.40.) go to 10631
	             go to 10632
10631	call nucltransK(1.063,0.047,1.1e-3,0.1e-4,tclev,thlev,tdlev)
	return
10632	call nucltransK(0.942,0.047,1.3e-3,0.,tclev,thlev,tdlev)
	go to 12100
c-----------------------------------------------------------------------
1054	call gamma(0.049,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le.57.) go to 10541
	             go to 10542
10541	call nucltransK(0.933,0.047,1.3e-3,0.,tclev,thlev,tdlev)
	go to 12100
10542	call nucltransK(0.857,0.047,1.6e-3,0.,tclev,thlev,tdlev)
	go to 19700
c-----------------------------------------------------------------------
1007	call gamma(0.049,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le.45.) go to 10071
	             go to 10072
10071	call nucltransK(0.886,0.047,1.5e-3,0.,tclev,thlev,tdlev)
	go to 12100
10072	call nucltransK(0.809,0.047,9.0e-3,0.,tclev,thlev,tdlev)
	go to 19700
c-----------------------------------------------------------------------
799	call gamma(0.049,0.,0.,t)
79900	thlev=0.
	p=100.*rnd1(d)
	if(p.le.24.2) go to 79901
	if(p.le.71.7) go to 79902
	              go to 79903
79901	call nucltransK(0.799,0.047,4.5e-3,0.,tclev,thlev,tdlev)
	return
79902	call nucltransK(0.678,0.047,1.4e-2,0.,tclev,thlev,tdlev)
	go to 12100
79903	call nucltransK(0.601,0.047,1.8e-2,0.,tclev,thlev,tdlev)
	go to 19700
c-----------------------------------------------------------------------
197	p=100.*rnd1(d)
	if(p.le.99.4) call gamma(0.049,0.,0.,t)
	if(p.gt.99.4) call beta(0.511,-62.,0.,0.,t)
19700	thlev=1.3e-9
	p=100.*rnd1(d)
	if(p.le.97.2) go to 19701
	              go to 19702
19701	call nucltransK(0.197,0.047,2.2e-1,0.,tclev,thlev,tdlev)
	return
19702	call nucltransK(0.076,0.047,3.5e-0,0.,tclev,thlev,tdlev)
	go to 12100
c-----------------------------------------------------------------------
121	p=100.*rnd1(d)
	if(p.le.99.5) call gamma(0.049,0.,0.,t)
	if(p.gt.99.5) call beta(0.587,-62.,0.,0.,t)
12100	thlev=0.77e-9
	call nucltransK(0.121,0.047,1.1e-0,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	p=100.*rnd1(d)
	if(p.le.99.3) call gamma(0.049,0.,0.,t)
	if(p.gt.99.3) call beta(0.708,-62.,0.,0.,t)
	return
c-----------------------------------------------------------------------
	end
c
c end of Eu147.f
c
c Local Variables: --
c mode: fortran --
c End: --
