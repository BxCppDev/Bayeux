c Sb133.f 
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
        subroutine Sb133(tcnuc,tdnuc)
c Scheme of 133Sb decay (S.Rab, NDS 75(1995)491).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 11.12.2003. 
	thnuc=150
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le.27.84) go to 2756
	if(pbeta.le.53.70) go to 2750
	if(pbeta.le.58.28) go to 2211
	if(pbeta.le.58.58) go to 2024
	if(pbeta.le.59.58) go to 1976
	if(pbeta.le.68.54) go to 1913
	if(pbeta.le.75.31) go to 1729
	if(pbeta.le.76.31) go to 1706
	if(pbeta.le.80.19) go to 1642
	if(pbeta.le.83.97) go to 1640
	if(pbeta.le.88.15) go to 1552
	if(pbeta.le.88.95) go to 1501
	if(pbeta.le.91.14) go to 1421
	if(pbeta.le.94.03) go to 1265
	                   go to 1096
c-----------------------------------------------------------------------
2756	call beta(1.247,52.,0.,0.,t)
27560	thlev=0.
	p=100.*rnd1(d)
	if(p.le.45.15) go to 27561
	if(p.le.49.81) go to 27562
	if(p.le.57.90) go to 27563
	if(p.le.67.22) go to 27564
	if(p.le.87.10) go to 27565
	               go to 27566
27561	call nucltransK(2.755,0.032,1.5e-4,5.7e-4,tclev,thlev,tdlev)
	return
27562	call nucltransK(2.447,0.032,1.5e-4,5.2e-4,tclev,thlev,tdlev)
	go to 30800
27563	call nucltransK(1.659,0.032,4.0e-4,1.3e-4,tclev,thlev,tdlev)
	go to 10960
27564	call nucltransK(1.490,0.032,5.0e-4,0.9e-4,tclev,thlev,tdlev)
	go to 12650
27565	call nucltransK(1.026,0.032,1.5e-3,0.,tclev,thlev,tdlev)
	go to 17290
27566	call nucltransK(0.423,0.032,1.2e-2,0.,tclev,thlev,tdlev)
	go to 23320
c-----------------------------------------------------------------------
2750	call beta(1.253,52.,0.,0.,t)
27500	thlev=0.
	p=100.*rnd1(d)
	if(p.le.32.79) go to 27501
	if(p.le.39.00) go to 27502
	if(p.le.42.36) go to 27503
	if(p.le.49.24) go to 27504
	if(p.le.92.58) go to 27505
	               go to 27506
27501	call nucltransK(2.416,0.032,8.0e-5,5.0e-4,tclev,thlev,tdlev)
	go to 33400
27502	call nucltransK(1.654,0.032,4.0e-4,1.3e-4,tclev,thlev,tdlev)
	go to 10960
27503	call nucltransK(1.250,0.032,1.0e-3,0.4e-4,tclev,thlev,tdlev)
	go to 15010
27504	call nucltransK(1.111,0.032,1.5e-3,0.1e-4,tclev,thlev,tdlev)
	go to 16400
27505	call nucltransK(0.837,0.032,8.0e-4,0.,tclev,thlev,tdlev)
	go to 19130
27506	call nucltransK(0.539,0.032,7.0e-3,0.,tclev,thlev,tdlev)
	go to 22110
c-----------------------------------------------------------------------
23320	thlev=0.
	p=100.*rnd1(d)
	if(p.le.65.82) go to 23321
	               go to 23322
23321	call nucltransK(1.236,0.032,1.0e-3,0.4e-4,tclev,thlev,tdlev)
	go to 10960
23322	call nucltransK(0.308,0.032,2.5e-2,0.,tclev,thlev,tdlev)
	go to 20240
c-----------------------------------------------------------------------
2211	call beta(1.792,52.,0.,0.,t)
22110	thlev=0.
	p=100.*rnd1(d)
	if(p.le.23.75) go to 22111
	if(p.le.97.38) go to 22112
	               go to 22113
22111	call nucltransK(1.877,0.032,3.5e-4,1.9e-4,tclev,thlev,tdlev)
	go to 33400
22112	call nucltransK(1.115,0.032,1.5e-3,0.1e-4,tclev,thlev,tdlev)
	go to 10960
22113	call nucltransK(0.572,0.032,5.5e-3,0.,tclev,thlev,tdlev)
	go to 16400
c-----------------------------------------------------------------------
2024	call beta(1.979,52.,0.,0.,t)
20240	thlev=0.
	p=100.*rnd1(d)
	if(p.le.81.61) go to 20241
	               go to 20242
20241	call nucltransK(0.928,0.032,1.8e-3,0.,tclev,thlev,tdlev)
	go to 10960
20242	call nucltransK(0.523,0.032,7.0e-3,0.,tclev,thlev,tdlev)
	go to 15010
c-----------------------------------------------------------------------
1976	call beta(2.027,52.,0.,0.,t)
19760	thlev=0.
	call nucltransK(1.976,0.032,3.0e-4,3.2e-4,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
1913	call beta(2.090,52.,0.,0.,t)
19130	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 7.81) go to 19131
	if(p.le.98.08) go to 19132
	               go to 19133
19131	call nucltransK(1.579,0.032,4.5e-4,0.4e-4,tclev,thlev,tdlev)
	go to 33400
19132	call nucltransK(0.818,0.032,8.0e-4,0.,tclev,thlev,tdlev)
	go to 10960
19133	call nucltransK(0.413,0.032,1.2e-2,0.,tclev,thlev,tdlev)
	go to 15010
c-----------------------------------------------------------------------
1729	call beta(2.274,52.,0.,0.,t)
17290	thlev=0.
	p=100.*rnd1(d)
	if(p.le.68.47) go to 17291
	               go to 17292
17291	call nucltransK(1.729,0.032,4.5e-4,1.5e-4,tclev,thlev,tdlev)
	return
17292	call nucltransK(0.632,0.032,4.5e-3,0.,tclev,thlev,tdlev)
	go to 10960
c-----------------------------------------------------------------------
1706	call beta(2.297,52.,0.,0.,t)
17060	thlev=0.
	call nucltransK(1.706,0.032,4.5e-4,1.5e-4,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
1642	call beta(2.361,52.,0.,0.,t)
16420	thlev=0.
	call nucltransK(1.642,0.032,5.0e-4,1.2e-4,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
1640	call beta(2.363,52.,0.,0.,t)
16400	thlev=0.
	call nucltransK(1.305,0.032,9.0e-4,0.5e-4,tclev,thlev,tdlev)
	go to 33400
c-----------------------------------------------------------------------
1552	call beta(2.451,52.,0.,0.,t)
15520	thlev=0.
	call nucltransK(1.552,0.032,5.0e-4,1.0e-4,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
1501	call beta(2.502,52.,0.,0.,t)
15010	thlev=0.
	call nucltransK(0.404,0.032,1.3e-2,0.,tclev,thlev,tdlev)
	go to 10960
c-----------------------------------------------------------------------
1421	call beta(2.582,52.,0.,0.,t)
14210	thlev=0.
	p=100.*rnd1(d)
	if(p.le.13.44) go to 14211
	               go to 14212
14211	call nucltransK(1.421,0.032,6.5e-4,0.7e-4,tclev,thlev,tdlev)
	return
14212	call nucltransK(1.113,0.032,1.3e-3,0.,tclev,thlev,tdlev)
	go to 30800
c-----------------------------------------------------------------------
1265	call beta(2.738,52.,0.,0.,t)
12650	thlev=0.
	call nucltransK(1.265,0.032,9.0e-4,0.5e-4,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
1096	call beta(2.907,52.,0.,0.,t)
10960	thlev=0.
	call nucltransK(1.096,0.032,1.1e-3,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
33400	return ! creation of isomeric 133mTe with E_exc=334 keV and T1/2=55.4 m
c-----------------------------------------------------------------------
30800	thlev=0.
	call nucltransK(0.308,0.032,2.5e-2,0.,tclev,thlev,tdlev)
	return ! creation of 133Te in g.s. (T1/2=12.5 m)
c-----------------------------------------------------------------------
	return
	end
c
c end of Sb133.f
c
c Local Variables: --
c mode: fortran --
c End: --
