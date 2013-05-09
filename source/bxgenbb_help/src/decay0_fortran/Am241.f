c Am241.f
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
	subroutine Am241(tcnuc,tdnuc)
c Model for scheme of Am241 decay (NDS 107(2006)3323 and ENSDF at NNDC site
c on 15.01.2011). Decays to excited levels of 237-Np with probabilities <0.01%
c are neglected.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 23.01.2011.
	thnuc=1.365144e10
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	palpha=100.*rnd1(d)
	!palpha=99.5		!//XXX
	if(palpha.le. 0.015) go to   226
	if(palpha.le. 1.672) go to   159
	if(palpha.le. 1.682) go to   130
	if(palpha.le.14.758) go to   103
	if(palpha.le.99.406) go to    60
	if(palpha.le.99.631) go to    33
	                     go to 10000
c-----------------------------------------------------------------------
226	call alpha(5.322,0.,0.,t)
22600	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.43) go to 22601
	if(p.le.34.21) go to 22602
	               go to 22603
22601	call nucltransKLM(0.150,0.119,1.5e-1,0.022,3.4e-2,0.005,8.4e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 76000
22602	call nucltransKLM(0.123,0.119,1.9e-1,0.022,4.13,0.005,1.59,
     +                    0.,tclev,thlev,tdlev)
	go to 10300
22603	call nucltransKLM(0.067,0.119,0.,0.022,23.,0.005,8.1,
     +                    0.,tclev,thlev,tdlev)
	go to 15900
c-----------------------------------------------------------------------
159	call alpha(5.388,0.,0.,t)
15900	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.34) go to 15901
	if(p.le.21.79) go to 15902
	               go to 15903
15901	call nucltransKLM(0.125,0.119,2.3e-1,0.022,5.5e-2,0.005,1.7e-2,
     +                    0.,tclev,thlev,tdlev)
	go to 33000
15902	call nucltransKLM(0.099,0.119,0.,0.022,11.3,0.005,4.33,
     +                    0.,tclev,thlev,tdlev)
	go to 60000
15903	call nucltransKLM(0.056,0.119,0.,0.022,49.,0.005,18.,
     +                    0.,tclev,thlev,tdlev)
	go to 10300
c-----------------------------------------------------------------------
130	call alpha(5.416,0.,0.,t)
13000	thlev=0.
	call nucltransKLM(0.054,0.119,0.,0.022,23.2,0.005,7.7,
     +                    0.,tclev,thlev,tdlev)
	go to 76000
c-----------------------------------------------------------------------
103	call alpha(5.443,0.,0.,t)
10300	thlev=80.e-12
	p=100.*rnd1(d)
	if(p.le. 0.18) go to 10301
	if(p.le. 0.21) go to 10302
	               go to 10303
10301	call nucltransKLM(0.103,0.119,0.,0.022,9.1e-2,0.005,3.0e-2,
     +                    0.,tclev,thlev,tdlev)
	return
10302	call nucltransKLM(0.070,0.119,0.,0.022,2.5e-1,0.005,8.4e-2,
     +                    0.,tclev,thlev,tdlev)
	go to 33000
10303	call nucltransKLM(0.043,0.119,0.,0.022,124.,0.005,43.,
     +                    0.,tclev,thlev,tdlev)
	go to 60000
c-----------------------------------------------------------------------
76000	thlev=56.e-12
	p=100.*rnd1(d)
	if(p.le. 7.02) go to 76001
	               go to 76002
76001	call nucltransKLM(0.076,0.119,0.,0.022,39.4,0.005,15.0,
     +                    0.,tclev,thlev,tdlev)
	return
76002	call nucltransKLM(0.043,0.119,0.,0.022,59.,0.005,21.,
     +                    0.,tclev,thlev,tdlev)
	go to 33000
c-----------------------------------------------------------------------
60	call alpha(5.486,0.,0.,t)
60000	thlev=67.e-9
	p=100.*rnd1(d)
	if(p.le.78.20) go to 60001
	               go to 60002
60001	call nucltransKLM(0.060,0.119,0.,0.022,0.84,0.005,0.32,
     +                    0.,tclev,thlev,tdlev)
	return
60002	call nucltransKLM(0.026,0.119,0.,0.022,6.,0.005,2.,
     +                    0.,tclev,thlev,tdlev)
	go to 33000
c-----------------------------------------------------------------------
33	call alpha(5.512,0.,0.,t)
33000	thlev=54.e-12
	call nucltransKLM(0.033,0.119,0.,0.022,138.,0.005,47.,
     +                    0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	call alpha(5.544,0.,0.,t)
	return
c-----------------------------------------------------------------------
	end
c
c end of Am241.f
c
c Local Variables: --
c mode: fortran --
c End: --
