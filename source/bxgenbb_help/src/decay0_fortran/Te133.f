c Te133.f 
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
        subroutine Te133(tcnuc,tdnuc)
c Model for scheme of Te133 decay (S.Rab, Nucl. Data Sheets 
c 75(1995)491).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 14.12.2003.
	thnuc=750.
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pbeta=100.*rnd1(d)
	if(pbeta.le. 0.13) go to 2936
	if(pbeta.le. 0.60) go to 2866
	if(pbeta.le. 0.86) go to 2825
	if(pbeta.le. 1.29) go to 2808
	if(pbeta.le. 2.34) go to 2768
	if(pbeta.le. 2.52) go to 2661
	if(pbeta.le. 2.70) go to 2597
	if(pbeta.le. 4.86) go to 2542
	if(pbeta.le. 5.24) go to 2526
	if(pbeta.le. 5.60) go to 2493
	if(pbeta.le. 6.20) go to 2467
	if(pbeta.le. 6.75) go to 2417
	if(pbeta.le. 6.86) go to 2393
	if(pbeta.le. 7.00) go to 2364
	if(pbeta.le. 7.26) go to 2284
	if(pbeta.le. 7.72) go to 2266
	if(pbeta.le. 8.96) go to 2255
	if(pbeta.le. 9.95) go to 2225
	if(pbeta.le.11.27) go to 2210
	if(pbeta.le.13.53) go to 2194
	if(pbeta.le.15.90) go to 2136
	if(pbeta.le.16.97) go to 2054
	if(pbeta.le.17.31) go to 2040
	if(pbeta.le.18.70) go to 2025
	if(pbeta.le.28.99) go to 1718
	if(pbeta.le.30.31) go to 1671
	if(pbeta.le.33.78) go to 1564
	if(pbeta.le.34.97) go to 1374
	if(pbeta.le.47.93) go to 1333
	if(pbeta.le.51.20) go to 1313
	if(pbeta.le.51.40) go to 1240
	if(pbeta.le.79.41) go to  720
	                   go to  312
c-----------------------------------------------------------------------
2936	call beta(0.001,53.,0.,0.,t)
29360	thlev=0.
	p=100.*rnd1(d)
	if(p.le.72.87) go to 29361 
	if(p.le.92.25) go to 29362 
	               go to 29363
29361	call nucltransK(2.624,0.033,1.5e-4,5.1e-4,tclev,thlev,tdlev)
	go to 31200
29362	call nucltransK(2.148,0.033,3.0e-4,2.9e-4,tclev,thlev,tdlev)
	go to 78700
29363	call nucltransK(1.372,0.033,1.0e-3,0.7e-4,tclev,thlev,tdlev)
	go to 15640
c-----------------------------------------------------------------------
2866	call beta(0.054,53.,0.,0.,t)
28660	thlev=0.
	p=100.*rnd1(d)
	if(p.le.74.63) go to 28661 
	if(p.le.94.67) go to 28662 
	               go to 28663
28661	call nucltransK(2.554,0.033,1.8e-4,4.9e-4,tclev,thlev,tdlev)
	go to 31200
28662	call nucltransK(2.079,0.033,3.2e-4,2.7e-4,tclev,thlev,tdlev)
	go to 78700
28663	call nucltransK(1.493,0.033,7.5e-4,0.9e-4,tclev,thlev,tdlev)
	go to 13740
c-----------------------------------------------------------------------
2825	call beta(0.095,53.,0.,0.,t)
28250	thlev=0.
	p=100.*rnd1(d)
	if(p.le.60.94) go to 28251 
	               go to 28252
28251	call nucltransK(2.825,0.033,1.3e-4,5.9e-4,tclev,thlev,tdlev)
	return
28252	call nucltransK(2.106,0.033,3.2e-4,2.7e-4,tclev,thlev,tdlev)
	go to 72000
c-----------------------------------------------------------------------
2808	call beta(0.112,53.,0.,0.,t)
28080	thlev=0.
	p=100.*rnd1(d)
	if(p.le.46.23) go to 28081 
	if(p.le.63.26) go to 28082 
	if(p.le.92.46) go to 28083 
	               go to 28084
28081	call nucltransK(2.496,0.033,2.0e-4,4.7e-4,tclev,thlev,tdlev)
	go to 31200
28082	call nucltransK(1.244,0.033,1.1e-3,0.4e-4,tclev,thlev,tdlev)
	go to 15640
28083	call nucltransK(1.137,0.033,1.5e-3,0.2e-4,tclev,thlev,tdlev)
	go to 16710
28084	call nucltransK(0.341,0.033,3.0e-2,0.,tclev,thlev,tdlev)
	go to 24670
c-----------------------------------------------------------------------
2768	call beta(0.152,53.,0.,0.,t)
27680	thlev=0.
	p=100.*rnd1(d)
	if(p.le.24.57) go to 27681 
	if(p.le.28.07) go to 27682 
	if(p.le.42.25) go to 27683 
	if(p.le.71.55) go to 27684 
	if(p.le.82.89) go to 27685 
	if(p.le.88.56) go to 27686 
	if(p.le.91.49) go to 27687 
	               go to 27688
27681	call nucltransK(2.456,0.033,2.0e-4,4.4e-4,tclev,thlev,tdlev)
	go to 31200
27682	call nucltransK(2.049,0.033,1.2e-3,2.5e-4,tclev,thlev,tdlev)
	go to 72000
27683	call nucltransK(1.455,0.033,1.0e-3,0.8e-4,tclev,thlev,tdlev)
	go to 13130
27684	call nucltransK(0.743,0.033,5.0e-3,0.,tclev,thlev,tdlev)
	go to 20250
27685	call nucltransK(0.544,0.033,2.0e-4,0.,tclev,thlev,tdlev)
	go to 22250
27686	call nucltransK(0.485,0.033,1.0e-2,0.,tclev,thlev,tdlev)
	go to 22840
27687	call nucltransK(0.302,0.033,4.5e-2,0.,tclev,thlev,tdlev)
	go to 24670
27688	call nucltransK(0.171,0.033,2.0e-1,0.,tclev,thlev,tdlev)
	go to 25970
c-----------------------------------------------------------------------
2661	call beta(0.259,53.,0.,0.,t)
26610	thlev=0.
	p=100.*rnd1(d)
	if(p.le.43.10) go to 26611 
	if(p.le.47.70) go to 26612 
	if(p.le.82.18) go to 26613 
	               go to 26614
26611	call nucltransK(2.661,0.033,1.5e-4,5.2e-4,tclev,thlev,tdlev)
	return
26612	call nucltransK(2.349,0.033,2.2e-4,3.9e-4,tclev,thlev,tdlev)
	go to 31200
26613	call nucltransK(0.943,0.033,2.5e-3,0.,tclev,thlev,tdlev)
	go to 17180
26614	call nucltransK(0.620,0.033,6.5e-3,0.,tclev,thlev,tdlev)
	go to 20400
c-----------------------------------------------------------------------
2597	call beta(0.323,53.,0.,0.,t)
25970	thlev=0.
	p=100.*rnd1(d)
	if(p.le.20.00) go to 25971 
	if(p.le.23.21) go to 25972 
	if(p.le.73.21) go to 25973 
	if(p.le.80.00) go to 25974 
	if(p.le.88.93) go to 25975 
	if(p.le.91.07) go to 25976 
	               go to 25977
25971	call nucltransK(2.597,0.033,1.6e-4,5.1e-4,tclev,thlev,tdlev)
	return
25972	call nucltransK(2.286,0.033,2.3e-4,3.5e-4,tclev,thlev,tdlev)
	go to 31200
25973	call nucltransK(1.683,0.033,5.0e-4,0.6e-4,tclev,thlev,tdlev)
	go to 91500
25974	call nucltransK(1.290,0.033,1.0e-3,0.5e-4,tclev,thlev,tdlev)
	go to 13070
25975	call nucltransK(1.285,0.033,1.0e-3,0.5e-4,tclev,thlev,tdlev)
	go to 13130
25976	call nucltransK(1.224,0.033,1.0e-3,0.1e-4,tclev,thlev,tdlev)
	go to 13740
25977	call nucltransK(0.572,0.033,9.0e-3,0.,tclev,thlev,tdlev)
	go to 20250
c-----------------------------------------------------------------------
2542	call beta(0.378,53.,0.,0.,t)
25420	thlev=0.
	p=100.*rnd1(d)
	if(p.le.23.06) go to 25421 
	if(p.le.63.19) go to 25422 
	if(p.le.73.34) go to 25423 
	if(p.le.75.37) go to 25424 
	if(p.le.77.40) go to 25425 
	if(p.le.84.78) go to 25426 
	if(p.le.90.32) go to 25427 
	if(p.le.94.47) go to 25428 
	               go to 25429
25421	call nucltransK(2.542,0.033,1.6e-4,4.8e-4,tclev,thlev,tdlev)
	return
25422	call nucltransK(2.230,0.033,2.5e-4,3.2e-4,tclev,thlev,tdlev)
	go to 31200
25423	call nucltransK(1.822,0.033,5.0e-4,1.8e-4,tclev,thlev,tdlev)
	go to 72000
25424	call nucltransK(1.755,0.033,5.5e-4,1.6e-4,tclev,thlev,tdlev)
	go to 78700
25425	call nucltransK(1.302,0.033,1.0e-3,0.5e-4,tclev,thlev,tdlev)
	go to 12400
25426	call nucltransK(1.209,0.033,1.4e-3,0.3e-4,tclev,thlev,tdlev)
	go to 13330
25427	call nucltransK(0.978,0.033,2.5e-3,0.,tclev,thlev,tdlev)
	go to 15640
25428	call nucltransK(0.488,0.033,1.2e-2,0.,tclev,thlev,tdlev)
	go to 20540
25429	call nucltransK(0.332,0.033,3.5e-2,0.,tclev,thlev,tdlev)
	go to 22100
c-----------------------------------------------------------------------
2526	call beta(0.394,53.,0.,0.,t)
25260	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 3.97) go to 25261 
	if(p.le.37.30) go to 25262 
	if(p.le.78.57) go to 25263 
	if(p.le.86.51) go to 25264 
	if(p.le.90.48) go to 25265 
	               go to 25266
25261	call nucltransK(2.526,0.033,1.7e-4,5.7e-4,tclev,thlev,tdlev)
	return
25262	call nucltransK(2.214,0.033,2.5e-4,3.1e-4,tclev,thlev,tdlev)
	go to 31200
25263	call nucltransK(1.807,0.033,5.0e-4,1.8e-4,tclev,thlev,tdlev)
	go to 72000
25264	call nucltransK(1.738,0.033,5.5e-4,1.5e-4,tclev,thlev,tdlev)
	go to 78700
25265	call nucltransK(1.286,0.033,1.0e-3,0.5e-4,tclev,thlev,tdlev)
	go to 12400
25266	call nucltransK(0.854,0.033,3.5e-3,0.,tclev,thlev,tdlev)
	go to 16710
c-----------------------------------------------------------------------
2493	call beta(0.427,53.,0.,0.,t)
24930	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 8.93) go to 24931 
	if(p.le.48.72) go to 24932 
	if(p.le.65.81) go to 24933 
	               go to 24934
24931	call nucltransK(2.181,0.033,2.5e-4,3.4e-4,tclev,thlev,tdlev)
	go to 31200
24932	call nucltransK(1.773,0.033,5.0e-4,0.9e-4,tclev,thlev,tdlev)
	go to 72000
24933	call nucltransK(1.706,0.033,6.0e-4,1.3e-4,tclev,thlev,tdlev)
	go to 78700
24934	call nucltransK(0.928,0.033,2.8e-3,0.,tclev,thlev,tdlev)
	go to 15640
c-----------------------------------------------------------------------
2467	call beta(0.453,53.,0.,0.,t)
24670	thlev=0.
	p=100.*rnd1(d)
	if(p.le.61.38) go to 24671 
	if(p.le.65.12) go to 24672 
	if(p.le.78.59) go to 24673 
	if(p.le.95.36) go to 24674 
	               go to 24675
24671	call nucltransK(2.467,0.033,1.8e-4,5.2e-4,tclev,thlev,tdlev)
	return
24672	call nucltransK(2.155,0.033,3.0e-4,3.0e-4,tclev,thlev,tdlev)
	go to 31200
24673	call nucltransK(1.680,0.033,5.0e-4,1.3e-4,tclev,thlev,tdlev)
	go to 78700
24674	call nucltransK(1.228,0.033,1.2e-3,0.4e-4,tclev,thlev,tdlev)
	go to 12400
24675	call nucltransK(0.242,0.033,8.0e-2,0.,tclev,thlev,tdlev)
	go to 22250
c-----------------------------------------------------------------------
2417	call beta(0.503,53.,0.,0.,t)
24170	thlev=0.
	p=100.*rnd1(d)
	if(p.le.34.48) go to 24171 
	if(p.le.52.63) go to 24172 
	if(p.le.63.52) go to 24173 
	if(p.le.68.06) go to 24174 
	if(p.le.73.69) go to 24175 
	if(p.le.95.46) go to 24176 
	               go to 24177
24171	call nucltransK(2.417,0.033,2.0e-4,4.2e-4,tclev,thlev,tdlev)
	return
24172	call nucltransK(2.106,0.033,3.0e-4,2.7e-4,tclev,thlev,tdlev)
	go to 31200
24173	call nucltransK(1.697,0.033,5.0e-4,1.3e-4,tclev,thlev,tdlev)
	go to 72000
24174	call nucltransK(1.630,0.033,5.2e-4,1.2e-4,tclev,thlev,tdlev)
	go to 78700
24175	call nucltransK(1.503,0.033,7.0e-4,0.3e-4,tclev,thlev,tdlev)
	go to 91500
24176	call nucltransK(1.110,0.033,1.8e-3,0.1e-4,tclev,thlev,tdlev)
	go to 13070
24177	call nucltransK(0.207,0.033,1.2e-1,0.,tclev,thlev,tdlev)
	go to 22100
c-----------------------------------------------------------------------
2393	call beta(0.527,53.,0.,0.,t)
23930	thlev=0.
	p=100.*rnd1(d)
	if(p.le.11.32) go to 23931 
	if(p.le.64.16) go to 23932 
	if(p.le.82.08) go to 23933 
	               go to 23934
23931	call nucltransK(2.393,0.033,2.0e-4,4.1e-4,tclev,thlev,tdlev)
	return
23932	call nucltransK(2.081,0.033,3.0e-4,2.6e-4,tclev,thlev,tdlev)
	go to 31200
23933	call nucltransK(0.722,0.033,5.0e-3,0.,tclev,thlev,tdlev)
	go to 16710
23934	call nucltransK(0.183,0.033,1.7e-1,0.,tclev,thlev,tdlev)
	go to 22100
c-----------------------------------------------------------------------
2364	call beta(0.556,53.,0.,0.,t)
23640	thlev=0.
	p=100.*rnd1(d)
	if(p.le.18.52) go to 23641 
	if(p.le.62.96) go to 23642 
	               go to 23643
23641	call nucltransK(2.363,0.033,2.0e-4,4.0e-4,tclev,thlev,tdlev)
	return
23642	call nucltransK(1.124,0.033,1.8e-3,0.3e-4,tclev,thlev,tdlev)
	go to 12400
23643	call nucltransK(1.051,0.033,2.0e-3,0.,tclev,thlev,tdlev)
	go to 13130
c-----------------------------------------------------------------------
2284	call beta(0.636,53.,0.,0.,t)
22840	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 5.77) go to 22841 
	if(p.le.34.62) go to 22842 
	if(p.le.53.85) go to 22843 
	               go to 22844
22841	call nucltransK(1.972,0.033,3.5e-4,1.7e-4,tclev,thlev,tdlev)
	go to 31200
22842	call nucltransK(1.564,0.033,6.0e-4,0.4e-4,tclev,thlev,tdlev)
	go to 72000
22843	call nucltransK(0.971,0.033,2.5e-3,0.,tclev,thlev,tdlev)
	go to 13130
22844	call nucltransK(0.910,0.033,3.0e-3,0.,tclev,thlev,tdlev)
	go to 13740
c-----------------------------------------------------------------------
2266	call beta(0.654,53.,0.,0.,t)
22660	thlev=0.
	p=100.*rnd1(d)
	if(p.le.52.17) go to 22661 
	if(p.le.64.34) go to 22662 
	if(p.le.90.43) go to 22663 
	               go to 22664
22661	call nucltransK(2.266,0.033,2.5e-4,3.5e-4,tclev,thlev,tdlev)
	return
22662	call nucltransK(1.027,0.033,2.2e-3,0.,tclev,thlev,tdlev)
	go to 12400
22663	call nucltransK(0.934,0.033,2.8e-3,0.,tclev,thlev,tdlev)
	go to 13330
22664	call nucltransK(0.702,0.033,5.5e-3,0.,tclev,thlev,tdlev)
	go to 15640
c-----------------------------------------------------------------------
2255	call beta(0.665,53.,0.,0.,t)
22550	thlev=0.
	p=100.*rnd1(d)
	if(p.le.16.92) go to 22551 
	if(p.le.23.45) go to 22552 
	if(p.le.40.37) go to 22553 
	if(p.le.44.40) go to 22554 
	if(p.le.54.07) go to 22555 
	if(p.le.79.05) go to 22556 
	if(p.le.88.72) go to 22557 
	               go to 22558
22551	call nucltransK(2.255,0.033,3.0e-4,3.8e-4,tclev,thlev,tdlev)
	return
22552	call nucltransK(1.944,0.033,3.5e-4,2.1e-4,tclev,thlev,tdlev)
	go to 31200
22553	call nucltransK(1.535,0.033,6.0e-4,1.0e-4,tclev,thlev,tdlev)
	go to 72000
22554	call nucltransK(1.468,0.033,6.5e-4,0.9e-4,tclev,thlev,tdlev)
	go to 78700
22555	call nucltransK(1.015,0.033,2.3e-3,0.,tclev,thlev,tdlev)
	go to 12400
22556	call nucltransK(0.942,0.033,2.7e-3,0.,tclev,thlev,tdlev)
	go to 13130
22557	call nucltransK(0.922,0.033,2.8e-3,0.,tclev,thlev,tdlev)
	go to 13330
22558	call nucltransK(0.691,0.033,5.5e-3,0.,tclev,thlev,tdlev)
	go to 15640
c-----------------------------------------------------------------------
2225	call beta(0.695,53.,0.,0.,t)
22250	thlev=0.
	p=100.*rnd1(d)
	if(p.le.19.00) go to 22251 
	if(p.le.29.28) go to 22252 
	if(p.le.35.32) go to 22253 
	if(p.le.35.84) go to 22254 
	if(p.le.47.67) go to 22255 
	if(p.le.53.02) go to 22256 
	if(p.le.79.79) go to 22257 
	if(p.le.84.97) go to 22258 
	if(p.le.96.80) go to 22259 
	               go to 22260
22251	call nucltransK(2.225,0.033,3.0e-4,3.2e-4,tclev,thlev,tdlev)
	return
22252	call nucltransK(1.913,0.033,4.5e-4,2.0e-4,tclev,thlev,tdlev)
	go to 31200
22253	call nucltransK(1.505,0.033,7.5e-4,0.9e-4,tclev,thlev,tdlev)
	go to 72000
22254	call nucltransK(1.438,0.033,8.5e-4,0.8e-4,tclev,thlev,tdlev)
	go to 78700
22255	call nucltransK(1.310,0.033,9.0e-4,0.1e-4,tclev,thlev,tdlev)
	go to 91500
22256	call nucltransK(0.912,0.033,3.0e-3,0.,tclev,thlev,tdlev)
	go to 13130
22257	call nucltransK(0.852,0.033,2.5e-3,0.,tclev,thlev,tdlev)
	go to 13740
22258	call nucltransK(0.554,0.033,9.5e-3,0.,tclev,thlev,tdlev)
	go to 16710
22259	call nucltransK(0.507,0.033,1.2e-2,0.,tclev,thlev,tdlev)
	go to 17180
22260	call nucltransK(0.200,0.033,1.4e-1,0.,tclev,thlev,tdlev)
	go to 20250
c-----------------------------------------------------------------------
2210	call beta(0.710,53.,0.,0.,t)
22100	thlev=0.
	p=100.*rnd1(d)
	if(p.le.46.12) go to 22101 
	if(p.le.53.21) go to 22102 
	if(p.le.61.23) go to 22103 
	if(p.le.73.93) go to 22104 
	               go to 22105
22101	call nucltransK(2.210,0.033,2.9e-4,3.6e-4,tclev,thlev,tdlev)
	return
22102	call nucltransK(1.898,0.033,4.5e-4,2.0e-4,tclev,thlev,tdlev)
	go to 31200
22103	call nucltransK(1.490,0.033,7.5e-4,0.9e-4,tclev,thlev,tdlev)
	go to 72000
22104	call nucltransK(0.903,0.033,3.0e-3,0.,tclev,thlev,tdlev)
	go to 13070
22105	call nucltransK(0.646,0.033,6.5e-3,0.,tclev,thlev,tdlev)
	go to 15640
c-----------------------------------------------------------------------
2194	call beta(0.726,53.,0.,0.,t)
21940	thlev=0.
	p=100.*rnd1(d)
	if(p.le.25.07) go to 21941 
	if(p.le.78.72) go to 21942 
	if(p.le.92.79) go to 21943 
	if(p.le.94.72) go to 21944 
	if(p.le.97.36) go to 21945 
	               go to 21946
21941	call nucltransK(2.194,0.033,3.0e-4,3.1e-4,tclev,thlev,tdlev)
	return
21942	call nucltransK(1.882,0.033,4.5e-4,1.9e-4,tclev,thlev,tdlev)
	go to 31200
21943	call nucltransK(1.474,0.033,7.5e-4,0.9e-4,tclev,thlev,tdlev)
	go to 72000
21944	call nucltransK(0.886,0.033,3.0e-3,0.,tclev,thlev,tdlev)
	go to 13070
21945	call nucltransK(0.881,0.033,3.0e-3,0.,tclev,thlev,tdlev)
	go to 13130
21946	call nucltransK(0.860,0.033,3.4e-3,0.,tclev,thlev,tdlev)
	go to 13330
c-----------------------------------------------------------------------
2136	call beta(0.784,53.,0.,0.,t)
21360	thlev=0.
	p=100.*rnd1(d)
	if(p.le.57.73) go to 21361 
	if(p.le.69.61) go to 21362 
	if(p.le.75.10) go to 21363 
	if(p.le.79.32) go to 21364 
	if(p.le.80.12) go to 21365 
	if(p.le.82.23) go to 21366 
	if(p.le.86.03) go to 21367 
	if(p.le.89.41) go to 21368 
	if(p.le.94.94) go to 21369 
	               go to 21370
21361	call nucltransK(2.136,0.033,3.0e-4,2.9e-4,tclev,thlev,tdlev)
	return
21362	call nucltransK(1.824,0.033,5.0e-4,1.7e-4,tclev,thlev,tdlev)
	go to 31200
21363	call nucltransK(1.417,0.033,1.0e-3,0.8e-4,tclev,thlev,tdlev)
	go to 72000
21364	call nucltransK(1.350,0.033,1.2e-3,0.7e-4,tclev,thlev,tdlev)
	go to 78700
21365	call nucltransK(1.222,0.033,1.0e-3,0.1e-4,tclev,thlev,tdlev)
	go to 91500
21366	call nucltransK(0.897,0.033,3.0e-3,0.,tclev,thlev,tdlev)
	go to 12400
21367	call nucltransK(0.829,0.033,3.7e-3,0.,tclev,thlev,tdlev)
	go to 13070
21368	call nucltransK(0.824,0.033,3.7e-3,0.,tclev,thlev,tdlev)
	go to 13130
21369	call nucltransK(0.803,0.033,4.0e-3,0.,tclev,thlev,tdlev)
	go to 13330
21370	call nucltransK(0.763,0.033,3.2e-3,0.,tclev,thlev,tdlev)
	go to 13740
c-----------------------------------------------------------------------
2054	call beta(0.866,53.,0.,0.,t)
20540	thlev=0.
	p=100.*rnd1(d)
	if(p.le.12.21) go to 20541 
	if(p.le.24.15) go to 20542 
	if(p.le.31.12) go to 20543 
	if(p.le.47.69) go to 20544 
	if(p.le.58.15) go to 20545 
	if(p.le.67.74) go to 20546 
	if(p.le.84.31) go to 20547 
	if(p.le.94.77) go to 20548 
	               go to 20549
20541	call nucltransK(2.054,0.033,4.0e-4,2.5e-4,tclev,thlev,tdlev)
	return
20542	call nucltransK(1.742,0.033,5.0e-4,1.5e-4,tclev,thlev,tdlev)
	go to 31200
20543	call nucltransK(1.334,0.033,1.2e-3,0.6e-4,tclev,thlev,tdlev)
	go to 72000
20544	call nucltransK(1.267,0.033,1.3e-3,0.5e-4,tclev,thlev,tdlev)
	go to 78700
20545	call nucltransK(0.813,0.033,3.8e-3,0.,tclev,thlev,tdlev)
	go to 12400
20546	call nucltransK(0.746,0.033,4.5e-3,0.,tclev,thlev,tdlev)
	go to 13070
20547	call nucltransK(0.741,0.033,4.5e-3,0.,tclev,thlev,tdlev)
	go to 13130
20548	call nucltransK(0.720,0.033,5.0e-3,0.,tclev,thlev,tdlev)
	go to 13330
20549	call nucltransK(0.680,0.033,4.0e-3,0.,tclev,thlev,tdlev)
	go to 13740
c-----------------------------------------------------------------------
2040	call beta(0.880,53.,0.,0.,t)
20400	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 6.87) go to 20401 
	if(p.le.12.09) go to 20402 
	if(p.le.23.08) go to 20403 
	if(p.le.75.28) go to 20404 
	               go to 20405
20401	call nucltransK(1.320,0.033,9.5e-4,0.1e-4,tclev,thlev,tdlev)
	go to 72000
20402	call nucltransK(1.254,0.033,1.2e-3,0.5e-4,tclev,thlev,tdlev)
	go to 78700
20403	call nucltransK(0.727,0.033,5.0e-3,0.,tclev,thlev,tdlev)
	go to 13130
20404	call nucltransK(0.667,0.033,6.0e-3,0.,tclev,thlev,tdlev)
	go to 13740
20405	call nucltransK(0.369,0.033,2.5e-2,0.,tclev,thlev,tdlev)
	go to 16710
c-----------------------------------------------------------------------
2025	call beta(0.895,53.,0.,0.,t)
20250	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 4.60) go to 20251 
	if(p.le.25.61) go to 20252 
	if(p.le.40.94) go to 20253 
	if(p.le.47.75) go to 20254 
	if(p.le.54.00) go to 20255 
	if(p.le.64.79) go to 20256 
	               go to 20257
20251	call nucltransK(2.025,0.033,3.5e-4,2.2e-4,tclev,thlev,tdlev)
	return
20252	call nucltransK(1.713,0.033,5.0e-4,1.4e-4,tclev,thlev,tdlev)
	go to 31200
20253	call nucltransK(1.306,0.033,1.2e-3,0.6e-4,tclev,thlev,tdlev)
	go to 72000
20254	call nucltransK(1.239,0.033,1.3e-3,0.5e-4,tclev,thlev,tdlev)
	go to 78700
20255	call nucltransK(0.718,0.033,1.4e-3,0.,tclev,thlev,tdlev)
	go to 13070
20256	call nucltransK(0.713,0.033,5.0e-3,0.,tclev,thlev,tdlev)
	go to 13130
20257	call nucltransK(0.461,0.033,1.5e-2,0.,tclev,thlev,tdlev)
	go to 15640
c-----------------------------------------------------------------------
1718	call beta(1.202,53.,0.,0.,t)
17180	thlev=0.
	p=100.*rnd1(d)
	if(p.le.30.02) go to 17181 
	if(p.le.35.59) go to 17182 
	if(p.le.45.41) go to 17183 
	if(p.le.81.39) go to 17184 
	if(p.le.81.97) go to 17185 
	if(p.le.85.56) go to 17186 
	if(p.le.94.43) go to 17187 
	if(p.le.96.88) go to 17188 
	if(p.le.99.43) go to 17189 
	               go to 17190
17181	call nucltransK(1.718,0.033,6.0e-4,1.4e-4,tclev,thlev,tdlev)
	return
17182	call nucltransK(1.406,0.033,1.2e-3,0.7e-4,tclev,thlev,tdlev)
	go to 31200
17183	call nucltransK(0.998,0.033,2.4e-3,0.,tclev,thlev,tdlev)
	go to 72000
17184	call nucltransK(0.931,0.033,2.7e-3,0.,tclev,thlev,tdlev)
	go to 78700
17185	call nucltransK(0.803,0.033,1.1e-3,0.,tclev,thlev,tdlev)
	go to 91500
17186	call nucltransK(0.478,0.033,1.3e-2,0.,tclev,thlev,tdlev)
	go to 12400
17187	call nucltransK(0.410,0.033,2.0e-2,0.,tclev,thlev,tdlev)
	go to 13070
17188	call nucltransK(0.405,0.033,2.0e-2,0.,tclev,thlev,tdlev)
	go to 13130
17189	call nucltransK(0.384,0.033,2.5e-2,0.,tclev,thlev,tdlev)
	go to 13330
17190	call nucltransK(0.344,0.033,3.0e-2,0.,tclev,thlev,tdlev)
	go to 13740
c-----------------------------------------------------------------------
1671	call beta(1.249,53.,0.,0.,t)
16710	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 9.59) go to 16711 
	if(p.le.15.22) go to 16712 
	if(p.le.28.40) go to 16713 
	if(p.le.71.54) go to 16714 
	if(p.le.78.73) go to 16715 
	if(p.le.83.94) go to 16716 
	               go to 16717
16711	call nucltransK(1.671,0.033,5.0e-4,0.7e-4,tclev,thlev,tdlev)
	return
16712	call nucltransK(1.359,0.033,1.2e-3,0.7e-4,tclev,thlev,tdlev)
	go to 31200
16713	call nucltransK(0.952,0.033,2.6e-3,0.,tclev,thlev,tdlev)
	go to 72000
16714	call nucltransK(0.884,0.033,3.2e-3,0.,tclev,thlev,tdlev)
	go to 78700
16715	call nucltransK(0.432,0.033,1.8e-2,0.,tclev,thlev,tdlev)
	go to 12400
16716	call nucltransK(0.359,0.033,3.0e-2,0.,tclev,thlev,tdlev)
	go to 13130
16717	call nucltransK(0.338,0.033,3.5e-2,0.,tclev,thlev,tdlev)
	go to 13330
c-----------------------------------------------------------------------
1564	call beta(1.356,53.,0.,0.,t)
15640	thlev=0.
	p=100.*rnd1(d)
	if(p.le.28.18) go to 15641 
	if(p.le.92.95) go to 15642 
	if(p.le.96.86) go to 15643 
	if(p.le.97.85) go to 15644 
	if(p.le.98.46) go to 15645 
	if(p.le.98.83) go to 15646 
	               go to 15647
15641	call nucltransK(1.252,0.033,1.3e-3,0.5e-4,tclev,thlev,tdlev)
	go to 31200
15642	call nucltransK(0.844,0.033,3.5e-3,0.,tclev,thlev,tdlev)
	go to 72000
15643	call nucltransK(0.778,0.033,4.2e-3,0.,tclev,thlev,tdlev)
	go to 78700
15644	call nucltransK(0.324,0.033,3.7e-2,0.,tclev,thlev,tdlev)
	go to 12400
15645	call nucltransK(0.251,0.033,7.5e-2,0.,tclev,thlev,tdlev)
	go to 13130
15646	call nucltransK(0.231,0.033,8.5e-2,0.,tclev,thlev,tdlev)
	go to 13330
15647	call nucltransK(0.191,0.033,1.5e-1,0.,tclev,thlev,tdlev)
	go to 13740
c-----------------------------------------------------------------------
1374	call beta(1.546,53.,0.,0.,t)
13740	thlev=0.
	p=100.*rnd1(d)
	if(p.le.56.13) go to 13741 
	if(p.le.14.62) go to 13742 
	               go to 13743
13741	call nucltransK(1.062,0.033,1.5e-3,0.,tclev,thlev,tdlev)
	go to 31200
13742	call nucltransK(0.654,0.033,4.5e-3,0.,tclev,thlev,tdlev)
	go to 72000
13743	call nucltransK(0.587,0.033,8.5e-3,0.,tclev,thlev,tdlev)
	go to 78700
c-----------------------------------------------------------------------
1333	call beta(1.587,53.,0.,0.,t)
13330	thlev=0.
	p=100.*rnd1(d)
	if(p.le.74.49) go to 13331 
	if(p.le.94.05) go to 13332 
	if(p.le.96.28) go to 13333 
	if(p.le.99.83) go to 13334 
	               go to 13335
13331	call nucltransK(1.333,0.033,1.2e-3,0.6e-4,tclev,thlev,tdlev)
	return
13332	call nucltransK(1.021,0.033,2.3e-3,0.,tclev,thlev,tdlev)
	go to 31200
13333	call nucltransK(0.614,0.033,7.0e-3,0.,tclev,thlev,tdlev)
	go to 72000
13334	call nucltransK(0.546,0.033,9.5e-3,0.,tclev,thlev,tdlev)
	go to 78700
13335	call nucltransK(0.418,0.033,1.5e-2,0.,tclev,thlev,tdlev)
	go to 91500
c-----------------------------------------------------------------------
1313	call beta(1.607,53.,0.,0.,t)
13130	thlev=0.
	p=100.*rnd1(d)
	if(p.le.17.45) go to 13131 
	if(p.le.91.78) go to 13132 
	if(p.le.95.27) go to 13133 
	               go to 13134
13131	call nucltransK(1.313,0.033,9.0e-4,0.,tclev,thlev,tdlev)
	return
13132	call nucltransK(1.001,0.033,2.5e-3,0.,tclev,thlev,tdlev)
	go to 31200
13133	call nucltransK(0.593,0.033,8.5e-3,0.,tclev,thlev,tdlev)
	go to 72000
13134	call nucltransK(0.526,0.033,1.2e-2,0.,tclev,thlev,tdlev)
	go to 78700
c-----------------------------------------------------------------------
13070	thlev=0.
	p=100.*rnd1(d)
	if(p.le.33.13) go to 13071 
	if(p.le.75.46) go to 13072 
	if(p.le.81.59) go to 13073 
	if(p.le.82.76) go to 13074 
	if(p.le.84.66) go to 13075 
	               go to 13076 
13071	call nucltransK(1.307,0.033,1.2e-3,0.5e-4,tclev,thlev,tdlev)
	return
13072	call nucltransK(0.995,0.033,2.5e-3,0.,tclev,thlev,tdlev)
	go to 31200
13073	call nucltransK(0.588,0.033,8.5e-3,0.,tclev,thlev,tdlev)
	go to 72000
13074	call nucltransK(0.520,0.033,8.5e-3,0.,tclev,thlev,tdlev)
	go to 78700
13075	call nucltransK(0.394,0.033,2.0e-2,0.,tclev,thlev,tdlev)
	go to 91300
13076	call nucltransK(0.392,0.033,2.2e-2,0.,tclev,thlev,tdlev)
	go to 91500
c-----------------------------------------------------------------------
1240	call beta(1.680,53.,0.,0.,t)
12400	thlev=0.
	p=100.*rnd1(d)
	if(p.le.24.03) go to 12401 
	if(p.le.81.24) go to 12402 
	if(p.le.86.27) go to 12403 
	               go to 12404
12401	call nucltransK(1.240,0.033,1.2e-3,0.,tclev,thlev,tdlev)
	return
12402	call nucltransK(0.928,0.033,2.7e-3,0.,tclev,thlev,tdlev)
	go to 31200
12403	call nucltransK(0.520,0.033,1.2e-2,0.,tclev,thlev,tdlev)
	go to 72000
12404	call nucltransK(0.453,0.033,1.5e-2,0.,tclev,thlev,tdlev)
	go to 78700
c-----------------------------------------------------------------------
91500	thlev=0.
	call nucltransK(0.915,0.033,2.8e-3,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
91300	thlev=0.
	call nucltransK(0.913,0.033,2.0e-3,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
78700	thlev=0.
	p=100.*rnd1(d)
	if(p.le.78.49) go to 78701 
	if(p.le.91.28) go to 78702 
	               go to 78703
78701	call nucltransK(0.787,0.033,3.0e-3,0.,tclev,thlev,tdlev)
	return
78702	call nucltransK(0.475,0.033,1.2e-2,0.,tclev,thlev,tdlev)
	go to 31200
78703	call nucltransK(0.067,0.033,4.9,0.,tclev,thlev,tdlev)
	go to 72000
c-----------------------------------------------------------------------
720	call beta(2.200,53.,0.,0.,t)
72000	thlev=0.
	p=100.*rnd1(d)
	if(p.le.24.72) go to 72001 
	               go to 72002
72001	call nucltransK(0.720,0.033,5.0e-3,0.,tclev,thlev,tdlev)
	return
72002	call nucltransK(0.408,0.033,2.0e-2,0.,tclev,thlev,tdlev)
	go to 31200
c-----------------------------------------------------------------------
312	call beta(2.608,53.,0.,0.,t)
31200	thlev=0.
	call nucltransK(0.312,0.033,3.5e-2,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
	return
	end
c
c end of Te133.f
c
c Local Variables: --
c mode: fortran --
c End: --
