c K42.f 
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
	subroutine K42(tcnuc,tdnuc)
c Scheme of K42 decay (B.Singh et al., NDS 92(2001)1
c with additional information from ToI-1978 and ToI-1998).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 31.03.2006; 29.10.2006.
	thnuc=44496.
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	tclev=0.
        pbeta=100.*rnd1(d)
        if(pbeta.le. 0.07) go to 3446
        if(pbeta.le. 0.12) go to 2424
        if(pbeta.le. 0.46) go to 1837
        if(pbeta.le.18.10) go to 1525
                           go to 10000
c-----------------------------------------------------------------------
3446    call beta(0.080,20.,0.,0.,t)
34460   thlev=250.e-15 ! ToI-1998
	p=100.*rnd1(d)
        if(p.le.63.66) go to 34461
        if(p.le.94.88) go to 34462
                       go to 34463
34461   call nucltransK(1.921,0.004,1.2e-5,5.8e-4,tclev,thlev,tdlev)
        go to 15250
34462   call nucltransK(1.021,0.004,3.9e-5,0.,tclev,thlev,tdlev)
        go to 24240
34463   call nucltransK(0.692,0.004,8.7e-5,0.,tclev,thlev,tdlev)
        go to 27530
c-----------------------------------------------------------------------
27530   thlev=3.0e-12 ! ToI-1998
27531   call nucltransK(1.228,0.004,5.6e-5,1.4e-5,tclev,thlev,tdlev)
        go to 15250
c-----------------------------------------------------------------------
2424    call beta(1.101,20.,0.,0.,t)
24240   thlev=140.e-15 ! ToI-1998
	p=100.*rnd1(d)
        if(p.le.27.78) go to 24241
                       go to 24242
24241   call nucltransK(2.424,0.004,1.5e-5,5.2e-4,tclev,thlev,tdlev)
        return
24242   call nucltransK(0.900,0.004,8.3e-5,0.,tclev,thlev,tdlev)
        go to 15250
c-----------------------------------------------------------------------
c1837    call beta(1.688,20.,0.,0.,t)
c change to the 1st forbidden unique
1837	call beta_1fu(1.688,20.,0.,0.,t,0.,0.,0.,0.)
18370   thlev=0.33e-9      
	p=100.*rnd1(d)
        if(p.le.2.1) go to 18371 ! ToI-1978
                     go to 18372
18371   p=100.*rnd1(d)
	if(p.le.90.) then
	   call pair(0.815,tclev,thlev,tdlev)
	else
	   call electron(1.833,tclev,thlev,tdlev)
	   call gamma(0.004,0.,0.,tdlev)
	endif
        return
18372   call nucltransK(0.313,0.004,3.4e-3,0.,tclev,thlev,tdlev)
        go to 15250
c-----------------------------------------------------------------------
c1525    call beta(2.000,20.,0.,0.,t)
c change to forbidden spectrum with experimental corrections from
c H.Daniel, RMP 49(1968)659
1525    call beta1(2.000,20.,0.,0.,t,0.81,0.15,-0.02,0.)
15250   thlev=0.82e-12 ! ToI-1998
	call nucltransK(1.525,0.004,3.6e-5,9.8e-5,tclev,thlev,tdlev)
        return
c-----------------------------------------------------------------------
c10000	call beta(3.525,20.,0.,0.,t)
c 1st forbidden unique with experimental corrections from 
c H.Daniel et al., PRB 136(1964)1240 and RMP 40(1968)659
10000	call beta_1fu(3.525,20.,0.,0.,t,0.,-0.01,0.,0.)
	return
c-----------------------------------------------------------------------
	end
c
c end of K42.f
c
c Local Variables: --
c mode: fortran --
c End: --
