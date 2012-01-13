c Bi214.f 
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
	subroutine Bi214(tcnuc,tdnuc)
c Scheme of Bi214 decay ("Table of Isotopes", 7th ed., 1978).
c Two-, three- and four-figured labels correspond to energies of
c 210Tl or 214Po excited levels in keV. Beta-alfa decays to 210Pb
c are not considered (p=2.8e-3%).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 13.08.1992, 22.10.1995.
c VIT, 31.05.2005, updated to NDS 99(2003)649. Not well known alpha decays to 
c 210Tl levels with E>253 keV are omitted.
	thnuc=1194.
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
	pdecay=100.*rnd1(d)
	if(pdecay.le.0.021) go to 10000   !  0.021% alfa to 210Tl
	                    go to 20000   ! 99.979% beta to 214Po
c-----------------------------------------------------------------------
10000	palfa=100.*rnd1(d)
	if(palfa.le. 5.86) go to   253   !  5.86%
	if(palfa.le.60.36) go to    63   ! 54.50%
	                   go to 10001   ! 39.64%
c-----------------------------------------------------------------------
253	call alpha(5.273,0.,0.,t)
	thlev=0.
	call nucltransK(0.191,0.086,1.3,0.,tclev,thlev,tdlev)
	go to 63000
c-----------------------------------------------------------------------
63	call alpha(5.452,0.,0.,t)
63000	thlev=0.
	call nucltransK(0.063,0.015,6.48,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10001	call alpha(5.516,0.,0.,t)
	return
c-----------------------------------------------------------------------
20000	pbeta=100.*rnd1(d)
	if(pbeta.le. 0.001) go to  3184   !  0.001%
	if(pbeta.le. 0.002) go to  3160   !  0.001%
	if(pbeta.le. 0.003) go to  3143   !  0.001%
	if(pbeta.le. 0.008) go to  3082   !  0.005%
	if(pbeta.le. 0.012) go to  3054   !  0.004%
	if(pbeta.le. 0.017) go to  3014   !  0.005%
	if(pbeta.le. 0.024) go to  3003   !  0.007%
	if(pbeta.le. 0.034) go to  3000   !  0.010%
	if(pbeta.le. 0.043) go to  2986   !  0.009%
	if(pbeta.le. 0.060) go to  2979   !  0.017%
	if(pbeta.le. 0.102) go to  2941   !  0.042%
	if(pbeta.le. 0.104) go to  2935   !  0.002%
	if(pbeta.le. 0.105) go to  2929   !  0.001%
	if(pbeta.le. 0.128) go to  2922   !  0.023%
	if(pbeta.le. 0.129) go to  2920   !  0.001%
	if(pbeta.le. 0.134) go to  2897   !  0.005%
	if(pbeta.le. 0.145) go to  2894   !  0.011%
	if(pbeta.le. 0.156) go to  2880   !  0.011%
	if(pbeta.le. 0.170) go to  2870   !  0.014%
	if(pbeta.le. 0.184) go to  2861   !  0.014%
	if(pbeta.le. 0.186) go to  2827   !  0.002%
	if(pbeta.le. 0.222) go to  2786   !  0.036%
	if(pbeta.le. 0.258) go to  2770   !  0.036%
	if(pbeta.le. 0.819) go to  2729   !  0.561%
	if(pbeta.le. 1.097) go to  2719   !  0.278%
	if(pbeta.le. 1.151) go to  2699   !  0.054%
	if(pbeta.le. 1.204) go to  2698   !  0.053%
	if(pbeta.le. 1.467) go to  2695   !  0.263%
	if(pbeta.le. 1.594) go to  2662   !  0.127%
	if(pbeta.le. 1.615) go to  2631   !  0.021%
	if(pbeta.le. 1.699) go to  2605   !  0.084%
	if(pbeta.le. 1.752) go to  2545   !  0.053%
	if(pbeta.le. 1.892) go to  2508   !  0.140%
	if(pbeta.le. 2.085) go to  2505   !  0.193%
	if(pbeta.le. 3.417) go to  2482   !  1.332%
	if(pbeta.le. 6.232) go to  2448   !  2.815%
	if(pbeta.le. 6.312) go to  2423   !  0.080%
	if(pbeta.le. 6.872) go to  2293   !  0.560%
	if(pbeta.le. 7.073) go to  2266   !  0.201%
	if(pbeta.le.12.802) go to  2204   !  5.729%
	if(pbeta.le.13.635) go to  2193   !  0.833%
	if(pbeta.le.14.056) go to  2148   !  0.421%
	if(pbeta.le.18.323) go to  2119   !  4.267%
	if(pbeta.le.18.419) go to  2088   !  0.096%
	if(pbeta.le.20.623) go to  2017   !  2.204%
	if(pbeta.le.21.995) go to  2011   !  1.372%
	if(pbeta.le.23.137) go to  1995   !  1.142%
	if(pbeta.le.24.730) go to  1890   !  1.593%
	if(pbeta.le.32.923) go to  1847   !  8.193%
	if(pbeta.le.49.996) go to  1764   ! 17.073%
	if(pbeta.le.50.109) go to  1743   !  0.113%
	if(pbeta.le.67.963) go to  1730   ! 17.854%
	if(pbeta.le.68.113) go to  1713   !  0.150%
	if(pbeta.le.68.834) go to  1661   !  0.721%
	if(pbeta.le.71.789) go to  1543   !  2.955%
	if(pbeta.le.72.600) go to  1415   !  0.811%
	if(pbeta.le.80.042) go to  1378   !  7.442%
	if(pbeta.le.81.745) go to   609   !  1.703%
	                    go to 20001   ! 18.255%
c-----------------------------------------------------------------------
3184	call beta(0.088,84.,0.,0.,t)
31840	thlev=0.
	call nucltransK(3.184,0.093,4.0e-4,8.0e-4,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
3160	call beta(0.112,84.,0.,0.,t)
31600	thlev=0.
	p=100.*rnd1(d)
	if(p.le.41.) go to 31601   ! 41%
	             go to 31602   ! 59%
31601	call nucltransK(3.160,0.093,4.0e-4,8.0e-4,tclev,thlev,tdlev)
	return
31602	call nucltransK(2.551,0.093,6.0e-4,4.6e-4,tclev,thlev,tdlev)
	go to 60900
c-----------------------------------------------------------------------
3143	call beta(0.129,84.,0.,0.,t)
31430	thlev=0.
	call nucltransK(3.143,0.093,4.0e-4,8.0e-4,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
3082	call beta(0.190,84.,0.,0.,t)
30820	thlev=0.
	call nucltransK(3.082,0.093,4.2e-4,8.0e-4,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
3054	call beta(0.218,84.,0.,0.,t)
30540	thlev=0.
	p=100.*rnd1(d)
	if(p.le.60.0) go to 30541   ! 60.0%
	if(p.le.82.9) go to 30542   ! 22.9%
	              go to 30543   ! 17.1%
30541	call nucltransK(3.054,0.093,1.0e-3,8.0e-4,tclev,thlev,tdlev)
	return
30542	call nucltransK(2.445,0.093,1.5e-3,5.2e-4,tclev,thlev,tdlev)
	go to 60900
30543	call nucltransK(1.637,0.093,3.5e-3,0.5e-4,tclev,thlev,tdlev)
	go to 14150
c-----------------------------------------------------------------------
3014	call beta(0.258,84.,0.,0.,t)
30140	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.9) go to 30141   !  0.9%
	if(p.le.27.8) go to 30142   ! 26.9%
	if(p.le.41.7) go to 30143   ! 13.4%
	if(p.le.61.8) go to 30144   ! 20.6%
	              go to 30145   ! 38.2%
30141	call nucltransK(2.405,0.093,3.0e-3,4.1e-4,tclev,thlev,tdlev)
	go to 60900
30142	call nucltransK(1.636,0.093,7.0e-3,1.2e-4,tclev,thlev,tdlev)
	go to 13780
30143	call nucltransK(1.598,0.093,8.0e-3,1.1e-4,tclev,thlev,tdlev)
	go to 14150
30144	call nucltransK(1.471,0.093,9.0e-3,0.9e-4,tclev,thlev,tdlev)
	go to 15430
30145	call nucltransK(1.285,0.093,1.2e-2,0.5e-4,tclev,thlev,tdlev)
	go to 17300
c-----------------------------------------------------------------------
3003	call beta(0.269,84.,0.,0.,t)
30030	thlev=0.
	call nucltransK(1.156,0.093,1.8e-2,3.0e-6,tclev,thlev,tdlev)
	go to 18470
c-----------------------------------------------------------------------
3000	call beta(0.272,84.,0.,0.,t)
30000	thlev=0.
	p=100.*rnd1(d)
	if(p.le.84.6) go to 30001   ! 84.6%
	              go to 30002   ! 15.4%
30001	call nucltransK(3.000,0.093,1.4e-3,6.5e-4,tclev,thlev,tdlev)
	return
30002	call nucltransK(2.391,0.093,2.8e-3,4.0e-4,tclev,thlev,tdlev)
	go to 60900
c-----------------------------------------------------------------------
2986	call beta(0.286,84.,0.,0.,t)
29860	thlev=0.
	p=100.*rnd1(d)
	if(p.le.83.0) go to 29861   ! 83.0%
	              go to 29862   ! 17.0%
29861	call nucltransK(2.377,0.093,2.8e-3,4.0e-4,tclev,thlev,tdlev)
	go to 60900
29862	call nucltransK(1.711,0.093,6.0e-3,1.3e-4,tclev,thlev,tdlev)
	go to 12750
c-----------------------------------------------------------------------
2979	call beta(0.293,84.,0.,0.,t)
29790	thlev=0.
	p=100.*rnd1(d)
	if(p.le.83.6) go to 29791   ! 83.6%
	              go to 29792   ! 16.4%
29791	call nucltransK(2.979,0.093,1.4e-3,6.5e-4,tclev,thlev,tdlev)
	return
29792	call nucltransK(2.369,0.093,2.8e-3,4.0e-4,tclev,thlev,tdlev)
	go to 60900
c-----------------------------------------------------------------------
2941	call beta(0.331,84.,0.,0.,t)
29410	thlev=0.
	p=100.*rnd1(d)
	if(p.le.52.1) go to 29411   ! 52.1%
	if(p.le.71.7) go to 29412   ! 19.6%
	              go to 29413   ! 28.3%
29411	call nucltransK(2.331,0.093,3.0e-3,3.8e-4,tclev,thlev,tdlev)
	go to 60900
29412	call nucltransK(1.666,0.093,7.0e-3,1.1e-4,tclev,thlev,tdlev)
	go to 12750
29413	call nucltransK(1.279,0.093,1.2e-2,1.5e-5,tclev,thlev,tdlev)
	go to 16610
c-----------------------------------------------------------------------
2935	call beta(0.337,84.,0.,0.,t)
29350	thlev=0.
	p=100.*rnd1(d)
	if(p.le.21.3) go to 29351   ! 21.3%
	              go to 29352   ! 78.7%
29351	call nucltransK(2.935,0.093,1.5e-3,6.3e-4,tclev,thlev,tdlev)
	return
29352	call nucltransK(2.325,0.093,3.0e-3,3.7e-4,tclev,thlev,tdlev)
	go to 60900
c-----------------------------------------------------------------------
2929	call beta(0.343,84.,0.,0.,t)
29290	thlev=0.
	p=100.*rnd1(d)
	if(p.le.73.3) go to 29291   ! 73.3%
	              go to 29292   ! 26.7%
29291	call nucltransK(2.929,0.093,1.5e-3,6.2e-4,tclev,thlev,tdlev)
	return
29292	call nucltransK(2.319,0.093,3.0e-3,3.7e-4,tclev,thlev,tdlev)
	go to 60900
c-----------------------------------------------------------------------
2922	call beta(0.350,84.,0.,0.,t)
29220	thlev=0.
	p=100.*rnd1(d)
	if(p.le.60.9) go to 29221   ! 60.9%
	              go to 29222   ! 39.1%
29221	call nucltransK(2.922,0.093,4.5e-4,8.0e-4,tclev,thlev,tdlev)
	return
29222	call nucltransK(2.312,0.093,3.0e-3,3.7e-4,tclev,thlev,tdlev)
	go to 60900
c-----------------------------------------------------------------------
2920	call beta(0.352,84.,0.,0.,t)
29200	thlev=0.
	call nucltransK(2.310,0.093,3.0e-3,3.7e-4,tclev,thlev,tdlev)
	go to 60900
c-----------------------------------------------------------------------
2897	call beta(0.375,84.,0.,0.,t)
28970	thlev=0.
	call nucltransK(2.287,0.093,3.0e-3,3.6e-4,tclev,thlev,tdlev)
	go to 60900
c-----------------------------------------------------------------------
2894	call beta(0.378,84.,0.,0.,t)
28940	thlev=0.
	p=100.*rnd1(d)
	if(p.le.26.1) go to 28941   ! 26.1%
	if(p.le.48.3) go to 28942   ! 22.2%
	if(p.le.78.3) go to 28943   ! 30.0%
	              go to 28944   ! 21.7%
28941	call nucltransK(2.894,0.093,1.7e-3,6.1e-4,tclev,thlev,tdlev)
	return
28942	call nucltransK(2.284,0.093,3.0e-3,3.6e-4,tclev,thlev,tdlev)
	go to 60900
28943	call nucltransK(1.516,0.093,9.0e-3,6.7e-5,tclev,thlev,tdlev)
	go to 13780
28944	call nucltransK(0.626,0.093,7.5e-2,0.,tclev,thlev,tdlev)
	go to 22660
c-----------------------------------------------------------------------
2880	call beta(0.392,84.,0.,0.,t)
28800	thlev=0.
	p=100.*rnd1(d)
	if(p.le.87.6) go to 28801   ! 87.6%
	              go to 28802   ! 12.4%
28801	call nucltransK(2.880,0.093,1.7e-3,6.0e-4,tclev,thlev,tdlev)
	return
28802	call nucltransK(2.270,0.093,3.0e-3,3.5e-4,tclev,thlev,tdlev)
	go to 60900
c-----------------------------------------------------------------------
2870	call beta(0.402,84.,0.,0.,t)
28700	thlev=0.
	p=100.*rnd1(d)
	if(p.le.63.5) go to 28701   ! 63.5%
	              go to 28702   ! 36.5%
28701	call nucltransK(2.260,0.093,3.0e-3,3.5e-4,tclev,thlev,tdlev)
	go to 60900
28702	call nucltransK(1.595,0.093,8.0e-3,9.1e-5,tclev,thlev,tdlev)
	go to 12750
c-----------------------------------------------------------------------
2861	call beta(0.411,84.,0.,0.,t)
28610	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 2.7) go to 28611   !  2.7%
	if(p.le.41.5) go to 28612   ! 38.8%
	              go to 28613   ! 58.5%
28611	call nucltransK(2.861,0.093,1.8e-3,6.0e-4,tclev,thlev,tdlev)
	return
28612	call nucltransK(2.252,0.093,3.5e-3,3.4e-4,tclev,thlev,tdlev)
	go to 60900
28613	call nucltransK(1.014,0.093,2.2e-2,0.,tclev,thlev,tdlev)
	go to 18470
c-----------------------------------------------------------------------
2827	call beta(0.445,84.,0.,0.,t)
28270	thlev=0.
	call nucltransK(2.827,0.093,1.8e-3,5.8e-4,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
2786	call beta(0.486,84.,0.,0.,t)
27860	thlev=0.
	p=100.*rnd1(d)
	if(p.le.15.4) go to 27861   ! 15.4%
	if(p.le.24.4) go to 27862   !  9.0%
	if(p.le.63.6) go to 27863   ! 39.2%
	              go to 27864   ! 36.4%
27861	call nucltransK(2.786,0.093,1.8e-3,5.7e-4,tclev,thlev,tdlev)
	return
27862	call nucltransK(2.177,0.093,3.5e-3,3.1e-4,tclev,thlev,tdlev)
	go to 60900
27863	call nucltransK(1.021,0.093,1.2e-2,0.,tclev,thlev,tdlev)
	go to 17640
27864	call nucltransK(0.939,0.093,1.4e-2,0.,tclev,thlev,tdlev)
	go to 18470
c-----------------------------------------------------------------------
2770	call beta(0.502,84.,0.,0.,t)
27700	thlev=0.
	p=100.*rnd1(d)
	if(p.le.54.6) go to 27701   ! 54.6%
	if(p.le.58.5) go to 27702   !  3.9%
	              go to 27703   ! 41.5%
27701	call nucltransK(2.770,0.093,1.8e-3,5.6e-4,tclev,thlev,tdlev)
	return
27702	call nucltransK(2.160,0.093,3.5e-3,3.1e-4,tclev,thlev,tdlev)
	go to 60900
27703	call nucltransK(1.393,0.093,1.0e-2,3.6e-5,tclev,thlev,tdlev)
	go to 13780
c-----------------------------------------------------------------------
2729	call beta(0.543,84.,0.,0.,t)
27290	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 1.2) go to 27291   !  1.2%
	if(p.le. 6.0) go to 27292   !  4.8%
	if(p.le.71.5) go to 27293   ! 65.5%
	if(p.le.79.4) go to 27294   !  7.9%
	if(p.le.82.6) go to 27295   !  3.2%
	if(p.le.85.7) go to 27296   !  3.1%
	              go to 27297   ! 14.3%
27291	call nucltransK(2.120,0.093,3.5e-3,2.9e-4,tclev,thlev,tdlev)
	go to 60900
27292	call nucltransK(1.067,0.093,2.1e-2,1.4e-7,tclev,thlev,tdlev)
	go to 16610
27293	call nucltransK(0.964,0.093,1.4e-2,0.,tclev,thlev,tdlev)
	go to 17640
27294	call nucltransK(0.734,0.093,2.7e-2,0.,tclev,thlev,tdlev)
	go to 19950
27295	call nucltransK(0.525,0.093,6.0e-2,0.,tclev,thlev,tdlev)
	go to 22040
27296	call nucltransK(0.520,0.093,7.0e-2,0.,tclev,thlev,tdlev)
	go to 22090
27297	call nucltransK(0.281,0.093,3.3e-1,0.,tclev,thlev,tdlev)
	go to 24480
c-----------------------------------------------------------------------
2719	call beta(0.553,84.,0.,0.,t)
27190	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.7) go to 27191   !  0.7%
	if(p.le.33.6) go to 27192   ! 32.9%
	if(p.le.41.8) go to 27193   !  8.2%
	if(p.le.83.6) go to 27194   ! 41.8%
	if(p.le.90.7) go to 27195   !  7.1%
	if(p.le.97.0) go to 27196   !  6.3%
	              go to 27197   !  3.0%
27191	call nucltransK(2.719,0.093,2.0e-3,5.4e-4,tclev,thlev,tdlev)
	return
27192	call nucltransK(2.110,0.093,3.5e-3,2.9e-4,tclev,thlev,tdlev)
	go to 60900
27193	call nucltransK(1.341,0.093,1.0e-2,2.6e-5,tclev,thlev,tdlev)
	go to 13780
27194	call nucltransK(1.304,0.093,1.0e-2,1.9e-5,tclev,thlev,tdlev)
	go to 14150
27195	call nucltransK(0.976,0.093,2.6e-2,0.,tclev,thlev,tdlev)
	go to 17430
27196	call nucltransK(0.709,0.093,2.9e-2,0.,tclev,thlev,tdlev)
	go to 20110
27197	call nucltransK(0.600,0.093,9.0e-2,0.,tclev,thlev,tdlev)
	go to 21190
c-----------------------------------------------------------------------
2699	call beta(0.573,84.,0.,0.,t)
26990	thlev=0.
	p=100.*rnd1(d)
	if(p.le.5.3) go to 26991   !  5.3%
	             go to 26992   ! 94.7%
26991	call nucltransK(2.699,0.093,2.0e-3,5.3e-4,tclev,thlev,tdlev)
	return
26992	call nucltransK(2.090,0.093,4.0e-3,2.8e-4,tclev,thlev,tdlev)
	go to 60900
c-----------------------------------------------------------------------
2698	call beta(0.574,84.,0.,0.,t)
26980	thlev=0.
	p=100.*rnd1(d)
	if(p.le.29.5) go to 26981   ! 29.5%
	if(p.le.44.8) go to 26982   ! 15.3%
	if(p.le.63.3) go to 26983   ! 18.5%
	if(p.le.76.0) go to 26984   ! 12.7%
	              go to 26985   ! 24.0%
26981	call nucltransK(1.156,0.093,1.6e-2,2.9e-6,tclev,thlev,tdlev)
	go to 15430
26982	call nucltransK(1.038,0.093,2.0e-2,0.,tclev,thlev,tdlev)
	go to 16610
26983	call nucltransK(0.935,0.093,2.8e-2,0.,tclev,thlev,tdlev)
	go to 17640
26984	call nucltransK(0.688,0.093,6.5e-2,0.,tclev,thlev,tdlev)
	go to 20110
26985	call nucltransK(0.494,0.093,7.0e-2,0.,tclev,thlev,tdlev)
	go to 22040
c-----------------------------------------------------------------------
2695	call beta(0.577,84.,0.,0.,t)
26950	thlev=0.
	p=100.*rnd1(d)
	if(p.le.11.9) go to 26951   ! 11.9%
	if(p.le.15.4) go to 26952   !  3.5%
	if(p.le.17.4) go to 26953   !  2.0%
	if(p.le.48.7) go to 26954   ! 30.7%
	if(p.le.57.3) go to 26955   !  9.2%
	if(p.le.59.6) go to 26956   !  2.3%
	if(p.le.72.3) go to 26957   ! 12.7%
	if(p.le.82.3) go to 26958   ! 10.0%
	if(p.le.88.4) go to 26959   !  6.1%
	if(p.le.90.7) go to 26960   !  2.3%
	              go to 26961   !  9.3%
26951	call nucltransK(2.695,0.093,2.0e-3,5.3e-4,tclev,thlev,tdlev)
	return
26952	call nucltransK(2.085,0.093,4.0e-3,2.8e-4,tclev,thlev,tdlev)
	go to 60900
26953	call nucltransK(1.420,0.093,9.5e-3,4.2e-5,tclev,thlev,tdlev)
	go to 12750
26954	call nucltransK(1.317,0.093,1.2e-2,2.1e-5,tclev,thlev,tdlev)
	go to 13780
26955	call nucltransK(1.033,0.093,2.2e-2,0.,tclev,thlev,tdlev)
	go to 16610
26956	call nucltransK(0.952,0.093,2.8e-2,0.,tclev,thlev,tdlev)
	go to 17430
26957	call nucltransK(0.930,0.093,1.5e-2,0.,tclev,thlev,tdlev)
	go to 17640
26958	call nucltransK(0.847,0.093,1.9e-2,0.,tclev,thlev,tdlev)
	go to 18470
26959	call nucltransK(0.700,0.093,3.0e-2,0.,tclev,thlev,tdlev)
	go to 19950
26960	call nucltransK(0.677,0.093,7.5e-2,0.,tclev,thlev,tdlev)
	go to 20170
26961	call nucltransK(0.486,0.093,8.0e-2,0.,tclev,thlev,tdlev)
	go to 22090
c-----------------------------------------------------------------------
2662	call beta(0.610,84.,0.,0.,t)
26620	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.2) go to 26621   !  0.2%
	if(p.le.55.0) go to 26622   ! 54.8%
	if(p.le.63.7) go to 26623   !  8.7%
	if(p.le.95.6) go to 26624   ! 31.9%
	              go to 26625   !  4.4%
26621	call nucltransK(2.662,0.093,2.0e-3,5.1e-4,tclev,thlev,tdlev)
	return
26622	call nucltransK(2.053,0.093,4.0e-3,2.6e-4,tclev,thlev,tdlev)
	go to 60900
26623	call nucltransK(1.284,0.093,1.3e-2,1.6e-5,tclev,thlev,tdlev)
	go to 13780
26624	call nucltransK(1.119,0.093,1.9e-2,1.2e-6,tclev,thlev,tdlev)
	go to 15430
26625	call nucltransK(0.950,0.093,2.8e-2,0.,tclev,thlev,tdlev)
	go to 17130
c-----------------------------------------------------------------------
2631	call beta(0.641,84.,0.,0.,t)
26310	thlev=0.
	p=100.*rnd1(d)
	if(p.le.3.8) go to 26311   !  3.8%
	             go to 26312   ! 96.2%
26311	call nucltransK(2.631,0.093,2.2e-3,5.0e-4,tclev,thlev,tdlev)
	return
26312	call nucltransK(2.022,0.093,4.0e-3,2.5e-4,tclev,thlev,tdlev)
	go to 60900
c-----------------------------------------------------------------------
2605	call beta(0.667,84.,0.,0.,t)
26050	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.5) go to 26051   !  0.5%
	if(p.le. 7.1) go to 26052   !  6.6%
	if(p.le.21.7) go to 26053   ! 14.6%
	if(p.le.44.2) go to 26054   ! 22.5%
	if(p.le.56.1) go to 26055   ! 11.9%
	              go to 26056   ! 43.9%
26051	call nucltransK(2.605,0.093,2.2e-3,4.9e-4,tclev,thlev,tdlev)
	return
26052	call nucltransK(1.995,0.093,4.5e-3,2.4e-4,tclev,thlev,tdlev)
	go to 60900
26053	call nucltransK(1.330,0.093,1.1e-2,2.4e-5,tclev,thlev,tdlev)
	go to 12750
26054	call nucltransK(0.943,0.093,2.8e-2,0.,tclev,thlev,tdlev)
	go to 16610
26055	call nucltransK(0.840,0.093,4.0e-2,0.,tclev,thlev,tdlev)
	go to 17640
26056	call nucltransK(0.396,0.093,1.3e-1,0.,tclev,thlev,tdlev)
	go to 22090
c-----------------------------------------------------------------------
2545	call beta(0.727,84.,0.,0.,t)
25450	thlev=0.
	p=100.*rnd1(d)
	if(p.le.77.4) go to 25451   ! 77.4%
	              go to 25452   ! 22.6%
25451	call nucltransK(1.936,0.093,3.0e-3,2.1e-4,tclev,thlev,tdlev)
	go to 60900
25452	call nucltransK(1.167,0.093,1.6e-2,3.6e-6,tclev,thlev,tdlev)
	go to 13780
c-----------------------------------------------------------------------
2508	call beta(0.764,84.,0.,0.,t)
25080	thlev=0.
	p=100.*rnd1(d)
	if(p.le.49.8) go to 25081   ! 49.8%
	if(p.le.84.8) go to 25082   ! 35.0%
	if(p.le.93.5) go to 25083   !  8.7%
	              go to 25084   !  6.5%
25081	call nucltransK(1.899,0.093,5.0e-3,2.0e-4,tclev,thlev,tdlev)
	go to 60900
25082	call nucltransK(1.130,0.093,1.8e-2,1.6e-6,tclev,thlev,tdlev)
	go to 13780
25083	call nucltransK(0.965,0.093,2.6e-2,0.,tclev,thlev,tdlev)
	go to 15430
25084	call nucltransK(0.497,0.093,7.0e-2,0.,tclev,thlev,tdlev)
	go to 20110
c-----------------------------------------------------------------------
2505	call beta(0.767,84.,0.,0.,t)
25050	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 3.0) go to 25051   !  3.0%
	if(p.le.86.0) go to 25052   ! 83.0%
	if(p.le.93.8) go to 25053   !  7.8%
	              go to 25054   !  6.2%
25051	call nucltransK(2.505,0.093,2.4e-3,4.5e-4,tclev,thlev,tdlev)
	return
25052	call nucltransK(1.896,0.093,5.0e-3,2.0e-4,tclev,thlev,tdlev)
	go to 60900
25053	call nucltransK(1.231,0.093,1.4e-2,9.2e-6,tclev,thlev,tdlev)
	go to 12750
25054	call nucltransK(0.962,0.093,2.6e-2,0.,tclev,thlev,tdlev)
	go to 15430
c-----------------------------------------------------------------------
2482	call beta(0.790,84.,0.,0.,t)
24820	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.1) go to 24821   !  0.1%
	if(p.le.16.7) go to 24822   ! 16.6%
	if(p.le.51.0) go to 24823   ! 34.3%
	if(p.le.56.9) go to 24824   !  5.9%
	if(p.le.58.3) go to 24825   !  1.4%
	if(p.le.70.7) go to 24826   ! 12.4%
	if(p.le.80.9) go to 24827   ! 10.2%
	if(p.le.81.4) go to 24828   !  0.5%
	if(p.le.83.5) go to 24829   !  2.1%
	if(p.le.84.8) go to 24830   !  1.3%
	              go to 24831   ! 15.2%
24821	call nucltransK(2.483,0.093,2.5e-3,4.4e-4,tclev,thlev,tdlev)
	return
24822	call nucltransK(1.873,0.093,5.0e-3,1.9e-4,tclev,thlev,tdlev)
	go to 60900
24823	call nucltransKLM(1.208,0.093,1.6e-3,0.017,2.5e-4,0.004,8.0e-5,
     +                    6.4e-5,tclev,thlev,tdlev)
	go to 12750
24824	call nucltransK(1.105,0.093,1.0e-2,7.8e-7,tclev,thlev,tdlev)
	go to 13780
24825	call nucltransKLM(0.940,0.093,1.4e-2,0.017,2.5e-3,0.004,5.0e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 15430
24826	call nucltransK(0.821,0.093,3.7e-2,0.,tclev,thlev,tdlev)
	go to 16610
24827	call nucltransKLM(0.753,0.093,2.4e-2,0.017,4.4e-3,0.004,6.0e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 17300
24828	call nucltransKLM(0.635,0.093,3.6e-2,0.017,7.0e-3,0.004,7.0e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 18470
24829	call nucltransKLM(0.488,0.093,8.9e-3,0.017,1.5e-3,0.004,4.5e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 19950
24830	call nucltransK(0.394,0.093,1.3e-1,0.,tclev,thlev,tdlev)
	go to 20880
24831	call nucltransK(0.274,0.093,3.6e-1,0.,tclev,thlev,tdlev)
	go to 22090
c-----------------------------------------------------------------------
2448	call beta(0.824,84.,0.,0.,t)
24480	thlev=0.
	p=100.*rnd1(d)
	if(p.le.54.3) go to 24481   ! 54.3%
	if(p.le.66.8) go to 24482   ! 12.5%
	if(p.le.68.6) go to 24483   !  1.8%
	if(p.le.78.1) go to 24484   !  9.5%
	if(p.le.80.8) go to 24485   !  2.7%
	if(p.le.83.7) go to 24486   !  2.9%
	if(p.le.94.4) go to 24487   ! 10.7%
	if(p.le.96.0) go to 24488   !  1.6%
	if(p.le.98.8) go to 24489   !  2.8%
	              go to 24490   !  1.2%
24481	call nucltransK(2.448,0.093,6.0e-4,9.4e-4,tclev,thlev,tdlev)
	return
24482	call nucltransK(1.838,0.093,1.2e-3,5.3e-4,tclev,thlev,tdlev)
	go to 60900
24483	call nucltransKLM(1.173,0.093,4.4e-3,0.017,8.3e-4,0.004,2.8e-4,
     +                    6.2e-6,tclev,thlev,tdlev)
	go to 12750
24484	call nucltransKLM(1.070,0.093,2.0e-3,0.017,3.1e-4,0.004,1.0e-4,
     +                    5.5e-6,tclev,thlev,tdlev)
	go to 13780
24485	call nucltransKLM(1.032,0.093,2.1e-3,0.017,3.3e-4,0.004,1.1e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 14150
24486	call nucltransKLM(0.904,0.093,2.7e-3,0.017,4.2e-4,0.004,1.4e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 15430
24487	call nucltransKLM(0.786,0.093,3.5e-3,0.017,5.5e-4,0.004,1.9e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 16610
24488	call nucltransKLM(0.705,0.093,4.3e-3,0.017,6.9e-4,0.004,2.2e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 17430
24489	call nucltransKLM(0.683,0.093,4.6e-3,0.017,7.3e-4,0.004,2.4e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 17640
24490	call nucltransKLM(0.453,0.093,8.0e-2,0.017,1.7e-2,0.004,5.6e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 19950
c-----------------------------------------------------------------------
2423	call beta(0.849,84.,0.,0.,t)
24230	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 7.2) go to 24231   !  7.2%
	if(p.le.24.5) go to 24232   ! 17.3%
	if(p.le.65.4) go to 24233   ! 40.9%
	if(p.le.74.8) go to 24234   !  9.4%
	              go to 24235   ! 25.2%
24231	call nucltransK(2.423,0.093,6.5e-4,9.2e-4,tclev,thlev,tdlev)
	return
24232	call nucltransK(1.814,0.093,1.2e-3,5.1e-4,tclev,thlev,tdlev)
	go to 60900
24233	call nucltransK(1.046,0.093,2.8e-3,1.4e-6,tclev,thlev,tdlev)
	go to 13780
24234	call nucltransK(0.693,0.093,5.7e-3,0.,tclev,thlev,tdlev)
	go to 17300
24235	call nucltransK(0.659,0.093,3.5e-2,0.,tclev,thlev,tdlev)
	go to 17640
c-----------------------------------------------------------------------
2293	call beta(0.979,84.,0.,0.,t)
22930	thlev=0.
	p=100.*rnd1(d)
	if(p.le.54.6) go to 22931   ! 54.6%
	if(p.le.93.2) go to 22932   ! 38.6%
	if(p.le.97.9) go to 22933   !  4.7%
	              go to 22934   !  2.1%
22931	call nucltransK(2.293,0.093,3.0e-3,3.6e-4,tclev,thlev,tdlev)
	return
22932	call nucltransK(1.684,0.093,7.0e-3,1.2e-4,tclev,thlev,tdlev)
	go to 60900
22933	call nucltransK(0.916,0.093,1.8e-2,0.,tclev,thlev,tdlev)
	go to 13780
22934	call nucltransK(0.878,0.093,2.0e-2,0.,tclev,thlev,tdlev)
	go to 14150
c-----------------------------------------------------------------------
2266	call beta(1.006,84.,0.,0.,t)
22660	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 9.0) go to 22661   !  9.0%
	if(p.le.31.9) go to 22662   ! 22.9%
	if(p.le.36.9) go to 22663   !  5.0%
	if(p.le.54.8) go to 22664   ! 17.9%
	if(p.le.90.6) go to 22665   ! 35.8%
	              go to 22666   !  9.4%
22661	call nucltransK(2.266,0.093,1.8e-3,4.4e-4,tclev,thlev,tdlev)
	return
22662	call nucltransK(1.657,0.093,1.3e-3,3.9e-4,tclev,thlev,tdlev)
	go to 60900
22663	call nucltransK(0.991,0.093,1.3e-2,0.,tclev,thlev,tdlev)
	go to 12750
22664	call nucltransK(0.723,0.093,2.8e-2,0.,tclev,thlev,tdlev)
	go to 15430
22665	call nucltransK(0.537,0.093,6.0e-2,0.,tclev,thlev,tdlev)
	go to 17300
22666	call nucltransK(0.502,0.093,7.0e-2,0.,tclev,thlev,tdlev)
	go to 17640
c-----------------------------------------------------------------------
22090	thlev=0.
	p=100.*rnd1(d)
	if(p.le.82.1) go to 22091   ! 82.1%
	              go to 22092   ! 17.9%
22091	call nucltransK(1.599,0.093,8.0e-3,9.2e-5,tclev,thlev,tdlev)
	go to 60900
22092	call nucltransK(0.934,0.093,2.8e-2,0.,tclev,thlev,tdlev)
	go to 12750
c-----------------------------------------------------------------------
2204	call beta(1.068,84.,0.,0.,t)
22040	thlev=0.
	p=100.*rnd1(d)
	if(p.le.87.9) go to 22041   ! 87.9%
	if(p.le.92.2) go to 22042   !  4.3%
	if(p.le.94.2) go to 22043   !  2.0%
	if(p.le.94.5) go to 22044   !  0.3%
	if(p.le.95.3) go to 22045   !  0.8%
	if(p.le.96.9) go to 22046   !  1.6%
	if(p.le.99.0) go to 22047   !  2.1%
	              go to 22048   !  1.0%
22041	call nucltransK(2.204,0.093,3.5e-3,3.2e-4,tclev,thlev,tdlev)
	return
22042	call nucltransK(1.595,0.093,5.5e-3,9.1e-5,tclev,thlev,tdlev)
	go to 60900
22043	call nucltransKLM(0.826,0.093,2.9e-2,0.017,5.0e-3,0.004,2.0e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 13780
22044	call nucltransKLM(0.789,0.093,3.3e-2,0.017,5.7e-3,0.004,1.9e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 14150
22045	call nucltransKLM(0.661,0.093,3.3e-2,0.017,6.0e-3,0.004,2.0e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 15430
22046	call nucltransKLM(0.543,0.093,5.0e-2,0.017,1.1e-2,0.004,9.0e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 17130
22047	call nucltransKLM(0.474,0.093,7.0e-2,0.017,1.5e-2,0.004,4.9e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 17300
22048	call nucltransKLM(0.461,0.093,1.4e-1,0.017,2.3e-2,0.004,7.4e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 17430
c-----------------------------------------------------------------------
2193	call beta(1.079,84.,0.,0.,t)
21930	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 4.1) go to 21931   !  4.1%
	if(p.le.87.2) go to 21932   ! 83.1%
	if(p.le.87.8) go to 21933   !  0.6%
	if(p.le.92.5) go to 21934   !  4.7%
	              go to 21935   !  7.5%
21931	call nucltransK(2.193,0.093,2.0e-3,4.1e-4,tclev,thlev,tdlev)
	return
21932	call nucltransK(1.583,0.093,5.4e-3,8.7e-5,tclev,thlev,tdlev)
	go to 60900
21933	call nucltransKLM(0.918,0.093,2.6e-3,0.017,4.1e-4,0.004,1.4e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 12750
21934	call nucltransKLM(0.815,0.093,1.9e-2,0.017,3.6e-3,0.004,1.4e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 13780
21935	call nucltransKLM(0.649,0.093,3.4e-2,0.017,6.0e-3,0.004,3.0e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 15430
c-----------------------------------------------------------------------
2148	call beta(1.122,84.,0.,0.,t)
21480	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 3.2) go to 21481   !  3.2%
	if(p.le.89.0) go to 21482   ! 85.8%
	if(p.le.93.1) go to 21483   !  4.1%
	              go to 21484   !  6.9%
21481	call nucltransK(2.148,0.093,3.5e-3,3.0e-4,tclev,thlev,tdlev)
	return
21482	call nucltransK(1.539,0.093,8.0e-3,1.0e-4,tclev,thlev,tdlev)
	go to 60900
21483	call nucltransK(0.873,0.093,3.5e-2,0.,tclev,thlev,tdlev)
	go to 12750
21484	call nucltransK(0.770,0.093,4.5e-2,0.,tclev,thlev,tdlev)
	go to 13780
c-----------------------------------------------------------------------
2119	call beta(1.151,84.,0.,0.,t)
21190	thlev=0.
	p=100.*rnd1(d)
	if(p.le.26.7) go to 21191   ! 26.7%
	if(p.le.76.4) go to 21192   ! 49.7%
	if(p.le.77.3) go to 21193   !  0.9%
	if(p.le.89.0) go to 21194   ! 11.7%
	              go to 21195   ! 11.0%
21191	call nucltransK(2.119,0.093,3.5e-3,2.9e-4,tclev,thlev,tdlev)
	return
21192	call nucltransK(1.509,0.093,6.3e-3,6.5e-5,tclev,thlev,tdlev)
	go to 60900
21193	call nucltransK(0.741,0.093,3.1e-2,0.,tclev,thlev,tdlev)
	go to 13780
21194	call nucltransKLM(0.703,0.093,4.5e-2,0.017,7.6e-3,0.004,2.6e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 14150
21195	call nucltransKLM(0.389,0.093,2.1e-1,0.017,3.7e-2,0.004,1.2e-2,
     +                    0.,tclev,thlev,tdlev)
	go to 17300
c-----------------------------------------------------------------------
2088	call beta(1.181,84.,0.,0.,t)
20880	thlev=0.
	p=100.*rnd1(d)
	if(p.le.40.5) go to 20881   ! 40.5%
	              go to 20882   ! 59.5%
20881	call nucltransK(1.479,0.093,9.0e-3,5.7e-5,tclev,thlev,tdlev)
	go to 60900
20882	call nucltransK(0.711,0.093,6.0e-2,0.,tclev,thlev,tdlev)
	go to 13780
c-----------------------------------------------------------------------
2017	call beta(1.253,84.,0.,0.,t)
20170	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 0.27) go to 20171   !  0.27%
	if(p.le.98.01) go to 20172   ! 97.74%
	if(p.le.99.41) go to 20173   !  1.40%
	if(p.le.99.75) go to 20174   !  0.34%
	               go to 20175   !  0.25%
20171	p=100.*rnd1(d)
	if(p.le.95.08) then                       ! electron conversion
	   call electron(1.923,tclev,thlev,tdlev)          
	   call gamma(0.093,0.,0.,tdlev)
	else                                      ! pair conversion
	   call pair(0.995,tclev,thlev,tdlev)
	endif
	return
20172	call nucltransKLM(1.408,0.093,3.1e-3,0.017,5.7e-4,0.004,1.9e-4,
     +                    5.7e-5,tclev,thlev,tdlev)
	go to 60900
20173	call nucltransKLM(0.640,0.093,1.4e-2,0.017,3.7e-3,0.004,1.2e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 13780
20174	call nucltransKLM(0.356,0.093,4.6e-2,0.017,2.4e-2,0.004,8.1e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 16610
20175	call nucltransKLM(0.253,0.093,6.9e-1,0.017,1.2e-1,0.004,3.8e-2,
     +                    0.,tclev,thlev,tdlev)
	go to 17640
c-----------------------------------------------------------------------
2011	call beta(1.259,84.,0.,0.,t)
20110	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 3.4) go to 20111   !  3.4%
	if(p.le.94.7) go to 20112   ! 91.3%
	if(p.le.98.7) go to 20113   !  4.0%
	              go to 20114   !  1.3%
20111	call nucltransK(2.011,0.093,2.2e-3,3.2e-4,tclev,thlev,tdlev)
	return
20112	call nucltransKLM(1.402,0.093,4.4e-3,0.017,7.7e-4,0.004,2.3e-4,
     +                    3.8e-5,tclev,thlev,tdlev)
	go to 60900
20113	call nucltransK(0.633,0.093,4.5e-2,0.,tclev,thlev,tdlev)
	go to 13780
20114	call nucltransK(0.595,0.093,5.3e-2,0.,tclev,thlev,tdlev)
	go to 14150
c-----------------------------------------------------------------------
1995	call beta(1.275,84.,0.,0.,t)
19950	thlev=0.
	p=100.*rnd1(d)
	if(p.le.60.3) go to 19951   ! 60.3%
	if(p.le.90.8) go to 19952   ! 30.5%
	if(p.le.93.5) go to 19953   !  2.7%
	              go to 19954   !  6.5%
19951	call nucltransKLM(1.385,0.093,1.3e-3,0.017,2.0e-4,0.004,6.4e-5,
     +                    1.8e-4,tclev,thlev,tdlev)
	go to 60900
19952	call nucltransKLM(0.720,0.093,1.1e-2,0.017,2.7e-3,0.004,9.3e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 12750
19953	call nucltransK(0.617,0.093,6.8e-3,0.,tclev,thlev,tdlev)
	go to 13780
19954	call nucltransKLM(0.333,0.093,2.0e-2,0.017,3.5e-3,0.004,1.1e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 16610
c-----------------------------------------------------------------------
1890	call beta(1.380,84.,0.,0.,t)
18900	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 5.0) go to 18901   !  5.0%
	if(p.le.96.2) go to 18902   ! 91.2%
	              go to 18903   !  3.8%
18901	call nucltransK(1.890,0.093,2.8e-3,2.6e-4,tclev,thlev,tdlev)
	return
18902	call nucltransKLM(1.281,0.093,9.5e-3,0.017,1.6e-3,0.004,5.5e-4,
     +                    1.6e-5,tclev,thlev,tdlev)
	go to 60900
18903	call nucltransKLM(0.616,0.093,5.6e-3,0.017,9.0e-4,0.004,3.0e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 12750
c-----------------------------------------------------------------------
1847	call beta(1.423,84.,0.,0.,t)
18470	thlev=0.
	p=100.*rnd1(d)
	if(p.le.25.7) go to 18471   ! 25.7%
	if(p.le.97.0) go to 18472   ! 71.3%
	if(p.le.97.3) go to 18473   !  0.3%
	if(p.le.98.2) go to 18474   !  0.9%
	              go to 18475   !  1.8%
18471	call nucltransK(1.847,0.093,2.5e-3,2.4e-4,tclev,thlev,tdlev)
	return
18472	call nucltransKLM(1.238,0.093,1.0e-2,0.017,1.8e-3,0.004,5.5e-4,
     +                    1.0e-5,tclev,thlev,tdlev)
	go to 60900
18473	call nucltransKLM(0.832,0.093,8.2e-3,0.017,1.8e-3,0.004,6.4e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 10150
18474	call nucltransKLM(0.573,0.093,6.4e-3,0.017,1.1e-3,0.004,3.4e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 12750
18475	call nucltransKLM(0.470,0.093,8.0e-2,0.017,1.6e-2,0.004,5.1e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 13780
c-----------------------------------------------------------------------
1764	call beta(1.505,84.,0.,0.,t)
17640	thlev=0.
	p=100.*rnd1(d)
	if(p.le.87.61) go to 17641   ! 87.61%
	if(p.le.97.01) go to 17642   !  9.40%
	if(p.le.99.06) go to 17643   !  2.05%
	if(p.le.99.97) go to 17644   !  0.91%
	               go to 17645   !  0.03%
17641	call nucltransK(1.764,0.093,6.0e-3,1.5e-4,tclev,thlev,tdlev)
	return
17642	call nucltransKLM(1.155,0.093,1.2e-2,0.017,2.0e-3,0.004,6.3e-4,
     +                    2.8e-6,tclev,thlev,tdlev)
	go to 60900
17643	call nucltransKLM(0.387,0.093,1.3e-1,0.017,2.8e-2,0.004,8.9e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 13780
17644	call nucltransKLM(0.349,0.093,2.9e-1,0.017,5.0e-2,0.004,1.6e-2,
     +                    0.,tclev,thlev,tdlev)
	go to 14150
17645	call nucltransKLM(0.221,0.093,5.6e-1,0.017,1.6e-1,0.004,5.4e-2,
     +                    0.,tclev,thlev,tdlev)
	go to 15430
c-----------------------------------------------------------------------
1743	call beta(1.527,84.,0.,0.,t)
17430	thlev=0.
	call nucltransKLM(1.134,0.093,4.7e-3,0.017,9.0e-4,0.004,2.9e-4,
     +                    2.8e-6,tclev,thlev,tdlev)
	go to 60900
c-----------------------------------------------------------------------
1730	call beta(1.540,84.,0.,0.,t)
17300	thlev=0.
	p=100.*rnd1(d)
	if(p.le.15.66) go to 17301   ! 15.66%
	if(p.le.97.92) go to 17302   ! 82.26%
	if(p.le.99.55) go to 17303   !  1.63%
	               go to 17304   !  0.45%
17301	call nucltransK(1.730,0.093,2.7e-3,1.9e-4,tclev,thlev,tdlev)
	return
17302	call nucltransKLM(1.120,0.093,1.3e-2,0.017,2.2e-3,0.004,6.7e-4,
     +                    1.2e-6,tclev,thlev,tdlev)
	go to 60900
17303	call nucltransKLM(0.455,0.093,1.0e-2,0.017,1.7e-3,0.004,5.3e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 12750
17304	call nucltransKLM(0.352,0.093,1.6e-1,0.017,3.7e-2,0.004,1.2e-2,
     +                    0.,tclev,thlev,tdlev)
	go to 13780
c-----------------------------------------------------------------------
1713	call beta(1.540,84.,0.,0.,t)
17130	thlev=0.
	p=100.*rnd1(d)
	if(p.le.65.36) go to 17131   ! 65.36%
	               go to 17132   ! 34.64%
17131	call nucltransK(1.104,0.093,1.2e-2,7.6e-7,tclev,thlev,tdlev)
	go to 60900
17132	call nucltransK(0.698,0.093,6.0e-2,0.,tclev,thlev,tdlev)
	go to 10150
c-----------------------------------------------------------------------
1661	call beta(1.609,84.,0.,0.,t)
16610	thlev=0.
	p=100.*rnd1(d)
	if(p.le.78.23) go to 16611   ! 78.23%
	               go to 16612   ! 21.77%
16611	call nucltransK(1.661,0.093,3.0e-3,1.5e-4,tclev,thlev,tdlev)
	return
16612	call nucltransK(1.052,0.093,1.5e-2,0.,tclev,thlev,tdlev)
	go to 60900
c-----------------------------------------------------------------------
1543	call beta(1.727,84.,0.,0.,t)
15430	thlev=0.
	p=100.*rnd1(d)
	if(p.le. 6.00) go to 15431   !  6.00%
	if(p.le.99.26) go to 15432   ! 93.26%
	if(p.le.99.38) go to 15433   !  0.12%
	               go to 15434   !  0.62%
15431	call nucltransK(1.543,0.093,2.7e-3,1.1e-4,tclev,thlev,tdlev)
	return
15432	call nucltransKLM(0.934,0.093,2.0e-2,0.017,3.5e-3,0.004,1.2e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 60900
15433	call nucltransKLM(0.528,0.093,2.0e-2,0.017,6.4e-3,0.004,2.1e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 10150
15434	call nucltransKLM(0.269,0.093,3.3e-2,0.017,5.8e-3,0.004,1.8e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 12750
c-----------------------------------------------------------------------
1415	call beta(1.855,84.,0.,0.,t)
14150	thlev=99.e-12
	p=100.*rnd1(d)
	if(p.le.28.05) go to 14151   ! 28.05%
	               go to 14152   ! 71.95%
14151	p=100.*rnd1(d)
	if(p.le.99.63) then                       ! electron conversion
	   call electron(1.322,tclev,thlev,tdlev)          
	   call gamma(0.093,0.,0.,tdlev)
	else                                      ! pair conversion
	   call pair(0.393,tclev,thlev,tdlev)
	endif
	return
14152	call nucltransKLM(0.806,0.093,8.7e-3,0.017,2.0e-3,0.004,6.6e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 60900
c-----------------------------------------------------------------------
1378	call beta(1.892,84.,0.,0.,t)
13780	thlev=0.
	p=100.*rnd1(d)
	if(p.le.44.47) go to 13781   ! 44.47%
	               go to 13782   ! 55.53%
13781	call nucltransKLM(1.378,0.093,3.3e-3,0.017,5.9e-4,0.004,2.0e-4,
     +                    4.8e-5,tclev,thlev,tdlev)
	return
13782	call nucltransKLM(0.768,0.093,1.3e-2,0.017,2.7e-3,0.004,9.0e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 60900
c-----------------------------------------------------------------------
12750	thlev=0.
	call nucltransKLM(0.665,0.093,4.8e-3,0.017,7.7e-4,0.004,2.5e-4,
     +                    0.,tclev,thlev,tdlev)
	go to 60900
c-----------------------------------------------------------------------
10150	thlev=0.
	call nucltransKLM(0.406,0.093,3.5e-2,0.017,1.5e-2,0.004,5.1e-3,
     +                    0.,tclev,thlev,tdlev)
	go to 60900
c-----------------------------------------------------------------------
609	call beta(2.661,84.,0.,0.,t)
60900	thlev=0.
	call nucltransKLM(0.609,0.093,1.5e-2,0.017,4.2e-3,0.004,1.4e-3,
     +                    0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
20001	call beta(3.270,84.,0.,0.,t)
	return
c-----------------------------------------------------------------------
	end
c
c end of Bi214.f
c
c Local Variables: --
c mode: fortran --
c End: --
