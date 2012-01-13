c Te134.f 
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
	subroutine Te134(tcnuc,tdnuc)
c Model for scheme of Te134 decay (Yu.V.Sergeenkov, Nucl. Data Sheets 
c 71(1994)557).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 7.10.2002.
	thnuc=2508.
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le.14.) go to 1106 ! 14%
	if(pbeta.le.58.) go to  923 ! 44%
	                 go to  847 ! 42%
c-----------------------------------------------------------------------
1106	call beta(0.454,53.,0.,0.,t)
11060	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 3.33) go to 11061   !  3.33%
	if(p.le.14.45) go to 11062   ! 11.12%
	if(p.le.17.78) go to 11063   !  3.33%
	if(p.le.91.15) go to 11064   ! 73.37%
	if(p.le.94.70) go to 11065   !  3.55%
	               go to 11066   !  5.30%
11061	call nucltransK(1.027,0.033,1.5e-3,0.,tclev,thlev,tdlev)
	go to 79000
11062	call nucltransK(0.926,0.033,2.5e-3,0.,tclev,thlev,tdlev)
	go to 18100
11063	call nucltransK(0.896,0.033,3.0e-3,0.,tclev,thlev,tdlev)
	go to 21000
11064	call nucltransK(0.461,0.033,1.5e-2,0.,tclev,thlev,tdlev)
	go to 64500
11065	call nucltransK(0.260,0.033,6.0e-2,0.,tclev,thlev,tdlev)
	go to 84700
11066	call nucltransK(0.183,0.033,1.8e-1,0.,tclev,thlev,tdlev)
	go to 92300
c-----------------------------------------------------------------------
923	call beta(0.637,53.,0.,0.,t)
92300	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 2.71) go to 92301   !  2.71%
	if(p.le.37.31) go to 92302   ! 34.60%
	if(p.le.47.94) go to 92303   ! 10.63%
	if(p.le.98.37) go to 92304   ! 50.43%
	               go to 92305   !  1.63%
92301	call nucltransK(0.844,0.033,2.5e-3,0.,tclev,thlev,tdlev)
	go to 79000
92302	call nucltransK(0.743,0.033,4.5e-3,0.,tclev,thlev,tdlev)
	go to 18100
92303	call nucltransK(0.713,0.033,5.0e-3,0.,tclev,thlev,tdlev)
	go to 21000
92304	call nucltransK(0.278,0.033,4.9e-2,0.,tclev,thlev,tdlev)
	go to 64500
92305	call nucltransK(0.077,0.033,1.61,0.,tclev,thlev,tdlev)
	go to 84700
c----------------------------------------------------------------------
847	call beta(0.713,53.,0.,0.,t)
84700	thlev=0.
	p=100.*rnd1(d)
	if(p.le.69.71) go to 84701   ! 69.71%
	if(p.le.72.49) go to 84702   !  2.78%
	if(p.le.76.45) go to 84703   !  3.96%
	               go to 84704   ! 23.55%
84701	call nucltransK(0.767,0.033,3.3e-3,0.,tclev,thlev,tdlev)
	go to 79000
84702	call nucltransK(0.666,0.033,6.0e-3,0.,tclev,thlev,tdlev)
	go to 18100
84703	call nucltransK(0.636,0.033,7.0e-3,0.,tclev,thlev,tdlev)
	go to 21000
84704	call nucltransK(0.201,0.033,1.3e-1,0.,tclev,thlev,tdlev)
	go to 64500
c----------------------------------------------------------------------
64500	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 2.03) go to 64501   !  2.03%
	if(p.le.45.01) go to 64502   ! 42.98%
	if(p.le.55.87) go to 64503   ! 10.86%
	               go to 64504   ! 44.13%
64501	call nucltransK(0.645,0.033,5.0e-3,0.,tclev,thlev,tdlev)
	return
64502	call nucltransK(0.566,0.033,9.0e-3,0.,tclev,thlev,tdlev)
	go to 79000
64503	call nucltransK(0.465,0.033,1.5e-2,0.,tclev,thlev,tdlev)
	go to 18100
64504	call nucltransK(0.435,0.033,1.4e-2,0.,tclev,thlev,tdlev)
	go to 21000
c----------------------------------------------------------------------
21000	thlev=0.15e-9
	p=100.*rnd1(d)
	if(p.le.98.94) go to 21001   ! 98.94%
	               go to 21002   !  1.06%
21001	call nucltransK(0.210,0.033,1.1e-1,0.,tclev,thlev,tdlev)
	return
21002	call nucltransK(0.131,0.033,5.2e-1,0.,tclev,thlev,tdlev)
	go to 79000
c----------------------------------------------------------------------
18100	thlev=0.1e-9
	p=100.*rnd1(d)
	if(p.le.95.83) go to 18101   ! 95.83%
	if(p.le.96.45) go to 18102   !  0.62%
	               go to 18103   !  3.55%
18101	call nucltransK(0.181,0.033,1.8e-1,0.,tclev,thlev,tdlev)
	return
18102	call nucltransK(0.137,0.033,5.8e-1,0.,tclev,thlev,tdlev)
	go to 44000
18103	call nucltransK(0.101,0.033,1.2,0.,tclev,thlev,tdlev)
	go to 79000
c----------------------------------------------------------------------
79000	thlev=1.62e-9
	call nucltransK(0.079,0.033,1.50,0.,tclev,thlev,tdlev)
	return
c----------------------------------------------------------------------
44000	thlev=0.
	call nucltransK(0.044,0.033,7.97,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
	end
c
c end of Te134.f
c
c Local Variables: --
c mode: fortran --
c End: --
