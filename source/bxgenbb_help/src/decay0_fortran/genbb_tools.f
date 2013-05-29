c -- genbbtools.f
      subroutine genbbparprint ()
      include 'genbb_help/decay0_fortran/genbb_commons.inc'
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

      subroutine genbb_trace (id_, value_)
      implicit none
      integer*4 id_
      real*4    value_
      !print *, 'id=', id_ , ' value=', value_
      call tracer_trace(id_,value_)
      return
      end

      subroutine genbb_trace_msg (id_, word_)
      implicit none
      integer*4 id_
      character*256 word_
      call tracer_trace_msg(id_,word_)
      return
      end

      subroutine genbb_trace_tag (id_, word_, value_)
      implicit none
      integer*4 id_
      real*4    value_
      character*256 word_
      call tracer_trace_tag(id_,word_, value_)
      return
      end

c
c -- end of genbbtools.f
