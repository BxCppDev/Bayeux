c test_fermi.for
      program test_fermi
      real*4 z, de, ke
      integer*4 i
      z = 34.0
      !print *, '# DEVEL: decay0: test_fermi: ', fermi (z, 1.5)
      do i = 1, 120
         de = 0.025 ! MeV
         ke = i * de
         f = fermi (z, ke)
         print *, ke, f
      enddo

      end

      function fermi(Z,E)
c     Function fermi calculates the traditional function of Fermi in
c     theory of beta decay to take into account the Coulomb correction
c     to the shape of electron/positron energy spectrum.
c     Call  : corr=fermi(Z,E)
c     Input : Z - atomic number of daughter nuclei (>0 for e-, <0 for e+);
c     E - kinetic energy of particle (MeV; E>50 eV).
c     Output: corr - value of correction factor (without normalization -
c     constant factors are removed - for MC simulation).
c     V.I.Tretyak, 15.07.1992.
      complex carg,cgamma
      if(E.lt.50.e-6) E=50.e-6
      !print *, 'DEVEL: Decay0::fermi: E=', E
      alfaz=Z/137.036
      !print *, 'DEVEL: Decay0::fermi: alfaz=', alfaz
      w=E/0.511+1.
      !print *, 'DEVEL: Decay0::fermi: w=', w
      p=sqrt(w*w-1.)
      !print *, 'DEVEL: Decay0::fermi: p=', p
      y=alfaz*w/p
      !print *, 'DEVEL: Decay0::fermi: y=', y
      g=sqrt(1.-alfaz**2)
      !print *, 'DEVEL: Decay0::fermi: g=', g
      carg=cmplx(g,y)
      res=p**(2.*g-2.)*exp(3.1415927*y+2.*alog(cabs(cgamma(carg))))
      !print *, 'DEVEL: Decay0::fermi: res=', res
      fermi = res
      return
      end
c
c end of test_fermi.for
