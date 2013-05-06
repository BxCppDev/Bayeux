c Sb126.f 
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
        subroutine Sb126(tcnuc,tdnuc)
c Scheme of 126Sb decay (J.Katakura et al., NDS 97(2002)765).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 27.11.2003. Corrected 2.12.2003, thanks F.Capella.
	thnuc=1.0670e6
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le. 0.48) go to 3473
	if(pbeta.le. 2.48) go to 3450
	if(pbeta.le.30.28) go to 3194
	if(pbeta.le.35.94) go to 3171
	if(pbeta.le.43.99) go to 3071
	if(pbeta.le.48.02) go to 2989
	if(pbeta.le.48.50) go to 2974
	if(pbeta.le.49.27) go to 2840
	if(pbeta.le.57.04) go to 2812
	if(pbeta.le.61.74) go to 2766
	if(pbeta.le.77.08) go to 2497
	if(pbeta.le.77.94) go to 2396
	if(pbeta.le.80.82) go to 2218
	                   go to 1776
c-----------------------------------------------------------------------
3473	call beta(0.200,52.,0.,0.,t)
34730	thlev=0.
	call nucltransK(0.958,0.032,1.5e-3,0.,tclev,thlev,tdlev)
	go to 25150
c-----------------------------------------------------------------------
3450	call beta(0.223,52.,0.,0.,t)
34500	thlev=0.
	p=100.*rnd1(d)
	if(p.le.57.14) go to 34501
	               go to 34502
34501	call nucltransK(0.954,0.032,1.5e-3,0.,tclev,thlev,tdlev)
	go to 24970
34502	call nucltransK(0.639,0.032,3.6e-3,0.,tclev,thlev,tdlev)
	go to 28120
c-----------------------------------------------------------------------
3194	call beta(0.479,52.,0.,0.,t)
31940	thlev=0.
	call nucltransK(0.697,0.032,3.0e-3,0.,tclev,thlev,tdlev)
	go to 24970
c-----------------------------------------------------------------------
3171	call beta(0.502,52.,0.,0.,t)
31710	thlev=0.
	p=100.*rnd1(d)
	if(p.le.62.82) go to 31711
	               go to 31712
31711	call nucltransK(0.675,0.032,3.2e-3,0.,tclev,thlev,tdlev)
	go to 24970
31712	call nucltransK(0.656,0.032,3.4e-3,0.,tclev,thlev,tdlev)
	go to 25150
c-----------------------------------------------------------------------
3071	call beta(0.602,52.,0.,0.,t)
30710	thlev=0.
	p=100.*rnd1(d)
	if(p.le.79.86) go to 30711
	               go to 30712
30711	call nucltransK(0.574,0.032,5.0e-3,0.,tclev,thlev,tdlev)
	go to 24970
30712	call nucltransK(0.556,0.032,5.0e-3,0.,tclev,thlev,tdlev)
	go to 25150
c-----------------------------------------------------------------------
2989	call beta(0.684,52.,0.,0.,t)
29890	thlev=0.
	p=100.*rnd1(d)
	if(p.le.57.18) go to 29891
	if(p.le.90.43) go to 29892
	               go to 29893
29891	call nucltransK(1.213,0.032,1.0e-3,0.1e-4,tclev,thlev,tdlev)
	go to 17760
29892	call nucltransK(0.224,0.032,9.0e-2,0.,tclev,thlev,tdlev)
	go to 27660
29893	call nucltransK(0.149,0.032,4.0e-1,0.,tclev,thlev,tdlev)
	go to 28400
c-----------------------------------------------------------------------
2974	call beta(0.699,52.,0.,0.,t)
29740	thlev=0.
	call nucltransK(0.209,0.032,1.3e-1,0.,tclev,thlev,tdlev)
	go to 27660
c-----------------------------------------------------------------------
2840	call beta(0.833,52.,0.,0.,t)
28400	thlev=0.
	p=100.*rnd1(d)
	if(p.le.23.73) go to 28401
	               go to 28402
28401	call nucltransK(1.477,0.032,4.0e-4,0.3e-4,tclev,thlev,tdlev)
	go to 13620
28402	call nucltransK(1.064,0.032,1.2e-3,0.,tclev,thlev,tdlev)
	go to 17760
c-----------------------------------------------------------------------
2812	call beta(0.861,52.,0.,0.,t)
28120	thlev=0.
	p=100.*rnd1(d)
	if(p.le.83.33) go to 28121
	if(p.le.94.44) go to 28122
	               go to 28123
28121	call nucltransK(0.593,0.032,5.0e-3,0.,tclev,thlev,tdlev)
	go to 22180
28122	call nucltransK(0.415,0.032,1.2e-2,0.,tclev,thlev,tdlev)
	go to 23960
28123	call nucltransK(0.297,0.032,3.0e-2,0.,tclev,thlev,tdlev)
	go to 25150
c-----------------------------------------------------------------------
2766	call beta(0.907,52.,0.,0.,t)
27660	thlev=0.
	call nucltransK(0.990,0.032,1.5e-3,0.,tclev,thlev,tdlev)
	go to 17760
c-----------------------------------------------------------------------
25150	thlev=0.
	call nucltransK(0.297,0.032,4.0e-2,0.,tclev,thlev,tdlev)
	go to 22180
c-----------------------------------------------------------------------
2497	call beta(1.176,52.,0.,0.,t)
24970	thlev=0.152e-9
	p=100.*rnd1(d)
	if(p.le.95.56) go to 24971
	               go to 24972
24971	call nucltransK(0.721,0.032,1.0e-2,0.,tclev,thlev,tdlev)
	go to 17760
24972	call nucltransK(0.278,0.032,4.9e-2,0.,tclev,thlev,tdlev)
	go to 22180
c-----------------------------------------------------------------------
2396	call beta(1.277,52.,0.,0.,t)
23960	thlev=0.
	p=100.*rnd1(d)
	if(p.le.52.63) go to 23961
	               go to 23962
23961	call nucltransK(1.036,0.032,1.3e-3,0.,tclev,thlev,tdlev)
	go to 13620
23962	call nucltransK(0.620,0.032,5.0e-3,0.,tclev,thlev,tdlev)
	go to 17760
c-----------------------------------------------------------------------
2218	call beta(1.455,52.,0.,0.,t)
22180	thlev=0.
	call nucltransK(0.857,0.032,8.4e-4,0.,tclev,thlev,tdlev)
	go to 13620
c-----------------------------------------------------------------------
1776	call beta(1.897,52.,0.,0.,t)
17760	thlev=68.e-12
	call nucltransK(0.415,0.032,1.4e-2,0.,tclev,thlev,tdlev)
	go to 13620
c-----------------------------------------------------------------------
13620	thlev=0.
	call nucltransK(0.695,0.032,3.4e-3,0.,tclev,thlev,tdlev)
	go to 66700
c-----------------------------------------------------------------------
66700	thlev=0.
	call nucltransK(0.667,0.032,3.8e-3,0.,tclev,thlev,tdlev)
	return
	end
c
c end of Sb126.f
c
c Local Variables: --
c mode: fortran --
c End: --
