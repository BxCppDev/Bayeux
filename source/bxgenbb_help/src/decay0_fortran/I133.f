c I133.f 
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
        subroutine I133(tcnuc,tdnuc)
c Scheme of I133 decay in accordance with S.Rab, NDS 75(1995)491.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 13.12.2003.
	thnuc=74880.
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
        pbeta=100.*rnd1(d)
        if(pbeta.le. 0.42) go to 1590
        if(pbeta.le. 1.68) go to 1386
        if(pbeta.le. 2.08) go to 1350
        if(pbeta.le. 5.89) go to 1298
        if(pbeta.le. 9.10) go to 1236
        if(pbeta.le. 9.73) go to 1052
        if(pbeta.le.13.94) go to  875
        if(pbeta.le.15.77) go to  744
        if(pbeta.le.98.96) go to  530
	                   go to  233
c-----------------------------------------------------------------------
1590    call beta(0.181,54.,0.,0.,t)
15900	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.72) go to 15901
	if(p.le.33.78) go to 15902
	if(p.le.85.06) go to 15903
	if(p.le.90.33) go to 15904
	if(p.le.98.95) go to 15905
	               go to 15906
15901	call nucltransK(1.590,0.035,1.0e-3,1.1e-4,tclev,thlev,tdlev)
        return
15902	call nucltransK(1.060,0.035,1.7e-3,0.,tclev,thlev,tdlev)
        go to 53000
15903	call nucltransK(0.910,0.035,2.0e-2,0.,tclev,thlev,tdlev)
        go to 68000
15904	call nucltransK(0.679,0.035,5.5e-3,0.,tclev,thlev,tdlev)
        go to 91100
15905	call nucltransK(0.538,0.035,8.5e-3,0.,tclev,thlev,tdlev)
        go to 10520
15906	call nucltransK(0.204,0.035,1.5e-1,0.,tclev,thlev,tdlev)
        go to 13860
c-----------------------------------------------------------------------
1386    call beta(0.385,54.,0.,0.,t)
13860	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.72) go to 13861
	               go to 13862
13861	call nucltransK(1.386,0.035,3.5e-3,0.,tclev,thlev,tdlev)
        return
13862	call nucltransK(0.856,0.035,2.6e-3,0.,tclev,thlev,tdlev)
        go to 53000
c-----------------------------------------------------------------------
1350    call beta(0.421,54.,0.,0.,t)
13500	thlev=0.
	p=100.*rnd1(d)
	if(p.le.37.48) go to 13501
	if(p.le.40.53) go to 13502
	if(p.le.79.26) go to 13503
	if(p.le.90.00) go to 13504
	               go to 13505
13501	call nucltransK(1.350,0.035,9.9e-4,0.6e-4,tclev,thlev,tdlev)
        return
13502	call nucltransK(1.088,0.035,1.3e-3,0.,tclev,thlev,tdlev)
        go to 26300
13503	call nucltransK(0.821,0.035,3.1e-3,0.,tclev,thlev,tdlev)
        go to 53000
13504	call nucltransK(0.670,0.035,5.0e-3,0.,tclev,thlev,tdlev)
        go to 68000
13505	call nucltransK(0.439,0.035,1.4e-2,0.,tclev,thlev,tdlev)
        go to 91100
c-----------------------------------------------------------------------
1298    call beta(0.473,54.,0.,0.,t)
12980	thlev=0.
	p=100.*rnd1(d)
	if(p.le.62.29) go to 12981
	if(p.le.62.52) go to 12982
	if(p.le.74.71) go to 12983
	if(p.le.89.13) go to 12984
	if(p.le.97.51) go to 12985
	if(p.le.99.07) go to 12986
	               go to 12987
12981	call nucltransK(1.298,0.035,9.6e-4,0.5e-4,tclev,thlev,tdlev)
        return
12982	call nucltransK(1.036,0.035,1.5e-3,0.,tclev,thlev,tdlev)
        go to 26300
12983	call nucltransK(0.768,0.035,3.5e-3,0.,tclev,thlev,tdlev)
        go to 53000
12984	call nucltransK(0.618,0.035,5.9e-3,0.,tclev,thlev,tdlev)
        go to 68000
12985	call nucltransK(0.423,0.035,1.5e-2,0.,tclev,thlev,tdlev)
        go to 87500
12986	call nucltransK(0.387,0.035,2.0e-2,0.,tclev,thlev,tdlev)
        go to 91100
12987	call nucltransK(0.246,0.035,8.0e-2,0.,tclev,thlev,tdlev)
        go to 10520
c-----------------------------------------------------------------------
1236    call beta(0.535,54.,0.,0.,t)
12360	thlev=0.
	p=100.*rnd1(d)
	if(p.le.47.94) go to 12361
	if(p.le.95.88) go to 12362
	if(p.le.96.51) go to 12363
	               go to 12364
12361	call nucltransK(1.236,0.035,1.0e-3,0.1e-4,tclev,thlev,tdlev)
        return
12362	call nucltransK(0.707,0.035,4.3e-3,0.,tclev,thlev,tdlev)
        go to 53000
12363	call nucltransK(0.556,0.035,7.0e-3,0.,tclev,thlev,tdlev)
        go to 68000
12364	call nucltransK(0.361,0.035,3.0e-2,0.,tclev,thlev,tdlev)
        go to 87500
c-----------------------------------------------------------------------
1052    call beta(0.719,54.,0.,0.,t)
10520	thlev=0.
	p=100.*rnd1(d)
	if(p.le.90.26) go to 10521
	if(p.le.98.37) go to 10522
	               go to 10523
10521	call nucltransK(1.052,0.035,2.0e-3,0.,tclev,thlev,tdlev)
        return
10522	call nucltransK(0.790,0.035,3.0e-3,0.,tclev,thlev,tdlev)
        go to 26300
10523	call nucltransK(0.372,0.035,2.5e-2,0.,tclev,thlev,tdlev)
        go to 68000
c-----------------------------------------------------------------------
91100	thlev=0.
	p=100.*rnd1(d)
	if(p.le.31.08) go to 91101
	if(p.le.69.59) go to 91102
	               go to 91103
91101	call nucltransK(0.911,0.035,3.0e-3,0.,tclev,thlev,tdlev)
        return
91102	call nucltransK(0.649,0.035,6.0e-3,0.,tclev,thlev,tdlev)
        go to 26300
91103	call nucltransK(0.382,0.035,2.0e-2,0.,tclev,thlev,tdlev)
        go to 53000
c-----------------------------------------------------------------------
875	call beta(0.896,54.,0.,0.,t)
87500	thlev=0.
	p=100.*rnd1(d)
	if(p.le.97.75) go to 87501
	               go to 87502
87501	call nucltransK(0.875,0.035,2.2e-3,0.,tclev,thlev,tdlev)
        return
87502	call nucltransK(0.345,0.035,3.0e-2,0.,tclev,thlev,tdlev)
        go to 53000
c-----------------------------------------------------------------------
744	call beta(1.027,54.,0.,0.,t)
74400	thlev=0.
	call nucltransK(0.510,0.035,9.0e-3,0.,tclev,thlev,tdlev)
        go to 23300
c-----------------------------------------------------------------------
68000	thlev=0.
	p=100.*rnd1(d)
	if(p.le.77.66) go to 68001
	if(p.le.96.42) go to 68002
	               go to 68003
68001	call nucltransK(0.680,0.035,5.4e-3,0.,tclev,thlev,tdlev)
        return
68002	call nucltransK(0.418,0.035,1.6e-2,0.,tclev,thlev,tdlev)
        go to 26300
68003	call nucltransK(0.150,0.035,3.0e-1,0.,tclev,thlev,tdlev)
        go to 53000
c-----------------------------------------------------------------------
530	call beta(1.241,54.,0.,0.,t)
53000	thlev=0.
	p=100.*rnd1(d)
	if(p.le.99.87) go to 53001
	               go to 53002
53001	call nucltransK(0.530,0.035,8.2e-3,0.,tclev,thlev,tdlev)
        return
53002	call nucltransK(0.267,0.035,6.0e-2,0.,tclev,thlev,tdlev)
        go to 26300
c-----------------------------------------------------------------------
26300	thlev=0.
	call nucltransK(0.263,0.035,5.8e-2,0.,tclev,thlev,tdlev)
        return
c-----------------------------------------------------------------------
233	call beta(1.538,54.,0.,0.,t)
23300	thlev=189216.
	call nucltransK(0.233,0.035,8.8,0.,tclev,thlev,tdlev)
        return
c-----------------------------------------------------------------------
	return
	end
c
c end of I133.f
c
c Local Variables: --
c mode: fortran --
c End: --
