c As79.f 
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
	subroutine As79(tcnuc,tdnuc)
c Model for scheme of As79+Se79m decay ("Table of Isotopes", 8th ed., 
c 1998 and Nucl. Data Sheets 96(2002)1).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 1.10.2004.
	thnuc=540.6
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le. 0.24) go to 1089
	if(pbeta.le. 0.67) go to 1080
	if(pbeta.le. 2.43) go to 975
	if(pbeta.le. 2.69) go to 572
	if(pbeta.le. 3.79) go to 528
	if(pbeta.le. 5.24) go to 365
	                   go to 96
c-----------------------------------------------------------------------
1089	call beta(1.192,34.,0.,0.,t)
10890	thlev=0.
	p=100.*rnd1(d)
	if(p.le.53.70) go to 10891   
	               go to 10892   
10891	call nucltransK(0.993,0.013,4.0e-4,0.,tclev,thlev,tdlev)
	go to 96000
10892	call nucltransK(0.724,0.013,8.5e-4,0.,tclev,thlev,tdlev)
	go to 36500
c-----------------------------------------------------------------------
1080	call beta(1.201,34.,0.,0.,t)
10800	thlev=0.
	p=100.*rnd1(d)
	if(p.le.68.97) go to 10801   
	               go to 10802   
10801	call nucltransK(0.715,0.013,8.5e-4,0.,tclev,thlev,tdlev)
	go to 36500
10802	call nucltransK(0.552,0.013,1.5e-3,0.,tclev,thlev,tdlev)
	go to 52800
c-----------------------------------------------------------------------
975	call beta(1.306,34.,0.,0.,t)
97500	thlev=0.
	p=100.*rnd1(d)
	if(p.le.79.37) go to 97501   
	if(p.le.94.45) go to 97502   
	               go to 97503   
97501	call nucltransK(0.879,0.013,5.5e-4,0.,tclev,thlev,tdlev)
	go to 96000
97502	call nucltransK(0.447,0.013,2.5e-3,0.,tclev,thlev,tdlev)
	go to 52800
97503	call nucltransK(0.402,0.013,3.2e-3,0.,tclev,thlev,tdlev)
	go to 57200
c-----------------------------------------------------------------------
572	call beta(1.709,34.,0.,0.,t)
57200	thlev=1.6e-11
	p=100.*rnd1(d)
	if(p.le.95.33) go to 57201   
	               go to 57202   
57201	call nucltransK(0.476,0.013,1.0e-3,0.,tclev,thlev,tdlev)
	go to 96000
57202	call nucltransK(0.207,0.013,1.8e-2,0.,tclev,thlev,tdlev)
	go to 36500
c-----------------------------------------------------------------------
528	call beta(1.753,34.,0.,0.,t)
52800	thlev=3.1e-12
	call nucltransK(0.432,0.013,2.7e-3,0.,tclev,thlev,tdlev)
	go to 96000
c-----------------------------------------------------------------------
365	call beta(1.916,34.,0.,0.,t)
36500	thlev=9.4e-11
	call nucltransK(0.365,0.013,2.0e-3,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
96	call beta(2.185,34.,0.,0.,t)
96000	thlev=235.2
	p=100.*rnd1(d)
	if(p.le.99.944) go to 96001   
	                go to 96002   
96001	call nucltransK(0.096,0.013,9.39,0.,tclev,thlev,tdlev) ! IT to Se79 (gs)
	return
96002	call beta(0.247,35.,tclev,thlev,tdlev)                 ! beta to Br79 (gs)
	return
c-----------------------------------------------------------------------
	end
c
c end of As79.f
c
c Local Variables: --
c mode: fortran --
c End: --
