c Eu152.f 
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
	subroutine Eu152(tcnuc,tdnuc)
c Scheme of 152Eu decay ("Table of Isotopes", 8th ed., 1996 and
c NDS 79(1996)1).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 16.11.1996.
c VIT, approximate electron capture from K, L or M shells, 27.12.2006.
c VIT, correction to the 1fnu shape for Qbeta=1.474, 13.11.2007.
	thnuc=4.273413e8
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pdecay=100.*rnd1(d)
	if(pdecay.le.72.08) go to 1      ! EC to 152Sm
	                    go to 2      ! b- to 152Gd
c-----------------------------------------------------------------------
c-----------------------------------------------------------------------
c1	call gamma(0.049,0.,0.,tdlev)
c approximate electron capture from K (82%), L (14%) or M (4%) shell
1	pKLM=100.*rnd1(d)
	if(pKLM.le.82.)                   call gamma(0.047,0.,0.,tdlev)
	if(pKLM.gt.82. .and. pKLM.le.96.) call gamma(0.008,0.,0.,tdlev)
	if(pKLM.gt.96.)                   call gamma(0.001,0.,0.,tdlev)
	pec=100.*rnd1(d)
	if(pec.le. 0.071) go to 1769
	if(pec.le. 0.118) go to 1757
	if(pec.le. 0.180) go to 1730
	if(pec.le. 1.416) go to 1650
	if(pec.le. 1.445) go to 1613
	if(pec.le. 4.292) go to 1579
	if(pec.le.38.592) go to 1530
	if(pec.le.39.883) go to 1372
	if(pec.le.40.744) go to 1293
	if(pec.le.64.629) go to 1234
	if(pec.le.94.069) go to 1086
	if(pec.le.94.152) go to 1041
	if(pec.le.94.471) go to 1023
	if(pec.le.96.179) go to  810
	if(pec.le.97.359) go to  366
	                  go to  122
c-----------------------------------------------------------------------
1769	thlev=0.
	p=100.*rnd1(d)
	if(p.le.15.21) go to 17691
	if(p.le.25.62) go to 17692
	if(p.le.65.65) go to 17693
	if(p.le.88.07) go to 17694
	if(p.le.99.28) go to 17695
	               go to 17696
17691	call nucltransK(1.769,0.047,8.0e-4,0.9e-4,tclev,thlev,tdlev)
	return
17692	call nucltransK(1.647,0.047,4.0e-4,0.5e-4,tclev,thlev,tdlev)
	go to 122
17693	call nucltransK(0.959,0.047,2.5e-3,0.,tclev,thlev,tdlev)
	go to 810
17694	call nucltransK(0.806,0.047,1.4e-3,0.,tclev,thlev,tdlev)
	go to 963
17695	call nucltransK(0.536,0.047,1.0e-2,0.,tclev,thlev,tdlev)
	go to 1234
17696	call nucltransK(0.239,0.047,1.0e-1,0.,tclev,thlev,tdlev)
	go to 1530
c-----------------------------------------------------------------------
1757	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.43) go to 17571
	if(p.le.14.38) go to 17572
	if(p.le.81.12) go to 17573
	if(p.le.85.46) go to 17574
	               go to 17575
17571	call nucltransK(1.635,0.047,3.5e-4,0.5e-4,tclev,thlev,tdlev)
	go to 122
17572	call nucltransK(1.390,0.047,5.0e-4,0.2e-4,tclev,thlev,tdlev)
	go to 366
17573	call nucltransK(0.671,0.047,1.1e-2,0.,tclev,thlev,tdlev)
	go to 1086
17574	call nucltransK(0.523,0.047,3.5e-3,0.,tclev,thlev,tdlev)
	go to 1234
17575	call nucltransK(0.386,0.047,7.0e-3,0.,tclev,thlev,tdlev)
	go to 1372
c-----------------------------------------------------------------------
1730	thlev=0.
	p=100.*rnd1(d)
	if(p.le.11.26) go to 17301
	if(p.le.66.17) go to 17302
	if(p.le.79.35) go to 17303
	               go to 17304
17301	call nucltransK(1.608,0.047,4.0e-4,0.8e-4,tclev,thlev,tdlev)
	go to 122
17302	call nucltransK(1.364,0.047,5.3e-4,0.3e-4,tclev,thlev,tdlev)
	go to 366
17303	call nucltransK(0.644,0.047,2.2e-3,0.,tclev,thlev,tdlev)
	go to 1086
17304	call nucltransK(0.496,0.047,4.0e-3,0.,tclev,thlev,tdlev)
	go to 1234
c-----------------------------------------------------------------------
1650	thlev=0.
	p=100.*rnd1(d)
	if(p.le.30.00) go to 16501
	if(p.le.31.85) go to 16502
	if(p.le.34.02) go to 16503
	if(p.le.86.93) go to 16504
	if(p.le.99.47) go to 16505
	               go to 16506
16501	call nucltransK(0.769,0.047,8.0e-3,0.,tclev,thlev,tdlev)
	go to 122
16502	call nucltransK(0.769,0.047,8.0e-3,0.,tclev,thlev,tdlev)
	go to 810
16503	call nucltransK(0.769,0.047,8.0e-3,0.,tclev,thlev,tdlev)
	go to 963
16504	call nucltransK(0.769,0.047,8.0e-3,0.,tclev,thlev,tdlev)
	go to 1086
16505	call nucltransK(0.769,0.047,8.0e-3,0.,tclev,thlev,tdlev)
	go to 1234
16506	call nucltransK(0.769,0.047,8.0e-3,0.,tclev,thlev,tdlev)
	go to 1293
c-----------------------------------------------------------------------
1613	thlev=0.
	p=100.*rnd1(d)
	if(p.le.71.28) go to 16131
	if(p.le.94.09) go to 16132
	               go to 16133
16131	call nucltransK(0.906,0.047,2.8e-3,0.,tclev,thlev,tdlev)
	go to 707
16132	call nucltransK(0.572,0.047,2.8e-3,0.,tclev,thlev,tdlev)
	go to 1041
16133	call nucltransK(0.391,0.047,7.0e-3,0.,tclev,thlev,tdlev)
	go to 1222
c-----------------------------------------------------------------------
1579	thlev=0.
	p=100.*rnd1(d)
	if(p.le.24.04) go to 15791
	if(p.le.92.32) go to 15792
	if(p.le.96.69) go to 15793
	if(p.le.97.13) go to 15794
	if(p.le.98.06) go to 15795
	if(p.le.98.26) go to 15796
	if(p.le.99.79) go to 15797
	               go to 15798
15791	call nucltransK(1.458,0.047,4.5e-4,0.4e-4,tclev,thlev,tdlev)
	go to 122
15792	call nucltransK(1.213,0.047,6.2e-4,0.1e-4,tclev,thlev,tdlev)
	go to 366
15793	call nucltransK(0.769,0.047,1.5e-3,0.,tclev,thlev,tdlev)
	go to 810
15794	call nucltransK(0.616,0.047,7.0e-3,0.,tclev,thlev,tdlev)
	go to 963
15795	call nucltransK(0.557,0.047,3.0e-3,0.,tclev,thlev,tdlev)
	go to 1023
15796	call nucltransK(0.538,0.047,9.5e-3,0.,tclev,thlev,tdlev)
	go to 1041
15797	call nucltransK(0.494,0.047,4.0e-3,0.,tclev,thlev,tdlev)
	go to 1086
15798	call nucltransK(0.208,0.047,3.5e-2,0.,tclev,thlev,tdlev)
	go to 1372
c-----------------------------------------------------------------------
1530	thlev=0.
	p=100.*rnd1(d)
	if(p.le.84.56) go to 15301
	if(p.le.84.80) go to 15302
	if(p.le.85.32) go to 15303
	if(p.le.86.97) go to 15304
	if(p.le.98.22) go to 15305
	               go to 15306
15301	call nucltransK(1.408,0.047,5.0e-4,0.3e-4,tclev,thlev,tdlev)
	go to 122
15302	call nucltransK(0.719,0.047,2.0e-3,0.,tclev,thlev,tdlev)
	go to 810
15303	call nucltransK(0.566,0.047,1.4e-2,0.,tclev,thlev,tdlev)
	go to 963
15304	call nucltransK(0.489,0.047,1.4e-2,0.,tclev,thlev,tdlev)
	go to 1041
15305	call nucltransK(0.444,0.047,5.7e-3,0.,tclev,thlev,tdlev)
	go to 1086
15306	call nucltransK(0.296,0.047,1.5e-2,0.,tclev,thlev,tdlev)
	go to 1234
c-----------------------------------------------------------------------
1372	thlev=0.
	p=100.*rnd1(d)
	if(p.le.19.62) go to 13721
	if(p.le.88.70) go to 13722
	if(p.le.90.70) go to 13723
	if(p.le.90.81) go to 13724
	if(p.le.98.82) go to 13725
	               go to 13726
13721	call nucltransK(1.250,0.047,1.6e-3,0.1e-4,tclev,thlev,tdlev)
	go to 122
13722	call nucltransK(1.005,0.047,2.6e-3,0.,tclev,thlev,tdlev)
	go to 366
13723	call nucltransK(0.665,0.047,6.3e-3,0.,tclev,thlev,tdlev)
	go to 707
13724	call nucltransK(0.561,0.047,3.5e-3,0.,tclev,thlev,tdlev)
	go to 810
13725	call nucltransK(0.331,0.047,1.2e-2,0.,tclev,thlev,tdlev)
	go to 1041
13726	call nucltransK(0.286,0.047,6.6e-2,0.,tclev,thlev,tdlev)
	go to 1086
c-----------------------------------------------------------------------
1293	thlev=0.
	p=100.*rnd1(d)
	if(p.le.16.39) go to 12931
	if(p.le.22.12) go to 12932
	if(p.le.64.58) go to 12933
	if(p.le.68.91) go to 12934
	if(p.le.88.66) go to 12935
	if(p.le.89.98) go to 12936
	               go to 12937
12931	call nucltransK(1.293,0.047,1.3e-3,0.1e-4,tclev,thlev,tdlev)
	return
12932	call nucltransK(1.171,0.047,1.6e-3,0.,tclev,thlev,tdlev)
	go to 122
12933	call nucltransK(0.926,0.047,3.0e-3,0.,tclev,thlev,tdlev)
	go to 366
12934	call nucltransK(0.482,0.047,2.0e-2,0.,tclev,thlev,tdlev)
	go to 810
12935	call nucltransK(0.329,0.047,1.2e-2,0.,tclev,thlev,tdlev)
	go to 963
12936	call nucltransK(0.270,0.047,7.9e-2,0.,tclev,thlev,tdlev)
	go to 1023
12937	call nucltransK(0.252,0.047,2.3e-2,0.,tclev,thlev,tdlev)
	go to 1041
c-----------------------------------------------------------------------
1234	thlev=0.
	p=100.*rnd1(d)
	if(p.le.76.33) go to 12341
	if(p.le.99.76) go to 12342
	if(p.le.99.78) go to 12343
	               go to 12344
12341	call nucltransK(1.112,0.047,2.0e-3,0.2e-4,tclev,thlev,tdlev)
	go to 122
12342	call nucltransK(0.867,0.047,3.5e-3,0.,tclev,thlev,tdlev)
	go to 366
12343	call nucltransK(0.423,0.047,2.7e-2,0.,tclev,thlev,tdlev)
	go to 810
12344	call nucltransK(0.148,0.047,5.8e-1,0.,tclev,thlev,tdlev)
	go to 1086
c-----------------------------------------------------------------------
1222	thlev=0.
	p=100.*rnd1(d)
	if(p.le.80.65) go to 12221
	               go to 12222
12221	call nucltransK(0.855,0.047,1.6e-3,0.,tclev,thlev,tdlev)
	go to 366
12222	call nucltransK(0.515,0.047,3.7e-3,0.,tclev,thlev,tdlev)
	go to 707
c-----------------------------------------------------------------------
1086	thlev=0.85e-12
	p=100.*rnd1(d)
	if(p.le.40.36) go to 10861
	if(p.le.98.77) go to 10862
	if(p.le.99.86) go to 10863
	               go to 10864
10861	call nucltransK(1.086,0.047,2.1e-3,0.,tclev,thlev,tdlev)
	return
10862	call nucltransK(0.964,0.047,2.7e-3,0.,tclev,thlev,tdlev)
	go to 122
10863	call nucltransK(0.719,0.047,5.2e-3,0.,tclev,thlev,tdlev)
	go to 366
10864	call nucltransK(0.275,0.047,1.0e-1,0.,tclev,thlev,tdlev)
	go to 810
c-----------------------------------------------------------------------
1041	thlev=0.
	p=100.*rnd1(d)
	if(p.le.71.23) go to 10411
	               go to 10412
10411	call nucltransK(0.919,0.047,1.2e-3,0.,tclev,thlev,tdlev)
	go to 122
10412	call nucltransK(0.675,0.047,2.3e-3,0.,tclev,thlev,tdlev)
	go to 366
c-----------------------------------------------------------------------
1023	thlev=6.7e-12
	p=100.*rnd1(d)
	if(p.le.35.73) go to 10231
	if(p.le.91.56) go to 10232
	if(p.le.92.40) go to 10233
	               go to 10234
10231	call nucltransK(0.901,0.047,3.1e-3,0.,tclev,thlev,tdlev)
	go to 122
10232	call nucltransK(0.656,0.047,5.7e-2,0.,tclev,thlev,tdlev)
	go to 366
10233	call nucltransK(0.316,0.047,4.8e-2,0.,tclev,thlev,tdlev)
	go to 707
10234	call nucltransK(0.213,0.047,1.7e-1,0.,tclev,thlev,tdlev)
	go to 810
c-----------------------------------------------------------------------
963	thlev=28.2e-15
	p=100.*rnd1(d)
	if(p.le.45.143) go to 96301
	if(p.le.99.994) go to 96302
	if(p.le.99.995) go to 96303
	                go to 96304
96301	call nucltransK(0.963,0.047,1.1e-3,0.,tclev,thlev,tdlev)
	return
96302	call nucltransK(0.842,0.047,1.4e-3,0.,tclev,thlev,tdlev)
	go to 122
96303	call nucltransK(0.279,0.047,1.8e-2,0.,tclev,thlev,tdlev)
	go to 685
96304	call nucltransK(0.153,0.047,8.7e-2,0.,tclev,thlev,tdlev)
	go to 810
c-----------------------------------------------------------------------
810	thlev=7.2e-12
	p=100.*rnd1(d)
	if(p.le.21.66) go to 81001
	if(p.le.78.06) go to 81002
	if(p.le.99.21) go to 81003
	               go to 81004
81001	call nucltransK(0.810,0.047,4.0e-3,0.,tclev,thlev,tdlev)
	return
81002	call nucltransK(0.689,0.047,4.3e-2,0.,tclev,thlev,tdlev)
	go to 122
81003	call nucltransK(0.444,0.047,1.8e-2,0.,tclev,thlev,tdlev)
	go to 366
81004	call nucltransK(0.126,0.047,1.0e-0,0.,tclev,thlev,tdlev)
	go to 685
c-----------------------------------------------------------------------
707	thlev=10.1e-12
	call nucltransK(0.340,0.047,3.8e-4,0.,tclev,thlev,tdlev)
	go to 366
c-----------------------------------------------------------------------
685	thlev=6.2e-12
	p=100.*rnd1(d)
	if(p.le. 1.43) go to 68501
	               go to 68502
68501	call electron(0.638,tclev,thlev,tdlev)
	call gamma(0.047,0.,0.,tdlev)
	return
68502	call nucltransK(0.563,0.047,9.5e-3,0.,tclev,thlev,tdlev)
	go to 122
c-----------------------------------------------------------------------
366	thlev=57.7e-12
	call nucltransK(0.245,0.047,1.1e-1,0.,tclev,thlev,tdlev)
	go to 122
c-----------------------------------------------------------------------
122	thlev=1.428e-9
	call nucltransK(0.122,0.047,1.2e-0,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
c-----------------------------------------------------------------------
2	pbeta=100.*rnd1(d)
	if(pbeta.le. 0.071) go to  1692
	if(pbeta.le. 6.421) go to  1643
	if(pbeta.le. 6.778) go to  1606
	if(pbeta.le.15.339) go to  1434
	if(pbeta.le.15.981) go to  1318
	if(pbeta.le.16.063) go to  1282
	if(pbeta.le.65.291) go to  1123
	if(pbeta.le.66.325) go to  1109
	if(pbeta.le.67.395) go to   931
	if(pbeta.le.70.748) go to   755
	                    go to   344
c-----------------------------------------------------------------------
1692	call beta(0.126,64.,0.,0.,t)
16920	thlev=0.
	p=100.*rnd1(d)
	if(p.le.84.03) go to 16921
	               go to 16922
16921	call nucltransK(1.348,0.050,1.5e-3,0.2e-4,tclev,thlev,tdlev)
	go to 34400
16922	call nucltransK(0.937,0.050,3.2e-3,0.,tclev,thlev,tdlev)
	go to 75500
c-----------------------------------------------------------------------
1643	call beta(0.175,64.,0.,0.,t)
16430	thlev=0.
	p=100.*rnd1(d)
	if(p.le.89.21) go to 16431
	if(p.le.94.47) go to 16432
	if(p.le.96.83) go to 16433
	if(p.le.99.75) go to 16434
	if(p.le.99.76) go to 16435
	               go to 16436
16431	call nucltransK(1.299,0.050,7.5e-4,0.3e-4,tclev,thlev,tdlev)
	go to 34400
16432	call nucltransK(0.713,0.050,2.2e-3,0.,tclev,thlev,tdlev)
	go to 93100
16433	call nucltransK(0.534,0.050,4.5e-3,0.,tclev,thlev,tdlev)
	go to 11090
16434	call nucltransK(0.520,0.050,1.8e-2,0.,tclev,thlev,tdlev)
	go to 11230
16435	call nucltransK(0.325,0.050,1.3e-2,0.,tclev,thlev,tdlev)
	go to 13180
16436	call nucltransK(0.209,0.050,4.0e-2,0.,tclev,thlev,tdlev)
	go to 14340
c-----------------------------------------------------------------------
1606	call beta(0.212,64.,0.,0.,t)
16060	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 7.64) go to 16061
	if(p.le.40.86) go to 16062
	if(p.le.72.09) go to 16063
	if(p.le.91.36) go to 16064
	if(p.le.95.35) go to 16065
	if(p.le.99.99) go to 16066
	               go to 16067
16061	call nucltransK(1.606,0.050,1.2e-3,0.5e-4,tclev,thlev,tdlev)
	return
16062	call nucltransK(1.261,0.050,2.7e-3,0.5e-4,tclev,thlev,tdlev)
	go to 34400
16063	call nucltransK(0.990,0.050,3.0e-3,0.,tclev,thlev,tdlev)
	go to 61500
16064	call nucltransK(0.675,0.050,7.6e-3,0.,tclev,thlev,tdlev)
	go to 93100
16065	call nucltransK(0.558,0.050,1.1e-2,0.,tclev,thlev,tdlev)
	go to 10480
16066	call nucltransK(0.496,0.050,9.7e-2,0.,tclev,thlev,tdlev)
	go to 11090
16067	call nucltransK(0.482,0.050,5.0e-3,0.,tclev,thlev,tdlev)
	go to 11230
c-----------------------------------------------------------------------
1434	call beta(0.384,64.,0.,0.,t)
14340	thlev=0.
	p=100.*rnd1(d)
	if(p.le.71.07) go to 14341
	if(p.le.90.54) go to 14342
	if(p.le.96.87) go to 14343
	if(p.le.99.99) go to 14344
	               go to 14345
14341	call nucltransK(1.090,0.050,2.3e-3,0.,tclev,thlev,tdlev)
	go to 34400
14342	call nucltransK(0.679,0.050,6.9e-3,0.,tclev,thlev,tdlev)
	go to 75500
14343	call nucltransK(0.503,0.050,1.4e-2,0.,tclev,thlev,tdlev)
	go to 93100
14344	call nucltransK(0.325,0.050,6.3e-2,0.,tclev,thlev,tdlev)
	go to 11090
14345	call nucltransK(0.115,0.050,1.4e-0,0.,tclev,thlev,tdlev)
	go to 13180
c-----------------------------------------------------------------------
1318	call beta(0.500,64.,0.,0.,t)
13180	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 6.87) go to 13181
	if(p.le.62.27) go to 13182
	if(p.le.76.12) go to 13183
	if(p.le.77.39) go to 13184
	if(p.le.84.76) go to 13185
	if(p.le.92.35) go to 13186
	if(p.le.92.79) go to 13187
	               go to 13188
13181	call nucltransK(1.318,0.050,1.6e-3,0.1e-4,tclev,thlev,tdlev)
	return
13182	call nucltransK(0.974,0.050,5.6e-3,0.,tclev,thlev,tdlev)
	go to 34400
13183	call nucltransK(0.703,0.050,6.0e-3,0.,tclev,thlev,tdlev)
	go to 61500
13184	call nucltransK(0.563,0.050,1.0e-2,0.,tclev,thlev,tdlev)
	go to 75500
13185	call nucltransK(0.388,0.050,4.5e-1,0.,tclev,thlev,tdlev)
	go to 93100
13186	call nucltransK(0.271,0.050,8.4e-2,0.,tclev,thlev,tdlev)
	go to 10480
13187	call nucltransK(0.209,0.050,5.0e-1,0.,tclev,thlev,tdlev)
	go to 11090
13188	call nucltransK(0.195,0.050,4.9e-2,0.,tclev,thlev,tdlev)
	go to 11230
c-----------------------------------------------------------------------
1282	call beta(0.536,64.,0.,0.,t)
12820	thlev=0.
	p=100.*rnd1(d)
	if(p.le.58.41) go to 12821
	if(p.le.98.13) go to 12822
	               go to 12823
12821	call nucltransK(0.527,0.050,9.4e-2,0.,tclev,thlev,tdlev)
	go to 75500
12822	call nucltransK(0.352,0.050,3.8e-2,0.,tclev,thlev,tdlev)
	go to 93100
12823	call nucltransK(0.172,0.050,3.7e-1,0.,tclev,thlev,tdlev)
	go to 11090
c-----------------------------------------------------------------------
1123	call beta(0.695,64.,0.,0.,t)
11230	thlev=0.
	p=100.*rnd1(d)
	if(p.le.93.11) go to 11231
	if(p.le.99.26) go to 11232
	               go to 11233
11231	call nucltransK(0.779,0.050,1.9e-3,0.,tclev,thlev,tdlev)
	go to 34400
11232	call nucltransK(0.368,0.050,9.7e-3,0.,tclev,thlev,tdlev)
	go to 75500
11233	call nucltransK(0.193,0.050,5.0e-2,0.,tclev,thlev,tdlev)
	go to 93100
c-----------------------------------------------------------------------
1109	call beta(0.709,64.,0.,0.,t)
11090	thlev=0.
	p=100.*rnd1(d)
	if(p.le.50.00) go to 11091
	if(p.le.97.50) go to 11092
	               go to 11093
11091	call nucltransK(1.109,0.050,2.2e-3,0.,tclev,thlev,tdlev)
	return
11092	call nucltransK(0.765,0.050,5.2e-3,0.,tclev,thlev,tdlev)
	go to 34400
11093	call nucltransK(0.494,0.050,1.5e-2,0.,tclev,thlev,tdlev)
	go to 61500
c-----------------------------------------------------------------------
10480	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.88) go to 10481
	if(p.le.65.79) go to 10482
	if(p.le.83.77) go to 10483
	               go to 10484
10481	call electron(0.998,tclev,thlev,tdlev)
	call gamma(0.050,0.,0.,tdlev)
	return
10482	call nucltransK(0.703,0.050,6.0e-3,0.,tclev,thlev,tdlev)
	go to 34400
10483	call electron(0.383,tclev,thlev,tdlev)
	call gamma(0.050,0.,0.,tdlev)
	go to 61500
10484	call nucltransK(0.117,0.050,1.4e-0,0.,tclev,thlev,tdlev)
	go to 93100
c-----------------------------------------------------------------------
931	call beta(0.887,64.,0.,0.,t)
93100	thlev=7.3e-12
	p=100.*rnd1(d)
	if(p.le.12.40) go to 93101
	if(p.le.90.91) go to 93102
	if(p.le.99.55) go to 93103
	               go to 93104
93101	call nucltransK(0.931,0.050,3.2e-3,0.,tclev,thlev,tdlev)
	return
93102	call nucltransK(0.586,0.050,2.4e-2,0.,tclev,thlev,tdlev)
	go to 34400
93103	call nucltransK(0.315,0.050,5.2e-2,0.,tclev,thlev,tdlev)
	go to 61500
93104	call nucltransK(0.175,0.050,3.5e-1,0.,tclev,thlev,tdlev)
	go to 75500
c-----------------------------------------------------------------------
755	call beta(1.063,64.,0.,0.,t)
75500	thlev=7.3e-12
	call nucltransK(0.411,0.050,2.4e-2,0.,tclev,thlev,tdlev)
	go to 34400
c-----------------------------------------------------------------------
61500	thlev=37.e-12
	p=100.*rnd1(d)
	if(p.le.11.35) go to 61501
	               go to 61502
61501	call electron(0.565,tclev,thlev,tdlev)
	call gamma(0.050,0.,0.,tdlev)
	return
61502	call nucltransK(0.271,0.050,8.3e-2,0.,tclev,thlev,tdlev)
	go to 34400
c-----------------------------------------------------------------------
c344	call beta(1.474,64.,0.,0.,t)
c correction to the 1st forbidden non-unique shape in accordance with
c L.M.Langer et al., PR 119(1960)1308
344	call beta1(1.474,64.,0.,0.,t,0.,-0.4026,0.0928,0.)
34400	thlev=31.9e-12
	call nucltransK(0.344,0.050,4.0e-2,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
	end
c
c end of Eu152.f
c
c Local Variables: --
c mode: fortran --
c End: --
