c Po214.f 
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
	subroutine Po214(tcnuc,tdnuc)
c Scheme of Po214 decay (Nucl. Data Sheets 55(1988)665).
c Alpha decay to excited level 1097.7 keV of Pb210 is neglected (6e-5%).
c Input : tcnuc - time of creation of nucleus (sec);
c Output: tdnuc - time of decay of nucleus (sec);
c         common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 14.07.1995, 22.10.1995.
c VIT, 11.05.2005, updated to NDS 99(2003)649.
	thnuc=164.3e-6
	tdnuc=tcnuc-thnuc/alog(2.)*alog(rnd1(d))
	palpha=100.*rnd1(d)
	if(palpha.le.0.0104) go to   800 !  0.0104%
	                     go to 10000 ! 99.9896%
c-----------------------------------------------------------------------
800	call alpha(6.902,0.,0.,t)
	call nucltransK(0.800,0.088,1.1e-2,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	call alpha(7.687,0.,0.,t)
	return
	end
c
c end of Po214.f
c
c Local Variables: --
c mode: fortran --
c End: --
