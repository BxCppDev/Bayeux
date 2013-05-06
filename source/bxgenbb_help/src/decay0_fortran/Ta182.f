c Ta182.f 
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
	subroutine Ta182(tcnuc,tdnuc)
c Scheme of 182Ta decay ("Table of Isotopes", 7th ed., 1978).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 6.03.1996.
	thnuc=9.936e6
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le.28.586) go to 1553
	if(pbeta.le.28.716) go to 1510
	if(pbeta.le.31.416) go to 1488
	if(pbeta.le.32.076) go to 1443
	if(pbeta.le.52.066) go to 1374
	if(pbeta.le.54.366) go to 1331
	if(pbeta.le.94.346) go to 1289
	if(pbeta.le.94.846) go to 1257
	if(pbeta.le.99.846) go to 1221
	if(pbeta.le.99.942) go to  329
	                    go to  100
c-----------------------------------------------------------------------
1553	call beta(0.258,74.,0.,0.,t)
15530	thlev=1.23e-9
	p=100.*rnd1(d)
	if(p.le. 0.25) go to 15531
	if(p.le. 1.45) go to 15532
	if(p.le.22.47) go to 15533
	if(p.le.65.50) go to 15534
	if(p.le.83.50) go to 15535
	if(p.le.84.00) go to 15536
	               go to 15537
15531	call nucltransK(1.453,0.070,4.5e-3,0.1e-4,tclev,thlev,tdlev)
	go to 10000
15532	call nucltransK(1.223,0.070,2.5e-3,0.1e-4,tclev,thlev,tdlev)
	go to 32900
15533	call nucltransK(0.264,0.070,1.4e-1,0.,tclev,thlev,tdlev)
	go to 12890
15534	call nucltransK(0.222,0.070,5.0e-2,0.,tclev,thlev,tdlev)
	go to 13310
15535	call nucltransK(0.179,0.070,7.5e-1,0.,tclev,thlev,tdlev)
	go to 13740
15536	call nucltransK(0.110,0.070,3.0e-1,0.,tclev,thlev,tdlev)
	go to 14430
15537	call nucltransK(0.066,0.012,3.0e-0,0.,tclev,thlev,tdlev)
	go to 14880
c-----------------------------------------------------------------------
1510	call beta(0.301,74.,0.,0.,t)
15100	thlev=0.
	p=100.*rnd1(d)
	if(p.le.34.) go to 15101
	             go to 15102
15101	call nucltransK(1.410,0.070,2.4e-3,0.1e-4,tclev,thlev,tdlev)
	go to 10000
15102	call nucltransK(1.181,0.070,1.4e-3,0.1e-4,tclev,thlev,tdlev)
	go to 32900
c-----------------------------------------------------------------------
1488	call beta(0.323,74.,0.,0.,t)
14880	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 1.1) go to 14881
	if(p.le. 6.1) go to 14882
	if(p.le.29.3) go to 14883
	if(p.le.70.8) go to 14884
	              go to 14885
14881	call nucltransK(1.387,0.070,5.0e-3,0.5e-5,tclev,thlev,tdlev)
	go to 10000
14882	call nucltransK(1.158,0.070,1.5e-3,0.5e-5,tclev,thlev,tdlev)
	go to 32900
14883	call nucltransK(0.198,0.070,3.2e-1,0.,tclev,thlev,tdlev)
	go to 12890
14884	call nucltransK(0.156,0.070,1.2e-1,0.,tclev,thlev,tdlev)
	go to 13310
14885	call nucltransK(0.114,0.070,3.8e-0,0.,tclev,thlev,tdlev)
	go to 13740
c-----------------------------------------------------------------------
1443	call beta(0.368,74.,0.,0.,t)
14430	thlev=0.
	p=100.*rnd1(d)
	if(p.le.40.) go to 14431
	             go to 14432
14431	call nucltransK(1.343,0.070,2.8e-3,0.1e-4,tclev,thlev,tdlev)
	go to 10000
14432	call nucltransK(1.113,0.070,6.0e-3,0.,tclev,thlev,tdlev)
	go to 32900
c-----------------------------------------------------------------------
1374	call beta(0.437,74.,0.,0.,t)
13740	thlev=0.08e-9
	p=100.*rnd1(d)
	if(p.le. 2.0) go to 13741
	if(p.le. 7.7) go to 13742
	if(p.le. 9.8) go to 13743
	if(p.le.71.3) go to 13744
	if(p.le.75.1) go to 13745
	if(p.le.97.9) go to 13746
	              go to 13747
13741	call nucltransK(1.374,0.070,5.5e-3,0.5e-5,tclev,thlev,tdlev)
	return
13742	call nucltransK(1.274,0.070,3.0e-3,0.1e-4,tclev,thlev,tdlev)
	go to 10000
13743	call nucltransK(1.044,0.070,6.6e-3,0.,tclev,thlev,tdlev)
	go to 32900
13744	call nucltransK(0.152,0.070,1.2e-1,0.,tclev,thlev,tdlev)
	go to 12210
13745	call nucltransK(0.116,0.070,2.6e-1,0.,tclev,thlev,tdlev)
	go to 12570
13746	call nucltransK(0.085,0.070,8.5e-0,0.,tclev,thlev,tdlev)
	go to 12890
13747	call nucltransK(0.043,0.012,7.0e-1,0.,tclev,thlev,tdlev)
	go to 13310
c-----------------------------------------------------------------------
1331	call beta(0.480,74.,0.,0.,t)
13310	thlev=0.
	p=100.*rnd1(d)
	if(p.le.85.) go to 13311
	             go to 13312
13311	call nucltransK(1.231,0.070,3.0e-3,0.1e-4,tclev,thlev,tdlev)
	go to 10000
13312	call nucltransK(1.002,0.070,4.7e-3,0.,tclev,thlev,tdlev)
	go to 32900
c-----------------------------------------------------------------------
1289	call beta(0.522,74.,0.,0.,t)
12890	thlev=1.12e-9
	p=100.*rnd1(d)
	if(p.le. 2.35) go to 12891
	if(p.le.29.75) go to 12892
	if(p.le.30.34) go to 12893
	if(p.le.99.00) go to 12894
	               go to 12895
12891	call nucltransK(1.289,0.070,1.3e-2,0.3e-4,tclev,thlev,tdlev)
	return
12892	call nucltransK(1.189,0.070,5.3e-3,0.1e-4,tclev,thlev,tdlev)
	go to 10000
12893	call nucltransK(0.960,0.070,1.3e-2,0.,tclev,thlev,tdlev)
	go to 32900
12894	call nucltransK(0.068,0.012,2.0e-1,0.,tclev,thlev,tdlev)
	go to 12210
12895	call nucltransK(0.032,0.012,1.6e-0,0.,tclev,thlev,tdlev)
	go to 12570
c-----------------------------------------------------------------------
1257	call beta(0.554,74.,0.,0.,t)
12570	thlev=1.7e-12
	p=100.*rnd1(d)
	if(p.le.54.5) go to 12571
	if(p.le.77.8) go to 12572
	              go to 12573
12571	call nucltransK(1.257,0.070,3.0e-3,0.1e-4,tclev,thlev,tdlev)
	return
12572	call nucltransK(1.157,0.070,5.3e-3,0.,tclev,thlev,tdlev)
	go to 10000
12573	call nucltransK(0.928,0.070,5.5e-3,0.,tclev,thlev,tdlev)
	go to 32900
c-----------------------------------------------------------------------
1221	call beta(0.590,74.,0.,0.,t)
12210	thlev=0.37e-12
	p=100.*rnd1(d)
	if(p.le.44.00) go to 12211
	if(p.le.99.92) go to 12212
	               go to 12213
12211	call nucltransK(1.221,0.070,3.0e-3,0.1e-4,tclev,thlev,tdlev)
	return
12212	call nucltransK(1.121,0.070,3.5e-3,0.1e-5,tclev,thlev,tdlev)
	go to 10000
12213	call nucltransK(0.892,0.070,6.0e-3,0.,tclev,thlev,tdlev)
	go to 32900
c-----------------------------------------------------------------------
329	call beta(1.482,74.,0.,0.,t)
32900	thlev=64.e-12
	call nucltransK(0.229,0.070,2.4e-1,0.,tclev,thlev,tdlev)
	go to 10000
c-----------------------------------------------------------------------
100	call beta(1.711,74.,0.,0.,t)
10000	thlev=1.38e-9
	call nucltransK(0.100,0.012,4.0e-0,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
	end
c
c end of Ta182.f
c
c Local Variables: --
c mode: fortran --
c End: --
