c -- genbbtools.f
      subroutine genbbparprint ()
      include 'genbb_help/genbb_commons.inc'
      print *,'nevents=   ',nevents
      print *,'ievstart=  ',ievstart
      !print *,'irndmst=   ',irndmst
      print *,'iwrfile=   ',iwrfile
      print *,'chfile=    ',chfile
      return
      end

      subroutine genbb_init_enrange ()
      common/enrange/ebb1,ebb2,toallevents,levelE,chdspin      
      ebb1=0.
      ebb2=4.3
      return
      end

c -- end of genbbtools.f
