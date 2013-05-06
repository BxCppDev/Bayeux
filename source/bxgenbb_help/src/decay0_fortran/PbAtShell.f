c PbAtShell.f 
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
	subroutine PbAtShell(KLMenergy,tclev,thlev,tdlev)
c Subroutine describes in some approximation the deexcitation process in
c atomic shell of Pb after creation of electron vacation in K, L or M shell.
c Call  : call PbAtShell(KLMenergy,tclev,thlev,tdlev)
c Input : KLMenergy - binding energy of electron on atomic shell [integer
c                     in keV] where the hole was created;
c         tclev     - time of hole creation (sec);
c         thlev     - hole halflife (sec).
c Output: tdlev     - time of decay of hole (sec).
c It is supposed electron binding energy on Pb K-shell = 88 keV, on L-shell =
c 15 keV, on M-shell = 3 keV. So, following values of KLMenergy are allowed:
c 88 (hole in K-shell), 15 (in L-shell) and 3 (in M-shell).
c VIT, 7.07.1995, 22.10.1995.
	Lhole=0
	Mhole=0
	if(KLMenergy.eq.88) go to 88
	if(KLMenergy.eq.15) then
	   Lhole=1
	   go to 15
	endif
	if(KLMenergy.eq.3) then
	   Mhole=1
	   go to 3
	endif
	go to 20000
c-----------------------------------------------------------------------
c K-shell
88	p=100.*rnd1(d)
	if(p.le.22.) go to 88001
	             go to 88002
88001	call gamma(0.085,tclev,thlev,tdlev)          ! X ray K-M
	Mhole=Mhole+1
	go to 3
88002	p=100.*rnd1(d)
	if(p.le.96.) then
	   call gamma(0.073,tclev,thlev,tdlev)       ! X ray K-L
	else
	   call electron(0.058,tclev,thlev,tdlev)    ! Auger electron K-LL
	   Lhole=Lhole+1
	end if
	Lhole=Lhole+1
c-----------------------------------------------------------------------
c L-shell
15	do i=1,Lhole
	   p=100.*rnd1(d)
	   if(p.le.40.) then
	      call gamma(0.012,tclev,thlev,tdlev)    ! X ray L-M
	   else
	      call electron(0.009,tclev,thlev,tdlev) ! Auger electron L-MM
	      Mhole=Mhole+1
	   end if
	   Mhole=Mhole+1
	enddo
c-----------------------------------------------------------------------
c M-shell
3	do i=1,Mhole
	   call gamma(0.003,tclev,thlev,tdlev)       ! X ray M-inf
	enddo
	return
c-----------------------------------------------------------------------
20000	print *,'Pb_At_Shell: wrong hole level [keV] ',KLMenergy
	return
c-----------------------------------------------------------------------
	end
c
c end of PbAtShell.f
c
c Local Variables: --
c mode: fortran --
c End: --
