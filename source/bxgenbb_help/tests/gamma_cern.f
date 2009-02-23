      program gamma_cern
      real zx,zy, zr, ttt
      complex carg, cgamma, z1

      zx=4.0
      zy=0.0
      carg=cmplx(zx,zy)
      print * , 'DEVEL: carg=' , carg
      !z1 = cgamma(carg)
      a = algama(carg)
      print * , 'DEVEL: a=' , a
      print * , 'DEVEL: exp(a)=' , exp(a)
      z1 = cgamma(carg)
      !ttt = wgamma(zx)
      !print * , 'DEVEL: ttt=' , ttt
      print * , 'DEVEL: z1=' , z1
      zr=cabs(z1)
      print * , 'DEVEL: zr=' , zr

      return
      end
