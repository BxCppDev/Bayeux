c tsimpr.f 
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
	function tsimpr(f,a,b,h)
c Function tsimpr calculates the value of integral of function f from a to b
c with step h using Simpson's formula.
c (b-a)/h must be equal 2*m where m is integer.
c Function f must be decribed as external in main program.
c VIT, DLP KINR, 20.11.1984.
	n=(b-a)/h-1
	fi=f(a)-f(b)
	x1=a
	x2=a
	ni=1
1	x1=x2+h
	x2=x1+h
	fi=fi+4.*f(x1)+2.*f(x2)
	ni=ni+2
	if(ni.lt.n) go to 1
	tsimpr=fi*h/3.
	return
	end
c
c end of tsimpr.f
c
c Local Variables: --
c mode: fortran --
c End: --
