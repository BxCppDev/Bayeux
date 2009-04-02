c -- Fermi function based on C code from the fermi.cc 
c    GSL based functions 
c
 	real*4 function fermi(Z,E)
	implicit none
	real*4 Z,E
	real*4 zz,ee
	real*4 fermi_wrap
	zz=Z
	ee=E
 	fermi=fermi_wrap(zz,ee)
 	return
 	end
c -- end
