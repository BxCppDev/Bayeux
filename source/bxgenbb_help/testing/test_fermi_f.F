c     test_fermi_f.F
      program test_fermi_f
      real emin, emax, z, e, de, f0,f1,f2
      integer i
      emin = 0.
      emax = 5.
      z = 44.
      e = emin
      de = 0.01
      do i=1,500
         e = emin + i * de
         f0 = fermi (z, e)
         !f1 = fermi_cern (z, e)
         !f2 = fermi_cern_2 (z, e)
1000     format (f8.4, ' ', f8.4) !, ' ', f8.4, ' ', f8.4)
         print 1000 ,e,f0 !,f1,f2
      enddo
      stop
      end
c     end of test_fermi_f.f
