/* units.cc
 *
 * All units from "include/CLHEP/Units/SystemOfUnits.h"
 *
 */

// Ourselves:
#include <datatools/units.h>

// Standard Library:
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <limits>
#include <list>
#include <sstream>
#include <stdexcept>
#include <algorithm>

// Third Party:
// - Boost:
#include <boost/scoped_ptr.hpp>

// This Project:
#include <datatools/utils.h>
#include <datatools/clhep_units.h>

namespace datatools {

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
  DT_THROW_BAD_UNIT("length", word);
  // return std::numeric_limits<double>::quiet_NaN();
}


double units::get_surface_unit_from(const std::string& word) {
  if ((word == "m2")) return CLHEP::meter2;
  if ((word == "cm2")) return CLHEP::centimeter2;
  if ((word == "mm2")) return CLHEP::millimeter2;
  if ((word == "km2")) return CLHEP::kilometer2;
  DT_THROW_BAD_UNIT("surface", word);
  // return std::numeric_limits<double>::quiet_NaN();
}


double units::get_volume_unit_from(const std::string& word) {
  if ((word == "m3")) return CLHEP::meter3;
  if ((word == "cm3")) return CLHEP::centimeter3;
  if ((word == "mm3")) return CLHEP::millimeter3;
  if ((word == "km3")) return CLHEP::kilometer3;
  DT_THROW_BAD_UNIT("volume", word);
  // return std::numeric_limits<double>::quiet_NaN();
}


double units::get_time_unit_from(const std::string& word) {
  if ((word == "fs") || (word == "femtosecond")) return 1.0e-3*CLHEP::picosecond;
  if ((word == "ps") || (word == "picosecond")) return CLHEP::picosecond;
  if ((word == "ns") || (word == "nanosecond")) return CLHEP::nanosecond;
  if ((word == "us") || (word == "microsecond")) return CLHEP::microsecond;
  if ((word == "ms") || (word == "millisecond")) return CLHEP::millisecond;
  if ((word == "s") || (word == "second")) return CLHEP::second;
  if ((word == "minute")) return 60 * CLHEP::second;
  if ((word == "h") || (word == "hour")) return 3600 * CLHEP::second;
  DT_THROW_BAD_UNIT("time", word);
  //return std::numeric_limits<double>::quiet_NaN();
}


double units::get_angle_unit_from(const std::string& word) {
  if ((word == "rad") || (word == "radian")) return CLHEP::radian;
  if ((word == "mrad") || (word == "milliradian")) return CLHEP::milliradian;
  if ((word == "deg") || (word == "degree")) return CLHEP::degree;
  DT_THROW_BAD_UNIT("angle", word);
  // return std::numeric_limits<double>::quiet_NaN();
}


double units::get_solid_angle_unit_from(const std::string& word) {
  if ((word == "steradian")) return CLHEP::steradian;
  DT_THROW_BAD_UNIT("solid angle", word);
  //return std::numeric_limits<double>::quiet_NaN();
}


double units::get_energy_unit_from(const std::string& word) {
  if ((word == "eV") || (word == "electronvolt")) return CLHEP::electronvolt;
  if ((word == "keV") || (word == "kiloelectronvolt")) return CLHEP::kiloelectronvolt;
  if ((word == "MeV") || (word == "megaelectronvolt")) return CLHEP::megaelectronvolt;
  if ((word == "GeV") || (word == "gigaelectronvolt")) return CLHEP::gigaelectronvolt;
  if ((word == "TeV") || (word == "teraelectronvolt")) return CLHEP::teraelectronvolt;
  if ((word == "PeV") || (word == "petaelectronvolt")) return CLHEP::petaelectronvolt;
  if ((word == "J") || (word == "joule")) return CLHEP::joule;
  DT_THROW_BAD_UNIT("energy", word);
  //return std::numeric_limits<double>::quiet_NaN();
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
  DT_THROW_BAD_UNIT("mass", word);
  //return std::numeric_limits<double>::quiet_NaN();
}


double units::get_pressure_unit_from(const std::string& word) {
  if ((word == "mbar") || (word == "millibar")) return 1.0e-3 * CLHEP::bar;
  if ((word == "bar")) return CLHEP::bar;
  if ((word == "atmosphere")) return CLHEP::atmosphere;
  if ((word == "Pa") || (word == "pascal")) return CLHEP::hep_pascal;
  DT_THROW_BAD_UNIT("pressure", word);
  return std::numeric_limits<double>::quiet_NaN();
}


double units::get_magnetic_field_unit_from(const std::string& word) {
  if ((word == "T") || (word == "tesla")) return CLHEP::tesla;
  if ((word == "G") || (word == "gauss")) return CLHEP::gauss;
  if ((word == "kG") || (word == "kilogauss")) return CLHEP::kilogauss;
  DT_THROW_BAD_UNIT("magnetic field", word);
  // return std::numeric_limits<double>::quiet_NaN();
}


double units::get_electric_field_unit_from(const std::string& word) {
  if ((word == "V/cm")) return CLHEP::volt / CLHEP::centimeter;
  if ((word == "V/m")) return CLHEP::volt / CLHEP::meter;
  if ((word == "kV/cm")) return CLHEP::kilovolt / CLHEP::centimeter;
  if ((word == "kV/m")) return CLHEP::kilovolt / CLHEP::meter;
  DT_THROW_BAD_UNIT("electric field", word);
  // return std::numeric_limits<double>::quiet_NaN();
}


double units::get_electric_tension_unit_from(const std::string& word) {
  if ((word == "uV")) return 1.e-6 * CLHEP::volt;
  if ((word == "mV")) return 1.e-3 * CLHEP::volt;
  if ((word == "V")) return CLHEP::volt;
  if ((word == "kV")) return CLHEP::kilovolt;
  if ((word == "MV")) return CLHEP::megavolt;
  DT_THROW_BAD_UNIT("electric tension", word);
  // return std::numeric_limits<double>::quiet_NaN();
}


double units::get_temperature_unit_from(const std::string& word) {
  if ((word == "kelvin")) return CLHEP::kelvin;
  DT_THROW_BAD_UNIT("temperature", word);
  // return std::numeric_limits<double>::quiet_NaN();
}


double units::get_density_unit_from(const std::string& word) {
  if ((word == "mg/cm3")) return CLHEP::milligram / CLHEP::cm3;
  if ((word == "g/cm3")) return CLHEP::gram / CLHEP::cm3;
  if ((word == "kg/m3")) return CLHEP::kg / CLHEP::m3;
  DT_THROW_BAD_UNIT("density", word);
  // return std::numeric_limits<double>::quiet_NaN();
}


double units::get_frequency_unit_from(const std::string& word) {
  if ((word == "Hz")) return CLHEP::hertz;
  if ((word == "mHz")) return 1.e-3 * CLHEP::hertz;
  if ((word == "kHz")) return CLHEP::kilohertz;
  if ((word == "MHz")) return CLHEP::megahertz;
  if ((word == "GHz")) return 1.e3 * CLHEP::megahertz;
  DT_THROW_BAD_UNIT("frequency", word);
  // return std::numeric_limits<double>::quiet_NaN();
}


double units::get_activity_unit_from(const std::string& word) {
  if ((word == "Bq"))  return CLHEP::becquerel;
  if ((word == "mBq")) return 1.e-3 * CLHEP::becquerel;
  if ((word == "uBq")) return 1.e-6 * CLHEP::becquerel;
  if ((word == "kBq")) return 1.e+3 * CLHEP::becquerel;
  if ((word == "MBq")) return 1.e+6 * CLHEP::becquerel;
  if ((word == "GBq")) return 1.e+9 * CLHEP::becquerel;
  if ((word == "Ci"))  return CLHEP::curie;
  if ((word == "kCi")) return 1.e3   * CLHEP::curie;
  if ((word == "MCi")) return 1.e6   * CLHEP::curie;
  if ((word == "mCi")) return 1.e-3  * CLHEP::curie;
  if ((word == "uCi")) return 1.e-6  * CLHEP::curie;
  if ((word == "nCi")) return 1.e-9  * CLHEP::curie;
  if ((word == "pCi")) return 1.e-12 * CLHEP::curie;
  if ((word == "dpm")) return CLHEP::becquerel / 60.0;
  DT_THROW_BAD_UNIT("activity", word);
  // return std::numeric_limits<double>::quiet_NaN();
}


double units::get_volume_activity_unit_from(const std::string& word) {
  double Bq_per_m3 = 1. * CLHEP::becquerel / CLHEP::m3;
  if ((word == "Bq/m3"))  return 1.    * Bq_per_m3;
  if ((word == "mBq/m3")) return 1.e-3 * Bq_per_m3;
  if ((word == "uBq/m3")) return 1.e-6 * Bq_per_m3;
  if ((word == "kBq/m3")) return 1.e+3 * Bq_per_m3;
  if ((word == "MBq/m3")) return 1.e+6 * Bq_per_m3;
  if ((word == "GBq/m3")) return 1.e+9 * Bq_per_m3;
  if ((word == "dpm/m3")) return CLHEP::becquerel / 60.0 * Bq_per_m3;
  DT_THROW_BAD_UNIT("volume activity", word);
  // return std::numeric_limits<double>::quiet_NaN();
}


double units::get_surface_activity_unit_from(const std::string& word) {
  double Bq_per_m2 = 1. * CLHEP::becquerel / CLHEP::m2;
  if ((word == "Bq/m2"))  return 1.    * Bq_per_m2;
  if ((word == "mBq/m2")) return 1.e-3 * Bq_per_m2;
  if ((word == "uBq/m2")) return 1.e-6 * Bq_per_m2;
  if ((word == "kBq/m2")) return 1.e+3 * Bq_per_m2;
  if ((word == "MBq/m2")) return 1.e+6 * Bq_per_m2;
  if ((word == "GBq/m2")) return 1.e+9 * Bq_per_m2;
  if ((word == "dpm/m2")) return CLHEP::becquerel / 60.0 * Bq_per_m2;
  DT_THROW_BAD_UNIT("surface activity", word);
  // return std::numeric_limits<double>::quiet_NaN();
}


double units::get_electric_charge_unit_from(const std::string& word) {
  if ((word == "C"))  return CLHEP::coulomb;
  if ((word == "nC")) return 1.e-9 * CLHEP::coulomb;
  if ((word == "pC")) return 1.e-12 * CLHEP::coulomb;
  DT_THROW_BAD_UNIT("electric charge", word);
  // return std::numeric_limits<double>::quiet_NaN();
}


double units::get_electric_current_unit_from(const std::string& word) {
  if ((word == "A"))  return CLHEP::ampere;
  if ((word == "mA")) return CLHEP::milliampere;
  if ((word == "uA")) return CLHEP::microampere;
  if ((word == "nA")) return CLHEP::nanoampere;
  DT_THROW_BAD_UNIT("electric current", word);
  // return std::numeric_limits<double>::quiet_NaN();
}


double units::get_mass_activity_unit_from(const std::string& word) {
  double Bq_per_kg = 1. * CLHEP::becquerel / CLHEP::kg;
  if ((word == "Bq/kg"))       return 1.    * Bq_per_kg;
  else if ((word == "mBq/kg")) return 1.e-3 * Bq_per_kg;
  else if ((word == "uBq/kg")) return 1.e-6 * Bq_per_kg;
  else if ((word == "kBq/kg")) return 1.e+3 * Bq_per_kg;
  else if ((word == "MBq/kg")) return 1.e+6 * Bq_per_kg;
  else if ((word == "GBq/kg")) return 1.e+9 * Bq_per_kg;
  if ((word == "dpm/kg")) return CLHEP::becquerel / 60.0 * Bq_per_kg;
  DT_THROW_BAD_UNIT("mass activity", word);
  // return std::numeric_limits<double>::quiet_NaN();
}


double units::get_speed_unit_from(const std::string& word) {
  if ((word == "km/s"))  return CLHEP::kilometer/CLHEP::second;
  if ((word == "km/h"))  return CLHEP::kilometer/(3600.0 * CLHEP::second);
  if ((word == "m/s"))   return CLHEP::meter/CLHEP::second;
  if ((word == "mm/s"))  return CLHEP::millimeter/CLHEP::second;
  if ((word == "cm/s"))  return CLHEP::centimeter/CLHEP::second;
  if ((word == "m/ms"))  return CLHEP::meter/CLHEP::millisecond;
  if ((word == "mm/ms")) return CLHEP::millimeter/CLHEP::millisecond;
  if ((word == "cm/ms")) return CLHEP::centimeter/CLHEP::millisecond;
  if ((word == "m/us"))  return CLHEP::meter/CLHEP::microsecond;
  if ((word == "mm/us")) return CLHEP::millimeter/CLHEP::microsecond;
  if ((word == "cm/us")) return CLHEP::centimeter/CLHEP::microsecond;
  if ((word == "m/ns"))  return CLHEP::meter/CLHEP::nanosecond;
  if ((word == "mm/ns")) return CLHEP::millimeter/CLHEP::nanosecond;
  if ((word == "cm/ns")) return CLHEP::centimeter/CLHEP::nanosecond;
  DT_THROW_BAD_UNIT("speed", word);
  // return std::numeric_limits<double>::quiet_NaN();
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
  } else if (unit_type == "electric_tension") {
    return get_electric_tension_unit_from(unit_str);
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
  } else if (unit_type == "electric_charge") {
    return get_electric_charge_unit_from(unit_str);
  } else if (unit_type == "electric_current") {
    return get_electric_current_unit_from(unit_str);
  } else if (unit_type == "speed") {
    return get_speed_unit_from(unit_str);
  }
  DT_THROW_BAD_UNIT(unit_type,unit_str);
  //return std::numeric_limits<double>::quiet_NaN();
}


bool units::is_unit_label_valid(const std::string & unit_label)
{
  const std::vector<std::string>& vl = get_unit_labels_registry();
  std::vector<std::string>::const_iterator found =  std::find(vl.begin(), vl.end(), unit_label);
  return found != vl.end();
}

std::string units::get_default_unit_symbol_from_label(const std::string & unit_type)
{
  if (unit_type == "length") {
    return "m";
  } else if (unit_type == "surface") {
    return "m2";
  } else if (unit_type == "volume") {
    return "m3";
  } else if (unit_type == "time") {
    return "s";
  } else if (unit_type == "angle") {
    return "rad";
  } else if (unit_type == "solid_angle") {
    return "steradian";
  } else if (unit_type == "energy") {
    return "J";
  } else if (unit_type == "mass") {
    return "kg";
  } else if (unit_type == "pressure") {
    return "Pa";
  } else if (unit_type == "magnetic_field") {
    return "T";
  } else if (unit_type == "electric_field") {
    return "V/cm";
  } else if (unit_type == "electric_tension") {
    return "V";
  } else if (unit_type == "temperature") {
    return "kelvin";
  } else if (unit_type == "density") {
    return "g/cm3";
  } else if (unit_type == "frequency") {
    return "Hz";
  } else if (unit_type == "activity") {
    return "Bq";
  } else if (unit_type == "surface_activity") {
    return "Bq/m2";
  } else if (unit_type == "volume_activity") {
    return "Bq/m3";
  } else if (unit_type == "mass_activity") {
    return "Bq/kg";
  } else if (unit_type == "electric_charge") {
    return "C";
  } else if (unit_type == "electric_current") {
    return "A";
  } else if (unit_type == "speed") {
    return "m/s";
  }
  DT_THROW_IF(true,std::logic_error,"Invalid unit label " << unit_type << "' !");
  //return "";
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
    ulabels.push_back("electric_field");
    ulabels.push_back("electric_tension");
    ulabels.push_back("temperature");
    ulabels.push_back("density");
    ulabels.push_back("activity");
    ulabels.push_back("surface_activity");
    ulabels.push_back("volume_activity");
    ulabels.push_back("mass_activity");
    ulabels.push_back("frequency");
    ulabels.push_back("electric_charge");
    ulabels.push_back("electric_current");
    ulabels.push_back("speed");
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
                                 std::string& unit_label,
                                 double default_unit_) {
  value = std::numeric_limits<double>::quiet_NaN();
  unit_label = "";
  double val;
  std::istringstream iss(word);
  iss >> val;
  if (!iss) {
    // std::ostringstream message;
    // message << "datatools::units::find_value_with_unit: Format error while reading a double value !";
    return false;
  }
  bool use_default_unit = true;
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
    use_default_unit = false;
  }
  if (datatools::is_valid(default_unit_) && use_default_unit) {
    val *= default_unit_;
  }
  value = val;
  return true;
}


double units::get_value_with_unit(const std::string& word) {
  double unit_value = std::numeric_limits<double>::quiet_NaN();
  std::string unit_label;
  DT_THROW_IF (!find_value_with_unit(word, unit_value, unit_label),
               std::logic_error,
               "Cannot parse a value with its units from '" << word << "' !");
  return unit_value;
}


} // end of namespace datatools
