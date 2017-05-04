c genbb_divdif.f 
c
c This file provides some routines for tabulated function
c interpolation. This code is reused from the CERNLIB 2006.
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
c
c 2006/src/packlib/kernlib/kernnum/e105fort/divdif.F
c
      FUNCTION DIVDIF(F,A,NN,X,MM)
      DIMENSION A(NN),F(NN),T(20),D(20)
      LOGICAL EXTRA
      LOGICAL MFLAG,RFLAG
      DATA MMAX/10/
C
C  TABULAR INTERPOLATION USING SYMMETRICALLY PLACED ARGUMENT POINTS.
C
C  START.  FIND SUBSCRIPT IX OF X IN ARRAY A.
      IF( (NN.LT.2) .OR. (MM.LT.1) ) GO TO 20
      N=NN
      M=MIN0(MM,MMAX,N-1)
      MPLUS=M+1
      IX=0
      IY=N+1
      IF(A(1).GT.A(N)) GO TO 4
C     (SEARCH INCREASING ARGUMENTS.)
    1    MID=(IX+IY)/2
         IF(X.GE.A(MID)) GO TO 2
            IY=MID
            GO TO 3
C        (IF TRUE.)
    2       IX=MID
    3    IF(IY-IX.GT.1) GO TO 1
         GO TO 7
C     (SEARCH DECREASING ARGUMENTS.)
    4    MID=(IX+IY)/2
         IF(X.LE.A(MID)) GO TO 5
            IY=MID
            GO TO 6
C        (IF TRUE.)
    5       IX=MID
    6    IF(IY-IX.GT.1) GO TO 4
C
C  COPY REORDERED INTERPOLATION POINTS INTO (T(I),D(I)), SETTING
C  *EXTRA* TO TRUE IF M+2 POINTS TO BE USED.
    7 NPTS=M+2-MOD(M,2)
      IP=0
      L=0
      GO TO 9
    8    L=-L
         IF(L.GE.0) L=L+1
    9    ISUB=IX+L
         IF((1.LE.ISUB).AND.(ISUB.LE.N)) GO TO 10
C        (SKIP POINT.)
            NPTS=MPLUS
            GO TO 11
C        (INSERT POINT.)
   10       IP=IP+1
            T(IP)=A(ISUB)
            D(IP)=F(ISUB)
   11    IF(IP.LT.NPTS) GO TO 8
      EXTRA=NPTS.NE.MPLUS
C
C  REPLACE D BY THE LEADING DIAGONAL OF A DIVIDED-DIFFERENCE TABLE, SUP-
C  PLEMENTED BY AN EXTRA LINE IF *EXTRA* IS TRUE.
      DO 14 L=1,M
         IF(.NOT.EXTRA) GO TO 12
            ISUB=MPLUS-L
            D(M+2)=(D(M+2)-D(M))/(T(M+2)-T(ISUB))
   12    I=MPLUS
         DO 13 J=L,M
            ISUB=I-L
            D(I)=(D(I)-D(I-1))/(T(I)-T(ISUB))
            I=I-1
   13    CONTINUE
   14 CONTINUE
C
C  EVALUATE THE NEWTON INTERPOLATION FORMULA AT X, AVERAGING TWO VALUES
C  OF LAST DIFFERENCE IF *EXTRA* IS TRUE.
      SUM=D(MPLUS)
      IF(EXTRA) SUM=0.5*(SUM+D(M+2))
      J=M
      DO 15 L=1,M
         SUM=D(J)+(X-T(J))*SUM
         J=J-1
   15 CONTINUE
      DIVDIF=SUM
      RETURN
C
   20 print *, 'genbb_help::DIVDIF(port): invalid parameters! Stop!'
      DIVDIF=0	
c$$$   20 CALL KERMTR('E105.1',LGFILE,MFLAG,RFLAG)
c$$$      DIVDIF=0
c$$$      IF(MFLAG) THEN
c$$$         IF(LGFILE.EQ.0) THEN
c$$$            IF(MM.LT.1) WRITE(*,101) MM
c$$$            IF(NN.LT.2) WRITE(*,102) NN
c$$$         ELSE
c$$$            IF(MM.LT.1) WRITE(LGFILE,101) MM
c$$$            IF(NN.LT.2) WRITE(LGFILE,102) NN
c$$$         ENDIF
c$$$      ENDIF
c$$$      IF(.NOT.RFLAG) CALL ABEND
      RETURN
  101 FORMAT( 7X, 'FUNCTION DIVDIF ... M =',I6,' IS LESS THAN 1')
  102 FORMAT( 7X, 'FUNCTION DIVDIF ... N =',I6,' IS LESS THAN 2')
      END
c
c end of genbb_divdif.f 
