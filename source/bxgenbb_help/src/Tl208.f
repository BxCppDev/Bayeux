c Tl208.f 
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
	subroutine Tl208(tcnuc,tdnuc)
c Scheme of Tl208 decay ("Table of Isotopes", 7th ed., 1978).
c Four-figured labels correspond to energies of 208Pb excited
c levels in keV.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 27.07.1992, 22.10.1995.
c VIT, 11.05.2005, updated to NDS 47(1986)797 (last NDS issue for A=208).
c VIT, 4.02.2009, updated to NDS 108(2007)1583; also were added:
c LM conversion electrons; more complex emission of X rays emitted in K conversion.
	thnuc=183.18
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le. 0.052) go to 4481   !  0.052%
	if(pbeta.le. 0.069) go to 4383   !  0.017%
	if(pbeta.le. 0.113) go to 4358   !  0.044%
	if(pbeta.le. 0.118) go to 4323   !  0.005%
	if(pbeta.le. 0.219) go to 4296   !  0.101%
	if(pbeta.le. 0.221) go to 4262   !  0.002%
	if(pbeta.le. 0.448) go to 4180   !  0.227%
	if(pbeta.le. 0.623) go to 4125   !  0.175%
	if(pbeta.le. 0.630) go to 3996   !  0.007%
	if(pbeta.le. 3.810) go to 3961   !  3.180%
	if(pbeta.le. 3.856) go to 3946   !  0.046%
	if(pbeta.le. 4.486) go to 3920   !  0.630%
	if(pbeta.le.28.686) go to 3708   ! 24.200%
	if(pbeta.le.50.886) go to 3475   ! 22.200%
	                    go to 3198   ! 49.114%
c-----------------------------------------------------------------------
4481	call beta(0.518,82.,0.,0.,t)
	thlev=0.
	call nucltransKLM_Pb(1.283,0.088,7.75e-3,0.015,1.27e-3,
     +                       0.004,0.41e-3,2.3e-5,tclev,thlev,tdlev)
	go to 31980
c-----------------------------------------------------------------------
4383	call beta(0.616,82.,0.,0.,t)
	thlev=0.
c	call nucltransK(1.185,0.088,1.1e-2,0.3e-4,tclev,thlev,tdlev)
	call nucltransKLM_Pb(1.185,0.088,9.49e-3,0.015,1.56e-3,
     +                       0.004,0.47e-3,4.9e-6,tclev,thlev,tdlev)
	go to 31980
c-----------------------------------------------------------------------
4358	call beta(0.641,82.,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 4.55) go to 43581   !  4.55%
	if(p.le.29.55) go to 43582   ! 25.00%
	               go to 43583   ! 70.45%
c43581	call nucltransK(1.744,0.088,4.0e-3,1.5e-4,tclev,thlev,tdlev)
43581	call nucltransKLM_Pb(1.744,0.088,3.56e-3,0.015,0.58e-3,
     +                       0.004,0.17e-3,2.6e-4,tclev,thlev,tdlev)
	go to 26150
c43582	call nucltransK(1.161,0.088,1.1e-2,0.3e-4,tclev,thlev,tdlev)
43582	call nucltransKLM_Pb(1.161,0.088,9.99e-3,0.015,1.64e-3,
     +                       0.004,0.51e-3,2.6e-6,tclev,thlev,tdlev)
	go to 31980
c43583	call nucltransK(0.883,0.088,2.2e-2,0.,tclev,thlev,tdlev)
43583	call nucltransKLM_Pb(0.883,0.088,20.13e-3,0.015,3.33e-3,
     +                       0.004,1.02e-3,0.,tclev,thlev,tdlev)
	go to 34750
c-----------------------------------------------------------------------
4323	call beta(0.676,82.,0.,0.,t)
	thlev=0.
c	call nucltransK(1.126,0.088,1.1e-2,0.3e-4,tclev,thlev,tdlev)
	call nucltransKLM_Pb(1.126,0.088,1.69e-3,0.015,0.26e-3,
     +                       0.004,0.08e-3,2.1e-6,tclev,thlev,tdlev)
	go to 31980
c-----------------------------------------------------------------------
4296	call beta(0.703,82.,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le.87.23) go to 42961   ! 87.23%
	               go to 42962   ! 12.77%
c42961	call nucltransK(0.821,0.088,2.6e-2,0.,tclev,thlev,tdlev)
42961	call nucltransKLM_Pb(0.821,0.088,2.43e-2,0.015,0.40e-2,
     +                       0.004,0.12e-2,0.,tclev,thlev,tdlev)
	go to 34750
c42962	call nucltransK(0.588,0.088,6.5e-2,0.,tclev,thlev,tdlev)
42962	call nucltransKLM_Pb(0.588,0.088,5.78e-2,0.015,0.97e-2,
     +                       0.004,0.29e-2,0.,tclev,thlev,tdlev)
	go to 37080
c-----------------------------------------------------------------------
4262	call beta(0.737,82.,0.,0.,t)
	thlev=0.
c	call nucltransK(1.648,0.088,1.1e-2,0.3e-4,tclev,thlev,tdlev)
	call nucltransKLM_Pb(1.648,0.088,4.11e-3,0.015,0.67e-3,
     +                       0.004,0.20e-3,0.19e-3,tclev,thlev,tdlev)
	go to 26150
c-----------------------------------------------------------------------
4180	call beta(0.819,82.,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le.90.31) go to 41801   ! 90.31%
                       go to 41802   !  9.69%
c41801	call nucltransK(0.983,0.088,1.7e-2,0.,tclev,thlev,tdlev)
41801	call nucltransKLM_Pb(0.983,0.088,1.53e-2,0.015,0.25e-2,
     +                       0.004,0.08e-2,0.,tclev,thlev,tdlev)
	go to 31980
c41802	call nucltransK(0.705,0.088,4.0e-2,0.,tclev,thlev,tdlev)
41802	call nucltransKLM_Pb(0.705,0.088,3.60e-2,0.015,0.60e-2,
     +                       0.004,0.18e-2,0.,tclev,thlev,tdlev)
	go to 34750
c-----------------------------------------------------------------------
4125	call beta(0.874,82.,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le.96.15) go to 41251   ! 96.15%
	               go to 41252   !  3.85%
c41251	call nucltransK(0.928,0.088,1.9e-2,0.,tclev,thlev,tdlev)
41251	call nucltransKLM_Pb(0.928,0.088,1.77e-2,0.015,0.29e-2,
     +                       0.004,0.10e-2,0.,tclev,thlev,tdlev)
	go to 31980
c41252	call nucltransK(0.650,0.088,5.0e-2,0.,tclev,thlev,tdlev)
41252	call nucltransKLM_Pb(0.650,0.088,4.45e-2,0.015,0.75e-2,
     +                       0.004,0.22e-2,0.,tclev,thlev,tdlev)
	go to 34750
c-----------------------------------------------------------------------
3996	call beta(1.003,82.,0.,0.,t)
	thlev=0.
c	call nucltransK(1.381,0.088,8.0e-3,0.6e-4,tclev,thlev,tdlev)
	call nucltransKLM_Pb(1.381,0.088,6.43e-3,0.015,1.05e-3,
     +                       0.004,0.32e-3,0.05e-3,tclev,thlev,tdlev)
	go to 26150
c-----------------------------------------------------------------------
3961	call beta(1.038,82.,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le.51.25) go to 39611   ! 51.25%
	if(p.le.64.82) go to 39612   ! 13.57%
                       go to 39613   ! 35.18%
c39611	call nucltransK(0.763,0.088,3.8e-2,0.,tclev,thlev,tdlev)
39611	call nucltransKLM_Pb(0.763,0.088,2.93e-2,0.015,0.49e-2,
     +                       0.004,0.14e-2,0.,tclev,thlev,tdlev)
	go to 31980
c39612	call nucltransK(0.486,0.088,0.21,0.,tclev,thlev,tdlev)
39612	call nucltransKLM_Pb(0.486,0.088,9.54e-2,0.015,1.61e-2,
     +                       0.004,0.49e-2,0.,tclev,thlev,tdlev)
	go to 34750
c39613	call nucltransK(0.253,0.088,0.71,0.,tclev,thlev,tdlev)
39613	call nucltransKLM_Pb(0.253,0.088,51.60e-2,0.015,8.83e-2,
     +                       0.004,2.57e-2,0.,tclev,thlev,tdlev)
	go to 37080
c-----------------------------------------------------------------------
3946	call beta(1.053,82.,0.,0.,t)
	thlev=0.
c	call nucltransK(0.749,0.088,3.5e-2,0.,tclev,thlev,tdlev)
	call nucltransKLM_Pb(0.749,0.088,3.08e-2,0.015,0.51e-2,
     +                       0.004,0.16e-2,0.,tclev,thlev,tdlev)
	go to 31980
c-----------------------------------------------------------------------
3920	call beta(1.079,82.,0.,0.,t)
	thlev=0.
	p=100.*rnd1(d)
	if(p.le.39.49) go to 39201   ! 39.49%
                       go to 39202   ! 60.51%
c39201	call nucltransK(0.722,0.088,4.1e-2,0.,tclev,thlev,tdlev)
39201	call nucltransKLM_Pb(0.722,0.088,3.20e-2,0.015,0.54e-2,
     +                       0.004,0.16e-2,0.,tclev,thlev,tdlev)
	go to 31980
c39202	call nucltransK(0.211,0.088,1.17,0.,tclev,thlev,tdlev)
39202	call nucltransKLM_Pb(0.211,0.088,9.22e-1,0.015,1.59e-1,
     +                       0.004,0.45e-1,0.,tclev,thlev,tdlev)
	go to 37080
c-----------------------------------------------------------------------
3708	call beta(1.291,82.,0.,0.,t)
37080	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 1.66) go to 37081   !  1.66%
	if(p.le.97.95) go to 37082   ! 96.29%
	               go to 37083   !  2.05%
c37081	call nucltransK(1.094,0.088,0.6e-2,0.,tclev,thlev,tdlev)
37081	call nucltransKLM_Pb(1.094,0.088,4.49e-3,0.015,0.84e-3,
     +                       0.004,0.27e-3,0.,tclev,thlev,tdlev)
	go to 26150
c37082	call nucltransK(0.511,0.088,0.107,0.,tclev,thlev,tdlev)
37082	call nucltransKLM_Pb(0.511,0.088,8.42e-2,0.015,1.42e-2,
     +                       0.004,0.43e-2,0.,tclev,thlev,tdlev)
	go to 31980
c37083	call nucltransK(0.233,0.088,0.888,0.,tclev,thlev,tdlev)
37083	call nucltransKLM_Pb(0.233,0.088,5.47e-1,0.015,1.16e-1,
     +                       0.004,0.37e-1,0.,tclev,thlev,tdlev)
	go to 34750
c-----------------------------------------------------------------------
3475	call beta(1.524,82.,0.,0.,t)
34750	thlev=4.e-12
	p=100.*rnd1(d)
	if(p.le.55.95) go to 34751   ! 55.95%
	               go to 34752   ! 44.05%
c34751	call nucltransK(0.861,0.088,2.8e-2,0.,tclev,thlev,tdlev)
34751	call nucltransKLM_Pb(0.861,0.088,2.17e-2,0.015,0.36e-2,
     +                       0.004,0.11e-2,0.,tclev,thlev,tdlev)
	go to 26150
c34752	call nucltransK(0.277,0.088,0.551,0.,tclev,thlev,tdlev)
34752	call nucltransKLM_Pb(0.277,0.088,4.36e-1,0.015,0.75e-1,
     +                       0.004,0.22e-1,0.,tclev,thlev,tdlev)
	go to 31980
c-----------------------------------------------------------------------
3198	call beta(1.801,82.,0.,0.,t)
31980	thlev=294.e-12
c	call nucltransK(0.583,0.088,2.1e-2,0.,tclev,thlev,tdlev)
	call nucltransKLM_Pb(0.583,0.088,1.51e-2,0.015,0.41e-2,
     +                       0.004,0.13e-2,0.,tclev,thlev,tdlev)
	go to 26150
c-----------------------------------------------------------------------
26150	thlev=16.7e-12
c	call nucltransK(2.615,0.088,8.5e-3,4.3e-4,tclev,thlev,tdlev)
c change in accordance with NNDC-ENSDF on 19.12.2006; confirmed on 23.01.2009
c	call nucltransK(2.615,0.088,2.47e-3,4.3e-4,tclev,thlev,tdlev)
	call nucltransKLM_Pb(2.615,0.088,1.71e-3,0.015,0.29e-3,
     +                       0.004,0.10e-3,0.37e-3,tclev,thlev,tdlev)
c-----------------------------------------------------------------------
	return
	end
c
c end of Tl208.f
c
c Local Variables: --
c mode: fortran --
c End: --
