c I134.f 
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
        subroutine I134(tcnuc,tdnuc)
c Scheme of I134 decay in accordance with Yu.V.Sergeenkov, 
c NDS 71(1994)557.
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 8.10.2002.
	thnuc=3168.
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
        pbeta=100.*rnd1(d)
        if(pbeta.le. 0.368) go to 3492
        if(pbeta.le. 0.435) go to 3477
        if(pbeta.le. 2.216) go to 3375
        if(pbeta.le. 3.300) go to 3360
        if(pbeta.le. 3.509) go to 3314
        if(pbeta.le. 3.567) go to 3300
        if(pbeta.le. 3.786) go to 3256
        if(pbeta.le. 5.129) go to 3084
        if(pbeta.le.35.372) go to 2867
        if(pbeta.le.35.889) go to 2773
        if(pbeta.le.42.754) go to 2654
        if(pbeta.le.58.871) go to 2588
        if(pbeta.le.62.950) go to 2548
        if(pbeta.le.69.516) go to 2408
        if(pbeta.le.80.460) go to 2353
        if(pbeta.le.82.689) go to 2302
        if(pbeta.le.84.281) go to 2272
        if(pbeta.le.87.564) go to 1920
                            go to 1731
c-----------------------------------------------------------------------
3492    call beta(0.678,54.,0.,0.,t)
34920	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 5.17) go to 34921   !  5.17%
	               go to 34922   ! 94.83%
34921	call nucltransK(2.646,0.035,1.5e-4,5.2e-4,tclev,thlev,tdlev)
        go to 84700
34922	call nucltransK(1.190,0.035,2.0e-3,0.2e-4,tclev,thlev,tdlev)
        go to 23020
c-----------------------------------------------------------------------
3477    call beta(0.693,54.,0.,0.,t)
34770	thlev=0.
	p=100.*rnd1(d)
	if(p.le.18.77) go to 34771   ! 18.77%
	               go to 34772   ! 81.23%
34771	call nucltransK(2.630,0.035,1.5e-4,5.1e-4,tclev,thlev,tdlev)
        go to 84700
34772	call nucltransK(0.162,0.035,2.5e-1,0.,tclev,thlev,tdlev)
        go to 33140
c-----------------------------------------------------------------------
3375    call beta(0.795,54.,0.,0.,t)
33750   thlev=0.
	p=100.*rnd1(d)
        if(p.le.21.79) go to 33751
        if(p.le.33.52) go to 33752
        if(p.le.78.21) go to 33753
                       go to 33754
33751   call nucltransK(1.644,0.035,1.5e-3,1.2e-4,tclev,thlev,tdlev)
        go to 17310
33752   call nucltransK(1.239,0.035,1.7e-3,0.4e-4,tclev,thlev,tdlev)
        go to 21370
33753   call nucltransK(1.103,0.035,2.1e-3,0.1e-4,tclev,thlev,tdlev)
        go to 22720
33754   call nucltransK(0.967,0.035,2.5e-3,0.,tclev,thlev,tdlev)
        go to 24080
c-----------------------------------------------------------------------
3360    call beta(0.810,54.,0.,0.,t)
33600   thlev=0.
	p=100.*rnd1(d)
        if(p.le. 6.16) go to 33601
        if(p.le.23.64) go to 33602
                       go to 33603
33601   call nucltransK(2.513,0.035,2.0e-4,4.6e-4,tclev,thlev,tdlev)
        go to 84700
33602   call nucltransK(1.629,0.035,9.0e-4,1.1e-4,tclev,thlev,tdlev)
        go to 17310
33603   call nucltransK(0.707,0.035,5.0e-3,0.,tclev,thlev,tdlev)
        go to 26540
c----------------------------------------------------------------------
3314    call beta(0.856,54.,0.,0.,t)
33140   thlev=0.
	p=100.*rnd1(d)
        if(p.le.63.81) go to 33141
                       go to 33142
33141	call nucltransK(2.467,0.035,2.0e-4,4.5e-4,tclev,thlev,tdlev)
        go to 84700
33142	call nucltransK(1.395,0.035,1.2e-3,0.7e-4,tclev,thlev,tdlev)
        go to 19200
c-----------------------------------------------------------------------
3300    call beta(0.870,54.,0.,0.,t)
33000   thlev=0.
	call nucltransK(2.453,0.035,2.0e-4,4.4e-4,tclev,thlev,tdlev)
        go to 84700
c-----------------------------------------------------------------------
3256    call beta(0.914,54.,0.,0.,t)
32560   thlev=0.
	p=100.*rnd1(d)
        if(p.le.35.78) go to 32561
                       go to 32562
32561	call nucltransK(2.409,0.035,2.1e-4,4.1e-4,tclev,thlev,tdlev)
        go to 84700
32562	call nucltransK(1.336,0.035,1.2e-3,0.6e-4,tclev,thlev,tdlev)
        go to 19200
c-----------------------------------------------------------------------
3084    call beta(1.086,54.,0.,0.,t)
30840   thlev=0.
	p=100.*rnd1(d)
        if(p.le. 3.95) go to 30841
        if(p.le.59.80) go to 30842
        if(p.le.90.32) go to 30843
                       go to 30844
30841   call nucltransK(2.237,0.035,3.0e-4,3.2e-4,tclev,thlev,tdlev)
        go to 84700
30842   call nucltransK(1.470,0.035,1.0e-3,0.9e-4,tclev,thlev,tdlev)
        go to 16140
30843   call nucltransK(1.353,0.035,1.2e-3,0.6e-4,tclev,thlev,tdlev)
        go to 17310
30844   call nucltransK(1.164,0.035,1.6e-3,0.2e-4,tclev,thlev,tdlev)
        go to 19200
c-----------------------------------------------------------------------
2867    call beta(1.303,54.,0.,0.,t)
28670   thlev=0.
	p=100.*rnd1(d)
        if(p.le. 0.62) go to 28671
        if(p.le.30.54) go to 28672
        if(p.le.43.73) go to 28673
        if(p.le.49.75) go to 28674
        if(p.le.86.25) go to 28675
        if(p.le.93.65) go to 28676
        if(p.le.97.99) go to 28677
        if(p.le.99.50) go to 28678
                       go to 28679
28671   call nucltransK(2.021,0.035,3.0e-4,2.3e-4,tclev,thlev,tdlev)
        go to 84700
28672   call nucltransK(1.136,0.035,1.6e-3,0.3e-4,tclev,thlev,tdlev)
        go to 17310
28673   call nucltransK(0.948,0.035,2.4e-3,0.,tclev,thlev,tdlev)
        go to 19200
28674   call nucltransK(0.731,0.035,3.4e-3,0.,tclev,thlev,tdlev)
        go to 21370
28675   call nucltransK(0.595,0.035,7.2e-3,0.,tclev,thlev,tdlev)
        go to 22720
28676   call nucltransK(0.514,0.035,8.0e-3,0.,tclev,thlev,tdlev)
        go to 23530
28677   call nucltransK(0.459,0.035,1.1e-2,0.,tclev,thlev,tdlev)
        go to 24080
28678   call nucltransK(0.320,0.035,3.7e-2,0.,tclev,thlev,tdlev)
        go to 25480
28679   call nucltransK(0.279,0.035,5.2e-2,0.,tclev,thlev,tdlev)
        go to 25880
c-----------------------------------------------------------------------
2773    call beta(1.397,54.,0.,0.,t)
27730   thlev=0.
	p=100.*rnd1(d)
        if(p.le.34.62) go to 27731
                       go to 27732
27731   call nucltransK(1.926,0.035,4.0e-4,2.0e-4,tclev,thlev,tdlev)
        go to 84700
27732   call nucltransK(1.159,0.035,7.0e-4,0.3e-4,tclev,thlev,tdlev)
        go to 16140
c-----------------------------------------------------------------------
2654    call beta(1.516,54.,0.,0.,t)
26540   thlev=0.
	p=100.*rnd1(d)
        if(p.le.71.82) go to 26541
        if(p.le.98.18) go to 26542
                       go to 26543
26541   call nucltransK(1.807,0.035,6.0e-4,1.7e-4,tclev,thlev,tdlev)
        go to 84700
26542   call nucltransK(1.040,0.035,2.0e-3,0.,tclev,thlev,tdlev)
        go to 16140
26543   call nucltransK(0.922,0.035,2.1e-3,0.,tclev,thlev,tdlev)
        go to 17310
c-----------------------------------------------------------------------
2588    call beta(1.582,54.,0.,0.,t)
25880   thlev=0.
	p=100.*rnd1(d)
        if(p.le.15.66) go to 25881
        if(p.le.44.90) go to 25882
        if(p.le.85.87) go to 25883
                       go to 25884
25881   call nucltransK(1.741,0.035,6.0e-4,0.8e-4,tclev,thlev,tdlev)
        go to 84700
25882   call nucltransK(0.975,0.035,1.7e-3,0.,tclev,thlev,tdlev)
        go to 16140
25883   call nucltransK(0.857,0.035,2.9e-3,0.,tclev,thlev,tdlev)
        go to 17310
25884   call nucltransK(0.235,0.035,8.5e-2,0.,tclev,thlev,tdlev)
        go to 23530
c-----------------------------------------------------------------------
2548    call beta(1.622,54.,0.,0.,t)
25480   thlev=0.
	p=100.*rnd1(d)
        if(p.le.13.78) go to 25481
        if(p.le.63.11) go to 25482
        if(p.le.75.78) go to 25483
                       go to 25484
25481   call nucltransK(0.816,0.035,4.0e-3,0.,tclev,thlev,tdlev)
        go to 17310
25482   call nucltransK(0.628,0.035,4.9e-3,0.,tclev,thlev,tdlev)
        go to 19200
25483   call nucltransK(0.411,0.035,2.0e-2,0.,tclev,thlev,tdlev)
        go to 21370
25484   call nucltransK(0.139,0.035,4.5e-1,0.,tclev,thlev,tdlev)
        go to 24080
c-----------------------------------------------------------------------
2408    call beta(1.762,54.,0.,0.,t)
24080   thlev=0.
	p=100.*rnd1(d)
        if(p.le.84.57) go to 24081
                       go to 24082
24081   call nucltransK(0.677,0.035,5.3e-3,0.,tclev,thlev,tdlev)
        go to 17310
24082   call nucltransK(0.489,0.035,9.6e-3,0.,tclev,thlev,tdlev)
        go to 19200
c-----------------------------------------------------------------------
2353    call beta(1.817,54.,0.,0.,t)
23530   thlev=0.
	p=100.*rnd1(d)
        if(p.le. 4.42) go to 23531
        if(p.le.73.01) go to 23532
                       go to 23533
23531   call nucltransK(0.739,0.035,3.3e-3,0.,tclev,thlev,tdlev)
        go to 16140
23532   call nucltransK(0.622,0.035,6.1e-3,0.,tclev,thlev,tdlev)
        go to 17310
23533   call nucltransK(0.433,0.035,1.6e-2,0.,tclev,thlev,tdlev)
        go to 19200
c-----------------------------------------------------------------------
2302    call beta(1.868,54.,0.,0.,t)
23020   thlev=0.
	p=100.*rnd1(d)
        if(p.le.88.08) go to 23021
                       go to 23022
23021   call nucltransK(1.455,0.035,1.1e-3,0.8e-4,tclev,thlev,tdlev)
        go to 84700
23022   call nucltransK(0.571,0.035,9.0e-3,0.,tclev,thlev,tdlev)
        go to 17310
c-----------------------------------------------------------------------
2272    call beta(1.898,54.,0.,0.,t)
22720   thlev=0.
	p=100.*rnd1(d)
        if(p.le.57.01) go to 22721
                       go to 22722
22721   call nucltransK(0.541,0.035,7.7e-3,0.,tclev,thlev,tdlev)
        go to 17310
22722   call nucltransK(0.135,0.035,3.5e-1,0.,tclev,thlev,tdlev)
        go to 21370
c-----------------------------------------------------------------------
21370   thlev=0.
	p=100.*rnd1(d)
        if(p.le.96.64) go to 21371
                       go to 21372
21371   call nucltransK(0.405,0.035,1.8e-2,0.,tclev,thlev,tdlev)
        go to 17310
21372   call nucltransK(0.217,0.035,1.2e-1,0.,tclev,thlev,tdlev)
        go to 19200
c-----------------------------------------------------------------------
1920    call beta(2.250,54.,0.,0.,t)
19200   thlev=0.
	p=100.*rnd1(d)
        if(p.le.94.36) go to 19201
                       go to 19202
19201   call nucltransK(1.073,0.035,1.9e-3,0.1e-4,tclev,thlev,tdlev)
        go to 84700
19202   call nucltransK(0.188,0.035,1.7e-1,0.,tclev,thlev,tdlev)
        go to 17310
c-----------------------------------------------------------------------
1731    call beta(2.439,54.,0.,0.,t)
17310   thlev=0.
	call nucltransK(0.884,0.035,2.2e-3,0.,tclev,thlev,tdlev)
        go to 84700
c-----------------------------------------------------------------------
16140   thlev=0.
	p=100.*rnd1(d)
        if(p.le.50.83) go to 16141
                       go to 16142
16141   call nucltransK(1.614,0.035,3.0e-4,0.5e-4,tclev,thlev,tdlev)
        return
16142   call nucltransK(0.767,0.035,3.2e-3,0.,tclev,thlev,tdlev)
        go to 84700
c-----------------------------------------------------------------------
84700   thlev=1.9e-12
	call nucltransK(0.847,0.035,2.4e-3,0.,tclev,thlev,tdlev)
        return
c-----------------------------------------------------------------------
        end
c
c end of I134.f
c
c Local Variables: --
c mode: fortran --
c End: --
