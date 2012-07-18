c dshelp1.f 
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
	subroutine dshelp1(m,du1,df1,d_el)
	double precision d_el(2),df1(2),du1(2),dens,denf,d
c --- 2012-07-18 FM : explicit real*8 for dgmlt2
        double precision dgmlt2
	external dshelp2
	external dgmlt2
	common/denrange/dens,denf,mode
        !print *, 'genbb_help::dshelp1:dshelp1: Entering...'
	do i=1,m
	   d_el(1)=du1(i)
	   d=dmax1(0.d0,dens-d_el(1))
	   df1(i)=dgmlt2(dshelp2,d,denf-d_el(1),16,8,d_el)
           !print *, 'genbb_help::dshelp1:dshelp1: df1(',i,')=',df1(i)
	enddo
	return
	end
c
c end of dshelp1.f
c
c Local Variables: --
c mode: fortran --
c End: --
