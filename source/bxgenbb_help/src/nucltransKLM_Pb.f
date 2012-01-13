c nucltransKLM_Pb.f 
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
	subroutine nucltransKLM_Pb(Egamma,EbindeK,conveK,EbindeL,conveL,
     +                          EbindeM,conveM,convp,tclev,thlev,tdlev)
c
c The same as nucltransKLM but two X rays are emitted after K conversion
c in deexcitation of 208-Pb in decay 208Tl->208Pb. 
c VIT, 4.02.2009.
c
c Subroutine nucltransKLM choises one of the three concurent processes
c by which the transition from one nuclear state to another is
c occured: gamma-ray emission, internal conversion and internal
c pair creation. Conversion electrons are emitted with three fixed energies:
c Egamma-E(K)_binding_energy, Egamma-E(L)_binding_energy and 
c Egamma-E(M)_binding_energy).
c Call  : common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100)
c         call nucltransKLM(Egamma,EbindeK,conveK,EbindeL,conveL,
c                           EbindeM,conveM,convp,tclev,thlev,tdlev)
c Input:  Egamma  - gamma-ray energy (MeV) [=difference in state energies];
c         EbindeK - binding energy of electron (MeV) on K-shell;
c         conveK  - internal conversion coefficient [=Nelectron/Ngamma] from
c                   K-shell;
c         EbindeL - binding energy of electron (MeV) on L-shell;
c         conveL  - internal conversion coefficient [=Nelectron/Ngamma] from
c                   L-shell;
c         EbindeM - binding energy of electron (MeV) on M-shell;
c         convem  - internal conversion coefficient [=Nelectron/Ngamma] from
c                   M-shell;
c         convp   - pair conversion coefficient [=Npair/Ngamma];
c         tclev   - time of creation of level from which particle will be
c                   emitted (sec);
c         thlev   - level halflife (sec).
c Output: tdlev               - time of decay of level (sec);
c         tevst               - time of event's start (sec);
c         npfull              - current number of last particle in event;
c         npgeant(npfull)     - GEANT number for particle identification
c                               (1 for gamma, 2 for e+ and 3 for e-);
c         pmoment(1-3,npfull) - x,y,z components of particle momentum (MeV);
c         ptime(npfull)       - time shift from previous time to calculate
c                               when particle was emitted (sec).
c VIT, 4.01.2007.
	common/const/pi,emass,datamass(50)
	p=(1.+conveK+conveL+conveM+convp)*rnd1(d)
	if(p.le.1.) then
	   call gamma(Egamma,tclev,thlev,tdlev)
	else if(p.le.1.+conveK) then
	   call electron(Egamma-EbindeK,tclev,thlev,tdlev)
c	   call gamma(EbindeK,tdlev,0.,tdlev)                ! for real time
c	   call gamma(EbindeK,0.,0.,tdlev)                   ! for time shift
c emission of two X rays with E(X1)+E(X2)=EbindeK instead of one 
c with E(X)=EbindeK; probabilities are from ToI'98, Appendix F.5.a (Table 7a).
c VIT, 4.02.2009.
	   p1=100.*rnd1(d)
c 73.9% - E(X1)=74 keV, 21.3% - E(X1)=85 keV
	   if(p1.le.73.9) then
	      call gamma(0.074,0.,0.,tdlev)                
	      call gamma(0.014,0.,0.,tdlev)                
	   else if(p1.le.95.2) then
	      call gamma(0.085,0.,0.,tdlev)                
	      call gamma(0.003,0.,0.,tdlev)                
c in 4.8% few low energy particles are emitted; they are neglected
	   end if                     
	else if(p.le.1.+conveK+conveL) then
	   call electron(Egamma-EbindeL,tclev,thlev,tdlev)
c	   call gamma(EbindeL,tdlev,0.,tdlev)                ! for real time
	   call gamma(EbindeL,0.,0.,tdlev)                   ! for time shift
	else if(p.le.1.+conveK+conveL+conveM) then
	   call electron(Egamma-EbindeM,tclev,thlev,tdlev)
c	   call gamma(EbindeM,tdlev,0.,tdlev)                ! for real time
	   call gamma(EbindeM,0.,0.,tdlev)                   ! for time shift
	else
	   call pair(Egamma-2.*emass,tclev,thlev,tdlev)
	end if
	return
	end
c
c end of nucltransKLM_Pb.f
c
c Local Variables: --
c mode: fortran --
c End: --
