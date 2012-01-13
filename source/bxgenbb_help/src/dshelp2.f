c dshelp2.f 
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
	subroutine dshelp2(m,du2,df2,d_el)
	double precision d_el(2),df2(2),du2(2),dens,denf
	common/denrange/dens,denf,mode
	common/helpbb/Zdbb,Adbb,e0,e1
	common/const/pi,emass,datamass(50)
	do i=1,m
	   d_el(2)=du2(i)
	   e1=d_el(1)
	   e2=d_el(2)
	   df2(i)=0.d0
	   if(e1.gt.0..and.e2.gt.0..and.e1+e2.lt.e0) then
	      if(mode.eq.4) df2(i)=fe12_mod4(e2)
	      if(mode.eq.5) df2(i)=fe12_mod5(e2)
	      if(mode.eq.6) df2(i)=fe12_mod6(e2)
	      if(mode.eq.13) df2(i)=fe12_mod13(e2)
	      if(mode.eq.14) df2(i)=fe12_mod14(e2)
	      if(mode.eq.8) df2(i)=fe12_mod8(e2)
	   endif
	   if(d_el(1)+d_el(2).lt.dens.or.d_el(1)+d_el(2).gt.denf)
     +        df2(i)=0.d0
	enddo
	return
	end
c
c end of dshelp2.f
c
c Local Variables: --
c mode: fortran --
c End: --
