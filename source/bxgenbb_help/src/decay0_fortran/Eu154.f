c Eu154.f 
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
	subroutine Eu154(tcnuc,tdnuc)
c Scheme of 154Eu decay ("Table of Isotopes", 8th ed., 1996 and
c NDS 69(1993)507).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 13.11.1996.
c VIT, correction to the 1fnu shape for Qbeta=1.846, 13.11.2007.
	thnuc=2.711670e8
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pdecay=100.*rnd1(d)
	if(pdecay.le.0.020) go to 1      !  0.020% EC to 154Sm
	                    go to 2      ! 99.980% b- to 154Gd
c-----------------------------------------------------------------------
c-----------------------------------------------------------------------
1	call gamma(0.049,0.,0.,tdlev)
	if(pdecay.le.0.005) go to 267
	                    go to  82
c-----------------------------------------------------------------------
267	thlev=172.e-12
	call nucltransK(0.185,0.047,2.7e-1,0.,tclev,thlev,tdlev)
	go to 82
c-----------------------------------------------------------------------
82	thlev=3.02e-9
	call nucltransK(0.082,0.047,4.9+0,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
c-----------------------------------------------------------------------
2	pbeta=100.*rnd1(d)
	if(pbeta.le. 0.019) go to  1839
	if(pbeta.le. 0.087) go to  1797
	if(pbeta.le.28.587) go to  1720
	if(pbeta.le.29.417) go to  1661
	if(pbeta.le.29.564) go to  1646
	if(pbeta.le.31.174) go to  1617
	if(pbeta.le.31.271) go to  1560
	if(pbeta.le.31.571) go to  1531
	if(pbeta.le.31.679) go to  1418
	if(pbeta.le.67.879) go to  1398
	if(pbeta.le.68.599) go to  1264
	if(pbeta.le.68.909) go to  1252
	if(pbeta.le.85.610) go to  1128
	if(pbeta.le.89.110) go to   996
	if(pbeta.le.89.810) go to   815
	if(pbeta.le.90.000) go to   371
	                    go to   123
c-----------------------------------------------------------------------
1839	call beta(0.130,64.,0.,0.,t)
18390	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 4.0) go to 18391
	if(p.le. 6.8) go to 18392
	if(p.le.38.1) go to 18393
	if(p.le.87.8) go to 18394
	if(p.le.94.0) go to 18395
	              go to 18396
18391	call nucltransK(1.839,0.050,1.0e-3,1.0e-4,tclev,thlev,tdlev)
	return
18392	call nucltransK(1.717,0.050,1.1e-3,0.7e-4,tclev,thlev,tdlev)
	go to 12300
18393	call nucltransK(1.023,0.050,3.8e-3,0.,tclev,thlev,tdlev)
	go to 81500
18394	call nucltransK(0.790,0.050,5.0e-3,0.,tclev,thlev,tdlev)
	go to 10480
18395	call nucltransK(0.193,0.050,2.8e-1,0.,tclev,thlev,tdlev)
	go to 16460
18396	call nucltransK(0.063,0.050,1.4e+1,0.,tclev,thlev,tdlev)
	go to 17750
c-----------------------------------------------------------------------
1797	call beta(0.172,64.,0.,0.,t)
17970	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 2.4) go to 17971
	if(p.le. 4.0) go to 17972
	if(p.le.15.3) go to 17973
	if(p.le.58.7) go to 17974
	if(p.le.77.0) go to 17975
	if(p.le.82.0) go to 17976
	if(p.le.91.6) go to 17977
	if(p.le.99.9) go to 17978
	              go to 17979
17971	call nucltransK(1.674,0.050,5.0e-4,0.9e-4,tclev,thlev,tdlev)
	go to 12300
17972	call nucltransK(1.426,0.050,6.0e-4,0.4e-4,tclev,thlev,tdlev)
	go to 37100
17973	call nucltransK(0.982,0.050,1.2e-3,0.,tclev,thlev,tdlev)
	go to 81500
17974	call nucltransK(0.801,0.050,1.8e-3,0.,tclev,thlev,tdlev)
	go to 99600
17975	call nucltransK(0.669,0.050,2.5e-3,0.,tclev,thlev,tdlev)
	go to 11280
17976	call nucltransK(0.556,0.050,1.1e-2,0.,tclev,thlev,tdlev)
	go to 12410
17977	call nucltransK(0.533,0.050,4.0e-3,0.,tclev,thlev,tdlev)
	go to 12640
17978	call nucltransK(0.393,0.050,3.0e-2,0.,tclev,thlev,tdlev)
	go to 14040
17979	call nucltransK(0.266,0.050,2.0e-2,0.,tclev,thlev,tdlev)
	go to 15310
c-----------------------------------------------------------------------
17750	thlev=0.
	p=100.*rnd1(d)
	if(p.le.19.6) go to 17751
	if(p.le.62.3) go to 17752
	if(p.le.79.4) go to 17753
	if(p.le.88.4) go to 17754
	if(p.le.92.2) go to 17755
	if(p.le.98.7) go to 17756
	              go to 17757
17751	call nucltransK(1.776,0.050,1.0e-3,0.8e-4,tclev,thlev,tdlev)
	return
17752	call nucltransK(1.652,0.050,1.6e-3,0.6e-4,tclev,thlev,tdlev)
	go to 12300
17753	call nucltransK(1.405,0.050,1.4e-3,0.2e-4,tclev,thlev,tdlev)
	go to 37100
17754	call nucltransK(1.095,0.050,2.3e-3,0.,tclev,thlev,tdlev)
	go to 68100
17755	call nucltransK(0.960,0.050,3.0e-3,0.,tclev,thlev,tdlev)
	go to 81500
17756	call nucltransK(0.728,0.050,6.0e-3,0.,tclev,thlev,tdlev)
	go to 10480
17757	call nucltransK(0.648,0.050,1.4e-2,0.,tclev,thlev,tdlev)
	go to 11280
c-----------------------------------------------------------------------
1720	call beta(0.249,64.,0.,0.,t)
17200	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 6.272) go to 17201
	if(p.le. 9.404) go to 17202
	if(p.le.79.950) go to 17203
	if(p.le.97.361) go to 17204
	if(p.le.98.150) go to 17205
	if(p.le.98.362) go to 17206
	if(p.le.98.594) go to 17207
	if(p.le.98.629) go to 17208
	if(p.le.98.665) go to 17209
	if(p.le.98.674) go to 17210
	if(p.le.99.513) go to 17211
	if(p.le.99.986) go to 17212
	                go to 17213
17201	call nucltransK(1.596,0.050,5.0e-4,0.7e-4,tclev,thlev,tdlev)
	go to 12300
17202	call nucltransK(0.904,0.050,1.4e-3,0.,tclev,thlev,tdlev)
	go to 81500
17203	call nucltransK(0.723,0.050,2.2e-3,0.,tclev,thlev,tdlev)
	go to 99600
17204	call nucltransK(0.592,0.050,3.3e-3,0.,tclev,thlev,tdlev)
	go to 11280
17205	call nucltransK(0.478,0.050,1.6e-2,0.,tclev,thlev,tdlev)
	go to 12410
17206	call nucltransK(0.468,0.050,3.5e-2,0.,tclev,thlev,tdlev)
	go to 12520
17207	call nucltransK(0.322,0.050,9.0e-2,0.,tclev,thlev,tdlev)
	go to 13980
17208	call nucltransK(0.305,0.050,9.5e-2,0.,tclev,thlev,tdlev)
	go to 14140
17209	call nucltransK(0.301,0.050,1.6e-2,0.,tclev,thlev,tdlev)
	go to 14180
17210	call nucltransK(0.209,0.050,3.0e-1,0.,tclev,thlev,tdlev)
	go to 15100
17211	call nucltransK(0.188,0.050,5.5e-2,0.,tclev,thlev,tdlev)
	go to 15310
17212	call nucltransK(0.160,0.050,4.5e-1,0.,tclev,thlev,tdlev)
	go to 15600
17213	call nucltransK(0.058,0.050,1.2e-0,0.,tclev,thlev,tdlev)
	go to 16610
c-----------------------------------------------------------------------
1661	call beta(0.308,64.,0.,0.,t)
16610	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 6.03) go to 16611
	if(p.le. 8.84) go to 16612
	if(p.le.75.84) go to 16613
	if(p.le.79.11) go to 16614
	if(p.le.89.76) go to 16615
	if(p.le.94.78) go to 16616
	if(p.le.95.18) go to 16617
	if(p.le.98.45) go to 16618
	               go to 16619
16611	call nucltransK(1.538,0.050,1.2e-3,0.4e-4,tclev,thlev,tdlev)
	go to 12300
16612	call nucltransK(1.290,0.050,1.8e-3,0.1e-4,tclev,thlev,tdlev)
	go to 37100
16613	call nucltransK(0.845,0.050,4.0e-3,0.,tclev,thlev,tdlev)
	go to 81500
16614	call nucltransK(0.665,0.050,7.0e-3,0.,tclev,thlev,tdlev)
	go to 99600
16615	call nucltransK(0.613,0.050,1.2e-2,0.,tclev,thlev,tdlev)
	go to 10480
16616	call nucltransK(0.533,0.050,1.2e-2,0.,tclev,thlev,tdlev)
	go to 11280
16617	call nucltransK(0.420,0.050,1.4e-1,0.,tclev,thlev,tdlev)
	go to 12410
16618	call nucltransK(0.397,0.050,3.5e-2,0.,tclev,thlev,tdlev)
	go to 12640
16619	call nucltransK(0.130,0.050,1.0e-0,0.,tclev,thlev,tdlev)
	go to 15310
c-----------------------------------------------------------------------
1646	call beta(0.323,64.,0.,0.,t)
16460	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 1.26) go to 16461
	if(p.le. 2.57) go to 16462
	if(p.le. 3.97) go to 16463
	if(p.le. 6.77) go to 16464
	if(p.le.51.79) go to 16465
	if(p.le.56.52) go to 16466
	if(p.le.82.63) go to 16467
	if(p.le.84.79) go to 16468
	if(p.le.89.70) go to 16469
	if(p.le.91.41) go to 16470
	if(p.le.95.37) go to 16471
	if(p.le.99.02) go to 16472
	               go to 16473
16461	call nucltransK(1.523,0.050,1.2e-3,0.4e-4,tclev,thlev,tdlev)
	go to 12300
16462	call nucltransK(1.275,0.050,1.8e-3,0.1e-4,tclev,thlev,tdlev)
	go to 37100
16463	call nucltransK(0.928,0.050,3.5e-3,0.,tclev,thlev,tdlev)
	go to 71800
16464	call nucltransK(0.830,0.050,5.7e-3,0.,tclev,thlev,tdlev)
	go to 81500
16465	call nucltransK(0.650,0.050,7.3e-3,0.,tclev,thlev,tdlev)
	go to 99600
16466	call nucltransK(0.598,0.050,1.4e-2,0.,tclev,thlev,tdlev)
	go to 10480
16467	call nucltransK(0.518,0.050,1.3e-2,0.,tclev,thlev,tdlev)
	go to 11280
16468	call nucltransK(0.394,0.050,8.5e-3,0.,tclev,thlev,tdlev)
	go to 12520
16469	call nucltransK(0.382,0.050,3.4e-2,0.,tclev,thlev,tdlev)
	go to 12640
16470	call nucltransK(0.368,0.050,9.0e-3,0.,tclev,thlev,tdlev)
	go to 12770
16471	call nucltransK(0.352,0.050,3.5e-2,0.,tclev,thlev,tdlev)
	go to 12940
16472	call nucltransK(0.242,0.050,3.5e-2,0.,tclev,thlev,tdlev)
	go to 14040
16473	call nucltransK(0.228,0.050,1.4e-1,0.,tclev,thlev,tdlev)
	go to 14180
c-----------------------------------------------------------------------
1617	call beta(0.352,64.,0.,0.,t)
16170	thlev=0.
	p=100.*rnd1(d)
	if(p.le.43.76) go to 16171
	if(p.le.97.79) go to 16172
	if(p.le.97.80) go to 16173
	if(p.le.98.37) go to 16174
	if(p.le.99.00) go to 16175
	if(p.le.99.43) go to 16176
	if(p.le.99.73) go to 16177
	if(p.le.99.85) go to 16178
	               go to 16179
16171	call nucltransK(1.494,0.050,5.6e-4,0.5e-4,tclev,thlev,tdlev)
	go to 12300
16172	call nucltransK(1.246,0.050,7.6e-4,0.2e-4,tclev,thlev,tdlev)
	go to 37100
16173	call nucltransK(0.801,0.050,1.8e-3,0.,tclev,thlev,tdlev)
	go to 81500
16174	call nucltransK(0.621,0.050,2.8e-3,0.,tclev,thlev,tdlev)
	go to 99600
16175	call nucltransK(0.569,0.050,3.5e-3,0.,tclev,thlev,tdlev)
	go to 10480
16176	call nucltransK(0.488,0.050,5.0e-3,0.,tclev,thlev,tdlev)
	go to 11280
16177	call nucltransK(0.481,0.050,5.0e-3,0.,tclev,thlev,tdlev)
	go to 11360
16178	call nucltransK(0.375,0.050,4.0e-2,0.,tclev,thlev,tdlev)
	go to 12410
16179	call nucltransK(0.219,0.050,2.0e-1,0.,tclev,thlev,tdlev)
	go to 13980
c-----------------------------------------------------------------------
1560	call beta(0.409,64.,0.,0.,t)
15600	thlev=0.
	p=100.*rnd1(d)
	if(p.le.90.61) go to 15601
	if(p.le.90.62) go to 15602
	if(p.le.92.94) go to 15603
	if(p.le.98.83) go to 15604
	               go to 15605
15601	call nucltransK(1.189,0.050,1.1e-3,0.1e-4,tclev,thlev,tdlev)
	go to 37100
15602	call nucltransK(0.563,0.050,6.0e-2,0.,tclev,thlev,tdlev)
	go to 99600
15603	call nucltransK(0.296,0.050,1.6e-2,0.,tclev,thlev,tdlev)
	go to 12640
15604	call nucltransK(0.283,0.050,1.7e-2,0.,tclev,thlev,tdlev)
	go to 12770
15605	call nucltransK(0.163,0.050,5.0e-1,0.,tclev,thlev,tdlev)
	go to 13980
c-----------------------------------------------------------------------
1531	call beta(0.438,64.,0.,0.,t)
15310	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.99) go to 15311
	if(p.le. 4.80) go to 15312
	if(p.le.12.06) go to 15313
	if(p.le.52.17) go to 15314
	if(p.le.82.25) go to 15315
	if(p.le.90.87) go to 15316
	if(p.le.91.69) go to 15317
	if(p.le.95.70) go to 15318
	if(p.le.96.26) go to 15319
	if(p.le.96.75) go to 15320
	if(p.le.98.55) go to 15321
	if(p.le.99.59) go to 15322
	               go to 15323
15311	call nucltransK(1.531,0.050,1.1e-3,0.4e-4,tclev,thlev,tdlev)
	return
15312	call nucltransK(1.408,0.050,3.7e-3,0.2e-4,tclev,thlev,tdlev)
	go to 12300
15313	call nucltransK(1.161,0.050,2.2e-3,0.1e-4,tclev,thlev,tdlev)
	go to 37100
15314	call nucltransK(0.851,0.050,3.9e-3,0.,tclev,thlev,tdlev)
	go to 68100
15315	call nucltransK(0.716,0.050,1.3e-2,0.,tclev,thlev,tdlev)
	go to 81500
15316	call nucltransK(0.535,0.050,2.5e-2,0.,tclev,thlev,tdlev)
	go to 99600
15317	call nucltransK(0.484,0.050,1.6e-2,0.,tclev,thlev,tdlev)
	go to 10480
15318	call nucltransK(0.404,0.050,2.8e-2,0.,tclev,thlev,tdlev)
	go to 11280
15319	call nucltransK(0.290,0.050,1.8e-2,0.,tclev,thlev,tdlev)
	go to 12410
15320	call nucltransK(0.280,0.050,1.9e-2,0.,tclev,thlev,tdlev)
	go to 12520
15321	call nucltransK(0.267,0.050,8.0e-2,0.,tclev,thlev,tdlev)
	go to 12640
15322	call nucltransK(0.238,0.050,1.2e-1,0.,tclev,thlev,tdlev)
	go to 12940
15323	call nucltransK(0.117,0.050,2.0e-1,0.,tclev,thlev,tdlev)
	go to 14140
c-----------------------------------------------------------------------
15100	thlev=0.
	p=100.*rnd1(d)
	if(p.le.20.) go to 15101
	             go to 15102
15101	call nucltransK(1.510,0.050,6.0e-4,0.5e-4,tclev,thlev,tdlev)
	return
15102	call nucltransK(1.387,0.050,6.5e-4,0.3e-4,tclev,thlev,tdlev)
	go to 12300
c-----------------------------------------------------------------------
1418	call beta(0.551,64.,0.,0.,t)
14180	thlev=0.
	p=100.*rnd1(d)
	if(p.le.11.51) go to 14181
	if(p.le.20.34) go to 14182
	if(p.le.68.31) go to 14183
	if(p.le.69.27) go to 14184
	if(p.le.90.86) go to 14185
	if(p.le.91.87) go to 14186
	if(p.le.95.71) go to 14187
	if(p.le.96.56) go to 14188
	if(p.le.97.71) go to 14189
	if(p.le.98.30) go to 14190
	if(p.le.98.94) go to 14191
	if(p.le.99.45) go to 14192
	               go to 14193
14181	call nucltransK(1.418,0.050,1.4e-3,0.2e-4,tclev,thlev,tdlev)
	return
14182	call nucltransK(1.295,0.050,1.6e-3,0.1e-4,tclev,thlev,tdlev)
	go to 12300
14183	call nucltransK(1.047,0.050,2.5e-3,0.,tclev,thlev,tdlev)
	go to 37100
14184	call nucltransK(0.737,0.050,5.5e-3,0.,tclev,thlev,tdlev)
	go to 68100
14185	call nucltransK(0.603,0.050,3.8e-2,0.,tclev,thlev,tdlev)
	go to 81500
14186	call nucltransK(0.422,0.050,1.4e-1,0.,tclev,thlev,tdlev)
	go to 99600
14187	call nucltransK(0.371,0.050,3.2e-2,0.,tclev,thlev,tdlev)
	go to 10480
14188	call nucltransK(0.290,0.050,6.5e-2,0.,tclev,thlev,tdlev)
	go to 11280
14189	call nucltransK(0.236,0.050,1.2e-1,0.,tclev,thlev,tdlev)
	go to 11820
14190	call nucltransK(0.177,0.050,6.0e-2,0.,tclev,thlev,tdlev)
	go to 12410
14191	call nucltransK(0.167,0.050,7.0e-2,0.,tclev,thlev,tdlev)
	go to 12520
14192	call nucltransK(0.124,0.050,1.2e-0,0.,tclev,thlev,tdlev)
	go to 12940
14193	call nucltransK(0.123,0.050,1.2e-0,0.,tclev,thlev,tdlev)
	go to 12960
c-----------------------------------------------------------------------
14140	thlev=0.
	p=100.*rnd1(d)
	if(p.le.21.22) go to 14141
	if(p.le.97.01) go to 14142
	if(p.le.99.68) go to 14143
	if(p.le.99.79) go to 14144
	               go to 14145
14141	call nucltransK(1.414,0.050,6.1e-4,0.4e-4,tclev,thlev,tdlev)
	return
14142	call nucltransK(1.291,0.050,7.2e-4,0.2e-4,tclev,thlev,tdlev)
	go to 12300
14143	call nucltransK(0.599,0.050,3.5e-3,0.,tclev,thlev,tdlev)
	go to 81500
14144	call nucltransK(0.167,0.050,4.0e-1,0.,tclev,thlev,tdlev)
	go to 12520
14145	call nucltransK(0.120,0.050,1.8e-1,0.,tclev,thlev,tdlev)
	go to 12940
c-----------------------------------------------------------------------
14040	thlev=0.
	call nucltransK(1.033,0.050,1.1e-3,0.,tclev,thlev,tdlev)
	go to 37100
c-----------------------------------------------------------------------
1398	call beta(0.571,64.,0.,0.,t)
13980	thlev=0.
	p=100.*rnd1(d)
	if(p.le.96.877) go to 13981
	if(p.le.99.338) go to 13982
	if(p.le.99.876) go to 13983
	if(p.le.99.896) go to 13984
	if(p.le.99.902) go to 13985
	if(p.le.99.929) go to 13986
	                go to 13987
13981	call nucltransK(1.274,0.050,7.4e-4,0.2e-4,tclev,thlev,tdlev)
	go to 12300
13982	call nucltransK(0.582,0.050,3.4e-3,0.,tclev,thlev,tdlev)
	go to 81500
13983	call nucltransK(0.401,0.050,7.0e-2,0.,tclev,thlev,tdlev)
	go to 99600
13984	call nucltransK(0.270,0.050,2.2e-2,0.,tclev,thlev,tdlev)
	go to 11280
13985	call nucltransK(0.260,0.050,2.3e-2,0.,tclev,thlev,tdlev)
	go to 11360
13986	call nucltransK(0.156,0.050,9.0e-2,0.,tclev,thlev,tdlev)
	go to 12410
13987	call nucltransK(0.146,0.050,9.5e-2,0.,tclev,thlev,tdlev)
	go to 12520
c-----------------------------------------------------------------------
12960	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.73) go to 12961
	if(p.le.73.72) go to 12962
	if(p.le.74.45) go to 12963
	if(p.le.78.83) go to 12964
	               go to 12965
12961	call electron(1.245,tclev,thlev,tdlev)         ! only ec
	call gamma(0.050,0.,0.,tdlev)
	return
12962	call nucltransK(1.173,0.050,2.2e-3,0.,tclev,thlev,tdlev)
	go to 12300
12963	call electron(0.565,tclev,thlev,tdlev)         ! only ec
	call gamma(0.050,0.,0.,tdlev)
	go to 68100
12964	call nucltransK(0.480,0.050,1.6e-2,0.,tclev,thlev,tdlev)
	go to 81500
12965	call nucltransK(0.299,0.050,6.0e-2,0.,tclev,thlev,tdlev)
	go to 99600
c-----------------------------------------------------------------------
12940	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.51) go to 12941
	if(p.le.47.79) go to 12942
	if(p.le.83.25) go to 12943
	               go to 12944
12941	call nucltransK(1.294,0.050,1.8e-3,0.1e-4,tclev,thlev,tdlev)
	return
12942	call nucltransK(1.171,0.050,2.2e-3,0.1e-4,tclev,thlev,tdlev)
	go to 12300
12943	call nucltransK(0.923,0.050,3.5e-3,0.,tclev,thlev,tdlev)
	go to 37100
12944	call nucltransK(0.112,0.050,1.7e-0,0.,tclev,thlev,tdlev)
	go to 11820
c-----------------------------------------------------------------------
12770	thlev=0.
	call nucltransK(0.906,0.050,1.4e-3,0.,tclev,thlev,tdlev)
	go to 37100
c-----------------------------------------------------------------------
1264	call beta(0.705,64.,0.,0.,t)
12640	thlev=0.
	p=100.*rnd1(d)
	if(p.le.30.09) go to 12641
	if(p.le.95.92) go to 12642
	if(p.le.97.76) go to 12643
	               go to 12644
12641	call nucltransK(1.141,0.050,2.1e-3,0.,tclev,thlev,tdlev)
	go to 12300
12642	call nucltransK(0.893,0.050,3.7e-3,0.,tclev,thlev,tdlev)
	go to 37100
12643	call nucltransK(0.546,0.050,1.2e-2,0.,tclev,thlev,tdlev)
	go to 71800
12644	call nucltransK(0.267,0.050,9.5e-2,0.,tclev,thlev,tdlev)
	go to 99600
c-----------------------------------------------------------------------
1252	call beta(0.717,64.,0.,0.,t)
12520	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 3.76) go to 12521
	if(p.le.80.51) go to 12522
	               go to 12523
12521	call nucltransK(1.252,0.050,3.5e-3,0.,tclev,thlev,tdlev)
	return
12522	call nucltransK(1.129,0.050,9.1e-4,0.1e-4,tclev,thlev,tdlev)
	go to 12300
12523	call nucltransK(0.881,0.050,1.5e-3,0.,tclev,thlev,tdlev)
	go to 37100
c-----------------------------------------------------------------------
12410	thlev=0.
	p=100.*rnd1(d)
	if(p.le.54.62) go to 12411
	if(p.le.99.02) go to 12412
	if(p.le.99.68) go to 12413
	               go to 12414
12411	call nucltransK(1.241,0.050,7.7e-4,0.2e-4,tclev,thlev,tdlev)
	return
12412	call nucltransK(1.118,0.050,9.3e-4,0.1e-4,tclev,thlev,tdlev)
	go to 12300
12413	call nucltransK(0.561,0.050,4.0e-3,0.,tclev,thlev,tdlev)
	go to 68100
12414	call nucltransK(0.426,0.050,7.0e-3,0.,tclev,thlev,tdlev)
	go to 81500
c-----------------------------------------------------------------------
11820	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.21) go to 11821
	if(p.le.84.00) go to 11822
	if(p.le.84.84) go to 11823
	               go to 11824
11821	call electron(1.132,tclev,thlev,tdlev)
	call gamma(0.050,0.,0.,tdlev)
	return
11822	call nucltransK(1.059,0.050,2.5e-3,0.,tclev,thlev,tdlev)
	go to 12300
11823	call electron(0.451,tclev,thlev,tdlev)
	call gamma(0.050,0.,0.,tdlev)
	go to 68100
11824	call nucltransK(0.367,0.050,3.3e-2,0.,tclev,thlev,tdlev)
	go to 81500
c-----------------------------------------------------------------------
11360	thlev=0.
	p=100.*rnd1(d)
	if(p.le.71.94) go to 11361
	               go to 11362
11361	call nucltransK(1.136,0.050,2.2e-3,0.,tclev,thlev,tdlev)
	return
11362	call nucltransK(1.013,0.050,2.8e-3,0.,tclev,thlev,tdlev)
	go to 12300
c-----------------------------------------------------------------------
1128	call beta(0.841,64.,0.,0.,t)
11280	thlev=0.
	p=100.*rnd1(d)
	if(p.le.79.629) go to 11281
	if(p.le.99.855) go to 11282
	if(p.le.99.938) go to 11283
	if(p.le.99.987) go to 11284
	                go to 11285
11281	call nucltransK(1.005,0.050,2.8e-3,0.,tclev,thlev,tdlev)
	go to 12300
11282	call nucltransK(0.757,0.050,5.2e-3,0.,tclev,thlev,tdlev)
	go to 37100
11283	call nucltransK(0.312,0.050,5.5e-2,0.,tclev,thlev,tdlev)
	go to 81500
11284	call nucltransK(0.132,0.050,9.5e-1,0.,tclev,thlev,tdlev)
	go to 99600
11285	call nucltransK(0.080,0.050,6.0e-0,0.,tclev,thlev,tdlev)
	go to 10480
c-----------------------------------------------------------------------
10480	thlev=0.
	p=100.*rnd1(d)
	if(p.le.23.98) go to 10481
	if(p.le.86.75) go to 10482
	if(p.le.90.58) go to 10483
	               go to 10484
10481	call nucltransK(0.925,0.050,3.3e-3,0.,tclev,thlev,tdlev)
	go to 12300
10482	call nucltransK(0.677,0.050,5.1e-2,0.,tclev,thlev,tdlev)
	go to 37100
10483	call nucltransK(0.330,0.050,4.5e-2,0.,tclev,thlev,tdlev)
	go to 71800
10484	call nucltransK(0.232,0.050,1.4e-1,0.,tclev,thlev,tdlev)
	go to 81500
c-----------------------------------------------------------------------
996	call beta(0.973,64.,0.,0.,t)
99600	thlev=0.
	p=100.*rnd1(d)
	if(p.le.46.43) go to 99601
	if(p.le.98.59) go to 99602
	if(p.le.99.95) go to 99603
	if(p.le.99.98) go to 99604
	               go to 99605
99601	call nucltransK(0.996,0.050,2.8e-3,0.,tclev,thlev,tdlev)
	return
99602	call nucltransK(0.873,0.050,3.7e-3,0.,tclev,thlev,tdlev)
	go to 12300
99603	call nucltransK(0.625,0.050,8.0e-3,0.,tclev,thlev,tdlev)
	go to 37100
99604	call nucltransK(0.315,0.050,5.2e-2,0.,tclev,thlev,tdlev)
	go to 68100
99605	call nucltransK(0.181,0.050,3.5e-1,0.,tclev,thlev,tdlev)
	go to 81500
c-----------------------------------------------------------------------
815	call beta(1.154,64.,0.,0.,t)
81500	thlev=6.9e-12
	p=100.*rnd1(d)
	if(p.le.17.86) go to 81501
	if(p.le.80.18) go to 81502
	if(p.le.99.75) go to 81503
	               go to 81504
81501	call nucltransK(0.816,0.050,4.3e-3,0.,tclev,thlev,tdlev)
	return
81502	call nucltransK(0.692,0.050,4.6e-2,0.,tclev,thlev,tdlev)
	go to 12300
81503	call nucltransK(0.444,0.050,1.9e-2,0.,tclev,thlev,tdlev)
	go to 37100
81504	call nucltransK(0.135,0.050,8.7e-1,0.,tclev,thlev,tdlev)
	go to 68100
c-----------------------------------------------------------------------
71800	thlev=7.8e-12
	call nucltransK(0.347,0.050,3.9e-2,0.,tclev,thlev,tdlev)
	go to 37100
c-----------------------------------------------------------------------
68100	thlev=4.0e-12
	p=100.*rnd1(d)
	if(p.le. 2.06) go to 68101
	               go to 68102
68101	call electron(0.631,tclev,thlev,tdlev)
	call gamma(0.050,0.,0.,tdlev)
	return
68102	call nucltransK(0.558,0.050,1.1e-2,0.,tclev,thlev,tdlev)
	go to 12300
c-----------------------------------------------------------------------
371	call beta(1.598,64.,0.,0.,t)
37100	thlev=45.2e-12
	call nucltransK(0.248,0.050,1.1e-1,0.,tclev,thlev,tdlev)
	go to 12300
c-----------------------------------------------------------------------
c123	call beta(1.846,64.,0.,0.,t)
c correction to the 1st forbidden non-unique shape in accordance with
c L.M.Langer et al., PR 119(1960)1308
123	call beta1(1.846,64.,0.,0.,t,0.,-0.2280,0.04465,0.)
12300	thlev=1.186e-9
	call nucltransK(0.123,0.050,1.2e-0,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
	end
c
c end of Eu154.f
c
c Local Variables: --
c mode: fortran --
c End: --
