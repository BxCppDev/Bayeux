c Te133m.f 
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
        subroutine Te133m(tcnuc,tdnuc)
c Model for scheme of Te133m decay (S.Rab, Nucl. Data Sheets 
c 75(1995)491; E_exc=334 keV).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 18.12.2003.
	thnuc=3324.
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pdecay=100.*rnd1(d)
c 17.5% IT to 133Te(g.s.)
	if(pdecay.le.17.5) then
	   call nucltransK(0.334,0.033,1.431,0.,tclev,thlev,tdlev)
	   return
	endif
c 82.5% beta decay to 133I
	pbeta=100.*rnd1(d)
	if(pbeta.le. 1.77) go to  3051
	if(pbeta.le. 2.70) go to  3029
	if(pbeta.le. 3.03) go to  2975
	if(pbeta.le. 3.16) go to  2968
	if(pbeta.le. 4.25) go to  2881
	if(pbeta.le. 5.22) go to  2826
	if(pbeta.le. 6.02) go to  2808
	if(pbeta.le. 7.59) go to  2784
	if(pbeta.le.10.97) go to  2686
	if(pbeta.le.31.35) go to  2596
	if(pbeta.le.36.78) go to  2556
	if(pbeta.le.37.87) go to  2516
	if(pbeta.le.39.00) go to  2506
	if(pbeta.le.41.05) go to  2500
	if(pbeta.le.41.77) go to  2483
	if(pbeta.le.45.63) go to  2467
	if(pbeta.le.46.96) go to  2445
	if(pbeta.le.47.36) go to  2427
	if(pbeta.le.48.10) go to  2419
	if(pbeta.le.55.22) go to  2372
	if(pbeta.le.56.31) go to  2262
	if(pbeta.le.57.64) go to  2249
	if(pbeta.le.59.21) go to  2212
	if(pbeta.le.61.14) go to  2142
	if(pbeta.le.63.07) go to  2049
	if(pbeta.le.66.21) go to  2005
	if(pbeta.le.72.72) go to  1991
	if(pbeta.le.73.32) go to  1975
	if(pbeta.le.74.41) go to  1943
	if(pbeta.le.78.03) go to  1886
	if(pbeta.le.86.59) go to  1777
	if(pbeta.le.88.28) go to  1647
	if(pbeta.le.91.06) go to  1560
	if(pbeta.le.91.66) go to  1516
	if(pbeta.le.92.87) go to  1455
	if(pbeta.le.93.96) go to   915
	if(pbeta.le.95.17) go to   913
	                   go to 10000
c-----------------------------------------------------------------------
3051	call beta(0.203,53.,0.,0.,t)
30510	thlev=0.
	p=100.*rnd1(d)
	if(p.le.21.62) go to 30511 
	if(p.le.29.05) go to 30512 
	               go to 30513
30511	call nucltransK(3.051,0.033,2.0e-5,18.1e-4,tclev,thlev,tdlev)
	return
30512	call nucltransK(1.405,0.033,3.5e-4,0.3e-4,tclev,thlev,tdlev)
	go to 16470
30513	call nucltransK(0.535,0.033,8.0e-3,0.,tclev,thlev,tdlev)
	go to 25160
c-----------------------------------------------------------------------
3029	call beta(0.225,53.,0.,0.,t)
30290	thlev=0.
	p=100.*rnd1(d)
	if(p.le.35.90) go to 30291 
	if(p.le.78.21) go to 30292 
	               go to 30293
30291	call nucltransK(1.574,0.033,2.6e-4,0.7e-4,tclev,thlev,tdlev)
	go to 14550
30292	call nucltransK(1.252,0.033,4.0e-4,0.2e-4,tclev,thlev,tdlev)
	go to 17770
30293	call nucltransK(1.054,0.033,7.0e-4,0.,tclev,thlev,tdlev)
	go to 19750
c-----------------------------------------------------------------------
2975	call beta(0.279,53.,0.,0.,t)
29750	thlev=0.
	p=100.*rnd1(d)
	if(p.le.31.25) go to 29751 
	               go to 29752
29751	call nucltransK(2.062,0.033,1.4e-4,4.8e-4,tclev,thlev,tdlev)
	go to 91300
29752	call nucltransK(1.198,0.033,5.0e-4,0.1e-4,tclev,thlev,tdlev)
	go to 17770
c-----------------------------------------------------------------------
2968	call beta(0.286,53.,0.,0.,t)
29680	thlev=0.
	call nucltransK(2.968,0.033,8.0e-5,15.1e-4,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
2881	call beta(0.373,53.,0.,0.,t)
28810	thlev=0.
	p=100.*rnd1(d)
	if(p.le.17.89) go to 28811 
	if(p.le.47.36) go to 28812 
	if(p.le.76.83) go to 28813 
	               go to 28814
28811	call nucltransK(1.968,0.033,1.7e-4,3.2e-4,tclev,thlev,tdlev)
	go to 91300
28812	call nucltransK(0.890,0.033,3.1e-3,0.,tclev,thlev,tdlev)
	go to 19910
28813	call nucltransK(0.632,0.033,7.0e-3,0.,tclev,thlev,tdlev)
	go to 22490
28814	call nucltransK(0.285,0.033,5.0e-2,0.,tclev,thlev,tdlev)
	go to 25960
c-----------------------------------------------------------------------
2826	call beta(0.428,53.,0.,0.,t)
28260	thlev=0.
	p=100.*rnd1(d)
	if(p.le.16.10) go to 28261 
	if(p.le.23.00) go to 28262 
	if(p.le.55.18) go to 28263 
	if(p.le.67.82) go to 28264 
	               go to 28265
28261	call nucltransK(2.826,0.033,9.0e-5,13.7e-4,tclev,thlev,tdlev)
	return
28262	call nucltransK(1.914,0.033,1.8e-4,2.7e-4,tclev,thlev,tdlev)
	go to 91300
28263	call nucltransK(1.372,0.033,3.5e-4,0.3e-4,tclev,thlev,tdlev)
	go to 14550
28264	call nucltransK(0.852,0.033,1.0e-3,0.,tclev,thlev,tdlev)
	go to 19750
28265	call nucltransK(0.326,0.033,3.6e-2,0.,tclev,thlev,tdlev)
	go to 25000
c-----------------------------------------------------------------------
2808	call beta(0.446,53.,0.,0.,t)
28080	thlev=0.
	p=100.*rnd1(d)
	if(p.le.16.42) go to 28081 
	if(p.le.58.21) go to 28082 
	               go to 28083
28081	call nucltransK(1.104,0.033,6.0e-4,0.,tclev,thlev,tdlev)
	go to 17040
28082	call nucltransK(0.308,0.033,1.0e-2,0.,tclev,thlev,tdlev)
	go to 25000
28083	call nucltransK(0.251,0.033,7.5e-2,0.,tclev,thlev,tdlev)
	go to 25560
c-----------------------------------------------------------------------
2784	call beta(0.470,53.,0.,0.,t)
27840	thlev=0.
	p=100.*rnd1(d)
	if(p.le.42.97) go to 27841 
	if(p.le.94.53) go to 27842 
	               go to 27843
27841	call nucltransK(1.871,0.033,1.8e-4,2.2e-4,tclev,thlev,tdlev)
	go to 91300
27842	call nucltransK(1.008,0.033,7.0e-4,0.,tclev,thlev,tdlev)
	go to 17770
27843	call nucltransK(0.734,0.033,1.4e-3,0.,tclev,thlev,tdlev)
	go to 20490
c-----------------------------------------------------------------------
2686	call beta(0.568,53.,0.,0.,t)
26860	thlev=0.
	p=100.*rnd1(d)
	if(p.le.23.49) go to 26861 
	if(p.le.62.64) go to 26862 
	if(p.le.70.47) go to 26863 
	if(p.le.74.38) go to 26864 
	if(p.le.88.26) go to 26865 
	               go to 26866
26861	call nucltransK(1.773,0.033,1.9e-4,1.4e-4,tclev,thlev,tdlev)
	go to 91300
26862	call nucltransK(0.801,0.033,1.1e-3,0.,tclev,thlev,tdlev)
	go to 18860
26863	call nucltransK(0.637,0.033,1.7e-3,0.,tclev,thlev,tdlev)
	go to 20490
26864	call nucltransK(0.475,0.033,3.5e-3,0.,tclev,thlev,tdlev)
	go to 22120
26865	call nucltransK(0.314,0.033,1.0e-2,0.,tclev,thlev,tdlev)
	go to 23720
26866	call nucltransK(0.241,0.033,2.0e-2,0.,tclev,thlev,tdlev)
	go to 24450
c-----------------------------------------------------------------------
2596	call beta(0.658,53.,0.,0.,t)
25960	thlev=0.
	p=100.*rnd1(d)
	if(p.le.24.04) go to 25961 
	if(p.le.27.23) go to 25962 
	if(p.le.31.05) go to 25963 
	if(p.le.37.13) go to 25964 
	if(p.le.41.94) go to 25965 
	if(p.le.42.92) go to 25966 
	if(p.le.47.09) go to 25967 
	if(p.le.61.18) go to 25968 
	if(p.le.64.71) go to 25969 
	if(p.le.67.61) go to 25970 
	if(p.le.74.97) go to 25971 
	if(p.le.75.95) go to 25972 
	if(p.le.79.77) go to 25973
	if(p.le.99.02) go to 25974
	               go to 25975
25961	call nucltransK(1.683,0.033,2.5e-4,1.0e-4,tclev,thlev,tdlev)
	go to 91300
25962	call nucltransK(1.080,0.033,6.0e-4,0.,tclev,thlev,tdlev)
	go to 15160
25963	call nucltransK(0.949,0.033,8.0e-4,0.,tclev,thlev,tdlev)
	go to 16470
25964	call nucltransK(0.891,0.033,9.0e-4,0.,tclev,thlev,tdlev)
	go to 17040
25965	call nucltransK(0.889,0.033,9.0e-4,0.,tclev,thlev,tdlev)
	go to 17070
25966	call nucltransK(0.819,0.033,3.7e-3,0.,tclev,thlev,tdlev)
	go to 17770
25967	call nucltransK(0.710,0.033,1.4e-3,0.,tclev,thlev,tdlev)
	go to 18860
25968	call nucltransK(0.703,0.033,1.4e-3,0.,tclev,thlev,tdlev)
	go to 18930
25969	call nucltransK(0.653,0.033,1.7e-3,0.,tclev,thlev,tdlev)
	go to 19430
25970	call nucltransK(0.621,0.033,1.8e-3,0.,tclev,thlev,tdlev)
	go to 19750
25971	call nucltransK(0.605,0.033,7.7e-3,0.,tclev,thlev,tdlev)
	go to 19910
25972	call nucltransK(0.384,0.033,2.5e-2,0.,tclev,thlev,tdlev)
	go to 22120
25973	call nucltransK(0.347,0.033,3.0e-2,0.,tclev,thlev,tdlev)
	go to 22490
25974	call nucltransK(0.334,0.033,3.4e-2,0.,tclev,thlev,tdlev)
	go to 22620
25975	call nucltransK(0.224,0.033,1.0e-1,0.,tclev,thlev,tdlev)
	go to 23720
c-----------------------------------------------------------------------
2556	call beta(0.698,53.,0.,0.,t)
25560	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 6.82) go to 25561 
	if(p.le.15.08) go to 25562 
	if(p.le.26.44) go to 25563 
	if(p.le.63.01) go to 25564 
	if(p.le.65.28) go to 25565 
	if(p.le.75.61) go to 25566 
	if(p.le.77.06) go to 25567 
	if(p.le.91.94) go to 25568 
	if(p.le.96.49) go to 25569 
	               go to 25570
25561	call nucltransK(1.644,0.033,2.6e-4,0.9e-4,tclev,thlev,tdlev)
	go to 91300
25562	call nucltransK(0.996,0.033,7.0e-4,0.,tclev,thlev,tdlev)
	go to 15600
25563	call nucltransK(0.827,0.033,3.7e-3,0.,tclev,thlev,tdlev)
	go to 17290
25564	call nucltransK(0.780,0.033,4.2e-3,0.,tclev,thlev,tdlev)
	go to 17770
25565	call nucltransK(0.663,0.033,6.0e-3,0.,tclev,thlev,tdlev)
	go to 18930
25566	call nucltransK(0.581,0.033,8.2e-3,0.,tclev,thlev,tdlev)
	go to 19750
25567	call nucltransK(0.565,0.033,8.3e-3,0.,tclev,thlev,tdlev)
	go to 19910
25568	call nucltransK(0.344,0.033,3.2e-2,0.,tclev,thlev,tdlev)
	go to 22120
25569	call nucltransK(0.295,0.033,4.8e-2,0.,tclev,thlev,tdlev)
	go to 22620
25570	call nucltransK(0.185,0.033,1.6e-1,0.,tclev,thlev,tdlev)
	go to 23720
c-----------------------------------------------------------------------
2516	call beta(0.738,53.,0.,0.,t)
25160	thlev=0.
	p=100.*rnd1(d)
	if(p.le.30.50) go to 25161 
	if(p.le.72.00) go to 25162 
	if(p.le.86.00) go to 25163 
	               go to 25164
25161	call nucltransK(0.740,0.033,4.8e-3,0.,tclev,thlev,tdlev)
	go to 17770
25162	call nucltransK(0.719,0.033,4.9e-3,0.,tclev,thlev,tdlev)
	go to 17970
25163	call nucltransK(0.623,0.033,6.0e-3,0.,tclev,thlev,tdlev)
	go to 18930
25164	call nucltransK(0.526,0.033,1.1e-2,0.,tclev,thlev,tdlev)
	go to 19910
c-----------------------------------------------------------------------
2506	call beta(0.748,53.,0.,0.,t)
25060	thlev=0.
	p=100.*rnd1(d)
	if(p.le.64.89) go to 25061 
	               go to 25062
25061	call nucltransK(0.945,0.033,2.6e-3,0.,tclev,thlev,tdlev)
	go to 15600
25062	call nucltransK(0.244,0.033,7.5e-2,0.,tclev,thlev,tdlev)
	go to 22620
c-----------------------------------------------------------------------
2500	call beta(0.754,53.,0.,0.,t)
25000	thlev=0.
	p=100.*rnd1(d)
	if(p.le.62.53) go to 25001 
	if(p.le.67.31) go to 25002 
	if(p.le.72.09) go to 25003 
	if(p.le.84.25) go to 25004 
	if(p.le.91.63) go to 25005 
	               go to 25006
25001	call nucltransK(1.588,0.033,7.0e-4,1.1e-4,tclev,thlev,tdlev)
	go to 91300
25002	call nucltransK(0.796,0.033,1.1e-3,0.,tclev,thlev,tdlev)
	go to 17040
25003	call nucltransK(0.793,0.033,1.1e-3,0.,tclev,thlev,tdlev)
	go to 17070
25004	call nucltransK(0.724,0.033,5.0e-3,0.,tclev,thlev,tdlev)
	go to 17770
25005	call nucltransK(0.607,0.033,2.0e-3,0.,tclev,thlev,tdlev)
	go to 18930
25006	call nucltransK(0.495,0.033,3.0e-3,0.,tclev,thlev,tdlev)
	go to 20050
c-----------------------------------------------------------------------
2483	call beta(0.771,53.,0.,0.,t)
24830	thlev=0.
	p=100.*rnd1(d)
	if(p.le.11.66) go to 24831 
	if(p.le.31.09) go to 24832 
	               go to 24833
24831	call nucltransK(2.483,0.033,2.2e-4,4.5e-4,tclev,thlev,tdlev)
	return
24832	call nucltransK(1.570,0.033,7.0e-4,1.1e-4,tclev,thlev,tdlev)
	go to 91300
24833	call nucltransK(1.174,0.033,1.5e-3,0.3e-4,tclev,thlev,tdlev)
	go to 13070
c-----------------------------------------------------------------------
2467	call beta(0.787,53.,0.,0.,t)
24670	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 5.30) go to 24671 
	if(p.le.27.73) go to 24672 
	if(p.le.51.72) go to 24673 
	               go to 24674
24671	call nucltransK(1.552,0.033,7.0e-4,1.1e-4,tclev,thlev,tdlev)
	go to 91500
24672	call nucltransK(0.574,0.033,8.5e-3,0.,tclev,thlev,tdlev)
	go to 18930
24673	call nucltransK(0.493,0.033,1.2e-2,0.,tclev,thlev,tdlev)
	go to 19750
24674	call nucltransK(0.462,0.033,1.4e-2,0.,tclev,thlev,tdlev)
	go to 20050
c-----------------------------------------------------------------------
2445	call beta(0.809,53.,0.,0.,t)
24450	thlev=0.
	p=100.*rnd1(d)
	if(p.le.75.00) go to 24451 
	               go to 24452
24451	call nucltransK(0.885,0.033,9.5e-4,0.,tclev,thlev,tdlev)
	go to 15600
24452	call nucltransK(0.629,0.033,7.0e-3,0.,tclev,thlev,tdlev)
	go to 18170
c-----------------------------------------------------------------------
2427	call beta(0.827,53.,0.,0.,t)
24270	thlev=0.
	call nucltransK(0.178,0.033,1.7e-1,0.,tclev,thlev,tdlev)
	go to 22490
c-----------------------------------------------------------------------
2419	call beta(0.835,53.,0.,0.,t)
24190	thlev=0.
	p=100.*rnd1(d)
	if(p.le.45.91) go to 24191 
	if(p.le.63.94) go to 24192 
	if(p.le.81.97) go to 24193 
	               go to 24194
24191	call nucltransK(1.506,0.033,9.0e-4,0.9e-4,tclev,thlev,tdlev)
	go to 91300
24192	call nucltransK(0.859,0.033,2.9e-3,0.,tclev,thlev,tdlev)
	go to 15600
24193	call nucltransK(0.415,0.033,2.0e-2,0.,tclev,thlev,tdlev)
	go to 20050
24194	call nucltransK(0.158,0.033,2.5e-1,0.,tclev,thlev,tdlev)
	go to 22620
c-----------------------------------------------------------------------
2372	call beta(0.882,53.,0.,0.,t)
23720	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 2.55) go to 23721 
	if(p.le. 4.20) go to 23722 
	if(p.le. 5.85) go to 23723 
	if(p.le.19.06) go to 23724 
	if(p.le.37.37) go to 23725 
	if(p.le.39.02) go to 23726 
	if(p.le.53.13) go to 23727 
	if(p.le.86.29) go to 23728 
	if(p.le.97.10) go to 23729 
	if(p.le.98.75) go to 23730 
	               go to 23731
23721	call nucltransK(1.459,0.033,2.5e-4,0.5e-4,tclev,thlev,tdlev)
	go to 91300
23722	call nucltransK(1.456,0.033,2.5e-4,0.5e-4,tclev,thlev,tdlev)
	go to 91500
23723	call nucltransK(0.724,0.033,1.3e-2,0.,tclev,thlev,tdlev)
	go to 16470
23724	call nucltransK(0.642,0.033,5.0e-3,0.,tclev,thlev,tdlev)
	go to 17290
23725	call nucltransK(0.574,0.033,9.0e-3,0.,tclev,thlev,tdlev)
	go to 17970
23726	call nucltransK(0.555,0.033,7.0e-3,0.,tclev,thlev,tdlev)
	go to 18170
23727	call nucltransK(0.479,0.033,3.5e-3,0.,tclev,thlev,tdlev)
	go to 18930
23728	call nucltransK(0.429,0.033,4.5e-3,0.,tclev,thlev,tdlev)
	go to 19430
23729	call nucltransK(0.397,0.033,5.2e-3,0.,tclev,thlev,tdlev)
	go to 19750
23730	call nucltransK(0.322,0.033,9.0e-3,0.,tclev,thlev,tdlev)
	go to 20490
23731	call nucltransK(0.110,0.033,1.6e-1,0.,tclev,thlev,tdlev)
	go to 22620
c-----------------------------------------------------------------------
2262	call beta(0.992,53.,0.,0.,t)
22620	thlev=0.
	p=100.*rnd1(d)
	if(p.le.29.62) go to 22621 
	if(p.le.47.12) go to 22622 
	if(p.le.52.69) go to 22623 
	if(p.le.93.44) go to 22624 
	if(p.le.95.63) go to 22625 
	               go to 22626
22621	call nucltransK(1.349,0.033,5.0e-4,0.3e-4,tclev,thlev,tdlev)
	go to 91300
22622	call nucltransK(0.532,0.033,1.1e-2,0.,tclev,thlev,tdlev)
	go to 17290
22623	call nucltransK(0.464,0.033,3.5e-3,0.,tclev,thlev,tdlev)
	go to 17970
22624	call nucltransK(0.445,0.033,1.6e-2,0.,tclev,thlev,tdlev)
	go to 18170
22625	call nucltransK(0.369,0.033,6.0e-3,0.,tclev,thlev,tdlev)
	go to 18930
22626	call nucltransK(0.319,0.033,4.0e-2,0.,tclev,thlev,tdlev)
	go to 19430
c-----------------------------------------------------------------------
2249	call beta(1.005,53.,0.,0.,t)
22490	thlev=0.
	p=100.*rnd1(d)
	if(p.le.34.30) go to 22491 
	if(p.le.54.96) go to 22492 
	if(p.le.81.82) go to 22493 
	               go to 22494
22491	call nucltransK(0.472,0.033,1.3e-2,0.,tclev,thlev,tdlev)
	go to 17770
22492	call nucltransK(0.363,0.033,7.0e-3,0.,tclev,thlev,tdlev)
	go to 18860
22493	call nucltransK(0.355,0.033,7.0e-3,0.,tclev,thlev,tdlev)
	go to 18930
22494	call nucltransK(0.258,0.033,6.5e-2,0.,tclev,thlev,tdlev)
	go to 19910
c-----------------------------------------------------------------------
2212	call beta(1.042,53.,0.,0.,t)
22120	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 7.42) go to 22121 
	if(p.le.60.70) go to 22122 
	if(p.le.89.52) go to 22123 
	               go to 22124
22121	call nucltransK(1.299,0.033,4.0e-4,0.3e-4,tclev,thlev,tdlev)
	go to 91300
22122	call nucltransK(0.435,0.033,1.7e-2,0.,tclev,thlev,tdlev)
	go to 17770
22123	call nucltransK(0.413,0.033,1.6e-2,0.,tclev,thlev,tdlev)
	go to 17990
22124	call nucltransK(0.221,0.033,1.0e-1,0.,tclev,thlev,tdlev)
	go to 19910
c-----------------------------------------------------------------------
2142	call beta(1.112,53.,0.,0.,t)
21420	thlev=0.
	p=100.*rnd1(d)
	if(p.le.12.99) go to 21421 
	if(p.le.23.03) go to 21422 
	if(p.le.24.98) go to 21423 
	if(p.le.63.96) go to 21424 
	if(p.le.72.82) go to 21425 
	               go to 21426
21421	call nucltransK(1.230,0.033,1.5e-3,0.4e-4,tclev,thlev,tdlev)
	go to 91300
21422	call nucltransK(1.228,0.033,1.5e-3,0.4e-4,tclev,thlev,tdlev)
	go to 91500
21423	call nucltransK(0.249,0.033,7.5e-2,0.,tclev,thlev,tdlev)
	go to 18930
21424	call nucltransK(0.151,0.033,1.8e-2,0.,tclev,thlev,tdlev)
	go to 19910
21425	call nucltransK(0.137,0.033,4.0e-1,0.,tclev,thlev,tdlev)
	go to 20050
21426	call nucltransK(0.092,0.033,1.3,0.,tclev,thlev,tdlev)
	go to 20490
c-----------------------------------------------------------------------
2049	call beta(1.205,53.,0.,0.,t)
20490	thlev=0.
	p=100.*rnd1(d)
	if(p.le.50.00) go to 20491 
	if(p.le.61.48) go to 20492 
	if(p.le.75.00) go to 20493 
	if(p.le.90.98) go to 20494 
	               go to 20495
20491	call nucltransK(2.049,0.033,1.5e-4,2.2e-4,tclev,thlev,tdlev)
	return
20492	call nucltransK(1.137,0.033,1.7e-3,0.3e-4,tclev,thlev,tdlev)
	go to 91300
20493	call nucltransK(1.135,0.033,1.7e-3,0.3e-4,tclev,thlev,tdlev)
	go to 91500
20494	call nucltransK(0.743,0.033,3.5e-3,0.,tclev,thlev,tdlev)
	go to 13070
20495	call nucltransK(0.346,0.033,3.0e-2,0.,tclev,thlev,tdlev)
	go to 17040
c-----------------------------------------------------------------------
2005	call beta(1.249,53.,0.,0.,t)
20050	thlev=0.
	p=100.*rnd1(d)
	if(p.le.72.63) go to 20051 
	if(p.le.72.86) go to 20052 
	if(p.le.75.21) go to 20053 
	if(p.le.95.30) go to 20054 
	if(p.le.97.65) go to 20055 
	               go to 20056
20051	call nucltransK(2.005,0.033,8.0e-4,2.3e-4,tclev,thlev,tdlev)
	return
20052	call nucltransK(1.693,0.033,4.5e-4,0.7e-4,tclev,thlev,tdlev)
	go to 31200
20053	call nucltransK(1.091,0.033,2.0e-3,0.,tclev,thlev,tdlev)
	go to 91500
20054	call nucltransK(0.698,0.033,5.5e-3,0.,tclev,thlev,tdlev)
	go to 13070
20055	call nucltransK(0.119,0.033,5.0e-1,0.,tclev,thlev,tdlev)
	go to 18860
20056	call nucltransK(0.112,0.033,5.5e-1,0.,tclev,thlev,tdlev)
	go to 18930
c-----------------------------------------------------------------------
1991	call beta(1.263,53.,0.,0.,t)
19910	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 1.96) go to 19911 
	if(p.le.91.68) go to 19912 
	if(p.le.98.47) go to 19913 
	               go to 19914
19911	call nucltransK(1.078,0.033,6.0e-4,0.,tclev,thlev,tdlev)
	go to 91300
19912	call nucltransK(0.262,0.033,6.5e-2,0.,tclev,thlev,tdlev)
	go to 17290
19913	call nucltransK(0.193,0.033,3.5e-2,0.,tclev,thlev,tdlev)
	go to 17970
19914	call nucltransK(0.098,0.033,2.2e-1,0.,tclev,thlev,tdlev)
	go to 18930
c-----------------------------------------------------------------------
1975	call beta(1.279,53.,0.,0.,t)
19750	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 1.13) go to 19751 
	if(p.le.49.41) go to 19752 
	if(p.le.51.15) go to 19753 
	if(p.le.59.29) go to 19754 
	if(p.le.62.49) go to 19755 
	if(p.le.67.43) go to 19756 
	if(p.le.73.83) go to 19757 
	               go to 19758
19751	call nucltransK(1.975,0.033,4.0e-4,1.9e-4,tclev,thlev,tdlev)
	return
19752	call nucltransK(1.062,0.033,2.0e-3,0.1e-4,tclev,thlev,tdlev)
	go to 91300
19753	call nucltransK(1.060,0.033,2.0e-3,0.1e-4,tclev,thlev,tdlev)
	go to 91500
19754	call nucltransK(0.520,0.033,8.0e-3,0.,tclev,thlev,tdlev)
	go to 14550
19755	call nucltransK(0.458,0.033,1.5e-2,0.,tclev,thlev,tdlev)
	go to 15160
19756	call nucltransK(0.198,0.033,3.2e-2,0.,tclev,thlev,tdlev)
	go to 17770
19757	call nucltransK(0.177,0.033,1.9e-1,0.,tclev,thlev,tdlev)
	go to 17970
19758	call nucltransK(0.082,0.033,1.8,0.,tclev,thlev,tdlev)
	go to 18930
c-----------------------------------------------------------------------
1943	call beta(1.311,53.,0.,0.,t)
19430	thlev=0.
	p=100.*rnd1(d)
	if(p.le.35.36) go to 19431 
	if(p.le.97.97) go to 19432 
	               go to 19433
19431	call nucltransK(1.030,0.033,2.1e-3,0.,tclev,thlev,tdlev)
	go to 91300
19432	call nucltransK(0.213,0.033,2.6e-2,0.,tclev,thlev,tdlev)
	go to 17290
19433	call nucltransK(0.050,0.033,1.4e+1,0.,tclev,thlev,tdlev)
	go to 18930
c-----------------------------------------------------------------------
18930	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 2.13) go to 18931 
	if(p.le.23.29) go to 18932 
	if(p.le.92.90) go to 18933 
	if(p.le.96.02) go to 18934 
	               go to 18935
18931	call nucltransK(1.893,0.033,4.0e-4,1.4e-4,tclev,thlev,tdlev)
	return
18932	call nucltransK(0.980,0.033,2.5e-3,0.,tclev,thlev,tdlev)
	go to 91300
18933	call nucltransK(0.978,0.033,2.5e-3,0.,tclev,thlev,tdlev)
	go to 91500
18934	call nucltransK(0.377,0.033,2.5e-2,0.,tclev,thlev,tdlev)
	go to 15160
18935	call nucltransK(0.116,0.033,1.5e-1,0.,tclev,thlev,tdlev)
	go to 17770
c-----------------------------------------------------------------------
1886	call beta(1.368,53.,0.,0.,t)
18860	thlev=0.
	p=100.*rnd1(d)
	if(p.le.18.33) go to 18861 
	if(p.le.28.52) go to 18862 
	if(p.le.34.63) go to 18863 
	if(p.le.36.67) go to 18864 
	if(p.le.40.74) go to 18865 
	               go to 18866
18861	call nucltransK(1.886,0.033,2.0e-4,1.2e-4,tclev,thlev,tdlev)
	return
18862	call nucltransK(0.973,0.033,2.5e-3,0.,tclev,thlev,tdlev)
	go to 91300
18863	call nucltransK(0.971,0.033,2.5e-3,0.,tclev,thlev,tdlev)
	go to 91500
18864	call nucltransK(0.369,0.033,2.6e-2,0.,tclev,thlev,tdlev)
	go to 15160
18865	call nucltransK(0.177,0.033,2.0e-1,0.,tclev,thlev,tdlev)
	go to 17070
18866	call nucltransK(0.088,0.033,1.4,0.,tclev,thlev,tdlev)
	go to 17970
c-----------------------------------------------------------------------
18170	thlev=0.
	p=100.*rnd1(d)
	if(p.le.52.17) go to 18171 
	               go to 18172
18171	call nucltransK(0.040,0.033,3.2e+1,0.,tclev,thlev,tdlev)
	go to 17770
18172	call nucltransK(0.018,0.005,2.0e+1,0.,tclev,thlev,tdlev)
	go to 17990
c-----------------------------------------------------------------------
17990	thlev=0.
	call nucltransK(0.164,0.033,2.5e-1,0.,tclev,thlev,tdlev)
	go to 16340
c-----------------------------------------------------------------------
17970	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 3.06) go to 17971 
	if(p.le.19.90) go to 17972 
	if(p.le.57.49) go to 17973 
	if(p.le.65.99) go to 17974 
	if(p.le.67.86) go to 17975 
	if(p.le.75.34) go to 17976 
	               go to 17977
17971	call nucltransK(1.797,0.033,4.5e-4,0.9e-4,tclev,thlev,tdlev)
	return
17972	call nucltransK(0.885,0.033,3.0e-3,0.,tclev,thlev,tdlev)
	go to 91300
17973	call nucltransK(0.883,0.033,3.0e-3,0.,tclev,thlev,tdlev)
	go to 91500
17974	call nucltransK(0.343,0.033,3.0e-2,0.,tclev,thlev,tdlev)
	go to 14550
17975	call nucltransK(0.281,0.033,5.5e-2,0.,tclev,thlev,tdlev)
	go to 15160
17976	call nucltransK(0.151,0.033,3.3e-1,0.,tclev,thlev,tdlev)
	go to 16470
17977	call nucltransK(0.021,0.005,2.6,0.,tclev,thlev,tdlev)
	go to 17770
c-----------------------------------------------------------------------
1777	call beta(1.477,53.,0.,0.,t)
17770	thlev=0.
	p=100.*rnd1(d)
	if(p.le.98.61) go to 17771 
	               go to 17772
17771	call nucltransK(0.864,0.033,2.5e-3,0.,tclev,thlev,tdlev)
	go to 91300
17772	call nucltransK(0.047,0.033,1.7e+1,0.,tclev,thlev,tdlev)
	go to 17290
c-----------------------------------------------------------------------
17290	thlev=170.e-9
	p=100.*rnd1(d)
	if(p.le.37.93) go to 17291 
	               go to 17292
17291	call nucltransK(0.169,0.033,4.7e-2,0.,tclev,thlev,tdlev)
	go to 15600
17292	call nucltransK(0.095,0.033,2.1,0.,tclev,thlev,tdlev)
	go to 16340
c-----------------------------------------------------------------------
17070	thlev=0.
	p=100.*rnd1(d)
	if(p.le.90.91) go to 17071 
	               go to 17072
17071	call nucltransK(0.795,0.033,4.0e-3,0.,tclev,thlev,tdlev)
	go to 91300
17072	call nucltransK(0.793,0.033,3.0e-3,0.,tclev,thlev,tdlev)
	go to 91500
c-----------------------------------------------------------------------
17040	thlev=0.
	p=100.*rnd1(d)
	if(p.le.52.17) go to 17041 
	if(p.le.60.14) go to 17042 
	if(p.le.68.11) go to 17043 
	               go to 17044
17041	call nucltransK(1.704,0.033,6.0e-4,1.4e-4,tclev,thlev,tdlev)
	return
17042	call nucltransK(1.392,0.033,8.5e-4,0.2e-4,tclev,thlev,tdlev)
	go to 31200
17043	call nucltransK(0.792,0.033,4.0e-3,0.,tclev,thlev,tdlev)
	go to 91300
17044	call nucltransK(0.790,0.033,4.0e-3,0.,tclev,thlev,tdlev)
	go to 91500
c-----------------------------------------------------------------------
1647	call beta(1.607,53.,0.,0.,t)
16470	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 9.88) go to 16471 
	if(p.le.72.34) go to 16472 
	if(p.le.93.86) go to 16473 
	               go to 16474
16471	call nucltransK(1.646,0.033,5.0e-4,0.6e-4,tclev,thlev,tdlev)
	return
16472	call nucltransK(0.734,0.033,4.6e-3,0.,tclev,thlev,tdlev)
	go to 91300
16473	call nucltransK(0.732,0.033,4.6e-3,0.,tclev,thlev,tdlev)
	go to 91500
16474	call nucltransK(0.087,0.033,2.9,0.,tclev,thlev,tdlev)
	go to 15600
c-----------------------------------------------------------------------
16340	thlev=9.
	call nucltransK(0.074,0.033,2.4e+1,0.,tclev,thlev,tdlev)
	go to 15600
c-----------------------------------------------------------------------
1560	call beta(1.694,53.,0.,0.,t)
15600	thlev=0.
	call nucltransK(0.648,0.033,4.6e-3,0.,tclev,thlev,tdlev)
	go to 91300
c-----------------------------------------------------------------------
1516	call beta(1.738,53.,0.,0.,t)
15160	thlev=0.
	p=100.*rnd1(d)
	if(p.le.78.54) go to 15161 
	if(p.le.92.15) go to 15162 
	               go to 15163
15161	call nucltransK(1.516,0.033,8.0e-4,0.9e-4,tclev,thlev,tdlev)
	return
15162	call nucltransK(1.204,0.033,1.1e-3,0.1e-4,tclev,thlev,tdlev)
	go to 31200
15163	call nucltransK(0.602,0.033,8.0e-3,0.,tclev,thlev,tdlev)
	go to 91500
c-----------------------------------------------------------------------
1455	call beta(1.799,53.,0.,0.,t)
14550	thlev=0.
	p=100.*rnd1(d)
	if(p.le.30.90) go to 14551 
	if(p.le.87.98) go to 14552 
	               go to 14553
14551	call nucltransK(1.455,0.033,1.0e-3,0.9e-4,tclev,thlev,tdlev)
	return
14552	call nucltransK(1.143,0.033,1.8e-3,0.3e-4,tclev,thlev,tdlev)
	go to 31200
14553	call nucltransK(0.540,0.033,1.0e-2,0.,tclev,thlev,tdlev)
	go to 91500
c-----------------------------------------------------------------------
13070	thlev=0.
	p=100.*rnd1(d)
	if(p.le.39.00) go to 13071 
	if(p.le.89.00) go to 13072 
	               go to 13073
13071	call nucltransK(1.307,0.033,1.2e-3,0.5e-4,tclev,thlev,tdlev)
	return
13072	call nucltransK(0.995,0.033,2.5e-3,0.,tclev,thlev,tdlev)
	go to 31200
13073	call nucltransK(0.392,0.033,2.1e-2,0.,tclev,thlev,tdlev)
	go to 91500
c-----------------------------------------------------------------------
915	call beta(2.339,53.,0.,0.,t)
91500	thlev=0.
	p=100.*rnd1(d)
	if(p.le.99.84) go to 91501 
	               go to 91502
91501	call nucltransK(0.915,0.033,2.7e-3,0.,tclev,thlev,tdlev)
	return
91502	call nucltransK(0.602,0.033,5.0e-3,0.,tclev,thlev,tdlev)
	go to 31200
c-----------------------------------------------------------------------
913	call beta(2.341,53.,0.,0.,t)
91300	thlev=0.
	call nucltransK(0.913,0.033,2.1e-3,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
31200	thlev=0.
	call nucltransK(0.312,0.033,4.0e-2,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	call beta(3.254,53.,0.,0.,t)
	return
c-----------------------------------------------------------------------
	return
	end
c
c end of Te133m.f
c
c Local Variables: --
c mode: fortran --
c End: --
