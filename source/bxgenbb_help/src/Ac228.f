c Ac228.f 
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
	subroutine Ac228(tcnuc,tdnuc)
c Scheme of 228Ac decay ("Table of Isotopes", 7th ed., 1978).
c Two-, three- and four-figured labels correspond to energies of
c 228Th excited levels in keV.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 8.08.1992, 22.10.1995.
	thnuc=22068.
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le. 0.20) go to  2027   !  0.20%
	if(pbeta.le. 0.40) go to  2010   !  0.20%
	if(pbeta.le. 0.44) go to  1965   !  0.04%
	if(pbeta.le. 0.67) go to  1945   !  0.23%
	if(pbeta.le. 0.77) go to  1900   !  0.10%
	if(pbeta.le. 0.91) go to  1893   !  0.14%
	if(pbeta.le. 1.02) go to  1760   !  0.11%
	if(pbeta.le. 1.31) go to  1744   !  0.29%
	if(pbeta.le. 2.81) go to  1724   !  1.50%
	if(pbeta.le. 4.91) go to  1689   !  2.10%
	if(pbeta.le. 6.41) go to  1683   !  1.50%
	if(pbeta.le.11.11) go to  1646   !  4.70%
	if(pbeta.le.11.91) go to  1643   !  0.80%
	if(pbeta.le.13.11) go to  1638   !  1.20%
	if(pbeta.le.13.18) go to  1547   !  0.07%
	if(pbeta.le.13.38) go to  1539   !  0.20%
	if(pbeta.le.21.38) go to  1532   !  8.00%
	if(pbeta.le.21.58) go to  1489   !  0.20%
	if(pbeta.le.23.48) go to  1450   !  1.90%
	if(pbeta.le.24.98) go to  1432   !  1.50%
	if(pbeta.le.25.18) go to  1344   !  0.20%
	if(pbeta.le.25.98) go to  1227   !  0.80%
	if(pbeta.le.29.58) go to  1169   !  3.60%
	if(pbeta.le.34.58) go to  1154   !  5.00%
	if(pbeta.le.39.98) go to  1123   !  5.40%
	if(pbeta.le.40.08) go to  1060   !  0.10%
	if(pbeta.le.42.08) go to  1022   !  2.00%
	if(pbeta.le.42.28) go to  1016   !  0.20%
	if(pbeta.le.42.38) go to   979   !  0.10%
	if(pbeta.le.74.33) go to   969   ! 31.95%
	if(pbeta.le.74.63) go to   968   !  0.30%
	if(pbeta.le.74.86) go to   944   !  0.23%
	if(pbeta.le.75.00) go to   875   !  0.14%
	if(pbeta.le.75.20) go to   519   !  0.20%
	if(pbeta.le.88.20) go to   396   ! 13.00%
	if(pbeta.le.89.00) go to   187   !  0.80%
	                   go to    58   ! 11.00%
c-----------------------------------------------------------------------
2027	call beta(0.110,90.,0.,0.,t)
20270	thlev=0.
	p=100.*rnd1(d)
	if(p.le.31.) go to 20271   ! 31%
	             go to 20272   ! 69%
20271	call nucltransK(1.509,0.110,3.2e-3,0.,tclev,thlev,tdlev)
	go to 96900
20272	call nucltransK(1.004,0.110,3.0e-3,0.,tclev,thlev,tdlev)
	go to 10220
c-----------------------------------------------------------------------
2010	call beta(0.127,90.,0.,0.,t)
20100	thlev=0.
	p=100.*rnd1(d)
	if(p.le.42.) go to 20101   ! 42%
	if(p.le.72.) go to 20102   ! 30%
	             go to 20103   ! 28%
20101	call nucltransK(1.952,0.110,2.8e-3,1.6e-4,tclev,thlev,tdlev)
	go to 58000
20102	call nucltransK(1.823,0.110,7.0e-3,1.7e-4,tclev,thlev,tdlev)
	go to 18700
20103	call nucltransK(0.920,0.110,0.7e+0,0.,tclev,thlev,tdlev)
	go to 10910
c-----------------------------------------------------------------------
1965	call beta(0.172,90.,0.,0.,t)
19650	thlev=0.
	p=100.*rnd1(d)
	if(p.le.33.) go to 19651   ! 33%
	             go to 19652   ! 67%
19651	call nucltransK(1.966,0.110,1.2e-3,2.9e-4,tclev,thlev,tdlev)
	go to 10000
19652	call nucltransK(1.907,0.110,1.8e-3,2.7e-4,tclev,thlev,tdlev)
	go to 58000
c-----------------------------------------------------------------------
1945	call beta(0.192,90.,0.,0.,t)
19450	thlev=0.
	p=100.*rnd1(d)
	if(p.le.31.4) go to 19451   ! 31.4%
	if(p.le.42.4) go to 19452   ! 11.0%
	if(p.le.44.3) go to 19453   !  1.9%
	if(p.le.75.7) go to 19454   ! 31.4%
	if(p.le.87.8) go to 19455   ! 12.1%
	if(p.le.89.0) go to 19456   !  1.2%
	if(p.le.94.0) go to 19457   !  5.0%
	if(p.le.96.0) go to 19458   !  2.0%
	              go to 19459   !  4.0%
19451	call nucltransK(1.887,0.110,3.0e-3,1.3e-4,tclev,thlev,tdlev)
	go to 58000
19452	call nucltransK(1.758,0.110,1.0e-2,2.0e-4,tclev,thlev,tdlev)
	go to 18700
19453	call nucltransK(1.549,0.110,1.7e-3,0.6e-4,tclev,thlev,tdlev)
	go to 39600
19454	call nucltransK(0.975,0.110,4.0e-2,0.,tclev,thlev,tdlev)
	go to 96900
19455	call nucltransK(0.922,0.110,4.5e-2,0.,tclev,thlev,tdlev)
	go to 10220
19456	call nucltransK(0.853,0.110,5.5e-2,0.,tclev,thlev,tdlev)
	go to 10910
19457	call nucltransK(0.791,0.110,6.5e-2,0.,tclev,thlev,tdlev)
	go to 11540
19458	call nucltransK(0.745,0.110,7.5e-2,0.,tclev,thlev,tdlev)
	go to 12000
19459	call nucltransK(0.220,0.110,2.1e+0,0.,tclev,thlev,tdlev)
	go to 17240
c-----------------------------------------------------------------------
1900	call beta(0.237,90.,0.,0.,t)
19000	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 1.7) go to 19001   !  1.7%
	if(p.le.20.7) go to 19002   ! 19.0%
	if(p.le.21.7) go to 19003   !  1.0%
	if(p.le.31.7) go to 19004   ! 10.0%
	if(p.le.67.9) go to 19005   ! 36.2%
	              go to 19006   ! 32.1%
19001	call nucltransK(1.900,0.110,3.0e-3,1.3e-4,tclev,thlev,tdlev)
	go to 10000
19002	call nucltransK(1.842,0.110,8.0e-3,2.0e-4,tclev,thlev,tdlev)
	go to 58000
19003	call nucltransK(1.712,0.110,3.5e-3,0.8e-4,tclev,thlev,tdlev)
	go to 18700
19004	call nucltransK(1.504,0.110,1.7e-3,0.5e-4,tclev,thlev,tdlev)
	go to 39600
19005	call nucltransK(0.884,0.110,4.3e-3,0.,tclev,thlev,tdlev)
	go to 10160
19006	call nucltransK(0.449,0.110,1.6e-2,0.,tclev,thlev,tdlev)
	go to 14500
c-----------------------------------------------------------------------
1893	call beta(0.244,90.,0.,0.,t)
18930	thlev=0.
	p=100.*rnd1(d)
	if(p.le.17.0) go to 18931   ! 17.0%
	if(p.le.22.6) go to 18932   !  5.6%
	if(p.le.38.6) go to 18933   ! 16.0%
	if(p.le.47.6) go to 18934   !  9.0%
	if(p.le.74.8) go to 18935   ! 27.2%
	if(p.le.76.8) go to 18936   !  2.0%
	if(p.le.77.8) go to 18937   !  1.0%
                      go to 18938   ! 22.2%
18931	call nucltransK(1.835,0.110,6.0e-3,2.0e-4,tclev,thlev,tdlev)
	go to 58000
18932	call nucltransK(1.706,0.110,1.0e-2,1.0e-4,tclev,thlev,tdlev)
	go to 18700
18933	call nucltransK(0.940,0.110,1.0e-1,0.,tclev,thlev,tdlev)
	go to 95200
18934	call nucltransK(0.924,0.110,4.5e-2,0.,tclev,thlev,tdlev)
	go to 96900
18935	call nucltransK(0.870,0.110,5.2e-2,0.,tclev,thlev,tdlev)
	go to 10220
18936	call nucltransK(0.739,0.110,3.3e-1,0.,tclev,thlev,tdlev)
	go to 11540
18937	call nucltransK(0.693,0.110,1.2e-1,0.,tclev,thlev,tdlev)
	go to 12000
18938	call nucltransK(0.461,0.110,2.8e-1,0.,tclev,thlev,tdlev)
	go to 14320
c-----------------------------------------------------------------------
1760	call beta(0.377,90.,0.,0.,t)
17600	thlev=0.
	p=100.*rnd1(d)
	if(p.le.62.) go to 17601   ! 62%
	             go to 17602   ! 38%
17601	call nucltransK(1.702,0.110,1.4e-3,1.1e-4,tclev,thlev,tdlev)
	go to 58000
17602	call nucltransK(1.574,0.110,1.7e-3,0.7e-4,tclev,thlev,tdlev)
	go to 18700
c-----------------------------------------------------------------------
1744	call beta(0.393,90.,0.,0.,t)
17440	thlev=0.
	p=100.*rnd1(d)
	if(p.le.32.) go to 17441   ! 32%
	if(p.le.94.) go to 17442   ! 62%
	             go to 17443   !  6%
17441	call nucltransK(1.686,0.110,1.4e-3,1.6e-4,tclev,thlev,tdlev)
	go to 58000
17442	call nucltransK(1.557,0.110,1.7e-3,0.6e-4,tclev,thlev,tdlev)
	go to 18700
17443	call nucltransK(1.348,0.110,2.0e-3,0.2e-4,tclev,thlev,tdlev)
	go to 39600
c-----------------------------------------------------------------------
1724	call beta(0.413,90.,0.,0.,t)
17240	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 2.0) go to 17241   !  2.0%
	if(p.le.15.0) go to 17242   ! 13.0%
	if(p.le.15.8) go to 17243   !  0.8%
	if(p.le.89.0) go to 17244   ! 73.2%
                      go to 17245   ! 11.0%
17241	call nucltransK(1.724,0.110,3.7e-3,0.7e-4,tclev,thlev,tdlev)
	go to 10000
17242	call nucltransK(1.666,0.110,1.0e-2,1.2e-4,tclev,thlev,tdlev)
	go to 58000
17243	call nucltransK(1.537,0.110,4.0e-3,0.4e-4,tclev,thlev,tdlev)
	go to 18700
17244	call nucltransK(0.755,0.110,6.9e-2,0.,tclev,thlev,tdlev)
	go to 96900
17245	call nucltransK(0.702,0.110,9.5e-2,0.,tclev,thlev,tdlev)
	go to 10220
c-----------------------------------------------------------------------
1689	call beta(0.448,90.,0.,0.,t)
16890	thlev=0.
	p=100.*rnd1(d)
	if(p.le.72.3) go to 16891   ! 72.3%
	if(p.le.98.0) go to 16892   ! 25.7%
	              go to 16893   !  2.0%
16891	call nucltransK(1.631,0.110,7.4e-3,1.2e-4,tclev,thlev,tdlev)
	go to 58000
16892	call nucltransK(1.502,0.110,1.7e-3,0.5e-4,tclev,thlev,tdlev)
	go to 18700
16893	call nucltransK(0.666,0.110,7.5e-3,0.,tclev,thlev,tdlev)
	go to 10220
c-----------------------------------------------------------------------
1683	call beta(0.454,90.,0.,0.,t)
16830	thlev=0.
	p=100.*rnd1(d)
	if(p.le.22.) go to 16831   ! 22%
	if(p.le.90.) go to 16832   ! 68%
	if(p.le.96.) go to 16833   !  6%
	             go to 16834   !  4%
16831	call nucltransK(1.625,0.110,1.5e-3,0.8e-4,tclev,thlev,tdlev)
	go to 58000
16832	call nucltransK(1.496,0.110,1.7e-3,0.5e-4,tclev,thlev,tdlev)
	go to 18700
16833	call nucltransK(1.287,0.110,2.2e-3,0.2e-4,tclev,thlev,tdlev)
	go to 39600
16834	call nucltransK(1.165,0.110,2.6e-3,0.1e-4,tclev,thlev,tdlev)
	go to 51900
c-----------------------------------------------------------------------
1646	call beta(0.491,90.,0.,0.,t)
16460	thlev=0.
	p=100.*rnd1(d)
	if(p.le.75.0) go to 16461   ! 75.0%
	if(p.le.95.0) go to 16462   ! 20.0%
	if(p.le.96.0) go to 16463   !  1.0%
	if(p.le.96.4) go to 16464   !  0.4%
	if(p.le.97.4) go to 16465   !  1.0%
	if(p.le.99.5) go to 16466   !  2.1%
	              go to 16467   !  0.5%
16461	call nucltransK(1.588,0.110,4.7e-3,0.5e-4,tclev,thlev,tdlev)
	go to 58000
16462	call nucltransK(1.459,0.110,5.0e-3,0.3e-4,tclev,thlev,tdlev)
	go to 18700
16463	call nucltransK(0.677,0.110,2.2e-2,0.,tclev,thlev,tdlev)
	go to 96900
16464	call nucltransK(0.624,0.110,8.0e-3,0.,tclev,thlev,tdlev)
	go to 10220
16465	call nucltransK(0.555,0.110,1.6e-1,0.,tclev,thlev,tdlev)
	go to 10910
16466	call nucltransK(0.523,0.110,1.2e-2,0.,tclev,thlev,tdlev)
	go to 11230
16467	call nucltransK(0.420,0.110,1.8e-2,0.,tclev,thlev,tdlev)
	go to 12270
c-----------------------------------------------------------------------
1643	call beta(0.494,90.,0.,0.,t)
16430	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 1.0) go to 16431   !  1.0%
	if(p.le.51.7) go to 16432   ! 50.7%
	if(p.le.61.7) go to 16433   ! 10.0%
	if(p.le.70.7) go to 16434   !  9.0%
	              go to 16435   ! 29.3%
16431	call nucltransK(1.315,0.110,1.8e-2,0.6e-4,tclev,thlev,tdlev)
	go to 32800
16432	call nucltransK(1.247,0.110,2.1e-2,0.4e-4,tclev,thlev,tdlev)
	go to 39600
16433	call nucltransK(0.675,0.110,7.0e-3,0.,tclev,thlev,tdlev)
	go to 96900
16434	call nucltransK(0.620,0.110,8.0e-3,0.,tclev,thlev,tdlev)
	go to 10220
16435	call nucltransK(0.210,0.110,7.9e-2,0.,tclev,thlev,tdlev)
	go to 14320
c-----------------------------------------------------------------------
1638	call beta(0.499,90.,0.,0.,t)
16380	thlev=0.
	p=100.*rnd1(d)
	if(p.le.38.) go to 16381   ! 38%
	if(p.le.96.) go to 16382   ! 58%
	             go to 16383   !  4%
16381	call nucltransK(1.638,0.110,4.0e-3,0.5e-4,tclev,thlev,tdlev)
	go to 10000
16382	call nucltransK(1.581,0.110,1.1e-2,1.1e-4,tclev,thlev,tdlev)
	go to 58000
16383	call nucltransK(0.516,0.110,1.2e-2,0.,tclev,thlev,tdlev)
	go to 11230
c-----------------------------------------------------------------------
1547	call beta(0.590,90.,0.,0.,t)
15470	thlev=0.
	p=100.*rnd1(d)
	if(p.le.50.) go to 15471   ! 50%
	if(p.le.86.) go to 15472   ! 36%
	             go to 15473   ! 14%
15471	call nucltransK(1.169,0.110,2.7e-3,0.2e-4,tclev,thlev,tdlev)
	go to 37800
15472	call nucltransK(0.378,0.110,2.2e-2,0.,tclev,thlev,tdlev)
	go to 11690
15473	call nucltransK(0.373,0.110,2.2e-2,0.,tclev,thlev,tdlev)
	go to 11750
c-----------------------------------------------------------------------
1539	call beta(0.598,90.,0.,0.,t)
15390	thlev=0.
	p=100.*rnd1(d)
	if(p.le.12.) go to 15391   ! 12%
	if(p.le.18.) go to 15392   !  6%
	if(p.le.32.) go to 15393   ! 14%
	             go to 15394   ! 68%
15391	call nucltransK(1.480,0.110,1.8e-3,0.5e-4,tclev,thlev,tdlev)
	go to 58000
15392	call nucltransK(1.143,0.110,2.6e-2,0.3e-4,tclev,thlev,tdlev)
	go to 39600
15393	call nucltransK(1.020,0.110,1.0e-2,0.,tclev,thlev,tdlev)
	go to 51900
15394	call nucltransK(0.571,0.110,1.5e-1,0.,tclev,thlev,tdlev)
	go to 96800
c-----------------------------------------------------------------------
1532	call beta(0.605,90.,0.,0.,t)
15320	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.4) go to 15321   !  0.4%
	if(p.le.30.7) go to 15322   ! 30.3%
	if(p.le.46.7) go to 15323   ! 16.0%
	if(p.le.51.3) go to 15324   !  4.6%
	if(p.le.52.3) go to 15325   !  1.0%
	if(p.le.53.0) go to 15326   !  0.7%
	              go to 15327   ! 47.0%
15321	call nucltransK(1.136,0.110,2.8e-3,0.2e-4,tclev,thlev,tdlev)
	go to 39600
15322	call nucltransK(0.563,0.110,5.0e-2,0.,tclev,thlev,tdlev)
	go to 96900
15323	call nucltransK(0.509,0.110,6.0e-2,0.,tclev,thlev,tdlev)
	go to 10220
15324	call nucltransK(0.441,0.110,3.0e-1,0.,tclev,thlev,tdlev)
	go to 10910
15325	call nucltransK(0.378,0.110,4.5e-1,0.,tclev,thlev,tdlev)
	go to 11540
15326	call nucltransK(0.357,0.110,1.7e+0,0.,tclev,thlev,tdlev)
	go to 11750
15327	call nucltransK(0.100,0.020,4.0e+0,0.,tclev,thlev,tdlev)
	go to 14320
c-----------------------------------------------------------------------
1489	call beta(0.648,90.,0.,0.,t)
14890	thlev=0.
	p=100.*rnd1(d)
	if(p.le.22.) go to 14891   ! 22%
	             go to 14892   ! 78%
14891	call nucltransK(0.399,0.110,2.0e-2,0.,tclev,thlev,tdlev)
	go to 10910
14892	call nucltransK(0.314,0.110,0.6e+0,0.,tclev,thlev,tdlev)
	go to 11750
c-----------------------------------------------------------------------
1450	call beta(0.687,90.,0.,0.,t)
14500	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 3.0) go to 14501   !  3.0%
	if(p.le.15.0) go to 14502   ! 12.0%
	if(p.le.16.5) go to 14503   !  1.5%
	if(p.le.58.0) go to 14504   ! 41.5%
	if(p.le.82.0) go to 14505   ! 24.0%
	              go to 14506   ! 18.0%
14501	call nucltransK(1.054,0.110,3.2e-2,0.,tclev,thlev,tdlev)
	go to 39600
14502	call nucltransK(0.498,0.110,4.2e-2,0.,tclev,thlev,tdlev)
	go to 95200
14503	call nucltransK(0.481,0.110,2.5e-1,0.,tclev,thlev,tdlev)
	go to 96800
14504	call nucltransK(0.328,0.110,4.4e-2,0.,tclev,thlev,tdlev)
	go to 11230
14505	call nucltransK(0.282,0.110,1.3e+0,0.,tclev,thlev,tdlev)
	go to 11690
14506	call nucltransK(0.224,0.110,1.5e+0,0.,tclev,thlev,tdlev)
	go to 12270
c-----------------------------------------------------------------------
1432	call beta(0.705,90.,0.,0.,t)
14320	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 1.0) go to 14321   !  1.0%
	if(p.le. 3.0) go to 14322   !  2.0%
	if(p.le.64.8) go to 14323   ! 61.8%
	if(p.le.92.2) go to 14324   ! 27.4%
	if(p.le.98.2) go to 14325   !  6.0%
	if(p.le.99.0) go to 14326   !  0.8%
	if(p.le.99.6) go to 14327   !  0.6%
	              go to 14328   !  0.4%
14321	call nucltransK(1.374,0.110,1.4e-2,0.7e-4,tclev,thlev,tdlev)
	go to 58000
14322	call nucltransK(1.245,0.110,2.0e-2,0.4e-4,tclev,thlev,tdlev)
	go to 18700
14323	call nucltransK(0.463,0.110,4.7e-2,0.,tclev,thlev,tdlev)
	go to 96900
14324	call nucltransK(0.410,0.110,8.3e-2,0.,tclev,thlev,tdlev)
	go to 10220
14325	call nucltransK(0.341,0.110,1.2e-1,0.,tclev,thlev,tdlev)
	go to 10910
14326	call nucltransK(0.308,0.110,3.5e-2,0.,tclev,thlev,tdlev)
	go to 11230
14327	call nucltransK(0.264,0.110,5.0e-2,0.,tclev,thlev,tdlev)
	go to 11690
14328	call nucltransK(0.258,0.110,5.0e-2,0.,tclev,thlev,tdlev)
	go to 11750
c-----------------------------------------------------------------------
1344	call beta(0.793,90.,0.,0.,t)
13440	thlev=0.
	p=100.*rnd1(d)
	if(p.le.27.0) go to 13441   ! 27.0%
	if(p.le.64.5) go to 13442   ! 37.5%
	if(p.le.95.0) go to 13443   ! 30.5%
	              go to 13444   !  5.0%
13441	call nucltransK(1.017,0.110,3.5e-3,0.,tclev,thlev,tdlev)
	go to 32800
13442	call nucltransK(0.948,0.110,3.7e-3,0.,tclev,thlev,tdlev)
	go to 39600
13443	call nucltransK(0.825,0.110,5.0e-3,0.,tclev,thlev,tdlev)
	go to 51900
13444	call nucltransK(0.169,0.110,1.4e-1,0.,tclev,thlev,tdlev)
	go to 11750
c-----------------------------------------------------------------------
1227	call beta(0.910,90.,0.,0.,t)
12270	thlev=0.
	p=100.*rnd1(d)
	if(p.le.10.) go to 12271   ! 10%
	if(p.le.68.) go to 12272   ! 58%
	if(p.le.79.) go to 12273   ! 11%
	if(p.le.97.) go to 12274   ! 18%
	             go to 12275   !  3%
12271	call nucltransK(1.040,0.110,3.5e-3,0.,tclev,thlev,tdlev)
	go to 18700
12272	call nucltransK(0.830,0.110,1.8e-2,0.,tclev,thlev,tdlev)
	go to 39600
12273	call nucltransK(0.707,0.110,1.0e-1,0.,tclev,thlev,tdlev)
	go to 51900
12274	call nucltransK(0.204,0.110,9.0e-2,0.,tclev,thlev,tdlev)
	go to 10220
12275	call nucltransK(0.136,0.110,1.7e+0,0.,tclev,thlev,tdlev)
	go to 10910
c-----------------------------------------------------------------------
12000	thlev=0.
	call nucltransK(0.178,0.110,6.0e+1,0.,tclev,thlev,tdlev)
	go to 10220
c-----------------------------------------------------------------------
11750	thlev=0.
	p=100.*rnd1(d)
	if(p.le.67.) go to 11751   ! 67%
	             go to 11752   ! 33%
11751	call nucltransK(0.988,0.110,3.5e-3,0.,tclev,thlev,tdlev)
	go to 18700
11752	call nucltransK(0.796,0.110,5.2e-3,0.,tclev,thlev,tdlev)
	go to 37800
c-----------------------------------------------------------------------
1169	call beta(0.968,90.,0.,0.,t)
11690	thlev=0.
	p=100.*rnd1(d)
	if(p.le.11.0) go to 11691   ! 11.0%
	if(p.le.38.8) go to 11692   ! 27.8%
	if(p.le.84.5) go to 11693   ! 45.7%
	if(p.le.84.8) go to 11694   !  0.3%
	if(p.le.93.8) go to 11695   !  9.0%
	if(p.le.98.9) go to 11696   !  5.1%
	              go to 11697   !  1.1%
11691	call nucltransK(1.110,0.110,2.9e-3,0.1e-4,tclev,thlev,tdlev)
	go to 58000
11692	call nucltransK(0.840,0.110,1.4e-2,0.,tclev,thlev,tdlev)
	go to 32800
11693	call nucltransK(0.772,0.110,1.5e-2,0.,tclev,thlev,tdlev)
	go to 39600
11694	call nucltransK(0.649,0.110,2.4e-2,0.,tclev,thlev,tdlev)
	go to 51900
11695	call nucltransK(0.200,0.110,9.5e-2,0.,tclev,thlev,tdlev)
	go to 96900
11696	call nucltransK(0.146,0.110,1.2e+0,0.,tclev,thlev,tdlev)
	go to 10220
11697	call nucltransK(0.078,0.020,2.2e-1,0.,tclev,thlev,tdlev)
	go to 10910
c-----------------------------------------------------------------------
1154	call beta(0.983,90.,0.,0.,t)
11540	thlev=0.
	p=100.*rnd1(d)
	if(p.le.14.9) go to 11541   ! 14.9%
	if(p.le.25.8) go to 11542   ! 10.9%
	if(p.le.40.7) go to 11543   ! 14.9%
	if(p.le.61.4) go to 11544   ! 20.7%
	if(p.le.84.1) go to 11545   ! 22.7%
	if(p.le.93.0) go to 11546   !  8.9%
	if(p.le.97.0) go to 11547   !  4.0%
	              go to 11548   !  3.0%
11541	call nucltransK(1.154,0.110,7.5e-3,0.1e-4,tclev,thlev,tdlev)
	go to 10000
11542	call nucltransK(1.096,0.110,2.8e-2,0.1e-4,tclev,thlev,tdlev)
	go to 58000
11543	call nucltransK(0.967,0.110,2.0e-2,0.,tclev,thlev,tdlev)
	go to 18700
11544	call nucltransK(0.322,0.110,5.2e-1,0.,tclev,thlev,tdlev)
	go to 83200
11545	call nucltransK(0.279,0.110,1.3e+0,0.,tclev,thlev,tdlev)
	go to 87500
11546	call nucltransK(0.185,0.110,5.4e+1,0.,tclev,thlev,tdlev)
	go to 96900
11547	call nucltransK(0.174,0.110,1.4e+0,0.,tclev,thlev,tdlev)
	go to 97900
11548	call nucltransK(0.138,0.110,4.9e+0,0.,tclev,thlev,tdlev)
	go to 10160
c-----------------------------------------------------------------------
1123	call beta(1.014,90.,0.,0.,t)
11230	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 1.4) go to 11231   !  1.4%
	if(p.le. 5.4) go to 11232   !  4.0%
	if(p.le. 5.6) go to 11233   !  0.2%
	if(p.le.73.8) go to 11234   ! 68.2%
	if(p.le.85.8) go to 11235   ! 12.0%
	if(p.le.98.1) go to 11236   ! 12.3%
	              go to 11237   !  1.9%
11231	call nucltransK(1.123,0.110,6.5e-2,0.1e-4,tclev,thlev,tdlev)
	go to 10000
11232	call nucltransK(1.065,0.110,3.2e-3,0.,tclev,thlev,tdlev)
	go to 58000
11233	call nucltransK(0.936,0.110,0.1e+0,0.,tclev,thlev,tdlev)
	go to 18700
11234	call nucltransK(0.795,0.110,1.9e-2,0.,tclev,thlev,tdlev)
	go to 32800
11235	call nucltransK(0.727,0.110,1.0e-2,0.,tclev,thlev,tdlev)
	go to 39600
11236	call nucltransK(0.154,0.110,0.2e+0,0.,tclev,thlev,tdlev)
	go to 96900
11237	call nucltransK(0.100,0.020,3.3e-1,0.,tclev,thlev,tdlev)
	go to 10220
c-----------------------------------------------------------------------
10910	thlev=0.
	p=100.*rnd1(d)
	if(p.le.21.) go to 10911   ! 21%
	             go to 10912   ! 79%
10911	call nucltransK(1.033,0.110,9.5e-3,0.,tclev,thlev,tdlev)
	go to 58000
10912	call nucltransK(0.904,0.110,2.6e-2,0.,tclev,thlev,tdlev)
	go to 18700
c-----------------------------------------------------------------------
1060	call beta(1.077,90.,0.,0.,t)
10600	thlev=0.
	p=100.*rnd1(d)
	if(p.le.74.) go to 10601   ! 74%
	if(p.le.87.) go to 10602   ! 13%
	             go to 10603   ! 13%
10601	call nucltransK(1.002,0.110,3.5e-3,0.,tclev,thlev,tdlev)
	go to 58000
10602	call nucltransK(0.664,0.110,7.0e-3,0.,tclev,thlev,tdlev)
	go to 39600
10603	call nucltransK(0.541,0.110,1.1e-2,0.,tclev,thlev,tdlev)
	go to 51900
c-----------------------------------------------------------------------
1022	call beta(1.115,90.,0.,0.,t)
10220	thlev=0.
	p=100.*rnd1(d)
	if(p.le.73.) go to 10221   ! 73%
	             go to 10222   ! 27%
10221	call nucltransK(0.964,0.110,9.2e-3,0.,tclev,thlev,tdlev)
	go to 58000
10222	call nucltransK(0.836,0.110,1.4e-2,0.,tclev,thlev,tdlev)
	go to 18700
c-----------------------------------------------------------------------
1016	call beta(1.121,90.,0.,0.,t)
10160	thlev=0.
	p=100.*rnd1(d)
	if(p.le.19.) go to 10161   ! 19%
	if(p.le.66.) go to 10162   ! 47%
	if(p.le.82.) go to 10163   ! 16%
	             go to 10164   ! 18%
10161	call nucltransK(1.017,0.110,2.4e-2,0.,tclev,thlev,tdlev)
	go to 10000
10162	call nucltransK(0.958,0.110,3.8e-3,0.,tclev,thlev,tdlev)
	go to 58000
10163	call nucltransK(0.688,0.110,2.1e-2,0.,tclev,thlev,tdlev)
	go to 32800
10164	call nucltransK(0.620,0.110,1.4e-1,0.,tclev,thlev,tdlev)
	go to 39600
c-----------------------------------------------------------------------
979	call beta(1.158,90.,0.,0.,t)
97900	thlev=0.
	p=100.*rnd1(d)
	if(p.le.35.) go to 97901   ! 35%
	             go to 97902   ! 65%
97901	call nucltransK(0.651,0.110,7.5e-3,0.,tclev,thlev,tdlev)
	go to 32800
97902	call nucltransK(0.583,0.110,9.5e-3,0.,tclev,thlev,tdlev)
	go to 39600
c-----------------------------------------------------------------------
969	call beta(1.168,90.,0.,0.,t)
96900	thlev=0.
	p=100.*rnd1(d)
	if(p.le.37.0) go to 96901   ! 37.0%
	if(p.le.98.8) go to 96902   ! 61.8%
	              go to 96903   !  1.2%
96901	call nucltransK(0.969,0.110,1.0e-2,0.,tclev,thlev,tdlev)
	go to 10000
96902	call nucltransK(0.911,0.110,1.2e-2,0.,tclev,thlev,tdlev)
	go to 58000
96903	call nucltransK(0.782,0.110,6.8e-2,0.,tclev,thlev,tdlev)
	go to 18700
c-----------------------------------------------------------------------
968	call beta(1.169,90.,0.,0.,t)
96800	thlev=0.
	p=100.*rnd1(d)
	if(p.le.22.) go to 96801   ! 22%
	             go to 96802   ! 78%
96801	call nucltransK(0.641,0.110,1.1e-1,0.,tclev,thlev,tdlev)
	go to 32800
96802	call nucltransK(0.572,0.110,1.3e-1,0.,tclev,thlev,tdlev)
	go to 39600
c-----------------------------------------------------------------------
95200	thlev=0.
	p=100.*rnd1(d)
	if(p.le.91.) go to 95201   ! 91%
	if(p.le.94.) go to 95202   !  3%
	             go to 95203   !  6%
95201	call nucltransK(0.894,0.110,4.2e-3,0.,tclev,thlev,tdlev)
	go to 58000
95202	call nucltransK(0.624,0.110,1.2e-1,0.,tclev,thlev,tdlev)
	go to 32800
95203	call nucltransK(0.556,0.110,3.5e-2,0.,tclev,thlev,tdlev)
	go to 39600
c-----------------------------------------------------------------------
944	call beta(1.193,90.,0.,0.,t)
94400	thlev=0.
	p=100.*rnd1(d)
	if(p.le.42.5) go to 94401   ! 42.5%
	if(p.le.57.5) go to 94402   ! 15.0%
	              go to 94403   ! 42.5%
94401	call nucltransK(0.944,0.110,1.1e-2,0.,tclev,thlev,tdlev)
	go to 10000
94402	call nucltransK(0.888,0.110,7.5e-1,0.,tclev,thlev,tdlev)
	go to 58000
94403	call nucltransK(0.616,0.110,8.5e-3,0.,tclev,thlev,tdlev)
	go to 32800
c-----------------------------------------------------------------------
875	call beta(1.262,90.,0.,0.,t)
87500	thlev=0.
	p=100.*rnd1(d)
	if(p.le.15.) go to 87501   ! 15%
	if(p.le.26.) go to 87502   ! 11%
	if(p.le.60.) go to 87503   ! 34%
	             go to 87504   ! 40%
87501	call nucltransK(0.874,0.110,1.3e-2,0.,tclev,thlev,tdlev)
	go to 10000
87502	call nucltransK(0.816,0.110,0.5e+0,0.,tclev,thlev,tdlev)
	go to 58000
87503	call nucltransK(0.546,0.110,1.1e-2,0.,tclev,thlev,tdlev)
	go to 32800
87504	call nucltransK(0.479,0.110,1.4e-2,0.,tclev,thlev,tdlev)
	go to 39600
c-----------------------------------------------------------------------
83200	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 4.) go to 83201   !  4%
	if(p.le.27.) go to 83202   ! 23%
	             go to 83203   ! 73%
83201	call electron(0.721,tclev,thlev,tdlev)      ! only ec
c	call gamma(0.110,tdlev,0.,tdlev)            ! for real time
	call gamma(0.110,0.,0.,tdlev)               ! for time shift
	go to 10000
83202	call nucltransK(0.771,0.110,1.7e-2,0.,tclev,thlev,tdlev)
	go to 58000
83203	call nucltransK(0.504,0.110,1.2e-2,0.,tclev,thlev,tdlev)
	go to 32800
c-----------------------------------------------------------------------
519	call beta(1.618,90.,0.,0.,t)
51900	thlev=0.
	p=100.*rnd1(d)
	if(p.le.90.) go to 51901   ! 90%
	             go to 51902   ! 10%
51901	call nucltransK(0.332,0.110,4.7e-1,0.,tclev,thlev,tdlev)
	go to 18700
51902	call nucltransK(0.141,0.110,0.9e+0,0.,tclev,thlev,tdlev)
	go to 37800
c-----------------------------------------------------------------------
396	call beta(1.741,90.,0.,0.,t)
39600	thlev=0.
	p=100.*rnd1(d)
	if(p.le.73.) go to 39601   ! 73%
	             go to 39602   ! 27%
39601	call nucltransK(0.338,0.110,1.0e-2,0.,tclev,thlev,tdlev)
	go to 58000
39602	call nucltransK(0.209,0.110,7.9e-2,0.,tclev,thlev,tdlev)
	go to 18700
c-----------------------------------------------------------------------
37800	thlev=0.
	call nucltransK(0.191,0.110,4.2e-1,0.,tclev,thlev,tdlev)
	go to 18700
c-----------------------------------------------------------------------
32800	thlev=0.
	p=100.*rnd1(d)
	if(p.le.47.) go to 32801   ! 47%
	             go to 32802   ! 53%
32801	call nucltransK(0.328,0.110,4.4e-2,0.,tclev,thlev,tdlev)
	go to 10000
32802	call nucltransK(0.270,0.110,3.4e-2,0.,tclev,thlev,tdlev)
	go to 58000
c-----------------------------------------------------------------------
187	call beta(1.950,90.,0.,0.,t)
18700	thlev=0.16e-9
	call nucltransK(0.129,0.020,2.7e+0,0.,tclev,thlev,tdlev)
	go to 58000
c-----------------------------------------------------------------------
58	call beta(2.079,90.,0.,0.,t)
58000	thlev=0.40e-9
	call nucltransK(0.058,0.020,1.2e+2,0.,tclev,thlev,tdlev)
c-----------------------------------------------------------------------
10000	return
	end
c
c end of Ac228.f
c
c Local Variables: --
c mode: fortran --
c End: --
