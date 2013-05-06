c I135.f 
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
        subroutine I135(tcnuc,tdnuc)
c Scheme of I135 decay (Yu.V.Sergeenkov et al., Nucl. Data Sheets 
c 84(1998)115).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 12.10.2002.
	thnuc=23652.
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
        pbeta=100.*rnd1(d)
        if(pbeta.le. 0.013) go to 2478
        if(pbeta.le. 0.155) go to 2475
        if(pbeta.le. 0.282) go to 2466
        if(pbeta.le. 0.422) go to 2447
        if(pbeta.le. 1.449) go to 2409
        if(pbeta.le. 2.366) go to 2372
        if(pbeta.le. 3.752) go to 2357
        if(pbeta.le. 8.518) go to 2255
        if(pbeta.le.15.896) go to 2233
        if(pbeta.le.15.919) go to 2152
        if(pbeta.le.15.989) go to 2112
        if(pbeta.le.17.574) go to 2093
        if(pbeta.le.18.681) go to 2046
        if(pbeta.le.26.657) go to 1968
        if(pbeta.le.26.707) go to 1927
        if(pbeta.le.27.315) go to 1894
        if(pbeta.le.36.089) go to 1791
        if(pbeta.le.57.825) go to 1678
        if(pbeta.le.65.801) go to 1565
        if(pbeta.le.73.279) go to 1458
        if(pbeta.le.96.810) go to 1260
        if(pbeta.le.98.106) go to 1132
                            go to  527
c-----------------------------------------------------------------------
2478    call beta(0.170,54.,0.,0.,t)
24780   thlev=0.
	p=100.*rnd1(d)
        if(p.le.9.72) go to 24781
                      go to 24782
24781   call nucltransK(2.477,0.035,2.0e-4,4.5e-4,tclev,thlev,tdlev)
        return
24782   call nucltransK(2.189,0.035,3.0e-4,3.1e-4,tclev,thlev,tdlev)
        go to 28800
c-----------------------------------------------------------------------
2475    call beta(0.173,54.,0.,0.,t)
24750   thlev=0.
	p=100.*rnd1(d)
        if(p.le.45.00) go to 24751
                       go to 24752
24751   call nucltransK(1.948,0.035,2.4e-4,2.1e-4,tclev,thlev,tdlev)
        go to 52700
24752   call nucltransK(1.344,0.035,4.0e-4,0.7e-4,tclev,thlev,tdlev)
        go to 11320
c-----------------------------------------------------------------------
2466    call beta(0.182,54.,0.,0.,t)
24660   thlev=0.
	p=100.*rnd1(d)
        if(p.le.56.69) go to 24661
        if(p.le.81.89) go to 24662
                       go to 24663
24661   call nucltransK(2.466,0.035,4.0e-4,5.3e-4,tclev,thlev,tdlev)
        return
24662   call nucltransK(1.335,0.035,1.2e-3,0.6e-4,tclev,thlev,tdlev)
        go to 11320
24663   call nucltransK(0.685,0.035,4.0e-3,0.,tclev,thlev,tdlev)
        go to 17810
c-----------------------------------------------------------------------
2447    call beta(0.201,54.,0.,0.,t)
24470   thlev=0.
	p=100.*rnd1(d)
        if(p.le.46.81) go to 24471
                       go to 24472
24471   call nucltransK(1.316,0.035,4.5e-4,0.5e-4,tclev,thlev,tdlev)
        go to 11320
24472   call nucltransK(0.656,0.035,1.6e-3,0.,tclev,thlev,tdlev)
        go to 17910
c-----------------------------------------------------------------------
2409    call beta(0.239,54.,0.,0.,t)
24090   thlev=0.
	p=100.*rnd1(d)
        if(p.le.93.48) go to 24091
        if(p.le.96.40) go to 24092
                       go to 24093
24091   call nucltransK(2.409,0.035,3.5e-4,4.1e-4,tclev,thlev,tdlev)
        return
24092   call nucltransK(0.960,0.035,2.6e-3,0.,tclev,thlev,tdlev)
        go to 14480
24093   call nucltransK(0.617,0.035,7.0e-3,0.,tclev,thlev,tdlev)
        go to 17910
c-----------------------------------------------------------------------
2372    call beta(0.276,54.,0.,0.,t)
23720   thlev=0.
	p=100.*rnd1(d)
        if(p.le. 0.66) go to 23721
                       go to 23722
23721   call nucltransK(1.845,0.035,2.6e-4,1.8e-4,tclev,thlev,tdlev)
        go to 52700
23722   call nucltransK(1.240,0.035,1.4e-3,0.5e-4,tclev,thlev,tdlev)
        go to 11320
c-----------------------------------------------------------------------
2357    call beta(0.291,54.,0.,0.,t)
23570   thlev=0.
	p=100.*rnd1(d)
        if(p.le.41.91) go to 23571
        if(p.le.45.02) go to 23572
        if(p.le.51.45) go to 23573
        if(p.le.55.42) go to 23574
        if(p.le.64.74) go to 23575
        if(p.le.86.41) go to 23576
                       go to 23577
23571   call nucltransK(1.831,0.035,2.6e-4,1.6e-4,tclev,thlev,tdlev)
        go to 52700
23572   call nucltransK(1.226,0.035,1.4e-3,0.4e-4,tclev,thlev,tdlev)
        go to 11320
23573   call nucltransK(1.097,0.035,1.4e-3,0.,tclev,thlev,tdlev)
        go to 12600
23574   call nucltransK(0.679,0.035,6.0e-3,0.,tclev,thlev,tdlev)
        go to 16780
23575   call nucltransK(0.576,0.035,8.5e-3,0.,tclev,thlev,tdlev)
        go to 17810
23576   call nucltransK(0.430,0.035,1.8e-2,0.,tclev,thlev,tdlev)
        go to 19270
23577   call nucltransK(0.264,0.035,6.5e-2,0.,tclev,thlev,tdlev)
        go to 20930
c-----------------------------------------------------------------------
2255    call beta(0.393,54.,0.,0.,t)
22550   thlev=0.
	p=100.*rnd1(d)
        if(p.le.12.88) go to 22551
        if(p.le.89.34) go to 22552
        if(p.le.92.51) go to 22553
        if(p.le.93.48) go to 22554
        if(p.le.97.07) go to 22555
        if(p.le.99.79) go to 22556
                       go to 22557
22551   call nucltransK(2.255,0.035,3.7e-4,3.8e-4,tclev,thlev,tdlev)
        return
22552   call nucltransK(1.124,0.035,1.7e-3,0.2e-4,tclev,thlev,tdlev)
        go to 11320
22553   call nucltransK(0.995,0.035,1.9e-3,0.,tclev,thlev,tdlev)
        go to 12600
22554   call nucltransK(0.807,0.035,2.6e-3,0.,tclev,thlev,tdlev)
        go to 14480
22555   call nucltransK(0.798,0.035,4.0e-3,0.,tclev,thlev,tdlev)
        go to 14580
22556   call nucltransK(0.690,0.035,5.5e-3,0.,tclev,thlev,tdlev)
        go to 15650
22557   call nucltransK(0.163,0.035,2.2e-1,0.,tclev,thlev,tdlev)
        go to 20930
c-----------------------------------------------------------------------
2233    call beta(0.415,54.,0.,0.,t)
22330   thlev=0.
	p=100.*rnd1(d)
        if(p.le.55.75) go to 22331
        if(p.le.77.64) go to 22332
        if(p.le.94.09) go to 22333
        if(p.le.98.39) go to 22334
        if(p.le.99.68) go to 22335
                       go to 22336
22331   call nucltransK(1.706,0.035,2.8e-4,1.0e-4,tclev,thlev,tdlev)
        go to 52700
22332   call nucltransK(1.102,0.035,1.4e-3,0.1e-4,tclev,thlev,tdlev)
        go to 11320
22333   call nucltransK(0.973,0.035,1.8e-3,0.,tclev,thlev,tdlev)
        go to 12600
22334   call nucltransK(0.452,0.035,1.6e-2,0.,tclev,thlev,tdlev)
        go to 17810
22335   call nucltransK(0.306,0.035,4.0e-2,0.,tclev,thlev,tdlev)
        go to 19270
22336   call nucltransK(0.184,0.035,4.0e-2,0.,tclev,thlev,tdlev)
        go to 20490
c-----------------------------------------------------------------------
2152    call beta(0.496,54.,0.,0.,t)
21520   thlev=0.
	call nucltransK(2.152,0.035,2.0e-4,2.9e-4,tclev,thlev,tdlev)
        return
c-----------------------------------------------------------------------
2112    call beta(0.536,54.,0.,0.,t)
21120   thlev=0.
	call nucltransK(2.112,0.035,2.0e-4,2.7e-4,tclev,thlev,tdlev)
        return
c-----------------------------------------------------------------------
2093    call beta(0.555,54.,0.,0.,t)
20930   thlev=0.
	p=100.*rnd1(d)
        if(p.le.72.79) go to 20931
        if(p.le.81.26) go to 20932
        if(p.le.98.25) go to 20933
                       go to 20934
20931   call nucltransK(1.566,0.035,2.8e-4,0.7e-4,tclev,thlev,tdlev)
        go to 52700
20932   call nucltransK(0.961,0.035,2.6e-3,0.,tclev,thlev,tdlev)
        go to 11320
20933   call nucltransK(0.415,0.035,2.0e-2,0.,tclev,thlev,tdlev)
        go to 16780
20934   call nucltransK(0.166,0.035,2.2e-1,0.,tclev,thlev,tdlev)
        go to 19270
c-----------------------------------------------------------------------
20490   thlev=0.
	call nucltransK(1.522,0.035,9.0e-4,0.9e-4,tclev,thlev,tdlev)
        go to 52700
c-----------------------------------------------------------------------
2046    call beta(0.602,54.,0.,0.,t)
20460   thlev=0.
	p=100.*rnd1(d)
        if(p.le.79.31) go to 20461
        if(p.le.93.16) go to 20462
        if(p.le.97.90) go to 20463
                       go to 20464
20461   call nucltransK(2.046,0.035,4.5e-4,2.3e-4,tclev,thlev,tdlev)
        return
20462   call nucltransK(0.785,0.035,3.6e-3,0.,tclev,thlev,tdlev)
        go to 12600
20463   call nucltransK(0.588,0.035,8.5e-3,0.,tclev,thlev,tdlev)
        go to 14580
20464   call nucltransK(0.255,0.035,6.5e-2,0.,tclev,thlev,tdlev)
        go to 17910
c-----------------------------------------------------------------------
1968    call beta(0.680,54.,0.,0.,t)
19680   thlev=0.
	p=100.*rnd1(d)
        if(p.le. 0.21) go to 19681
        if(p.le.84.72) go to 19682
        if(p.le.93.05) go to 19683
        if(p.le.95.98) go to 19684
                       go to 19685
19681   call nucltransK(1.442,0.035,0.8e-4,1.0e-3,tclev,thlev,tdlev)
        go to 52700
19682   call nucltransK(0.837,0.035,2.5e-3,0.,tclev,thlev,tdlev)
        go to 11320
19683   call nucltransK(0.708,0.035,5.0e-3,0.,tclev,thlev,tdlev)
        go to 12600
19684   call nucltransK(0.403,0.035,1.8e-2,0.,tclev,thlev,tdlev)
        go to 15650
19685   call nucltransK(0.290,0.035,4.6e-2,0.,tclev,thlev,tdlev)
        go to 16780
c-----------------------------------------------------------------------
1927    call beta(0.721,54.,0.,0.,t)
19270   thlev=0.
	p=100.*rnd1(d)
        if(p.le.58.50) go to 19271
        if(p.le.63.05) go to 19272
                       go to 19273
19271   call nucltransK(1.927,0.035,5.0e-4,1.4e-4,tclev,thlev,tdlev)
        return
19272   call nucltransK(0.796,0.035,4.0e-3,0.,tclev,thlev,tdlev)
        go to 11320
19273   call nucltransK(0.362,0.035,2.8e-2,0.,tclev,thlev,tdlev)
        go to 15650
c-----------------------------------------------------------------------
1894    call beta(0.754,54.,0.,0.,t)
18940   thlev=0.
	call nucltransK(1.368,0.035,9.0e-4,0.2e-4,tclev,thlev,tdlev)
        go to 52700
c-----------------------------------------------------------------------
1791    call beta(0.857,54.,0.,0.,t)
17910   thlev=0.
	p=100.*rnd1(d)
        if(p.le.86.68) go to 17911
        if(p.le.98.81) go to 17912
        if(p.le.99.17) go to 17913
        if(p.le.99.18) go to 17914
        if(p.le.99.60) go to 17915
        if(p.le.99.92) go to 17916
                       go to 17917
17911   call nucltransK(1.791,0.035,6.5e-4,1.6e-4,tclev,thlev,tdlev)
        return
17912   call nucltransK(1.503,0.035,7.5e-4,0.3e-4,tclev,thlev,tdlev)
        go to 28800
17913   call nucltransK(0.531,0.035,9.3e-3,0.,tclev,thlev,tdlev)
        go to 12600
17914   call nucltransK(0.343,0.035,3.0e-2,0.,tclev,thlev,tdlev)
        go to 14480
17915   call nucltransK(0.334,0.035,3.5e-2,0.,tclev,thlev,tdlev)
        go to 14580
17916   call nucltransK(0.248,0.035,8.0e-3,0.,tclev,thlev,tdlev)
        go to 15440
17917   call nucltransK(0.113,0.035,6.0e-1,0.,tclev,thlev,tdlev)
        go to 16780
c-----------------------------------------------------------------------
17810   thlev=0.
	p=100.*rnd1(d)
        if(p.le.1.29) go to 17811
                      go to 17812
17811   call nucltransK(1.255,0.035,4.7e-4,0.3e-4,tclev,thlev,tdlev)
        go to 52700
17812   call nucltransK(0.645,0.035,4.5e-3,0.,tclev,thlev,tdlev)
        go to 11320
c-----------------------------------------------------------------------
1678    call beta(0.970,54.,0.,0.,t)
16780   thlev=0.
	p=100.*rnd1(d)
        if(p.le.42.57) go to 16781
        if(p.le.74.58) go to 16782
        if(p.le.90.20) go to 16783
        if(p.le.91.38) go to 16784
        if(p.le.99.94) go to 16785
                       go to 16786
16781   call nucltransK(1.678,0.035,5.6e-4,0.7e-4,tclev,thlev,tdlev)
        return
16782   call nucltransK(0.547,0.035,9.1e-3,0.,tclev,thlev,tdlev)
        go to 11320
16783   call nucltransK(0.418,0.035,1.6e-2,0.,tclev,thlev,tdlev)
        go to 12600
16784   call nucltransK(0.230,0.035,1.0e-1,0.,tclev,thlev,tdlev)
        go to 14480
16785   call nucltransK(0.221,0.035,1.0e-1,0.,tclev,thlev,tdlev)
        go to 14580
16786   call nucltransK(0.113,0.035,6.0e-1,0.,tclev,thlev,tdlev)
        go to 15650
c-----------------------------------------------------------------------
1565    call beta(1.083,54.,0.,0.,t)
15650   thlev=0.
	p=100.*rnd1(d)
        if(p.le.93.00) go to 15651
        if(p.le.99.62) go to 15652
                       go to 15653
15651   call nucltransK(1.039,0.035,6.4e-4,0.,tclev,thlev,tdlev)
        go to 52700
15652   call nucltransK(0.434,0.035,1.6e-2,0.,tclev,thlev,tdlev)
        go to 11320
15653   call nucltransK(0.305,0.035,4.0e-2,0.,tclev,thlev,tdlev)
        go to 12600
c-----------------------------------------------------------------------
15440   thlev=0.
	p=100.*rnd1(d)
        if(p.le.81.25) go to 15441
                       go to 15442
15441   call nucltransK(1.544,0.035,9.0e-4,1.0e-4,tclev,thlev,tdlev)
        return
15442   call nucltransK(1.255,0.035,1.2e-3,0.5e-4,tclev,thlev,tdlev)
        go to 28800
c-----------------------------------------------------------------------
1458    call beta(1.190,54.,0.,0.,t)
14580   thlev=0.
	p=100.*rnd1(d)
        if(p.le.90.44) go to 14581
        if(p.le.99.59) go to 14582
        if(p.le.99.61) go to 14583
                       go to 14584
14581   call nucltransK(1.458,0.035,7.0e-4,0.9e-4,tclev,thlev,tdlev)
        return
14582   call nucltransK(1.169,0.035,1.3e-3,0.,tclev,thlev,tdlev)
        go to 28800
14583   call nucltransK(0.326,0.035,3.5e-2,0.,tclev,thlev,tdlev)
        go to 11320
14584   call nucltransK(0.197,0.035,1.5e-1,0.,tclev,thlev,tdlev)
        go to 12600
c-----------------------------------------------------------------------
14480   thlev=0.
	p=100.*rnd1(d)
        if(p.le.75.65) go to 14481
                       go to 14482
14481   call nucltransK(1.448,0.035,7.0e-4,0.9e-4,tclev,thlev,tdlev)
        return
14482   call nucltransK(1.160,0.035,1.3e-3,0.,tclev,thlev,tdlev)
        go to 28800
c-----------------------------------------------------------------------
1260    call beta(1.388,54.,0.,0.,t)
12600   thlev=0.
	p=100.*rnd1(d)
        if(p.le.96.99) go to 12601
                       go to 12602
12601   call nucltransK(1.260,0.035,1.2e-3,0.1e-4,tclev,thlev,tdlev)
        return
12602   call nucltransK(0.972,0.035,1.8e-3,0.,tclev,thlev,tdlev)
        go to 28800
c-----------------------------------------------------------------------
1132    call beta(1.516,54.,0.,0.,t)
11320   thlev=0.
	call nucltransK(1.132,0.035,1.3e-3,0.,tclev,thlev,tdlev)
        return
c-----------------------------------------------------------------------
527     call beta(2.121,54.,0.,0.,t)
52700   thlev=917.4
	call nucltransK(0.527,0.035,2.4e-1,0.,tclev,thlev,tdlev)
	return                        
c-----------------------------------------------------------------------
28800   thlev=0.
	call nucltransK(0.288,0.035,4.7e-2,0.,tclev,thlev,tdlev)
        return
c-----------------------------------------------------------------------
        end
c
c end of I135.f
c
c Local Variables: --
c mode: fortran --
c End: --
