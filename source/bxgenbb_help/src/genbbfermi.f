c -- Fermi function based on C code from the fermi.cc 
c    GSL based functions 
c
 	real*4 function fermi (Z_,E_)
	implicit none
	real*4 Z_,E_
	real*4 zz,ee
	real*4 fermi_wrap
	real*4 res
	zz = Z_
	ee = E_
 	res = fermi_wrap (zz, ee)
	!print *, "DEVEL: fermi: res=", res
	fermi = res
 	return
 	end
c -- end
