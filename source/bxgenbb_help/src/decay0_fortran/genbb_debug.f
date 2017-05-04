c dummy FORTRAN include file
c
c
      subroutine genbb_set_debug ()
      include 'genbb_help/decay0_fortran/dummy.inc'
      genbb_debug = .true.
      return
      end

      subroutine genbb_unset_debug ()
      include 'genbb_help/decay0_fortran/dummy.inc'
      genbb_debug = .false.
      return
      end


c end
