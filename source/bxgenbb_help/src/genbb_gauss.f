c genbb_gauss.f 
c
c This file provides some routines for Gauss quadrature.
c Part of this code is reused from the CERNLIB 2006.
c
c Copyright 2007-2011 F. Mauger
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
c
c -- Gaussian integration method : 
c    a replacement for CERNLIB gauss function using GSL based function 
c    typicaly relerr=1.e-4
c$$$ 	real*4 function gauss (func_, min_, max_, relerr_)
c$$$	implicit none
c$$$	real*4 func_
c$$$	real*4 min_, max_
c$$$	real*4 relerr_
c$$$	real*4 gauss_wrap
c$$$	real*4 res
c$$$        external func_
c$$$ 	res = gauss_wrap (func_, min_, max_, relerr_)
c$$$	!print *, "DEVEL: fermi: res=", res
c$$$	gauss = res
c$$$ 	return
c$$$ 	end

c
c ./packlib/kernlib/kernnum/obsolete/d103fort/gauss.F
*
* $Id: gausscod.inc,v 1.1.1.1 1996/04/01 15:02:13 mclareni Exp $
*
* $Log: gausscod.inc,v $
* Revision 1.1.1.1  1996/04/01 15:02:13  mclareni
* Mathlib gen
*
* gausscod.inc
*
C This will be GAUSS,IF=DOUBLE and GAUSS64,IF=-DOUBLE.
      FUNCTION GAUSS(F,A,B,EPS)

      CHARACTER NAME*(*)
      PARAMETER (NAME = 'GAUSS')

      DIMENSION W(12),X(12)

      PARAMETER (Z1 = 1, HF = Z1/2, CST = 5*Z1/1000)

      DATA X( 1) /9.6028985649753623D-1/, W( 1) /1.0122853629037626D-1/
      DATA X( 2) /7.9666647741362674D-1/, W( 2) /2.2238103445337447D-1/
      DATA X( 3) /5.2553240991632899D-1/, W( 3) /3.1370664587788729D-1/
      DATA X( 4) /1.8343464249564980D-1/, W( 4) /3.6268378337836198D-1/
      DATA X( 5) /9.8940093499164993D-1/, W( 5) /2.7152459411754095D-2/
      DATA X( 6) /9.4457502307323258D-1/, W( 6) /6.2253523938647893D-2/
      DATA X( 7) /8.6563120238783174D-1/, W( 7) /9.5158511682492785D-2/
      DATA X( 8) /7.5540440835500303D-1/, W( 8) /1.2462897125553387D-1/
      DATA X( 9) /6.1787624440264375D-1/, W( 9) /1.4959598881657673D-1/
      DATA X(10) /4.5801677765722739D-1/, W(10) /1.6915651939500254D-1/
      DATA X(11) /2.8160355077925891D-1/, W(11) /1.8260341504492359D-1/
      DATA X(12) /9.5012509837637440D-2/, W(12) /1.8945061045506850D-1/

      H=0
      IF(B .EQ. A) GO TO 99
      CONST=CST/ABS(B-A)
      BB=A
    1 AA=BB
      BB=B
    2 C1=HF*(BB+AA)
      C2=HF*(BB-AA)
      S8=0
      DO 3 I = 1,4
      U=C2*X(I)
    3 S8=S8+W(I)*(F(C1+U)+F(C1-U))
      S16=0
      DO 4 I = 5,12
      U=C2*X(I)
    4 S16=S16+W(I)*(F(C1+U)+F(C1-U))
      S16=C2*S16
      IF(ABS(S16-C2*S8) .LE. EPS*(1+ABS(S16))) THEN
       H=H+S16
       IF(BB .NE. B) GO TO 1
      ELSE
       BB=C1
       IF(1+CONST*ABS(C2) .NE. 1) GO TO 2
       H=0
       !CALL MTLPRT(NAME,'D103.1','TOO HIGH ACCURACY REQUIRED')
       print *, 'genbb_help::GAUSS(port): ' , 
     +      'TOO HIGH ACCURACY REQUIRED'
       GO TO 99
      END IF

   99 GAUSS=H
      RETURN
      END
c
c
c $Id: rgmlt64.F,v 1.1.1.1 1996/04/01 15:02:15 mclareni Exp $
c
c $Log: rgmlt64.F,v $
c Revision 1.1.1.1  1996/04/01 15:02:15  mclareni
c Mathlib gen
c
c
      FUNCTION DGMLT1(FSUB1,A,B,NI,NG,X)
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      CHARACTER NAME*(*)
      CHARACTER*80 ERRTXT
      PARAMETER (NAME = 'DGMLT1')
      PARAMETER (Z1 = 1, HALF = Z1/2)

      DIMENSION X(6),W(14),T(14),V(64),U(64),F(64)

      DATA (T(I),W(I),I=1,14)
     1/-0.93246 95142 03152 028D0, 0.17132 44923 79170 345D0,
     2 -0.66120 93864 66264 514D0, 0.36076 15730 48138 608D0,
     3 -0.23861 91860 83196 909D0, 0.46791 39345 72691 047D0,
     4  0.23861 91860 83196 909D0, 0.46791 39345 72691 047D0,
     5  0.66120 93864 66264 514D0, 0.36076 15730 48138 608D0,
     6  0.93246 95142 03152 028D0, 0.17132 44923 79170 345D0,
     7 -0.96028 98564 97536 232D0, 0.10122 85362 90376 259D0,
     8 -0.79666 64774 13626 740D0, 0.22238 10344 53374 471D0,
     9 -0.52553 24099 16328 986D0, 0.31370 66458 77887 287D0,
     A -0.18343 46424 95649 805D0, 0.36268 37833 78361 983D0,
     B  0.18343 46424 95649 805D0, 0.36268 37833 78361 983D0,
     C  0.52553 24099 16328 986D0, 0.31370 66458 77887 287D0,
     D  0.79666 64774 13626 740D0, 0.22238 10344 53374 471D0,
     E  0.96028 98564 97536 232D0, 0.10122 85362 90376 259D0/

      IF(NI .LE. 0) THEN
       !WRITE(ERRTXT,101) NI
       !CALL MTLPRT(NAME,'D110.1',ERRTXT)
       print *, 'genbb_help::DGMLT1(port): invalid NI parameter! Abort!'
       STOP 
      END IF
      M0=NG
      IF(M0 .NE. 8) M0=6
      I0=0
      IF(M0 .EQ. 8) I0=6
      D=(B-A)/NI
      R=HALF*D
      RA=R+A
      MV=MOD(M0*NI-1,64)+1
      S=0
      J=0
      DO 1 I = 1+I0,M0+I0
      RTA=R*T(I)+RA
      DO 2 K = 1,NI
      J=J+1
      V(J)=W(I)
      U(J)=RTA+(K-1)*D
      IF(J .EQ. MV) THEN
       CALL FSUB1(MV,U,F,X)
       DO 3 J = 1,MV
    3  S=S+V(J)*F(J)
       MV=64
       J=0
      END IF
    2 CONTINUE
    1 CONTINUE
      DGMLT1=R*S
      RETURN
  101 FORMAT('N1 = ',I4,' <= 0')
      END
c
c
c
c
      FUNCTION DGMLT2(FSUB2,A,B,NI,NG,X)
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      CHARACTER NAME*(*)
      CHARACTER*80 ERRTXT
      PARAMETER (NAME = 'DGMLT2')
      PARAMETER (Z1 = 1, HALF = Z1/2)

      DIMENSION X(6),W(14),T(14),V(64),U(64),F(64)

      DATA (T(I),W(I),I=1,14)
     1/-0.93246 95142 03152 028D0, 0.17132 44923 79170 345D0,
     2 -0.66120 93864 66264 514D0, 0.36076 15730 48138 608D0,
     3 -0.23861 91860 83196 909D0, 0.46791 39345 72691 047D0,
     4  0.23861 91860 83196 909D0, 0.46791 39345 72691 047D0,
     5  0.66120 93864 66264 514D0, 0.36076 15730 48138 608D0,
     6  0.93246 95142 03152 028D0, 0.17132 44923 79170 345D0,
     7 -0.96028 98564 97536 232D0, 0.10122 85362 90376 259D0,
     8 -0.79666 64774 13626 740D0, 0.22238 10344 53374 471D0,
     9 -0.52553 24099 16328 986D0, 0.31370 66458 77887 287D0,
     A -0.18343 46424 95649 805D0, 0.36268 37833 78361 983D0,
     B  0.18343 46424 95649 805D0, 0.36268 37833 78361 983D0,
     C  0.52553 24099 16328 986D0, 0.31370 66458 77887 287D0,
     D  0.79666 64774 13626 740D0, 0.22238 10344 53374 471D0,
     E  0.96028 98564 97536 232D0, 0.10122 85362 90376 259D0/

      IF(NI .LE. 0) THEN
       !WRITE(ERRTXT,101) NI
       !CALL MTLPRT(NAME,'D110.1',ERRTXT)
       print *, 'genbb_help::DGMLT1(port): invalid NI parameter! Abort!'
      END IF
      M0=NG
      IF(M0 .NE. 8) M0=6
      I0=0
      IF(M0 .EQ. 8) I0=6
      D=(B-A)/NI
      R=HALF*D
      RA=R+A
      MV=MOD(M0*NI-1,64)+1
      S=0
      J=0
      DO 1 I = 1+I0,M0+I0
      RTA=R*T(I)+RA
      DO 2 K = 1,NI
      J=J+1
      V(J)=W(I)
      U(J)=RTA+(K-1)*D
      IF(J .EQ. MV) THEN
       CALL FSUB2(MV,U,F,X)
       DO 3 J = 1,MV
    3  S=S+V(J)*F(J)
       MV=64
       J=0
      END IF
    2 CONTINUE
    1 CONTINUE
      DGMLT2=R*S
      RETURN
  101 FORMAT('N2 = ',I4,' <= 0')
      END
c
c end of genbb_gauss.f
