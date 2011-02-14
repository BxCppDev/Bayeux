c beta_1fu.f 
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
	subroutine beta_1fu(Qbeta,Zdtr,tcnuc,thnuc,tdnuc,c1,c2,c3,c4)
c Subroutine beta_1fu simulates the angles and energy of beta particles
c emitted in beta decay of nucleus. The decay is considered as 1st-forbidden
c unique. Its shape is product of theoretical spectrum shape for allowed 
c decay and two correction factors:
c 1. theoretical of BJ'1969 
c       cf1(e)=pnu**2+lamda2*pel**2,
c       where lambda2 is the Coulomb function calculated in BJ'1969,
c       and pel and pnu are impulses of electron and neutrino:
c       pel=sqrt(w**2-1), pnu=(Qbeta-e)/emass , w=e/emass+1;
c 2. empirical correction 
c       cf2(e)=(1+c1/w+c2*w+c3*w**2+c4*w**3).
c Call  : common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100)
c         call beta_1fu(Qbeta,Zdtr,tcnuc,thnuc,tdnuc,c1,c2,c3,c4)
c Input : Qbeta       - beta energy endpoint (MeV; Qbeta>50 eV);
c         Zdtr        - atomic number of daughter nucleus (Zdtr>0 for e- and
c                       Zdtr<0 for e+ particles);
c         tcnuc       - time of creation of nucleus (sec);
c         thnuc       - nucleus halflife (sec);
c         c1,c2,c3,c4 - coefficients in correction factor to the spectrum
c                       shape cf2(e)=(1+c1/w+c2*w+c3*w**2+c4*w**3), 
c                       where w=e/emass+1, e - kinetic energy of electron.                        .
c Output: tdnuc               - time of decay of nucleus (sec);
c         tevst               - time of event's start (sec);
c         npfull              - current number of particle in event;
c         npgeant(npfull)     - GEANT number for particle identification
c                               (2 for beta+ and 3 for beta- decay);
c         pmoment(1-3,npfull) - x,y,z components of particle momentum (MeV);
c         ptime(npfull)       - time shift from previous time to calculate
c                               when particle was emitted (sec).
c VIT, 24.10.2006.
	external funbeta_1fu
	common/const/pi,emass,datamass(50)
	common/parbeta/z,q
	common/parbeta1/c1h,c2h,c3h,c4h
	common/bj69plog/plog69(48)
	common/bj69sl2/sl2(48)
	z=Zdtr
	q=Qbeta
	c1h=c1
	c2h=c2
	c3h=c3
	c4h=c4
c Values of the "lambda2" Coulomb function for some Zdtr values from:
c H.Behrens, J.Janecke, "Numerical tables for beta-decay and electron 
c capture", Berlin, Springer-Verlag, 1969.
c Values are calculated as product between unscreened lambda2 (Table II of BJ'1969)
c and screened corrections (Table III), and are given for "standard" values of 
c momentum (0.1-50.0 in units of m_e*c). (Log values of these momenta are in array
c plog69.) 
	do i=1,48
	   sl2(i)=1.
	enddo
	if    (int(Zdtr).eq.19) then    ! 39-Ar, Q_beta=0.565 
	   sl2( 1)=2.0929               ! p=0.1
	   sl2( 2)=1.2337               ! p=0.2
	   sl2( 3)=1.0747               ! p=0.3
	   sl2( 4)=1.0234               ! p=0.4
	   sl2( 5)=0.99977              ! p=0.5
	   sl2( 6)=0.98728              ! p=0.6
	   sl2( 7)=0.98024              ! p=0.7
	   sl2( 8)=0.97624              ! p=0.8
	   sl2( 9)=0.97445              ! p=0.9
	   sl2(10)=0.97377              ! p=1.0
	   sl2(11)=0.97406              ! p=1.2
	   sl2(12)=0.97549              ! p=1.4
	   sl2(13)=0.9757               ! p=1.6
	   sl2(14)=0.9754               ! p=1.8
	   sl2(15)=0.9754               ! p=2.0
    	   sl2(16)=0.9756               ! p=2.2
    	   sl2(17)=0.9760               ! p=2.4, E=0.818
	elseif(int(Zdtr).eq.20) then    ! 42-Ar, Q_beta=0.600 + 42-K, Q_beta=3.525 
	   sl2( 1)=2.2248               ! p=0.1
	   sl2( 2)=1.2634               ! p=0.2
	   sl2( 3)=1.0851               ! p=0.3
	   sl2( 4)=1.0275               ! p=0.4
	   sl2( 5)=1.0008               ! p=0.5
	   sl2( 6)=0.98693              ! p=0.6
	   sl2( 7)=0.97884              ! p=0.7
	   sl2( 8)=0.97426              ! p=0.8
	   sl2( 9)=0.97213              ! p=0.9
	   sl2(10)=0.97128              ! p=1.0
	   sl2(11)=0.97138              ! p=1.2
	   sl2(12)=0.97276              ! p=1.4
	   sl2(13)=0.9731               ! p=1.6
	   sl2(14)=0.9728               ! p=1.8
	   sl2(15)=0.9728               ! p=2.0
    	   sl2(16)=0.9731               ! p=2.2
    	   sl2(17)=0.9735               ! p=2.4
	   sl2(18)=0.9740               ! p=2.6
	   sl2(19)=0.9745               ! p=2.8
	   sl2(20)=0.9750               ! p=3.0
	   sl2(21)=0.9756               ! p=3.2
	   sl2(22)=0.9762               ! p=3.4
	   sl2(23)=0.9768               ! p=3.6
	   sl2(24)=0.9774               ! p=3.8
	   sl2(25)=0.9780               ! p=4.0
	   sl2(26)=0.9794               ! p=4.5
	   sl2(27)=0.9808               ! p=5.0
	   sl2(28)=0.9821               ! p=5.5
	   sl2(29)=0.9834               ! p=6.0
	   sl2(30)=0.9846               ! p=6.5
	   sl2(31)=0.9859               ! p=7.0
	   sl2(32)=0.9870               ! p=7.5
	   sl2(33)=0.9882               ! p=8.0
	   sl2(34)=0.9903               ! p=9.0
	   sl2(35)=0.9924               ! p=10.0, E=4.625
	elseif(int(Zdtr).eq.39) then    ! 90-Sr, Q_beta=0.546 
	   sl2( 1)=5.6836               ! p=0.1
	   sl2( 2)=2.0435               ! p=0.2
	   sl2( 3)=1.3704               ! p=0.3
	   sl2( 4)=1.1386               ! p=0.4
	   sl2( 5)=1.0327               ! p=0.5
	   sl2( 6)=0.97761              ! p=0.6
	   sl2( 7)=0.94571              ! p=0.7
	   sl2( 8)=0.92621              ! p=0.8
	   sl2( 9)=0.91383              ! p=0.9
	   sl2(10)=0.90577              ! p=1.0
	   sl2(11)=0.89708              ! p=1.2
	   sl2(12)=0.89379              ! p=1.4
	   sl2(13)=0.89354              ! p=1.6
	   sl2(14)=0.89479              ! p=1.8
	   sl2(15)=0.89695              ! p=2.0
    	   sl2(16)=0.89953              ! p=2.2
    	   sl2(17)=0.90229              ! p=2.4, E=0.818
	elseif(int(Zdtr).eq.40) then    ! 90-Y, Q_beta=2.228 
	   sl2( 1)=5.8992               ! p=0.1
	   sl2( 2)=2.0922               ! p=0.2
	   sl2( 3)=1.3883               ! p=0.3
	   sl2( 4)=1.1454               ! p=0.4
	   sl2( 5)=1.0345               ! p=0.5
	   sl2( 6)=0.97692              ! p=0.6
	   sl2( 7)=0.94344              ! p=0.7
	   sl2( 8)=0.92294              ! p=0.8
	   sl2( 9)=0.90998              ! p=0.9
	   sl2(10)=0.90153              ! p=1.0
	   sl2(11)=0.89243              ! p=1.2
	   sl2(12)=0.88892              ! p=1.4
	   sl2(13)=0.88848              ! p=1.6
	   sl2(14)=0.88970              ! p=1.8
	   sl2(15)=0.89186              ! p=2.0
    	   sl2(16)=0.89454              ! p=2.2
    	   sl2(17)=0.89739              ! p=2.4
	   sl2(18)=0.90037              ! p=2.6
	   sl2(19)=0.90330              ! p=2.8
	   sl2(20)=0.90631              ! p=3.0
	   sl2(21)=0.90931              ! p=3.2
	   sl2(22)=0.91223              ! p=3.4
	   sl2(23)=0.91507              ! p=3.6
	   sl2(24)=0.9174               ! p=3.8
	   sl2(25)=0.9195               ! p=4.0
	   sl2(26)=0.9246               ! p=4.5
	   sl2(27)=0.9295               ! p=5.0
	   sl2(28)=0.9343               ! p=5.5
	   sl2(29)=0.9388               ! p=6.0
	   sl2(30)=0.9432               ! p=6.5, E=2.850
	elseif(int(Zdtr).eq.56) then    ! 137-Cs, Q_beta=0.514 (to level 0.662)
	   sl2( 1)=9.3262               ! p=0.1
	   sl2( 2)=2.8592               ! p=0.2
	   sl2( 3)=1.6650               ! p=0.3
	   sl2( 4)=1.2481               ! p=0.4
	   sl2( 5)=1.0580               ! p=0.5
	   sl2( 6)=0.95794              ! p=0.6
	   sl2( 7)=0.89948              ! p=0.7
	   sl2( 8)=0.86350              ! p=0.8
	   sl2( 9)=0.84043              ! p=0.9
	   sl2(10)=0.82535              ! p=1.0
	   sl2(11)=0.80875              ! p=1.2
	   sl2(12)=0.80209              ! p=1.4
	   sl2(13)=0.80046              ! p=1.6
	   sl2(14)=0.80152              ! p=1.8
	   sl2(15)=0.80409              ! p=2.0
    	   sl2(16)=0.80752              ! p=2.2
    	   sl2(17)=0.81167              ! p=2.4, E=0.818
	endif
	call tgold(50.e-6,Qbeta,funbeta_1fu,0.001*Qbeta,2,em,fm)
1	E=50.e-6+(Qbeta-50.e-6)*rnd1(d)
	fe=funbeta_1fu(E)
	f=fm*rnd1(d)
	if(f.gt.fe) go to 1
	if(Zdtr.ge.0.) np=3
	if(Zdtr.lt.0.) np=2
c isotropical emission of beta particle is supposed
	call particle(np,E,E,0.,pi,0.,2.*pi,tcnuc,thnuc,tdnuc)
	return
	end
c
c end of beta_1fu.f
c
c Local Variables: --
c mode: fortran --
c End: --
