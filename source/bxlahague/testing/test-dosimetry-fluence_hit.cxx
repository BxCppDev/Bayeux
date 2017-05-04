// test-dosimetry-fluence_hit.cxx
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//                       and Arnaud Chapon <achapon@cerap.fr>
//
// This file is part of LaHague.
//
// LaHague is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LaHague is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LaHague. If not, see <http://www.gnu.org/licenses/>.
//

// Standard library:
#include <cstdlib>
#include <iostream>
#include <exception>

// Third Party:
// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools:
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>
#include <datatools/units.h>

// This project:
#include <lahague/lahague_config.h>
#include <lahague/dosimetry/fluence_hit.h>
#include <lahague/dosimetry/fluence_to_h10.h>

int main (int /* argc_ */, char ** /* argv_*/)
{
  BAYEUX_INIT();
  // using namespace std;
  int error_code = EXIT_SUCCESS;
  try
    {
      std::clog << "Test program for the 'lahague::dosimetry::fluence_hit' class." << std::endl;

      lahague::dosimetry::fluence_hit flhit;

      geomtools::geom_id vol_gid;
      vol_gid.set_type(123);
      vol_gid.set(0, 2);
      vol_gid.set(1, 0);
      vol_gid.set(2, 3);

      flhit.set_hit_id(0);
      flhit.set_geom_id(vol_gid);
      flhit.set_particle_name("gamma");
      flhit.set_entrance_energy(1.17 * CLHEP::MeV);
      flhit.set_h10_star(lahague::dosimetry::gamma_fluence_to_h10_conversion(flhit.get_entrance_energy()));

      flhit.tree_dump(std::clog, "Fluence hit: ");

      std::clog << "1 eV    = " << CLHEP::eV << std::endl;
      std::clog << "1 Joule = " << CLHEP::joule << std::endl;
      std::clog << "1 kg    = " << CLHEP::kg << std::endl;
      std::clog << "1 Gray  = " << CLHEP::gray << std::endl;
      std::clog << "1 cm2   = " << CLHEP::cm2 << std::endl;
      std::clog << "1 Sv    = " << datatools::units::get_unit("sievert") << std::endl;
      std::clog << "1 pSv   = " << datatools::units::get_unit("picosievert") << std::endl;

      {
        datatools::data_writer writer("test-dosimetry-fluence_hit.xml");
        writer.store(flhit);
      }

      flhit.invalidate();

      {
        datatools::data_reader reader("test-dosimetry-fluence_hit.xml");
        reader.load(flhit);
      }

      flhit.tree_dump(std::clog, "Fluence hit (deserialized): ");

      // double A_source = 1.0 * datatools::units::get_unit("GBq");
      double r_icru = 15.0 * CLHEP::cm;
      double s_icru = M_PI * r_icru * r_icru;
      double H10_star = flhit.get_h10_star() / s_icru;
      double h10_unit = datatools::units::get_unit("uSv") / datatools::units::get_unit("GBq") /  datatools::units::get_unit("hour");
      std::clog << "H*(10) = " << H10_star / h10_unit << " uSv/GBq/h" << std::endl;

    }
  catch (std::exception & x)
    {
      std::cerr << "error: " << x.what () << std::endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "error: " << "unexpected error !" << std::endl;
      error_code = EXIT_FAILURE;
    }
  BAYEUX_FINI();
  return (error_code);
}
