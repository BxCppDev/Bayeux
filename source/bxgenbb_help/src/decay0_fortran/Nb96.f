c Nb96.f 
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
	subroutine Nb96(tcnuc,tdnuc)
c Scheme of Nb96 decay (NDS 68(1993)165 and ENSDF at NNDC site on 13.11.2007).
c Three-figured labels correspond to energies of 96Mo excited
c levels in keV.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 7.05.1998; update 13.11.2007.
	thnuc=8.406e4
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le. 0.024) go to 2975
	if(pbeta.le. 0.524) go to 2876
	if(pbeta.le. 1.014) go to 2755
	if(pbeta.le. 3.314) go to 2441
	                    go to 2439
c-----------------------------------------------------------------------
2975	call beta(0.212,42.,0.,0.,t)
29750	thlev=0.
	call nucltransK(1.347,0.020,4.3e-4,0.3e-4,tclev,thlev,tdlev)
	go to 16280
c-----------------------------------------------------------------------
2876	call beta(0.311,42.,0.,0.,t)
28760	thlev=0.
	p=100.*rnd1(d)
	if(p.le.93.26) go to 28761
	               go to 28762
28761	call nucltransK(0.435,0.020,5.4e-3,0.,tclev,thlev,tdlev)
	go to 24410
28762	call nucltransK(0.120,0.020,1.5e-1,0.,tclev,thlev,tdlev)
	go to 27550
c-----------------------------------------------------------------------
2755	call beta(0.432,42.,0.,0.,t)
27550	thlev=0.
	p=100.*rnd1(d)
	if(p.le.75.63) go to 27551
	if(p.le.86.64) go to 27552
	               go to 27553
27551	call nucltransK(1.127,0.020,5.2e-4,0.1e-5,tclev,thlev,tdlev)
	go to 16280
27552	call nucltransK(0.316,0.020,1.2e-2,0.,tclev,thlev,tdlev)
	go to 24390
27553	call nucltransKLM(0.314,0.020,1.1e-2,0.003,1.3e-3,0.001,4.2e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 24410
c-----------------------------------------------------------------------
2441	call beta(0.746,42.,0.,0.,t)
24410	thlev=0.
	call nucltransK(0.813,0.020,1.3e-3,0.,tclev,thlev,tdlev)
	go to 16280
c-----------------------------------------------------------------------
2439	call beta(0.748,42.,0.,0.,t)
24390	thlev=0.
	p=100.*rnd1(d)
	if(p.le.11.20) go to 24391
	if(p.le.69.87) go to 24392
	if(p.le.96.89) go to 24393
	               go to 24394
24391	call nucltransKLM(0.811,0.020,1.1e-3,0.003,1.3e-4,0.001,2.2e-5,
     +                    0.,tclev,thlev,tdlev)
	go to 16280
24392	call nucltransKLM(0.569,0.020,2.6e-3,0.003,2.8e-4,0.001,5.8e-5,
     +                    0.,tclev,thlev,tdlev)
	go to 18700
24393	call nucltransKLM(0.460,0.020,5.3e-3,0.003,6.4e-4,0.001,1.3e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 19780
24394	call nucltransKLM(0.219,0.020,3.2e-2,0.003,4.0e-3,0.001,1.4e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 22190
c-----------------------------------------------------------------------
22190	thlev=0.
	p=100.*rnd1(d)
	if(p.le.11.19) go to 22191
	if(p.le.36.94) go to 22192
	if(p.le.44.75) go to 22193
	if(p.le.68.44) go to 22194
	if(p.le.80.66) go to 22195
	               go to 22196
22191	call nucltransK(1.441,0.020,4.3e-4,0.5e-4,tclev,thlev,tdlev)
	go to 77800
22192	call nucltransKLM(0.722,0.020,1.5e-3,0.003,1.7e-4,0.001,3.6e-5,
     +                    0.,tclev,thlev,tdlev)
	go to 14980
22193	call nucltransKLM(0.593,0.020,2.3e-3,0.003,2.6e-4,0.001,5.2e-5,
     +                    0.,tclev,thlev,tdlev)
	go to 16260
22194	call nucltransKLM(0.591,0.020,2.4e-3,0.003,2.8e-4,0.001,4.9e-5,
     +                    0.,tclev,thlev,tdlev)
	go to 16280
22195	call nucltransKLM(0.350,0.020,1.0e-2,0.003,1.2e-3,0.001,5.0e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 18700
22196	call nucltransKLM(0.241,0.020,2.1e-2,0.003,2.4e-3,0.001,4.9e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 19780
c-----------------------------------------------------------------------
19780	thlev=0.
	p=100.*rnd1(d)
	if(p.le.71.90) go to 19781
	if(p.le.92.93) go to 19782
	if(p.le.95.95) go to 19783
	if(p.le.99.81) go to 19784
	               go to 19785
19781	call nucltransK(1.200,0.020,4.6e-4,7.7e-6,tclev,thlev,tdlev)
	go to 77800
19782	call nucltransK(0.481,0.020,4.5e-3,0.,tclev,thlev,tdlev)
	go to 14980
19783	call nucltransKLM(0.353,0.020,1.0e-2,0.003,1.2e-3,0.001,7.0e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 16260
19784	call nucltransKLM(0.350,0.020,1.0e-2,0.003,1.2e-3,0.001,5.0e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 16280
19785	call nucltransKLM(0.109,0.020,1.7e-1,0.003,2.0e-2,0.001,4.2e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 18700
c-----------------------------------------------------------------------
18700	thlev=6.4e-12
	p=100.*rnd1(d)
	if(p.le.88.61) go to 18701
	if(p.le.93.45) go to 18702
	               go to 18703
18701	call nucltransK(1.091,0.020,5.6e-4,0.,tclev,thlev,tdlev)
	go to 77800
18702	call nucltransKLM(0.372,0.020,1.0e-2,0.003,1.3e-3,0.001,4.3e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 14980
18703	call nucltransKLM(0.241,0.020,2.1e-2,0.003,2.4e-3,0.001,7.8e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 16280
c-----------------------------------------------------------------------
16280	thlev=1.2e-12
	call nucltransKLM(0.850,0.020,1.0e-3,0.003,1.1e-4,0.001,2.3e-5,
     +                    0.,tclev,thlev,tdlev)
	go to 77800
c-----------------------------------------------------------------------
16260	thlev=0.
	p=100.*rnd1(d)
	if(p.le.11.72) go to 16261
	if(p.le.98.63) go to 16262
	               go to 16263
16261	call nucltransK(1.626,0.020,2.8e-4,1.3e-4,tclev,thlev,tdlev)
	return
16262	call nucltransKLM(0.848,0.020,1.0e-3,0.003,1.1e-4,0.001,2.3e-5,
     +                    0.,tclev,thlev,tdlev)
	go to 77800
16263	call nucltransKLM(0.128,0.020,1.1e-1,0.003,1.3e-2,0.001,2.7e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 14980
c-----------------------------------------------------------------------
14980	thlev=0.78e-12
	p=100.*rnd1(d)
	if(p.le.32.43) go to 14981
	               go to 14982
14981	call nucltransK(1.498,0.020,3.3e-4,0.8e-4,tclev,thlev,tdlev)
	return
14982	call nucltransKLM(0.720,0.020,1.5e-3,0.003,1.7e-4,0.001,3.5e-5,
     +                    0.,tclev,thlev,tdlev)
	go to 77800
c-----------------------------------------------------------------------
77800	thlev=3.67e-12
	call nucltransKLM(0.778,0.020,1.2e-3,0.003,1.4e-3,0.001,2.9e-5,
     +                    0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
	end
c
c end of Nb96.f
c
c Local Variables: --
c mode: fortran --
c End: --
