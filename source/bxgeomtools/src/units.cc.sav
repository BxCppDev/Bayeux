// -*- mode: c++ ; -*- 
/* units.cc
 */

#include <geomtools/units.h>

namespace geomtools {

  using namespace std;

  void units::_throw_bad_unit (const string & type_, const string & unit_str_)
  {
    ostringstream message;
    message << "Invalid " << type_ << " unit :'" << unit_str_ << "' !";
    throw runtime_error (message.str ());
  }
  
  double units::get_length_unit_from (const string & str_)
  {
    if ((str_ == "angstrom")) return CLHEP::angstrom;
    if ((str_ == "fm") || (str_ == "fermi")) return CLHEP::fermi;
    if ((str_ == "nm") || (str_ == "nanometer")) return CLHEP::nanometer;
    if ((str_ == "um") || (str_ == "micrometer")) return CLHEP::micrometer;
    if ((str_ == "mm") || (str_ == "millimeter")) return CLHEP::millimeter;
    if ((str_ == "cm") || (str_ == "centimeter")) return CLHEP::centimeter;
    if ((str_ == "m") || (str_ == "meter")) return CLHEP::meter;
    if ((str_ == "km") || (str_ == "kilometer")) return CLHEP::kilometer;
    if ((str_ == "pc") || (str_ == "parsec")) return CLHEP::parsec;
    _throw_bad_unit ("length", str_);
  }

  double units::get_surface_unit_from (const string & str_)
  {
    if ((str_ == "m2")) return CLHEP::meter2;
    if ((str_ == "cm2")) return CLHEP::centimeter2;
    if ((str_ == "mm2")) return CLHEP::millimeter2;
    if ((str_ == "km2")) return CLHEP::kilometer2;
    _throw_bad_unit ("surface", str_);
  }

  double units::get_volume_unit_from (const string & str_)
  {
    if ((str_ == "m3")) return CLHEP::meter3;
    if ((str_ == "cm3")) return CLHEP::centimeter3;
    if ((str_ == "mm3")) return CLHEP::millimeter3;
    if ((str_ == "km3")) return CLHEP::kilometer3;
    _throw_bad_unit ("volume", str_);
  }

  double units::get_time_unit_from (const string & str_)
  {
    if ((str_ == "ps") || (str_ == "picosecond")) return CLHEP::picosecond;
    if ((str_ == "ns") || (str_ == "nanosecond")) return CLHEP::nanosecond;
    if ((str_ == "us") || (str_ == "microsecond")) return CLHEP::microsecond;
    if ((str_ == "ms") || (str_ == "millisecond")) return CLHEP::millisecond;
    if ((str_ == "s") || (str_ == "second")) return CLHEP::second;
    if ((str_ == "minute")) return 60 * CLHEP::second;
    if ((str_ == "h") || (str_ == "hour")) return 3600 * CLHEP::second;
    _throw_bad_unit ("time", str_);
  }

  double units::get_angle_unit_from (const string & str_)
  {
    if ((str_ == "rad") || (str_ == "radian")) return CLHEP::radian;
    if ((str_ == "mrad") || (str_ == "milliradian")) return CLHEP::milliradian;
    if ((str_ == "deg") || (str_ == "degree")) return CLHEP::degree;
    _throw_bad_unit ("angle", str_);
  }

  double units::get_solid_angle_unit_from (const string & str_)
  {
    if ((str_ == "steradian")) return CLHEP::steradian;
    _throw_bad_unit ("solid angle", str_);
  }

  double units::get_energy_unit_from (const string & str_)
  {
    if ((str_ == "eV") || (str_ == "electronvolt")) return CLHEP::electronvolt;
    if ((str_ == "keV") || (str_ == "kiloelectronvolt")) return CLHEP::kiloelectronvolt;
    if ((str_ == "MeV") || (str_ == "megaelectronvolt")) return CLHEP::megaelectronvolt;
    if ((str_ == "GeV") || (str_ == "gigaelectronvolt")) return CLHEP::gigaelectronvolt;
    if ((str_ == "TeV") || (str_ == "teraelectronvolt")) return CLHEP::teraelectronvolt;
    if ((str_ == "PeV") || (str_ == "petaelectronvolt")) return CLHEP::petaelectronvolt;
    if ((str_ == "J") || (str_ == "joule")) return CLHEP::joule;
    _throw_bad_unit ("energy", str_);
   }

  double units::get_mass_unit_from (const string & str_)
  {
    if ((str_ == "eV/c2")) return CLHEP::eV / CLHEP::c_squared;
    if ((str_ == "MeV/c2")) return CLHEP::MeV / CLHEP::c_squared;
    if ((str_ == "GeV/c2")) return CLHEP::GeV / CLHEP::c_squared;
    if ((str_ == "kg") || (str_ == "kilogram")) return CLHEP::kilogram;
    if ((str_ == "g") || (str_ == "gram")) return CLHEP::gram;
    if ((str_ == "mg") || (str_ == "milligram")) return CLHEP::milligram;
    if ((str_ == "ug") || (str_ == "microgram")) return 1.e-3 * CLHEP::milligram;
    if ((str_ == "t") || (str_ == "ton")) return 1000. * CLHEP::kilogram;
    _throw_bad_unit ("mass", str_);
   }

  double units::get_pressure_unit_from (const string & str_)
  {
    if ((str_ == "bar")) return CLHEP::bar;
    if ((str_ == "atmosphere")) return CLHEP::atmosphere;
    if ((str_ == "mg") || (str_ == "milligram")) return CLHEP::milligram;
    if ((str_ == "Pa") || (str_ == "pascal")) return CLHEP::hep_pascal;
    _throw_bad_unit ("pressure", str_);
   }

  double units::get_magnetic_field_unit_from (const string & str_)
  {
    if ((str_ == "T") || (str_ == "tesla")) return CLHEP::tesla;
    if ((str_ == "G") || (str_ == "gauss")) return CLHEP::gauss;
    if ((str_ == "kG") || (str_ == "kilogauss")) return CLHEP::kilogauss;
    _throw_bad_unit ("magnetic field", str_);
  }

  double units::get_temperature_unit_from (const string & str_)
  {
    if ((str_ == "kelvin")) return CLHEP::kelvin;
    _throw_bad_unit ("temperature", str_);
  }

  double units::get_density_unit_from (const string & str_)
  {
    if ((str_ == "mg/cm3")) return CLHEP::milligram / CLHEP::cm3;
    if ((str_ == "g/cm3")) return CLHEP::gram / CLHEP::cm3;
    if ((str_ == "kg/m3")) return CLHEP::kg / CLHEP::m3;
    _throw_bad_unit ("density", str_);
  }

  double units::get_unit_from (const string & unit_type_, 
			       const string & unit_str_)
  {
    if (unit_type_ == "length")
      {
	return get_length_unit_from (unit_str_);
      }
    if (unit_type_ == "surface")
      {
	return get_surface_unit_from (unit_str_);
      }
    if (unit_type_ == "volume")
      {
	return get_volume_unit_from (unit_str_);
      }
    if (unit_type_ == "time")
      {
	return get_time_unit_from (unit_str_);
      }
    if (unit_type_ == "angle")
      {
	return get_angle_unit_from (unit_str_);
      }
    if (unit_type_ == "solid_angle")
      {
	return get_solid_angle_unit_from (unit_str_);
      }
    if (unit_type_ == "energy")
      {
	return get_energy_unit_from (unit_str_);
      }
    if (unit_type_ == "mass")
      {
	return get_mass_unit_from (unit_str_);
      }
    if (unit_type_ == "pressure")
      {
	return get_pressure_unit_from (unit_str_);
      }
    if (unit_type_ == "magnetic_field")
      {
	return get_magnetic_field_unit_from (unit_str_);
      }
    if (unit_type_ == "temperature")
      {
	return get_temperature_unit_from (unit_str_);
      }
    if (unit_type_ == "density")
      {
	return get_density_unit_from (unit_str_);
      }
    ostringstream message;
    message << "Invalid " << unit_type_ << " of unit :'" << unit_type_ << "' !";
    throw runtime_error (message.str ());
  }
 
} // end of namespace geomtools

// end of units.cc
