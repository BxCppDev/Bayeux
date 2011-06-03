// -*- mode: c++; -*- 
// genbb_writer.cc
/*
 * Copyright 2007-2011 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public  License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 * Boston, MA 02110-1301, USA.
 *
 */

#include <genbb_help/genbb_writer.h>


#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <string>
#include <list>

//#include <genbb_help/primary_particle.ipp>
//#include <genbb_help/primary_event.ipp>
#include <genbb_help/primary_event.h>

#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/PhysicalConstants.h>
#include <CLHEP/Vector/ThreeVector.h>

namespace genbb {

  void genbb_writer::run ()
  {
    init ();

    std::ofstream __out ( __fout.c_str() );

    double ElectronMass = CLHEP::electron_mass_c2;
    double AlphaMass    = 3.72738 *CLHEP::GeV;

    for ( int i = 0; i < __Npart; i++ )
      {
	__out << i << " "
	      << 0 << " "
	      << __part_per_evt
	      << std::endl;

	for ( int j = 0; j < __part_per_evt; j++ )
	  {

	    double KineticEnergy      = myrandom ( __Emin, __Emax );
	    double cos_phi            = myrandom ( std::cos ( __phi_min ), std::cos ( __phi_max ) );
	    double phi                = std::acos ( cos_phi );
	    // double phi                = myrandom ( __phi_min, __phi_max );

	    double theta              = myrandom ( 0., 2*M_PI );
	    double RelativisticEnergy = 0.;
	    double TotalMomentum      = 0.;
	    
	    if ( __type == 2 || __type == 3 ) // e+ / e-
	      {
		RelativisticEnergy = KineticEnergy + ElectronMass;
		TotalMomentum = std::sqrt( RelativisticEnergy * RelativisticEnergy
					   - ElectronMass * ElectronMass );
	      }
	    else if ( __type == 47 )          // alpha
	      {
		RelativisticEnergy = KineticEnergy + AlphaMass;
		TotalMomentum = std::sqrt( RelativisticEnergy * RelativisticEnergy
					   - AlphaMass * AlphaMass );
	      }
	    else     	  	  	      // gamma
	      {
		TotalMomentum = KineticEnergy;
	      }
	    
	    double x_mom = TotalMomentum * cos ( theta ) * sin ( phi );
	    double y_mom = TotalMomentum * sin ( theta ) * sin ( phi );
	    double z_mom = TotalMomentum * cos ( phi );
	    
	    __out << __type << " "
		  << x_mom << " "
		  << y_mom << " "
		  << z_mom << " "
		  << 0
		  << std::endl;
	  }
      }
  }
  

  double genbb_writer::myrandom ( double min_, double max_ )
  {
    return ( max_ - min_ ) * drand48() + min_;
  }
  

  void genbb_writer::init ()
  {
    if ( __Npart < 0        ||
	 __part_per_evt < 0 ||
	 __type == -1       ||
	 __Emin < 0         ||
	 __Emax < 0         ||
	 __phi_min < 0      ||
	 __phi_max > M_PI    )
      {
	std::ostringstream message;
	message << "genbb_writer::init: "
		<< "All the parameters of the writer are not well defined. Abort!";
	throw std::runtime_error ( message.str() );
      }
  }
  

  void genbb_writer::dump ( std::ostream & out_ ) const
  {
    out_ << "genbb_writer::dump: " << std::endl;
    out_ << "|-- debug:         " << __debug        << std::endl;
    out_ << "|-- outfile:       " << __fout         << std::endl;
    out_ << "|-- Nb particles:  " << __Npart        << std::endl;
    out_ << "|-- part / evt:    " << __part_per_evt << std::endl;
    out_ << "|-- particle type: " << __type         << std::endl;
    out_ << "|-- energy min:    " << __Emin         << std::endl;
    out_ << "|-- energy max:    " << __Emax         << std::endl;
    out_ << "|-- phi_min:       " << __phi_min    << std::endl;
    out_ << "`-- phi_max:       " << __phi_max    << std::endl;
  }


  // ctor:
  genbb_writer::genbb_writer ()
  {
    __debug        = false;
    __seed         = 314159;
    __Npart        = -1;
    __part_per_evt = 1;
    __type         = -1;
    __Emin         = -1.;
    __Emax         = -1.;
    __phi_min      =  1.;
    __phi_max      = -1.;
    srand48 ( __seed );
  }
  
  // dtor:
  genbb_writer::~genbb_writer ()
  {
  }


} // end of namespace genbb

// end of genbb_writer.cc
