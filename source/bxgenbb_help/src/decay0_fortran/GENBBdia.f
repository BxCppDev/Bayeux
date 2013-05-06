c GENBBdia.f 
c
c This file was extracted from the 'decay0' program by V.I. Tretyak
c
c Copyright 1995-2011 V.I. Tretyak
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
	subroutine GENBBdia(i2bbs,chnuclide,ilevel,modebb,istart)
c
c Dialog to ask the inputs for GENBBsub subroutine.
c Descriptions of common blocks and parameters see in GENBBsub.
c VIT, 03.12.1995.
c
	character chfile*256,chnuclide*16,chn*16,chart*4,chdspin*4
	character chfile_eta*50,chline*50
	common/genevent/tevst,npfull,npgeant(100),pmoment(3,100),
     +                  ptime(100)
      common/const/pi,emass,datamass(50)
	common/enrange/ebb1,ebb2,toallevents,levelE,chdspin
	common/genbbpar/nevents,ievstart,irndmst,iwrfile,chfile
	common/artificial/nartparts,    chart(10),
     +                    artemin(10),  artemax(10),
     +                    arttmin(10),  arttmax(10),
     +                    artfmin(10),  artfmax(10),
     +                    artQb(10),    artZd(10),
     +                    arttdelay(10),artthalf(10),
     +                    nartnpg(10)
	common/eta_nme/chi_GTw,chi_Fw,chip_GT,chip_F,chip_T,
     +               chip_P,chip_R
	print *,'     '
	print *,
     +'    DECAY0: Generation of events of decay of natural radioactive'
	print *,
     +'             isotopes and various modes of double beta decay'
	print *,'     '
	print *,' DECAY units: energy   - MeV'
	print *,'              momentum - MeV/c'
	print *,'              time     - sec'
	print *,'              angle    - degree'
1000	print *,'     '
	print *,' Which type of events do you want to generate:'
	print *,'       1. double beta processes'
	print *,'       2. internal or external background ',
     +          'or calibration sources'
	print 1
c1	format('$? ')
1	format(' ? ',$)
	read *,i2bbs
2	print *,'     '
	if(i2bbs.eq.1) then
	   print *,' Double beta nuclides:'
	   print *,'        Ca48'
	   print *,'        Ni58'
	   print *,'        Zn64     Zn70'
	   print *,'        Ge76'
	   print *,'        Se74     Se82'
	   print *,'        Sr84'
	   print *,'        Zr94     Zr96'
	   print *,'        Mo92     Mo100'
	   print *,'        Ru96     Ru104'
	   print *,'        Cd106    Cd108    Cd114    Cd116'
	   print *,'        Sn112    Sn122    Sn124'
	   print *,'        Te120    Te128    Te130'
	   print *,'        Xe136'
	   print *,'        Ce136    Ce138    Ce142'
	   print *,'        Nd148    Nd150'
	   print *,'        Dy156    Dy158'
	   print *,'        W180     W186'
	   print *,'        Pt190    Pt198'
	   print *,'        Bi214+At214'
	   print *,'        Pb214+Po214'
	   print *,'        Po218+Rn218+Po214'
	   print *,'        Rn222+Ra222+Rn218+Po214'
	   print 1
	   read 3,chn
3	   format(a)
	   chnuclide=chn
	   print *,'     '
	   if((chn(1:2).eq.'Ca'.or.chn(1:2).eq.'CA'.or.chn(1:2).eq.'ca')
     +        .and.chn(3:4).eq.'48') then
4	      print *,'48-Ti level:    0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.984 MeV}'
	      print *,'                2. 2+ (2)  {2.421 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.2) then
	         print *,'   '
	         go to 4
	      endif
	   elseif((chn(1:2).eq.'Ni'.or.chn(1:2).eq.'NI'.or.
     +             chn(1:2).eq.'ni').and.chn(3:4).eq.'58') then
5	      print *,'58-Fe level:    0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.811 MeV}'
	      print *,'                2. 2+ (2)  {1.675 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.2) then
	         print *,'   '
	         go to 5
	      endif
	   elseif((chn(1:2).eq.'Zn'.or.chn(1:2).eq.'ZN'.or.
     +             chn(1:2).eq.'zn').and.chn(3:4).eq.'64') then
	      print *,'transition is possible only to ',
     +                '64-Ni 0+(gs) level'
	      ilevel=0
	   elseif((chn(1:2).eq.'Zn'.or.chn(1:2).eq.'ZN'.or.
     +             chn(1:2).eq.'zn').and.chn(3:4).eq.'70') then
	      print *,'transition is possible only to ',
     +                '70-Ge 0+(gs) level'
	      ilevel=0
	   elseif((chn(1:2).eq.'Ge'.or.chn(1:2).eq.'GE'.or.
     +             chn(1:2).eq.'ge').and.chn(3:4).eq.'76') then
6	      print *,'76-Se level:    0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.559 MeV}'
	      print *,'                2. 0+ (1)  {1.122 MeV}'
	      print *,'                3. 2+ (2)  {1.216 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.3) then
	         print *,'   '
	         go to 6
	      endif
	   elseif((chn(1:2).eq.'Se'.or.chn(1:2).eq.'SE'.or.
     +             chn(1:2).eq.'se').and.chn(3:4).eq.'74') then
7	      print *,'74-Ge level:    0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.596 MeV}'
	      print *,'                2. 2+ (2)  {1.204 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.2) then
	         print *,'   '
	         go to 7
	      endif
	   elseif((chn(1:2).eq.'Se'.or.chn(1:2).eq.'SE'.or.
     +             chn(1:2).eq.'se').and.chn(3:4).eq.'82') then
8	      print *,'82-Kr level:    0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.776 MeV}'
	      print *,'                2. 2+ (2)  {1.475 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.2) then
	         print *,'   '
	         go to 8
	      endif
	   elseif((chn(1:2).eq.'Sr'.or.chn(1:2).eq.'SR'.or.
     +             chn(1:2).eq.'sr').and.chn(3:4).eq.'84') then
81	      print *,'84-Kr level:    0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.882 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.1) then
	         print *,'   '
	         go to 81
	      endif
	   elseif((chn(1:2).eq.'Zr'.or.chn(1:2).eq.'ZR'.or.
     +             chn(1:2).eq.'zr').and.chn(3:4).eq.'94') then
9	      print *,'94-Mo level:    0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.871 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.1) then
	         print *,'   '
	         go to 9
	      endif
	   elseif((chn(1:2).eq.'Zr'.or.chn(1:2).eq.'ZR'.or.
     +             chn(1:2).eq.'zr').and.chn(3:4).eq.'96') then
10	      print *,'96-Mo level:    0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.778 MeV}'
	      print *,'                2. 0+ (1)  {1.148 MeV}'
	      print *,'                3. 2+ (2)  {1.498 MeV}'
	      print *,'                4. 2+ (3)  {1.626 MeV}'
	      print *,'                5. 2+ (4)  {2.096 MeV}'
	      print *,'                6. 2+ (5)  {2.426 MeV}'
	      print *,'                7. 0+ (2)  {2.623 MeV}'
	      print *,'                8. 2+ (6)  {2.700 MeV}'
	      print *,'                9. 2+?(7)  {2.713 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.9) then
	         print *,'   '
	         go to 10
	      endif
	   elseif((chn(1:2).eq.'Mo'.or.chn(1:2).eq.'MO'.or.
     +             chn(1:2).eq.'mo').and.chn(3:4).eq.'92') then
11	      print *,'92-Zr level:    0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.934 MeV}'
	      print *,'                2. 0+ (1)  {1.383 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.2) then
	         print *,'   '
	         go to 11
	      endif
	   elseif((chn(1:2).eq.'Mo'.or.chn(1:2).eq.'MO'.or.
     +             chn(1:2).eq.'mo').and.chn(3:5).eq.'100') then
12	      print *,'100-Ru level:   0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.540 MeV}'
	      print *,'                2. 0+ (1)  {1.130 MeV}'
	      print *,'                3. 2+ (2)  {1.362 MeV}'
	      print *,'                4. 0+ (2)  {1.741 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.4) then
	         print *,'   '
	         go to 12
	      endif
	   elseif((chn(1:2).eq.'Ru'.or.chn(1:2).eq.'RU'.or.
     +             chn(1:2).eq.'ru').and.chn(3:4).eq.'96') then
121	      print *,'96-Mo level:    0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.778 MeV}'
	      print *,'                2. 0+ (1)  {1.148 MeV}'
	      print *,'                3. 2+ (2)  {1.498 MeV}'
	      print *,'                4. 2+ (3)  {1.626 MeV}'
	      print *,'                5. 2+ (4)  {2.096 MeV}'
	      print *,'                6. 2+ (5)  {2.426 MeV}'
	      print *,'                7. 0+ (2)  {2.623 MeV}'
	      print *,'                8. 2+ (6)  {2.700 MeV}'
	      print *,'                9. 2+?(7)  {2.713 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.9) then
	         print *,'   '
	         go to 121
	      endif
	   elseif((chn(1:2).eq.'Ru'.or.chn(1:2).eq.'RU'.or.
     +             chn(1:2).eq.'ru').and.chn(3:5).eq.'104') then
122	      print *,'104-Pd level:   0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.556 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.1) then
	         print *,'   '
	         go to 122
	      endif
	   elseif((chn(1:2).eq.'Cd'.or.chn(1:2).eq.'CD'.or.
     +             chn(1:2).eq.'cd').and.chn(3:5).eq.'106') then
13	      print *,'106-Pd level:    0. 0+ (gs)     {0 MeV}'
	      print *,'                 1. 2+ (1)  {0.512 MeV}'
	      print *,'                 2. 2+ (2)  {1.128 MeV}'
	      print *,'                 3. 0+ (1)  {1.134 MeV}'
	      print *,'                 4. 2+ (3)  {1.562 MeV}'
	      print *,'                 5. 0+ (2)  {1.706 MeV}'
c	      print *,'                 6. 2+ (4)  {2.741 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.5) then
	         print *,'   '
	         go to 13
	      endif
	   elseif((chn(1:2).eq.'Cd'.or.chn(1:2).eq.'CD'.or.
     +             chn(1:2).eq.'cd').and.chn(3:5).eq.'108') then
	      print *,'transition is possible only to ',
     +                '108-Pd 0+(gs) level'
	      ilevel=0
	   elseif((chn(1:2).eq.'Cd'.or.chn(1:2).eq.'CD'.or.
     +             chn(1:2).eq.'cd').and.chn(3:5).eq.'114') then
	      print *,'transition is possible only to ',
     +                '114-Sn 0+(gs) level'
	      ilevel=0
	   elseif((chn(1:2).eq.'Cd'.or.chn(1:2).eq.'CD'.or.
     +             chn(1:2).eq.'cd').and.chn(3:5).eq.'116') then
14	      print *,'116-Sn level:   0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {1.294 MeV}'
	      print *,'                2. 0+ (1)  {1.757 MeV}'
	      print *,'                3. 0+ (2)  {2.027 MeV}'
	      print *,'                4. 2+ (2)  {2.112 MeV}'
	      print *,'                5. 2+ (3)  {2.225 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.5) then
	         print *,'   '
	         go to 14
	      endif
	   elseif((chn(1:2).eq.'Sn'.or.chn(1:2).eq.'SN'.or.
     +             chn(1:2).eq.'sn').and.chn(3:5).eq.'112') then
1014	      print *,'112-Cd level:   0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.618 MeV}'
	      print *,'                2. 0+ (1)  {1.224 MeV}'
	      print *,'                3. 2+ (2)  {1.312 MeV}'
	      print *,'                4. 0+ (2)  {1.433 MeV}'
	      print *,'                5. 2+ (3)  {1.469 MeV}'
	      print *,'                6. 0+ (3)  {1.871 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.6) then
	         print *,'   '
	         go to 1014
	      endif
	   elseif((chn(1:2).eq.'Sn'.or.chn(1:2).eq.'SN'.or.
     +             chn(1:2).eq.'sn').and.chn(3:5).eq.'122') then
	      print *,'transition is possible only to ',
     +                '122-Te 0+(gs) level'
	      ilevel=0
	   elseif((chn(1:2).eq.'Sn'.or.chn(1:2).eq.'SN'.or.
     +             chn(1:2).eq.'sn').and.chn(3:5).eq.'124') then
1015	      print *,'124-Te level:   0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.603 MeV}'
	      print *,'                2. 2+ (2)  {1.326 MeV}'
	      print *,'                3. 0+ (1)  {1.657 MeV}'
	      print *,'                4. 0+ (2)  {1.833 MeV}'
	      print *,'                5. 2+ (3)  {2.039 MeV}'
	      print *,'                6. 2+ (4)  {2.092 MeV}'
	      print *,'                7. 0+ (3)  {2.153 MeV}'
	      print *,'                8. 2+ (5)  {2.182 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.8) then
	         print *,'   '
	         go to 1015
	      endif
	   elseif((chn(1:2).eq.'Te'.or.chn(1:2).eq.'TE'.or.
     +             chn(1:2).eq.'te').and.chn(3:5).eq.'120') then
114	      print *,'120-Sn level:   0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {1.171 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.1) then
	         print *,'   '
	         go to 114
	      endif
	   elseif((chn(1:2).eq.'Te'.or.chn(1:2).eq.'TE'.or.
     +             chn(1:2).eq.'te').and.chn(3:5).eq.'128') then
115	      print *,'128-Xe level:   0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.443 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.1) then
	         print *,'   '
	         go to 115
	      endif
	   elseif((chn(1:2).eq.'Te'.or.chn(1:2).eq.'TE'.or.
     +             chn(1:2).eq.'te').and.chn(3:5).eq.'130') then
116	      print *,'130-Xe level:   0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.536 MeV}'
	      print *,'                2. 2+ (2)  {1.122 MeV}'
	      print *,'                3. 0+ (1)  {1.794 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.3) then
	         print *,'   '
	         go to 116
	      endif
	   elseif((chn(1:2).eq.'Xe'.or.chn(1:2).eq.'XE'.or.
     +             chn(1:2).eq.'xe').and.chn(3:5).eq.'136') then
15	      print *,'136-Ba level:   0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.819 MeV}'
	      print *,'                2. 2+ (2)  {1.551 MeV}'
	      print *,'                3. 0+ (1)  {1.579 MeV}'
	      print *,'                4. 2+ (3)  {2.080 MeV}'
	      print *,'                5. 2+ (4)  {2.129 MeV}'
	      print *,'                6. 0+ (2)  {2.141 MeV}'
	      print *,'                7. 2+ (5)  {2.223 MeV}'
	      print *,'                8. 0+ (3)  {2.315 MeV}'
	      print *,'                9. 2+ (6)  {2.400 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.9) then
	         print *,'   '
	         go to 15
	      endif
	   elseif((chn(1:2).eq.'Ce'.or.chn(1:2).eq.'CE'.or.
     +             chn(1:2).eq.'ce').and.chn(3:5).eq.'136') then
151	      print *,'136-Ba level:   0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.819 MeV}'
	      print *,'                2. 2+ (2)  {1.551 MeV}'
	      print *,'                3. 0+ (1)  {1.579 MeV}'
	      print *,'                4. 2+ (3)  {2.080 MeV}'
	      print *,'                5. 2+ (4)  {2.129 MeV}'
	      print *,'                6. 0+ (2)  {2.141 MeV}'
	      print *,'                7. 2+ (5)  {2.223 MeV}'
	      print *,'                8. 0+ (3)  {2.315 MeV}'
	      print *,'                9. 2+ (6)  {2.400 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.9) then
	         print *,'   '
	         go to 151
	      endif
	   elseif((chn(1:2).eq.'Ce'.or.chn(1:2).eq.'CE'.or.
     +             chn(1:2).eq.'ce').and.chn(3:5).eq.'138') then
	      print *,'transition is possible only to ',
     +                '138-Ba 0+(gs) level'
	      ilevel=0
	   elseif((chn(1:2).eq.'Ce'.or.chn(1:2).eq.'CE'.or.
     +             chn(1:2).eq.'ce').and.chn(3:5).eq.'142') then
	      print *,'transition is possible only to ',
     +                '142-Nd 0+(gs) level'
	      ilevel=0
	   elseif((chn(1:2).eq.'Nd'.or.chn(1:2).eq.'ND'.or.
     +             chn(1:2).eq.'nd').and.chn(3:5).eq.'148') then
16	      print *,'148-Sm level:    0. 0+ (gs)     {0 MeV}'
	      print *,'                 1. 2+ (1)  {0.550 MeV}'
	      print *,'                 2. 2+ (2)  {1.455 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.2) then
	         print *,'   '
	         go to 16
	      endif
	   elseif((chn(1:2).eq.'Nd'.or.chn(1:2).eq.'ND'.or.
     +             chn(1:2).eq.'nd').and.chn(3:5).eq.'150') then
17	      print *,'150-Sm level:   0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.334 MeV}'
	      print *,'                2. 0+ (1)  {0.740 MeV}'
	      print *,'                3. 2+ (2)  {1.046 MeV}'
	      print *,'                4. 2+ (3)  {1.194 MeV}'
	      print *,'                5. 0+ (2)  {1.256 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.5) then
	         print *,'   '
	         go to 17
	      endif
	   elseif((chn(1:2).eq.'Dy'.or.chn(1:2).eq.'DY'.or.
     +             chn(1:2).eq.'dy').and.chn(3:5).eq.'156') then
1701	      print *,'156-Gd level:   0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.089 MeV}'
	      print *,'                2. 0+ (1)  {1.050 MeV}'
	      print *,'                3. 2+ (2)  {1.129 MeV}'
	      print *,'                4. 2+ (3)  {1.154 MeV}'
	      print *,'                5. 0+ (2)  {1.168 MeV}'
	      print *,'                6. 2+ (4)  {1.258 MeV}'
	      print *,'                7. 0+ (3)  {1.715 MeV}'
	      print *,'                8. 2+ (5)  {1.771 MeV}'
	      print *,'                9. 2+ (6)  {1.828 MeV}'
	      print *,'               10. 0+ (4)  {1.851 MeV}'
	      print *,'               11. 2+ (7)  {1.915 MeV}'
	      print *,'               12. 1-      {1.946 MeV}'
	      print *,'               13. 0-      {1.952 MeV}'
	      print *,'               14. 0+ (5)  {1.989 MeV}'
	      print *,'               15. 2+ (8)  {2.004 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.15) then
	         print *,'   '
	         go to 1701
	      endif
	   elseif((chn(1:2).eq.'Dy'.or.chn(1:2).eq.'DY'.or.
     +             chn(1:2).eq.'dy').and.chn(3:5).eq.'158') then
1702	      print *,'158-Gd level:   0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.080 MeV}'
	      print *,'                2. 4+ (1)  {0.261 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.2) then
	         print *,'   '
	         go to 1702
	      endif
	   elseif((chn(1:1).eq.'W'.or.chn(1:1).eq.'w').and.
     +           chn(2:4).eq.'180') then
	      print *,'transition is possible only to ',
     +                '180-Hf 0+(gs) level'
	      ilevel=0
	   elseif((chn(1:1).eq.'W'.or.chn(1:1).eq.'w').and.
     +           chn(2:4).eq.'186') then
171	      print *,'186-Os level:   0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.137 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.2) then
	         print *,'   '
	         go to 171
	      endif
	   elseif((chn(1:2).eq.'Pt'.or.chn(1:2).eq.'PT'.or.
     +             chn(1:2).eq.'pt').and.chn(3:5).eq.'190') then
172	      print *,'190-Os level:   0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.187 MeV}'
	      print *,'                2. 2+ (2)  {0.558 MeV}'
	      print *,'                3. 0+ (1)  {0.912 MeV}'
	      print *,'                4. 2+ (3)  {1.115 MeV}'
	      print *,'                5. 0+ (2)  {1.382 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.5) then
	         print *,'   '
	         go to 172
	      endif
	   elseif((chn(1:2).eq.'Pt'.or.chn(1:2).eq.'PT'.or.
     +             chn(1:2).eq.'pt').and.chn(3:5).eq.'198') then
173	      print *,'198-Hg level:   0. 0+ (gs)     {0 MeV}'
	      print *,'                1. 2+ (1)  {0.412 MeV}'
	      print 1
	      read *,ilevel
	      if(ilevel.lt.0.or.ilevel.gt.1) then
	         print *,'   '
	         go to 173
	      endif
	   elseif((chn(1:2).eq.'Bi'.or.chn(1:2).eq.'BI'.or.
     +             chn(1:2).eq.'bi').and.chn(3:5).eq.'214') then
	      print *,'transition is possible only to ',
     +                '214-At 1-(gs) level'
	      ilevel=0
	   elseif((chn(1:2).eq.'Pb'.or.chn(1:2).eq.'PB'.or.
     +             chn(1:2).eq.'pb').and.chn(3:5).eq.'214') then
	      print *,'transition is possible only to ',
     +                '214-Po 0+(gs) level'
	      ilevel=0
	   elseif((chn(1:2).eq.'Po'.or.chn(1:2).eq.'PO'.or.
     +             chn(1:2).eq.'po').and.chn(3:5).eq.'218') then
	      print *,'transition is possible only to ',
     +                '218-Rn 0+(gs) level'
	      ilevel=0
	   elseif((chn(1:2).eq.'Rn'.or.chn(1:2).eq.'RN'.or.
     +             chn(1:2).eq.'rn').and.chn(3:5).eq.'222') then
	      print *,'transition is possible only to ',
     +                '222-Ra 0+(gs) level'
	      ilevel=0
	   else
	      print *,'unknown double beta nuclide'
	      go to 2
	   endif
18	   print *,'     '
	   print *,' mode of bb-decay:'
	   print *,'      1. 0nubb(mn)         0+ -> 0+     {2n}'
	   print *,'      2. 0nubb(rhc-lambda) 0+ -> 0+     {2n}'
	   print *,'      3. 0nubb(rhc-lambda) 0+ -> 0+, 2+ {N*}'
	   print *,'      4. 2nubb             0+ -> 0+     {2n}'
	   print *,'      5. 0nubbM1           0+ -> 0+     {2n}'
	   print *,'      6. 0nubbM2           0+ -> 0+     (2n}'
	   print *,'      7. 0nubbM3           0+ -> 0+     {2n}'
	   print *,'      8. 0nubbM7           0+ -> 0+     {2n}'
	   print *,'      9. 0nubb(rhc-lambda) 0+ -> 2+     {2n}'
	   print *,'     10. 2nubb             0+ -> 2+     {2n}, {N*}'
	   print *,'     11. 0nuKb+            0+ -> 0+, 2+'
	   print *,'     12. 2nuKb+            0+ -> 0+, 2+'
	   print *,'     13. 0nu2K             0+ -> 0+, 2+'
	   print *,'     14. 2nu2K             0+ -> 0+, 2+'
	   print *,'     15. 2nubb             0+ -> 0+ with bosonic',
     +                   ' neutrinos'
	   print *,'     16. 2nubb             0+ -> 2+ with bosonic',
     +                   ' neutrinos'
	   print *,'     17. 0nubb(rhc-eta)    0+ -> 0+ simplified',
     +                   ' expression'
	   print *,'     18. 0nubb(rhc-eta)    0+ -> 0+ with specific',
     +                   ' NMEs'
	   print *,'         5-8: Majoron(s) with spectral index SI:'
	   print *,'              SI=1 - old M of Gelmini-Roncadelli'
	   print *,'              SI=2 - bulk M of Mohapatra'
	   print *,'              SI=3 - double M, vector M, charged M'
	   print *,'              SI=7'
	   print 1
	   read *,modebb0
	   if(modebb0.ge.1 .and. modebb0.le.5)  modebb=modebb0
	   if(modebb0.eq.6)                     modebb=14
	   if(modebb0.eq.7)                     modebb=6
	   if(modebb0.eq.8)                     modebb=13
	   if(modebb0.ge.9 .and. modebb0.le.14) modebb=modebb0-2
	   if(modebb0.ge.15)                    modebb=modebb0
	   if(modebb.lt.1.or.modebb.gt.18) then
	      print *,'   '
	      print *,'unknown mode'
	      go to 18
	   endif
	   ebb1=0.
	   ebb2=4.3
	   if(modebb.eq. 4.or.modebb.eq. 5.or.modebb.eq. 6.or.
     +        modebb.eq. 8.or.modebb.eq.10.or.modebb.eq.13.or.
     +        modebb.eq.14) then
	      print 19
c19	      format('$do you want to restrict energy range for ',
c     +               'generated particles? ')
19	      format(' do you want to restrict energy range for ',
     +               'generated particles? ',$)
	      read 3,chn
	      if(chn.eq.'y'.or.chn.eq.'Y') then
	         print 20
c20	         format('$range for sum of e-/e+ energies (MeV): ')
20	         format(' range for sum of e-/e+ energies (MeV): ',$)
	         read *,ebb1,ebb2
	      endif
	   endif
	   if(modebb.eq.18) then
	      print 18000
18000	      format(' name of file with NMEs: ',$)
	      read 18001,chfile_eta
18001	      format(a) 
c             file chfile_eta should be of the following structure:
c             line 1 - comment
c             line 2 - comment
c             line 3 - 7 NMEs: chi_GTw, chi_Fw, chi'_GT, chi'_F, chi'_T, chi'_P, chi'_R
	      open(unit=33,file=chfile_eta)
	      read(33,18001) chline
	      read(33,18001) chline
	      read(33,*) chi_GTw,chi_Fw,chip_GT,chip_F,chip_T,
     +                   chip_P,chip_R
	      close(unit=33)
	      print *,'NMEs: ',chi_GTw,chi_Fw,chip_GT,chip_F,chip_T,
     +                chip_P,chip_R
	   endif
	endif
	if(i2bbs.eq.2) then
	   print *,' Background & sources:'
	   print *,'    Ac228             Hf182             Sb125'
	   print *,'    Am241             I126              Sb126'    
	   print *,'    Ar39              I133              Sb133'    
	   print *,'    Ar42              I134              Sr90'    
	   print *,'    As79+Se79m        I135              Ta182'     
	   print *,'    Bi207+Pb207m      K40               Te133'    
	   print *,'    Bi208             K42               Te133m'    
	   print *,'    Bi210             Kr81              Te134'         
	   print *,'    Bi212+Po212       Kr85              Th234'          
	   print *,'    Bi214+Po214       Mn54              Tl207'          
	   print *,'    C14               Na22              Tl208'          
	   print *,'    Ca48+Sc48         P32               Xe129m'         
	   print *,'    Cd113             Pa234m            Xe131m'         
	   print *,'    Co60              Pb210             Xe133'          
	   print *,'    Cs136             Pb211             Xe135'          
	   print *,'    Cs137+Ba137m      Pb212             Y88'
	   print *,'    Eu147             Pb214             Y90'
	   print *,'    Eu152             Ra228             Zn65'
	   print *,'    Eu154             Rb87              Zr96+Nb96'
	   print *,'    Gd146             Rh106             '
	   print *,'    Artificial event (beta decay + internal e+e-',
     +                  ' + GEANT particles)'
	   print *,'    Compton effect'
	   print *,'    Moller scattering'
	   print *,'    Pair creation from external gamma quanta'
c	   print *,'    Double Compton'
	   print *,'     '
	   print 1
	   read 3,chn
	   chnuclide=chn
	   if(chn(1:3).eq.'Art'.or.chn(1:3).eq.'ART'.or.
     +        chn(1:3).eq.'art') then
	      print *,'     '
	      print *,'   Emission of up to 10 beta [b], e+e-',
     +                ' internal conversion pairs [p]'
	      print *,'   and any of GEANT particles [G]',
     +                ' in region of energies and angles'
	      print *,'   with time delay and halflife'
	      print *,'     '
	      print *,'GEANT particles:'
	      print *,'       1 - gamma         2 - positron    ',
     +                ' 3 - electron'
	      print *,'       4 - neutrino      5 - muon+       ',
     +                ' 6 - muon-'
	      print *,'       7 - pion0         8 - pion+       ',
     +                ' 9 - pion-'
	      print *,'      10 - kaon0 long   11 - kaon+       ',
     +                '12 - kaon-'
	      print *,'      13 - neutron      14 - proton      ',
     +                '15 - antiproton'
	      print *,'      16 - kaon0 short  17 - eta         ',
     +                '18 - lambda'
	      print *,'      19 - sigma+       20 - sigma0      ',
     +                '21 - sigma-'
	      print *,'      22 - xi0          23 - xi-         ',
     +                '24 - omega'
	      print *,'      25 - antineutron  26 - antilambda  ',
     +                '27 - antisigma-'
	      print *,'      28 - antisigma0   29 - antisigma+  ',
     +                '30 - antixi0'
	      print *,'      31 - antixi+      32 - antiomega+  ',
     +                '45 - deuteron'
	      print *,'      46 - tritium      47 - alpha       ',
     +                '48 - geantino'
	      print *,'      49 - He3          50 - Cerenkov'
	      print *,'     '
	      print 21
c21	      format('$number of parts in artificial event: ')
21	      format(' number of parts in artificial event: ',$)
	      read *,nartparts
	      nartparts=min0(10,nartparts)
	      do i=1,nartparts
22	         print 23,i
c23	         format('$',i2,' identifier (b/p/G): ')
23	         format(' ',i2,' identifier (b/p/G): ',$)
	         read 3,chn
	         chart(i)='  '
	         if(chn(1:1).eq.'B' .or.chn(1:1).eq.'b')  chart(i)='Be'
	         if(chn(1:1).eq.'G' .or.chn(1:1).eq.'g')  chart(i)='GP'
	         if(chn(1:1).eq.'P' .or.chn(1:1).eq.'p')  chart(i)='Pi'
	         if(chart(i).eq.'  ') then
	            print *,'unknown particle'
	            go to 22
	         elseif(chart(i).eq.'Be') then
	            print 24
c24	            format('$   Qbeta and Z of daughter nucleus',
c     +                     ' (Z>0 for beta- and',
c     +                     ' Z<0 for beta+): ')
24	            format('    Qbeta and Z of daughter nucleus',
     +                     ' (Z>0 for beta- and',
     +                     ' Z<0 for beta+): ',$)
	            read *,artQb(i),artZd(i)
	            print 25
c25	            format('$   tdelay, thalf: ')
25	            format('    tdelay, thalf: ',$)
	            read *,arttdelay(i),artthalf(i)
	         elseif(chart(i).eq.'Pi') then
	            print 26
c26	            format('$   Epair, tdelay, thalf: ')
26	            format('    Epair, tdelay, thalf: ',$)
	            read *,artQb(i),arttdelay(i),artthalf(i)
	         else
227	            print 27
c27	            format('$   GEANT particle number, ',
c     +                     'Emin, Emax (MeV): ')
27	            format('    GEANT particle number, ',
     +                     'Emin, Emax (MeV): ',$)
	            read *,l,artemin(i),artemax(i)
	            if(l.lt.1.or.l.gt.50.or.(l.gt.32.and.l.lt.45)) then
	               print *,'unknown particle'
	               go to 227
	            else
	               nartnpg(i)=l
	            endif
	            print 28
c28	            format('$   tetamin, tetamax, phimin,',
c     +                     ' phimax, tdelay, thalf: ')
28	            format('    tetamin, tetamax, phimin,',
     +                     ' phimax, tdelay, thalf: ',$)
	            read *,arttmin(i),arttmax(i),artfmin(i),
     +                     artfmax(i),arttdelay(i),artthalf(i)
	         endif
	      enddo
	   elseif(chn(1:3).eq.'Com'.or.chn(1:3).eq.'COM'.or.
     +        chn(1:3).eq.'com') then
	      print *,'   Ranges for energy and angles of initial',
     +                ' gammas'
	      print 280
c280	      format('$   Emin, Emax (MeV): ')
280	      format('    Emin, Emax (MeV): ',$)
	      read *,artemin(1),artemax(1)
	      print 281
c281	      format('$   tetamin, tetamax, phimin, phimax: ')
281	      format('    tetamin, tetamax, phimin, phimax: ',$)
	      read *,arttmin(1),arttmax(1),artfmin(1),artfmax(1)
	   elseif(chn(1:3).eq.'Mol'.or.chn(1:3).eq.'MOL'.or.
     +        chn(1:3).eq.'mol') then
	      print *,'   Ranges for energy and angles of initial',
     +                ' electron'
	      print 280
	      read *,artemin(1),artemax(1)
	      print 281
	      read *,arttmin(1),arttmax(1),artfmin(1),artfmax(1)
	      print 282
c282	      format('$   lower energy threshold for emitted',
c     +               ' delta rays (MeV): ')
282	      format('    lower energy threshold for emitted',
     +               ' delta rays (MeV): ',$)
	      read *,artQb(1)
	   elseif(chn(1:3).eq.'Pai'.or.chn(1:3).eq.'PAI'.or.
     +        chn(1:3).eq.'pai') then
	      print *,'   Ranges for energy and angles of initial',
     +                ' gammas'
287	      print 283
c283	      format('$   Emin, Emax (MeV): ')
283	      format('    Emin, Emax (MeV): ',$)
	      read *,artemin(1),artemax(1)
	      if(amin1(artemin(1),artemax(1)).lt.1.022) then
	         print *,'energies should be not less than 1.022 MeV'
	         go to 287
	      endif
	      print 284
c284	      format('$   tetamin, tetamax, phimin, phimax: ')
284	      format('    tetamin, tetamax, phimin, phimax: ',$)
	      read *,arttmin(1),arttmax(1),artfmin(1),artfmax(1)
286	      print 285
c285	      format('$   Z of target: ')
285	      format('    Z of target: ',$)
	      read *,artZd(1)
	      if(artZd(1).lt.1.) then
	         print *,'Z should be not less than 1'
	         go to 286
	      endif
	   endif
	endif
c*********************************************************************
	print *,'     '                                    !**********
	print 29                                           ! comment
c29	format('$number of events to generate      : ')    ! if in
29	format(' number of events to generate      : ',$)  ! if in
	read *,nevents                                     ! nemo*.car
	print 30                                           !
c30	format('$number of first event [1]         : ')    !
30	format(' number of first event [1]         : ',$)  !
	read 31,ievstart                                   !
31	format(i12)                                        !**********
c*********************************************************************
	if(ievstart.le.0) ievstart=1
	if(ievstart.ne.1) then
	   print 32
c32	   format('$initial random integer for RDMIN  : ')
32	   format(' initial random integer for RDMIN  : ',$)
	   read *,irndmst
	endif
	chfile='no file'
	iwrfile=0
c*********************************************************************
	print 33                                           !**********
c33	format('$to write generated events in file ? ')    ! comment
33	format(' to write generated events in file ? ',$)  ! comment
	read 3,chn                                         ! if in
	if(chn(1:1).eq.'Y'.or.chn(1:1).eq.'y') then        ! nemo*.car
	   iwrfile=1                                       !
	   print 34                                        !
c34	   format('$name of file                      : ') !
34	   format(' name of file                      : ',$)!
	   read 3,chfile                                   !
	endif                                              !**********
c*********************************************************************
	print *,'   '
	istart=-1
	ier=0
        ! call GENBBsub(i2bbs,chnuclide,ilevel,modebb,istart,ier)
	if(ier.ne.0) then
	   print *,'incorrect input parameter in GENBBsub'
	   go to 1000
	endif
	return
	end
c
c end of GENBBdia.f
c
c Local Variables: --
c mode: fortran --
c End: --
