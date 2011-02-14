c genbb_fermi.f 
c
c This file provides a replacement of the 'fermi' function
c of the 'decay0' program by V.I. Tretyak. This 'fermi' function 
c uses the C 'fermi_wrap' function code from the 'fermi.cc' file. 
c
c Copyright 2007-2011 F. Mauger
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
 	real*4 function fermi (Z_, E_)
	implicit none
	real*4 Z_, E_
	real*4 zz, ee
	real*4 fermi_wrap
	real*4 res
	zz = Z_
	ee = E_
 	res = fermi_wrap (zz, ee)
	!print *, "DEVEL: fermi: res=", res
	fermi = res
 	return
 	end
c
c end of genbb_fermi.f
