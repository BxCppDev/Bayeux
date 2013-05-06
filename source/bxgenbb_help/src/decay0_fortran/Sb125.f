c Sb125.f 
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
        subroutine Sb125(tcnuc,tdnuc)
c Scheme of 125Sb decay (NDS 86(1999)955 + NNDC on 7.02.2010).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 13.02.2010.
	thnuc=8.705115e7
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le.13.446) go to 671
	if(pbeta.le.13.502) go to 653
	if(pbeta.le.19.265) go to 642
	if(pbeta.le.37.180) go to 636
	if(pbeta.le.38.791) go to 525
	if(pbeta.le.79.199) go to 463
	if(pbeta.le.79.247) go to 444
	if(pbeta.le.79.268) go to 402
	if(pbeta.le.86.464) go to 321
	                    go to 145
c-----------------------------------------------------------------------
671	call beta(0.096,52.,0.,0.,t)
6710	thlev=1.26e-12
	p=100.*rnd1(d)
	if(p.le.13.347) go to 6711
	if(p.le.96.921) go to 6712
	if(p.le.97.980) go to 6713
	if(p.le.99.993) go to 6714
	                go to 6715
6711	call nucltransK(0.671,0.032,3.8e-3,0.,tclev,thlev,tdlev)
	return
6712	call nucltransK(0.636,0.032,5.3e-3,0.,tclev,thlev,tdlev)
	go to 3500
6713	call nucltransK(0.228,0.032,8.4e-2,0.,tclev,thlev,tdlev)
	go to 4440
6714	call nucltransK(0.208,0.032,9.1e-2,0.,tclev,thlev,tdlev)
	go to 4630
6715	call nucltransK(0.133,0.032,3.1e-1,0.,tclev,thlev,tdlev)
	go to 5390
c-----------------------------------------------------------------------
653	call beta(0.114,52.,0.,0.,t)
6530	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 4.85) go to 6531
	if(p.le.14.56) go to 6532
	if(p.le.19.11) go to 6533
	               go to 6534
6531	call nucltransK(0.653,0.032,4.0e-3,0.,tclev,thlev,tdlev)
	return
6532	call nucltransK(0.617,0.032,5.6e-3,0.,tclev,thlev,tdlev)
	go to 3500
6533	call nucltransK(0.332,0.032,2.8e-2,0.,tclev,thlev,tdlev)
	go to 3210
6534	call nucltransK(0.209,0.032,8.9e-2,0.,tclev,thlev,tdlev)
	go to 4440
c-----------------------------------------------------------------------
642	call beta(0.125,52.,0.,0.,t)
6420	thlev=70e-12
	p=100.*rnd1(d)
	if(p.le.86.63) go to 6421
	if(p.le.86.70) go to 6422
	if(p.le.93.91) go to 6423
	if(p.le.94.17) go to 6424
	if(p.le.94.86) go to 6425
	               go to 6426
6421	call nucltransK(0.607,0.032,4.9e-3,0.,tclev,thlev,tdlev)
	go to 3500
6422	call nucltransK(0.497,0.032,3.2e-2,0.,tclev,thlev,tdlev)
	go to 1450
6423	call nucltransK(0.321,0.032,7.9e-3,0.,tclev,thlev,tdlev)
	go to 3210
6424	call nucltransK(0.199,0.032,1.5e-1,0.,tclev,thlev,tdlev)
	go to 4440
6425	call nucltransK(0.179,0.032,1.8e-1,0.,tclev,thlev,tdlev)
	go to 4630
6426	call nucltransK(0.117,0.032,1.3e-1,0.,tclev,thlev,tdlev)
	go to 5250
c-----------------------------------------------------------------------
636	call beta(0.131,52.,0.,0.,t)
6360	thlev=40e-12
	p=100.*rnd1(d)
	if(p.le.98.716) go to 6361
	if(p.le.98.743) go to 6362
	if(p.le.98.766) go to 6363
	if(p.le.99.994) go to 6364
	                go to 6365
6361	call nucltransK(0.601,0.032,5.0e-3,0.,tclev,thlev,tdlev)
	go to 3500
6362	call nucltransK(0.491,0.032,3.2e-2,0.,tclev,thlev,tdlev)
	go to 1450
6363	call nucltransK(0.315,0.032,8.3e-3,0.,tclev,thlev,tdlev)
	go to 3210
6364	call nucltransK(0.173,0.032,1.5e-1,0.,tclev,thlev,tdlev)
	go to 4630
6365	call nucltransK(0.111,0.032,1.5e-1,0.,tclev,thlev,tdlev)
	go to 5250
c-----------------------------------------------------------------------
5390	thlev=0.
	p=100.*rnd1(d)
	if(p.le.26.42) go to 5391
	               go to 5392
5391	call nucltransK(0.539,0.032,7.8e-3,0.,tclev,thlev,tdlev)
	return
5392	call nucltransK(0.503,0.032,9.3e-3,0.,tclev,thlev,tdlev)
	go to 3500
c-----------------------------------------------------------------------
525	call beta(0.242,52.,0.,0.,t)
5250	thlev=160e-12
	p=100.*rnd1(d)
	if(p.le. 0.07) go to 5251
	if(p.le.81.12) go to 5252
	if(p.le.99.89) go to 5253
	               go to 5254
5251	call nucltransK(0.490,0.032,3.3e-2,0.,tclev,thlev,tdlev)
	go to 3500
5252	call nucltransK(0.380,0.032,1.8e-2,0.,tclev,thlev,tdlev)
	go to 1450
5253	call nucltransK(0.204,0.032,1.3e-1,0.,tclev,thlev,tdlev)
	go to 3210
5254	call nucltransK(0.062,0.032,7.4e-1,0.,tclev,thlev,tdlev)
	go to 4630
c-----------------------------------------------------------------------
463	call beta(0.304,52.,0.,0.,t)
4630	thlev=13.2e-12
	p=100.*rnd1(d)
	if(p.le.26.08) go to 4631
	if(p.le.99.39) go to 4632
	               go to 4633
4631	call nucltransK(0.463,0.032,1.0e-2,0.,tclev,thlev,tdlev)
	return
4632	call nucltransK(0.428,0.032,1.4e-2,0.,tclev,thlev,tdlev)
	go to 3500
4633	call nucltransK(0.020,0.005,11.1,0.,tclev,thlev,tdlev)
	go to 4440
c-----------------------------------------------------------------------
444	call beta(0.323,52.,0.,0.,t)
4440	thlev=19.1e-12
	p=100.*rnd1(d)
	if(p.le.62.40) go to 4441
	               go to 4442
4441	call nucltransK(0.444,0.032,1.2e-2,0.,tclev,thlev,tdlev)
	return
4442	call nucltransK(0.408,0.032,1.5e-2,0.,tclev,thlev,tdlev)
	go to 3500
c-----------------------------------------------------------------------
402	call beta(0.365,52.,0.,0.,t)
4020	thlev=0.
	p=100.*rnd1(d)
	if(p.le.29.72) go to 4021
	if(p.le.67.45) go to 4022
	               go to 4023
4021	call nucltransK(0.402,0.032,1.9e-1,0.,tclev,thlev,tdlev)
	return
4022	call nucltransK(0.367,0.032,2.0e-2,0.,tclev,thlev,tdlev)
	go to 3500
4023	call nucltransK(0.081,0.032,3.6e-1,0.,tclev,thlev,tdlev)
	go to 3210
c-----------------------------------------------------------------------
321	call beta(0.446,52.,0.,0.,t)
3210	thlev=0.673e-9
	call nucltransK(0.176,0.032,1.7e-1,0.,tclev,thlev,tdlev)
	go to 1450
c-----------------------------------------------------------------------
145	call beta(0.622,52.,0.,0.,t)
1450	thlev=4.959e6
	call nucltransK(0.109,0.032,355.,0.,tclev,thlev,tdlev)
	go to 3500
c-----------------------------------------------------------------------
3500	thlev=1.48e-9
	call nucltransK(0.035,0.032,14.,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
	end
c
c end of Sb125.f
c
c Local Variables: --
c mode: fortran --
c End: --
