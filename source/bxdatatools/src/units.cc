// -*- mode: c++ ; -*- 
/* units.cc
 */

#include <datatools/utils/units.h>
#include <datatools/utils/clhep_units.h>
#include <vector>

namespace datatools {

  namespace utils {

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
      if ((str_ == "inch")) return 2.54 * CLHEP::centimeter;
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
      if ((str_ == "mbar") || (str_ == "millibar")) return 1.0e-3 * CLHEP::bar;
      if ((str_ == "bar")) return CLHEP::bar;
      if ((str_ == "atmosphere")) return CLHEP::atmosphere;
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

    double units::get_activity_unit_from (const string & str_)
    {
      if ((str_ == "Bq")) return 1. / CLHEP::second;
      if ((str_ == "mBq")) return 1.e-3 / CLHEP::second;
      if ((str_ == "uBq")) return 1.e-6 / CLHEP::second;
      if ((str_ == "kBq")) return 1.e+3 / CLHEP::second;
      if ((str_ == "MBq")) return 1.e+6 / CLHEP::second;
      if ((str_ == "GBq")) return 1.e+9 / CLHEP::second;
      _throw_bad_unit ("activity", str_);
    }

    double units::get_volume_activity_unit_from (const string & str_)
    {
      double Bq_per_m3 =  CLHEP::becquerel / CLHEP::m3;
      if ((str_ == "Bq/m3"))  return 1.    * Bq_per_m3;
      if ((str_ == "mBq/m3")) return 1.e-3 * Bq_per_m3;
      if ((str_ == "uBq/m3")) return 1.e-6 * Bq_per_m3;
      if ((str_ == "kBq/m3")) return 1.e+3 * Bq_per_m3;
      if ((str_ == "MBq/m3")) return 1.e+6 * Bq_per_m3;
      if ((str_ == "GBq/m3")) return 1.e+9 * Bq_per_m3;
      _throw_bad_unit ("volume_activity", str_);
    }

    double units::get_surface_activity_unit_from (const string & str_)
    {
      double Bq_per_m2 = 1. / CLHEP::second / CLHEP::m2;
      if ((str_ == "Bq/m2"))  return 1.    * Bq_per_m2;
      if ((str_ == "mBq/m2")) return 1.e-3 * Bq_per_m2;
      if ((str_ == "uBq/m2")) return 1.e-6 * Bq_per_m2; 
      if ((str_ == "kBq/m2")) return 1.e+3 * Bq_per_m2;
      if ((str_ == "MBq/m2")) return 1.e+6 * Bq_per_m2;
      if ((str_ == "GBq/m2")) return 1.e+9 * Bq_per_m2;
      _throw_bad_unit ("surface_activity", str_);
    }

    double units::get_mass_activity_unit_from (const string & str_)
    {
      double Bq_per_kg = 1. / CLHEP::second / CLHEP::kg;
      if ((str_ == "Bq/kg"))  return 1.    * Bq_per_kg;
      if ((str_ == "mBq/kg")) return 1.e-3 * Bq_per_kg;
      if ((str_ == "uBq/kg")) return 1.e-6 * Bq_per_kg; 
      if ((str_ == "kBq/kg")) return 1.e+3 * Bq_per_kg;
      if ((str_ == "MBq/kg")) return 1.e+6 * Bq_per_kg;
      if ((str_ == "GBq/kg")) return 1.e+9 * Bq_per_kg;
      _throw_bad_unit ("mass_activity", str_);
    }

    double units::get_unit_from (const string & unit_type_, 
				 const string & unit_str_)
    {
      if (unit_type_ == "length")
	{
	  return get_length_unit_from (unit_str_);
	}
      else if (unit_type_ == "surface")
	{
	  return get_surface_unit_from (unit_str_);
	}
      else if (unit_type_ == "volume")
	{
	  return get_volume_unit_from (unit_str_);
	}
      else if (unit_type_ == "time")
	{
	  return get_time_unit_from (unit_str_);
	}
      else if (unit_type_ == "angle")
	{
	  return get_angle_unit_from (unit_str_);
	}
      else if (unit_type_ == "solid_angle")
	{
	  return get_solid_angle_unit_from (unit_str_);
	}
      else if (unit_type_ == "energy")
	{
	  return get_energy_unit_from (unit_str_);
	}
      else if (unit_type_ == "mass")
	{
	  return get_mass_unit_from (unit_str_);
	}
      else if (unit_type_ == "pressure")
	{
	  return get_pressure_unit_from (unit_str_);
	}
      else if (unit_type_ == "magnetic_field")
	{
	  return get_magnetic_field_unit_from (unit_str_);
	}
      else if (unit_type_ == "temperature")
	{
	  return get_temperature_unit_from (unit_str_);
	}
      else if (unit_type_ == "density")
	{
	  return get_density_unit_from (unit_str_);
	}
      else if (unit_type_ == "activity")
	{
	  return get_activity_unit_from (unit_str_);
	}
      else if (unit_type_ == "surface_activity")
	{
	  return get_surface_activity_unit_from (unit_str_);
	}
      else if (unit_type_ == "volume_activity")
	{
	  return get_volume_activity_unit_from (unit_str_);
	}
      else if (unit_type_ == "mass_activity")
	{
	  return get_mass_activity_unit_from (unit_str_);
	}
      ostringstream message;
      message << "Invalid " << unit_type_ << " of unit :'" << unit_type_ << "' !";
      throw runtime_error (message.str ());
    }
 
    bool units::find_unit (const string & unit_str_, 
			   double & unit_value_, 
			   string & unit_label_)
    {
      //clog << endl << "DEVEL: units::find_unit: Entering for '" << unit_str_ << "'" <<  endl;
      unit_label_ = "";
      unit_value_ = numeric_limits<double>::quiet_NaN ();
     
      static vector<string> ulabels;
      ulabels.reserve (20);
      ulabels.push_back ("length");
      ulabels.push_back ("surface");
      ulabels.push_back ("volume");
      ulabels.push_back ("time");
      ulabels.push_back ("angle");
      ulabels.push_back ("solid_angle");
      ulabels.push_back ("energy");
      ulabels.push_back ("mass");
      ulabels.push_back ("pressure");
      ulabels.push_back ("magnetic_field");
      ulabels.push_back ("temperature");
      ulabels.push_back ("density");
      ulabels.push_back ("activity");
      ulabels.push_back ("volume_activity");
      ulabels.push_back ("surface_activity");
      ulabels.push_back ("mass_activity");
      double val = numeric_limits<double>::quiet_NaN ();
      int count = -1;
      for (vector<string>::const_iterator i = ulabels.begin ();
	   i != ulabels.end ();
	   i++)
	{
	  //clog << "DEVEL: units::find_unit: Checking " << *i << " for '" << unit_str_ << "'" <<  endl;
	  try
	    {
	      count++;
	      unit_label_ = *i;
	      val = get_unit_from (*i, unit_str_);
	      //clog << "DEVEL: units::find_unit: Found " << *i << endl;
	      break;
	    }
	  catch (exception & x)
	    {
	      val = numeric_limits<double>::quiet_NaN ();
	    }
	}
      if (! isnormal (val))
	{
	  unit_label_ = "";
	  unit_value_ = val;
	  return false;
	}
      unit_label_ = ulabels[count];
      unit_value_ = val;
      return true;
    }

    double units::get_unit (const string & unit_str_)
    {
      double unit_val;
      string unit_label;
      bool res = find_unit (unit_str_, unit_val, unit_label);
      if (! res)
	{
	  return numeric_limits<double>::quiet_NaN ();
	}
      /*
      static list<string> ulabels;
      ulabels.push_back ("length");
      ulabels.push_back ("surface");
      ulabels.push_back ("volume");
      ulabels.push_back ("time");
      ulabels.push_back ("angle");
      ulabels.push_back ("solid_angle");
      ulabels.push_back ("energy");
      ulabels.push_back ("mass");
      ulabels.push_back ("pressure");
      ulabels.push_back ("magnetic_field");
      ulabels.push_back ("temperature");
      ulabels.push_back ("density");
      ulabels.push_back ("activity");
      ulabels.push_back ("volume_activity");
      ulabels.push_back ("surface_activity");
      ulabels.push_back ("mass_activity");

      double l = numeric_limits<double>::quiet_NaN ();
      for (list<string>::const_iterator i = ulabels.begin ();
	   i != ulabels.end ();
	   i++)
	{
	  try
	    {
	      l = get_unit_from (*i, unit_str_);
	      break;
	    }
	  catch (exception & x)
	    {
	    }
	}
      if (! isnormal (l))
	{
	  _throw_bad_unit ("type of", unit_str_);
	}
      
      return l;
      */
      return unit_val;
    }

    bool units::find_value_with_unit (const string & word_, 
				      double & value_, 
				      string & unit_label_)
    {
      value_ = numeric_limits<double>::quiet_NaN ();
      unit_label_ = "";

      double val;
      istringstream iss (word_);
      iss >> val;
      if (! iss)
	{
	  ostringstream message;
	  message << "get_value_with_unit: Format error while reading a double value !";
	  return false;
	}
      iss >> ws;
      if (! iss.eof ())
	{
	  string ustr;
	  iss >> ustr;
	  if (ustr.empty ())
	    {
	      return false;
	    }
	  double any_unit_value;
	  string any_unit_label;
	  bool res = find_unit (ustr, any_unit_value, any_unit_label);
	  if (! res)
	    {
	      return false;
	    }
	  val *= any_unit_value;
	  unit_label_ = any_unit_label;
	}
      value_ = val;
      return true;
    }

    double units::get_value_with_unit (const string & word_)
    {
      double unit_value = numeric_limits<double>::quiet_NaN ();
      string unit_label;
      if (! find_value_with_unit (word_, unit_value, unit_label))
	{
	  ostringstream message;
	  message << "get_value_with_unit: Cannot parse a value with its units !";
	  throw logic_error (message.str ());
	}
      return unit_value;
      /*
     double val = numeric_limits<double>::quiet_NaN ();

      istringstream iss (word_);
      iss >> val;
      if (! iss)
	{
	  ostringstream message;
	  message << "get_value_with_unit: Format error while reading a double value !";
	  throw runtime_error (message.str ());
	}
      iss >> ws;
      if (! iss.eof ())
	{
	  string ustr;
	  iss >> ustr;
	  try
	    {
	      double any_unit = get_unit (ustr);
	      val *= any_unit;
	    }
	  catch (exception & x)
	    {
	      ostringstream message;
	      message << "get_value_with_unit: Unknown unit: " << x.what ();
	      throw runtime_error (message.str ());
	    } 
	}
      
      return val;
      */
    }

  } // end of namespace utils

} // end of namespace datatools

// end of units.cc
