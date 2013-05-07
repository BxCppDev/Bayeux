// -*- mode: c++; -*-
// genbb_utils.cc
/*
 * Copyright 2012-2013 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public 3.0 License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public 3.0 License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include <genbb_help/genbb_utils.h>

#include <boost/scoped_ptr.hpp>

#include <CLHEP/Units/SystemOfUnits.h>

namespace genbb
{

  const double utils::DEFAULT_ENERGY_RANGE_MIN = 0.0 * CLHEP::MeV;
  const double utils::DEFAULT_ENERGY_RANGE_MAX = 4.3 * CLHEP::MeV;

  const std::vector<std::string> & utils::get_dbd_nuclides ()
  {
    static boost::scoped_ptr<std::vector<std::string> > vptr (0);
    if (vptr.get () == 0) {
      vptr.reset (new std::vector<std::string>());
      std::vector<std::string> & v = *vptr.get ();
      v.reserve (40);
      v.push_back ("Ca48");
      v.push_back ("Ni58");
      v.push_back ("Zn64");
      v.push_back ("Zn70");
      v.push_back ("Ge76");
      v.push_back ("Se74");
      v.push_back ("Se82");
      v.push_back ("Sr84");
      v.push_back ("Zr94");
      v.push_back ("Zr96");
      v.push_back ("Mo92");
      v.push_back ("Mo100");
      v.push_back ("Ru96");
      v.push_back ("Ru104");
      v.push_back ("Cd106");
      v.push_back ("Cd108");
      v.push_back ("Cd114");
      v.push_back ("Cd116");
      v.push_back ("Sn112");
      v.push_back ("Sn122");
      v.push_back ("Sn124");
      v.push_back ("Te120");
      v.push_back ("Te128");
      v.push_back ("Te130");
      v.push_back ("Xe136");
      v.push_back ("Ce136");
      v.push_back ("Ce138");
      v.push_back ("Ce142");
      v.push_back ("Nd148");
      v.push_back ("Nd150");
      v.push_back ("Dy156");
      v.push_back ("Dy158");
      v.push_back ("W180");
      v.push_back ("W186");
      v.push_back ("Pt190");
      v.push_back ("Pt198");
      v.push_back ("Bi214+At214");
      v.push_back ("Pb214+Po214");
      v.push_back ("Po218+Rn218+Po214");
      v.push_back ("Rn222+Ra222+Rn218+Po214");
    }
    return *vptr.get ();
  }

  const std::map<int,std::string> & utils::get_dbd_modes ()
  {
    static boost::scoped_ptr<std::map<int,std::string> > mptr (0);
    if (mptr.get () == 0) {
      mptr.reset (new std::map<int,std::string>());
      std::map<int,std::string> & m = *mptr.get ();
      m[1] = "0nubb(mn)         0+ -> 0+     {2n}";
      m[2] = "0nubb(rhc-lambda) 0+ -> 0+     {2n}";
      m[3] = "0nubb(rhc-lambda) 0+ -> 0+, 2+ {N*}";
      m[4] = "2nubb             0+ -> 0+     {2n}";
      m[5] = "0nubbM1           0+ -> 0+     {2n}";
      m[6] = "0nubbM2           0+ -> 0+     (2n}";
      m[7] = "0nubbM3           0+ -> 0+     {2n}";
      m[8] = "0nubbM7           0+ -> 0+     {2n}";
      m[9] = "0nubb(rhc-lambda) 0+ -> 2+     {2n}";
      m[10] = "2nubb             0+ -> 2+     {2n}, {N*}";
      m[11] = "0nuKb+            0+ -> 0+, 2+";
      m[12] = "2nuKb+            0+ -> 0+, 2+";
      m[13] = "0nu2K             0+ -> 0+, 2+";
      m[14] = "2nu2K             0+ -> 0+, 2+";
      m[15] = "2nubb             0+ -> 0+ with bosonic neutrinos";
      m[16] = "2nubb             0+ -> 2+ with bosonic neutrinos";
      m[17] = "0nubb(rhc-eta)    0+ -> 0+ simplified expression";
      m[18] = "0nubb(rhc-eta)    0+ -> 0+ with specific NMEs";
    }
    return *mptr.get ();
  }

  const std::vector<int> & utils::get_dbd_modes_with_energy_range ()
  {
    static boost::scoped_ptr<std::vector<int> > eptr (0);
    if (eptr.get () == 0) {
      eptr.reset (new std::vector<int>());
      std::vector<int> & e = *eptr.get ();
      e.reserve (7);
      e.push_back (4);
      e.push_back (5);
      e.push_back (6);
      e.push_back (8);
      e.push_back (10);
      e.push_back (13);
      e.push_back (14);
    }
    return *eptr.get ();
  }


}

