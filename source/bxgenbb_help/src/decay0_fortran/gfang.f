c gfang.f 
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
	subroutine gfang(p,costh,sinth,cosph,sinph,rotate)
c
c Finds theta-phi angles for a particle with momentum P.
c The logical flag ROTATE indicates whether the director
c cosines are along the Z axis (.FALSE.) or not (.TRUE.).
c
c F.Carminati (GEANT).
c
	dimension p(3)
	logical rotate
	parameter (one=1)
c additional (as compared with original GFANG) normalization
	pfull=sqrt(p(1)**2+p(2)**2+p(3)**2)
	dux=p(1)/pfull
	duy=p(2)/pfull
	duz=p(3)/pfull
	rotate=.true.
	if(abs(duz).ge.0.85) then
	   dsith2=dux**2+duy**2
	   if(dsith2.gt.0.) then
	      costh=sign(one,duz)*sqrt(one-dsith2)
	      dsith=sqrt(dsith2)
	      sinth=dsith
	      cosph=dux/dsith
	      sinph=duy/dsith
	   elseif(duz.gt.0.) then
	      rotate=.false.
	      costh=1.
	      sinth=0.
	      cosph=1.
	      sinph=0.
	   else
	      costh=-1.
	      sinth=0.
	      cosph=1.
	      sinph=0.
	   endif
	else
	   costh=duz
	   dsith=sqrt((one+duz)*(one-duz))
	   sinth=dsith
	   dnorm=one/sqrt(dux**2+duy**2)
	   cosph=dux*dnorm
	   sinph=duy*dnorm
	endif
	return
	end
c
c end of gfang.f
c
c Local Variables: --
c mode: fortran --
c End: --
