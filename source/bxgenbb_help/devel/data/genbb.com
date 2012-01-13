#!/bin/sh -x

Msys=`uname -s`                               # get the system name
  case $Msys in                               # select the appropriate code
     Linux)
          F77='g77'
	  FFLAGS='-Wall'
          ;;
     OSF1)
          F77='f77'
	  FFLAGS='-extend_source'
          ;;
  esac

\rm nemosimul
\rm genbb.o
ln -s .. nemosimul

nypatchy << _ZORRO_
/nemosimul/geant/zebrac.car100 genbb genbb.cra tty - - dummy.dat .go
_ZORRO_

##
## without gebugging use:
$F77 -c $YOPTIOF $FFLAGS genbb.f
## with gebugging use (valid for DECS only (option -O incompatible with version -g)):
##$F77 -c -g $YOPTIOF $FFLAGS genbb.f
##

ar r lib.$YLIB/libgenbb.a genbb.o 
ranlib lib.$YLIB/libgenbb.a
ar t lib.$YLIB/libgenbb.a
\rm nemosimul
\rm genbb.o
\rm genbb.f
