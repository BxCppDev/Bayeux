c Gd158low.f 
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
	subroutine Gd158low(levelkeV)
c Subroutine describes the deexcitation process in Gd158 nucleus
c after 2b-decay of Dy158 to ground and excited 0+ and 2+ levels
c of Gd158 (NNDC site on 10.10.2010, NDS 101(2004)325).
c Call  : call Gd158low(levelkeV)
c Input : levelkeV - energy of Gd158 level (integer in keV) occupied
c                    initially; following levels can be occupied:
c                    0+(gs) -    0 keV,
c                    2+(1)  -   80 keV,
c                    4+(1)  -  261 keV.
c Output: common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),ptime(100).
c VIT, 10.10.2010.
	tclev=0.
	if(levelkev.eq.261) go to   261
	if(levelkev.eq. 80) go to    80
	if(levelkev.eq.  0) go to 10000
	                    go to 20000
c-----------------------------------------------------------------------
261	thlev=0.148e-9
	call nucltransK(0.182,0.050,0.308,0.,tclev,thlev,tdlev)
	go to 80
c-----------------------------------------------------------------------
80	thlev=2.52e-9
	call nucltransK(0.080,0.050,6.02,0.,tclev,thlev,tdlev)
	return
c-----------------------------------------------------------------------
10000	return
c-----------------------------------------------------------------------
20000	print *,'Gd158: wrong level [keV] ',levelkev
c-----------------------------------------------------------------------
	return
	end
c
c end of Gd158low.f
c
c Local Variables: --
c mode: fortran --
c End: --
