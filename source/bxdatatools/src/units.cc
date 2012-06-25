// -*- mode: c++ ; -*- 
/* units.cc
 *
 * All units from "include/CLHEP/Units/SystemOfUnits.h"
 *
 */
// Ourselves
#include <datatools/utils/units.h>

// Standard Library
#include <cmath>
#include <cstdlib>

#include <iostream>
#include <iomanip>
#include <limits>
#include <list>
#include <sstream>
#include <stdexcept>

// Third Party
// - A
#include <boost/scoped_ptr.hpp>

// This Project
#include <datatools/utils/clhep_units.h>


namespace datatools {
namespace utils {

double units::get_length_unit_from(const std::string& word) {
  if ((word == "angstrom")) return CLHEP::angstrom;
  if ((word == "fm") || (word == "fermi")) return CLHEP::fermi;
  if ((word == "nm") || (word == "nanometer")) return CLHEP::nanometer;
  if ((word == "um") || (word == "micrometer")) return CLHEP::micrometer;
  if ((word == "mm") || (word == "millimeter")) return CLHEP::millimeter;
  if ((word == "cm") || (word == "centimeter")) return CLHEP::centimeter;
  if ((word == "m") || (word == "meter")) return CLHEP::meter;
  if ((word == "km") || (word == "kilometer")) return CLHEP::kilometer;
  if ((word == "pc") || (word == "parsec")) return CLHEP::parsec;
  if ((word == "inch")) return 2.54 * CLHEP::centimeter;
  throw_bad_unit("length", word);
  return std::numeric_limits<double>::quiet_NaN();
}


double units::get_surface_unit_from(const std::string& word) {
  if ((word == "m2")) return CLHEP::meter2;
  if ((word == "cm2")) return CLHEP::centimeter2;
  if ((word == "mm2")) return CLHEP::millimeter2;
  if ((word == "km2")) return CLHEP::kilometer2;
  throw_bad_unit("surface", word);
  return std::numeric_limits<double>::quiet_NaN();
}


double units::get_volume_unit_from(const std::string& word) {
  if ((word == "m3")) return CLHEP::meter3;
  if ((word == "cm3")) return CLHEP::centimeter3;
  if ((word == "mm3")) return CLHEP::millimeter3;
  if ((word == "km3")) return CLHEP::kilometer3;
  throw_bad_unit("volume", word);
  return std::numeric_limits<double>::quiet_NaN();
}


double units::get_time_unit_from(const std::string& word) {
  if ((word == "ps") || (word == "picosecond")) return CLHEP::picosecond;
  if ((word == "ns") || (word == "nanosecond")) return CLHEP::nanosecond;
  if ((word == "us") || (word == "microsecond")) return CLHEP::microsecond;
  if ((word == "ms") || (word == "millisecond")) return CLHEP::millisecond;
  if ((word == "s") || (word == "second")) return CLHEP::second;
  if ((word == "minute")) return 60 * CLHEP::second;
  if ((word == "h") || (word == "hour")) return 3600 * CLHEP::second;
  throw_bad_unit("time", word);
  return std::numeric_limits<double>::quiet_NaN();
}


double units::get_angle_unit_from(const std::string& word) {
  if ((word == "rad") || (word == "radian")) return CLHEP::radian;
  if ((word == "mrad") || (word == "milliradian")) return CLHEP::milliradian;
  if ((word == "deg") || (word == "degree")) return CLHEP::degree;
  throw_bad_unit("angle", word);
  return std::numeric_limits<double>::quiet_NaN();
}


double units::get_solid_angle_unit_from(const std::string& word) {
  if ((word == "steradian")) return CLHEP::steradian;
  throw_bad_unit("solid angle", word);
  return std::numeric_limits<double>::quiet_NaN();
}


double units::get_energy_unit_from(const std::string& word) {
  if ((word == "eV") || (word == "electronvolt")) return CLHEP::electronvolt;
  if ((word == "keV") || (word == "kiloelectronvolt")) return CLHEP::kiloelectronvolt;
  if ((word == "MeV") || (word == "megaelectronvolt")) return CLHEP::megaelectronvolt;
  if ((word == "GeV") || (word == "gigaelectronvolt")) return CLHEP::gigaelectronvolt;
  if ((word == "TeV") || (word == "teraelectronvolt")) return CLHEP::teraelectronvolt;
  if ((word == "PeV") || (word == "petaelectronvolt")) return CLHEP::petaelectronvolt;
  if ((word == "J") || (word == "joule")) return CLHEP::joule;
  throw_bad_unit("energy", word);
  return std::numeric_limits<double>::quiet_NaN();
}


double units::get_mass_unit_from(const std::string& word) {
  if ((word == "eV/c2")) return CLHEP::eV / CLHEP::c_squared;
  if ((word == "MeV/c2")) return CLHEP::MeV / CLHEP::c_squared;
  if ((word == "GeV/c2")) return CLHEP::GeV / CLHEP::c_squared;
  if ((word == "kg") || (word == "kilogram")) return CLHEP::kilogram;
  if ((word == "g") || (word == "gram")) return CLHEP::gram;
  if ((word == "mg") || (word == "milligram")) return CLHEP::milligram;
  if ((word == "ug") || (word == "microgram")) return 1.e-3 * CLHEP::milligram;
  if ((word == "t") || (word == "ton")) return 1000. * CLHEP::kilogram;
  throw_bad_unit("mass", word);
  return std::numeric_limits<double>::quiet_NaN();
}


double units::get_pressure_unit_from(const std::string& word) {
  if ((word == "mbar") || (word == "millibar")) return 1.0e-3 * CLHEP::bar;
  if ((word == "bar")) return CLHEP::bar;
  if ((word == "atmosphere")) return CLHEP::atmosphere;
  if ((word == "Pa") || (word == "pascal")) return CLHEP::hep_pascal;
  throw_bad_unit("pressure", word);
  return std::numeric_limits<double>::quiet_NaN();
}


double units::get_magnetic_field_unit_from(const std::string& word) {
  if ((word == "T") || (word == "tesla")) return CLHEP::tesla;
  if ((word == "G") || (word == "gauss")) return CLHEP::gauss;
  if ((word == "kG") || (word == "kilogauss")) return CLHEP::kilogauss;
  throw_bad_unit("magnetic field", word);
  return std::numeric_limits<double>::quiet_NaN();
}


double units::get_electric_field_unit_from(const std::string& word) {
  if ((word == "V/cm")) return CLHEP::volt / CLHEP::centimeter;
  if ((word == "V/m")) return CLHEP::volt / CLHEP::meter;
  if ((word == "kV/cm")) return CLHEP::kilovolt / CLHEP::centimeter;
  if ((word == "kV/m")) return CLHEP::kilovolt / CLHEP::meter;
  throw_bad_unit("electric field", word);
  return std::numeric_limits<double>::quiet_NaN();
}


double units::get_temperature_unit_from(const std::string& word) {
  if ((word == "kelvin")) return CLHEP::kelvin;
  throw_bad_unit("temperature", word);
  return std::numeric_limits<double>::quiet_NaN();
}


double units::get_density_unit_from(const std::string& word) {
  if ((word == "mg/cm3")) return CLHEP::milligram / CLHEP::cm3;
  if ((word == "g/cm3")) return CLHEP::gram / CLHEP::cm3;
  if ((word == "kg/m3")) return CLHEP::kg / CLHEP::m3;
  throw_bad_unit("density", word);
  return std::numeric_limits<double>::quiet_NaN();
}


double units::get_frequency_unit_from(const std::string& word) {
  if ((word == "Hz")) return CLHEP::hertz;
  if ((word == "mHz")) return 1.e-3 * CLHEP::hertz;
  if ((word == "kHz")) return CLHEP::kilohertz;
  if ((word == "MHz")) return CLHEP::megahertz;
  if ((word == "GHz")) return 1.e3 * CLHEP::megahertz;
  throw_bad_unit("frequency", word);
  return std::numeric_limits<double>::quiet_NaN();
}


double units::get_activity_unit_from(const std::string& word) {
  if ((word == "Bq")) return 1. / CLHEP::second;
  if ((word == "mBq")) return 1.e-3 / CLHEP::second;
  if ((word == "uBq")) return 1.e-6 / CLHEP::second;
  if ((word == "kBq")) return 1.e+3 / CLHEP::second;
  if ((word == "MBq")) return 1.e+6 / CLHEP::second;
  if ((word == "GBq")) return 1.e+9 / CLHEP::second;
  throw_bad_unit("activity", word);
  return std::numeric_limits<double>::quiet_NaN();
}


double units::get_volume_activity_unit_from(const std::string& word) {
  double Bq_per_m3 = CLHEP::becquerel / CLHEP::m3;
  if ((word == "Bq/m3"))  return 1.    * Bq_per_m3;
  if ((word == "mBq/m3")) return 1.e-3 * Bq_per_m3;
  if ((word == "uBq/m3")) return 1.e-6 * Bq_per_m3;
  if ((word == "kBq/m3")) return 1.e+3 * Bq_per_m3;
  if ((word == "MBq/m3")) return 1.e+6 * Bq_per_m3;
  if ((word == "GBq/m3")) return 1.e+9 * Bq_per_m3;
  throw_bad_unit("volume_activity", word);
  return std::numeric_limits<double>::quiet_NaN();
}


double units::get_surface_activity_unit_from(const std::string& word) {
  double Bq_per_m2 = 1. / CLHEP::second / CLHEP::m2;
  if ((word == "Bq/m2"))  return 1.    * Bq_per_m2;
  if ((word == "mBq/m2")) return 1.e-3 * Bq_per_m2;
  if ((word == "uBq/m2")) return 1.e-6 * Bq_per_m2; 
  if ((word == "kBq/m2")) return 1.e+3 * Bq_per_m2;
  if ((word == "MBq/m2")) return 1.e+6 * Bq_per_m2;
  if ((word == "GBq/m2")) return 1.e+9 * Bq_per_m2;
  throw_bad_unit("surface_activity", word);
  return std::numeric_limits<double>::quiet_NaN();
}


double units::get_mass_activity_unit_from(const std::string& word) {
  double Bq_per_kg = 1. / CLHEP::second / CLHEP::kg;
  if ((word == "Bq/kg"))  return 1.    * Bq_per_kg;
  else if ((word == "mBq/kg")) return 1.e-3 * Bq_per_kg;
  else if ((word == "uBq/kg")) return 1.e-6 * Bq_per_kg; 
  else if ((word == "kBq/kg")) return 1.e+3 * Bq_per_kg;
  else if ((word == "MBq/kg")) return 1.e+6 * Bq_per_kg;
  else if ((word == "GBq/kg")) return 1.e+9 * Bq_per_kg;
  else throw_bad_unit("mass_activity", word);
  return std::numeric_limits<double>::quiet_NaN();
}


double units::get_unit_from(const std::string& unit_type, 
                            const std::string& unit_str) {
  if (unit_type == "length") {
    return get_length_unit_from(unit_str);
  } else if (unit_type == "surface") {
    return get_surface_unit_from(unit_str);
  } else if (unit_type == "volume") {
    return get_volume_unit_from(unit_str);
  } else if (unit_type == "time") {
    return get_time_unit_from(unit_str);
  } else if (unit_type == "angle") {
    return get_angle_unit_from(unit_str);
  } else if (unit_type == "solid_angle") {
    return get_solid_angle_unit_from(unit_str);
  } else if (unit_type == "energy") {
    return get_energy_unit_from(unit_str);
  } else if (unit_type == "mass") {
    return get_mass_unit_from(unit_str);
  } else if (unit_type == "pressure") {
    return get_pressure_unit_from(unit_str);
  } else if (unit_type == "magnetic_field") {
    return get_magnetic_field_unit_from(unit_str);
  } else if (unit_type == "electric_field") {
    return get_electric_field_unit_from(unit_str);
  } else if (unit_type == "temperature") {
    return get_temperature_unit_from(unit_str);
  } else if (unit_type == "density") {
    return get_density_unit_from(unit_str);
  } else if (unit_type == "frequency") {
    return get_frequency_unit_from(unit_str);
  } else if (unit_type == "activity") {
    return get_activity_unit_from(unit_str);
  } else if (unit_type == "surface_activity") {
    return get_surface_activity_unit_from(unit_str);
  } else if (unit_type == "volume_activity") {
    return get_volume_activity_unit_from(unit_str);
  } else if (unit_type == "mass_activity") {
    return get_mass_activity_unit_from(unit_str);
  }
  {
    std::ostringstream message;
    message << "Invalid " << unit_type 
            << " of unit :'" << unit_type << "' !";
    throw std::logic_error(message.str());
  }
  return std::numeric_limits<double>::quiet_NaN();
}


const std::vector<std::string>& units::get_unit_labels_registry() {
  static boost::scoped_ptr<std::vector<std::string> > ulabels_ptr(0);
  if (ulabels_ptr.get() == 0) {
    ulabels_ptr.reset(new std::vector<std::string>);
    std::vector<std::string>& ulabels = *ulabels_ptr.get();
    ulabels.reserve(20);
    ulabels.push_back("length");
    ulabels.push_back("surface");
    ulabels.push_back("volume");
    ulabels.push_back("time");
    ulabels.push_back("angle");
    ulabels.push_back("solid_angle");
    ulabels.push_back("energy");
    ulabels.push_back("mass");
    ulabels.push_back("pressure");
    ulabels.push_back("magnetic_field");
    ulabels.push_back("temperature");
    ulabels.push_back("density");
    ulabels.push_back("activity");
    ulabels.push_back("volume_activity");
    ulabels.push_back("surface_activity");
    ulabels.push_back("mass_activity");
    ulabels.push_back("frequency");          
  }
  return *ulabels_ptr.get();
}


bool units::find_unit(const std::string& unit_str, double& unit_value, 
                      std::string& unit_label) {
  unit_label = "";
  unit_value = std::numeric_limits<double>::quiet_NaN();

  const std::vector<std::string>& ulabels = get_unit_labels_registry();
  double val = std::numeric_limits<double>::quiet_NaN();
  int count = -1;
  for (std::vector<std::string>::const_iterator i = ulabels.begin();
       i != ulabels.end();
       ++i) {
    try {
      count++;
      unit_label = *i;
      val = get_unit_from(*i, unit_str);
      break;
    }
    catch (std::exception& x) {
      val = std::numeric_limits<double>::quiet_NaN();
    }
  }

  if (!std::isnormal(val)) {
    unit_label = "";
    unit_value = val;
    return false;
  }
  unit_label = ulabels[count];
  unit_value = val;
  return true;
}


double units::get_unit(const std::string& unit_str) {
  double unit_val;
  std::string unit_label;
  bool res = find_unit(unit_str, unit_val, unit_label);

  if (!res) return std::numeric_limits<double>::quiet_NaN();
  return unit_val;
}


bool units::find_value_with_unit(const std::string& word, 
                                 double& value, 
                                 std::string& unit_label) {
  value = std::numeric_limits<double>::quiet_NaN();
  unit_label = "";

  double val;
  std::istringstream iss(word);
  iss >> val;
  if (!iss) {
    std::ostringstream message;
    message << "datatools::utils::units::find_value_with_unit: Format error while reading a double value !";
    return false;
  }
  iss >> std::ws;
  if (!iss.eof()) {
    std::string ustr;
    iss >> ustr;

    if (ustr.empty()) return false;
  
    double any_unit_value;
    std::string any_unit_label;
    bool res = find_unit(ustr, any_unit_value, any_unit_label);
    
    if (!res) return false;
    
    val *= any_unit_value;
    unit_label = any_unit_label;
  }
  value = val;
  return true;
}


double units::get_value_with_unit(const std::string& word) {
  double unit_value = std::numeric_limits<double>::quiet_NaN();
  std::string unit_label;
  if (!find_value_with_unit(word, unit_value, unit_label)) {
    std::ostringstream message;
    message << "datatools::utils::units::get_value_with_unit: Cannot parse a value with its units !";
    throw std::logic_error(message.str());
  }
  return unit_value;
}


void units::throw_bad_unit(const std::string& a_type, 
                           const std::string& a_unit_str) {
  std::ostringstream message;
  message << "Invalid " << a_type << " unit :'" << a_unit_str << "' !";
  throw std::logic_error(message.str());
}


} // end of namespace utils
} // end of namespace datatools

