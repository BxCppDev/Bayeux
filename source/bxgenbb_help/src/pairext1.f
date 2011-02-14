c pairext1.f 
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
      subroutine pairext1(Z,pg0,pe1,pe2)
c
c Simulates e+e- pair production by photons in a target with atomic
c number Z.
c Call  : common/const/pi,emass
c         dimension pg0(3),pe1(3),pe2(3)
c         call pairext1(Z,pg0,pe1,pe2)
c Input : Z   - atomic number of the substance;
c         pg0 - momentum of initial gamma quantum (in MRS);
c Output: pe1 - momentum of 1 created electron (in MRS);
c         pe2 - momentum of 2 created electron (in MRS).
c Which particle is e+ and which e-, should be selected randomly.
c
c VIT, 11.03.1998 from GEANT subroutines GPAIRG of G.Patrick, L.Urban
c and GBTETH of L.Urban.
c
	common/const/pi,emass,datamass(50)
	dimension pg0(3),pe1(3),pe2(3)
	logical rotate
	Egam=sqrt(pg0(1)**2+pg0(2)**2+pg0(3)**2)
c If not enough energy, no pair production
	if(Egam.lt.2.*emass) go to 4
c For photons <2.1 MeV approximate the electron energy by sampling
c from a uniform distribution in the interval [emass,Egam/2]
	if(Egam.le.2.1) then
	   Eel1=emass+rnd1(r)*(0.5*Egam-emass)
	   x=Eel1/Egam
	   go to 2
	endif
c Calculate some constants
	z3=Z**(1./3.)
	f=8./3.*log(Z)
	if(Egam.gt.50.) then
	   aZ2=(Z/137.036)**2
	   fcoulomb=aZ2*(1./(1.+aZ2)+0.20206-0.0369*aZ2+
     +              0.0083*aZ2**2-0.002*aZ2**3)
	   f=f+8.*fcoulomb
	endif
	x0=emass/Egam
	dx=0.5-x0
	dmin=544.*x0/z3
	if(dmin.le.1.) then
	   f10=42.392-7.796*dmin+1.961*dmin**2-f
	   f20=41.405-5.828*dmin+0.8945*dmin**2-f
	else
	   f10=42.24-8.368*log(dmin+0.952)-f
	   f20=f10
	endif
c Calculate limit for screening variable, DELTA, to ensure
c that screening rejection functions always remain positive
	dmax=exp((42.24-f)/8.368)-0.952
	dsig1=dx*dx*f10/3.
	dsig2=0.5*f20
	bpar=dsig1/(dsig1+dsig2)
c Decide which screening rejection function to use and
c sample the electron/photon fractional energy
1	r1=rnd1(r)
	r2=rnd1(r)
	if(r1.lt.bpar) then
	   x=0.5-dx*r2**(1./3.)
	   irej=1
	else
	   x=x0+dx*r2
	   irej=2
	endif
c Calculate DELTA ensuring positivity
	d=0.25*dmin/(x*(1.-x))
	if(d.ge.dmax) goto 1
c Calculate F1 and F2 functions. F10 and F20 are the F1
c and F2 functions calculated for the DELTA=DELTA minimum.
	if(d.le.1.) then
	   f1=42.392-7.796*d+1.961*d**2-f
	   f2=41.405-5.828*d+0.8945*d**2-f
	else
	   f1=42.24-8.368*log(d+0.952)-f
	   f2=f1
	endif
	if(irej.eq.1) screj=f1/f10
	if(irej.eq.2) screj=f2/f20
c Accept or reject on basis of random variate
	if(rnd1(r).gt.screj) goto 1
	Eel1=x*Egam
c Generate electron decay angles with respect to a Z-axis defined
c along the parent photon. PHI is generated isotropically.
2	alfa=0.625
	d=0.13*(0.8+1.3/Z)*(100.+1./Eel1)*(1.+x)
	w1=9./(9.+d)
	umax=Eel1*pi/emass
3	beta=alfa
	if(rnd1(r).gt.w1) beta=3.*alfa
	r1=rnd1(r)
	u=-(log(rnd1(r)*r1))/beta
	if(u.ge.umax) goto 3
	theta=u*emass/Eel1
	sinth=sin(theta)
	costh=cos(theta)
	phi=2.*pi*rnd1(r)
	cosphi=cos(phi)
	sinphi=sin(phi)
c Momentum components of first electron
	pel1=sqrt((Eel1+emass)*(Eel1-emass))
	pe1(1)=pel1*sinth*cosphi
	pe1(2)=pel1*sinth*sinphi
	pe1(3)=pel1*costh
c Momentum vector of second electron. Recoil momentum of
c target nucleus/electron is ignored.
	Eel2=Egam-Eel1
	pel2=sqrt((Eel2+emass)*(Eel2-emass))
c correction for conservation of x and y components of momentum
	sinth=sinth*pel1/pel2
	costh=sqrt(max(0.,1.-sinth**2))
	pe2(1)=-pel2*sinth*cosphi
	pe2(2)=-pel2*sinth*sinphi
	pe2(3)=pel2*costh
c Rotate tracks to MRS
	call gfang(pg0,cosal,sinal,cosbt,sinbt,rotate)
	if(rotate) then
	   call gdrot(pe1,cosal,sinal,cosbt,sinbt)
	   call gdrot(pe2,cosal,sinal,cosbt,sinbt)
	endif
	return
4	print *,'e+e- pair is not created: Egamma < 1.022 MeV'
	return
	end
c
c end of pairext1.f
c
c Local Variables: --
c mode: fortran --
c End: --
