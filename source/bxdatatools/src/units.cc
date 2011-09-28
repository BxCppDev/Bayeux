// -*- mode: c++ ; -*- 
/* units.cc
 */

#include <datatools/utils/units.h>
#include <datatools/utils/clhep_units.h>
#include <vector>
#include <limits>

namespace datatools {

  namespace utils {

    using namespace std;

    void units::throw_bad_unit_ (const string & a_type, const string & a_unit_str)
    {
      ostringstream message;
      message << "Invalid " << a_type << " unit :'" << a_unit_str << "' !";
      throw runtime_error (message.str ());
    }
  
    double units::get_length_unit_from (const string & a_word)
    {
      if ((a_word == "angstrom")) return CLHEP::angstrom;
      if ((a_word == "fm") || (a_word == "fermi")) return CLHEP::fermi;
      if ((a_word == "nm") || (a_word == "nanometer")) return CLHEP::nanometer;
      if ((a_word == "um") || (a_word == "micrometer")) return CLHEP::micrometer;
      if ((a_word == "mm") || (a_word == "millimeter")) return CLHEP::millimeter;
      if ((a_word == "cm") || (a_word == "centimeter")) return CLHEP::centimeter;
      if ((a_word == "m") || (a_word == "meter")) return CLHEP::meter;
      if ((a_word == "km") || (a_word == "kilometer")) return CLHEP::kilometer;
      if ((a_word == "pc") || (a_word == "parsec")) return CLHEP::parsec;
      if ((a_word == "inch")) return 2.54 * CLHEP::centimeter;
      throw_bad_unit_ ("length", a_word);
      return numeric_limits<double>::quiet_NaN ();
    }

    double units::get_surface_unit_from (const string & a_word)
    {
      if ((a_word == "m2")) return CLHEP::meter2;
      if ((a_word == "cm2")) return CLHEP::centimeter2;
      if ((a_word == "mm2")) return CLHEP::millimeter2;
      if ((a_word == "km2")) return CLHEP::kilometer2;
      throw_bad_unit_ ("surface", a_word);
      return numeric_limits<double>::quiet_NaN ();
    }

    double units::get_volume_unit_from (const string & a_word)
    {
      if ((a_word == "m3")) return CLHEP::meter3;
      if ((a_word == "cm3")) return CLHEP::centimeter3;
      if ((a_word == "mm3")) return CLHEP::millimeter3;
      if ((a_word == "km3")) return CLHEP::kilometer3;
      throw_bad_unit_ ("volume", a_word);
      return numeric_limits<double>::quiet_NaN ();
    }

    double units::get_time_unit_from (const string & a_word)
    {
      if ((a_word == "ps") || (a_word == "picosecond")) return CLHEP::picosecond;
      if ((a_word == "ns") || (a_word == "nanosecond")) return CLHEP::nanosecond;
      if ((a_word == "us") || (a_word == "microsecond")) return CLHEP::microsecond;
      if ((a_word == "ms") || (a_word == "millisecond")) return CLHEP::millisecond;
      if ((a_word == "s") || (a_word == "second")) return CLHEP::second;
      if ((a_word == "minute")) return 60 * CLHEP::second;
      if ((a_word == "h") || (a_word == "hour")) return 3600 * CLHEP::second;
      throw_bad_unit_ ("time", a_word);
      return numeric_limits<double>::quiet_NaN ();
    }

    double units::get_angle_unit_from (const string & a_word)
    {
      if ((a_word == "rad") || (a_word == "radian")) return CLHEP::radian;
      if ((a_word == "mrad") || (a_word == "milliradian")) return CLHEP::milliradian;
      if ((a_word == "deg") || (a_word == "degree")) return CLHEP::degree;
      throw_bad_unit_ ("angle", a_word);
      return numeric_limits<double>::quiet_NaN ();
    }

    double units::get_solid_angle_unit_from (const string & a_word)
    {
      if ((a_word == "steradian")) return CLHEP::steradian;
      throw_bad_unit_ ("solid angle", a_word);
      return numeric_limits<double>::quiet_NaN ();
    }

    double units::get_energy_unit_from (const string & a_word)
    {
      if ((a_word == "eV") || (a_word == "electronvolt")) return CLHEP::electronvolt;
      if ((a_word == "keV") || (a_word == "kiloelectronvolt")) return CLHEP::kiloelectronvolt;
      if ((a_word == "MeV") || (a_word == "megaelectronvolt")) return CLHEP::megaelectronvolt;
      if ((a_word == "GeV") || (a_word == "gigaelectronvolt")) return CLHEP::gigaelectronvolt;
      if ((a_word == "TeV") || (a_word == "teraelectronvolt")) return CLHEP::teraelectronvolt;
      if ((a_word == "PeV") || (a_word == "petaelectronvolt")) return CLHEP::petaelectronvolt;
      if ((a_word == "J") || (a_word == "joule")) return CLHEP::joule;
      throw_bad_unit_ ("energy", a_word);
      return numeric_limits<double>::quiet_NaN ();
    }

    double units::get_mass_unit_from (const string & a_word)
    {
      if ((a_word == "eV/c2")) return CLHEP::eV / CLHEP::c_squared;
      if ((a_word == "MeV/c2")) return CLHEP::MeV / CLHEP::c_squared;
      if ((a_word == "GeV/c2")) return CLHEP::GeV / CLHEP::c_squared;
      if ((a_word == "kg") || (a_word == "kilogram")) return CLHEP::kilogram;
      if ((a_word == "g") || (a_word == "gram")) return CLHEP::gram;
      if ((a_word == "mg") || (a_word == "milligram")) return CLHEP::milligram;
      if ((a_word == "ug") || (a_word == "microgram")) return 1.e-3 * CLHEP::milligram;
      if ((a_word == "t") || (a_word == "ton")) return 1000. * CLHEP::kilogram;
      throw_bad_unit_ ("mass", a_word);
      return numeric_limits<double>::quiet_NaN ();
    }

    double units::get_pressure_unit_from (const string & a_word)
    {
      if ((a_word == "mbar") || (a_word == "millibar")) return 1.0e-3 * CLHEP::bar;
      if ((a_word == "bar")) return CLHEP::bar;
      if ((a_word == "atmosphere")) return CLHEP::atmosphere;
      if ((a_word == "Pa") || (a_word == "pascal")) return CLHEP::hep_pascal;
      throw_bad_unit_ ("pressure", a_word);
      return numeric_limits<double>::quiet_NaN ();
    }

    double units::get_magnetic_field_unit_from (const string & a_word)
    {
      if ((a_word == "T") || (a_word == "tesla")) return CLHEP::tesla;
      if ((a_word == "G") || (a_word == "gauss")) return CLHEP::gauss;
      if ((a_word == "kG") || (a_word == "kilogauss")) return CLHEP::kilogauss;
      throw_bad_unit_ ("magnetic field", a_word);
      return numeric_limits<double>::quiet_NaN ();
    }

    double units::get_temperature_unit_from (const string & a_word)
    {
      if ((a_word == "kelvin")) return CLHEP::kelvin;
      throw_bad_unit_ ("temperature", a_word);
      return numeric_limits<double>::quiet_NaN ();
    }

    double units::get_density_unit_from (const string & a_word)
    {
      if ((a_word == "mg/cm3")) return CLHEP::milligram / CLHEP::cm3;
      if ((a_word == "g/cm3")) return CLHEP::gram / CLHEP::cm3;
      if ((a_word == "kg/m3")) return CLHEP::kg / CLHEP::m3;
      throw_bad_unit_ ("density", a_word);
      return numeric_limits<double>::quiet_NaN ();
    }

    double units::get_activity_unit_from (const string & a_word)
    {
      if ((a_word == "Bq")) return 1. / CLHEP::second;
      if ((a_word == "mBq")) return 1.e-3 / CLHEP::second;
      if ((a_word == "uBq")) return 1.e-6 / CLHEP::second;
      if ((a_word == "kBq")) return 1.e+3 / CLHEP::second;
      if ((a_word == "MBq")) return 1.e+6 / CLHEP::second;
      if ((a_word == "GBq")) return 1.e+9 / CLHEP::second;
      throw_bad_unit_ ("activity", a_word);
      return numeric_limits<double>::quiet_NaN ();
    }

    double units::get_volume_activity_unit_from (const string & a_word)
    {
      double Bq_per_m3 =  CLHEP::becquerel / CLHEP::m3;
      if ((a_word == "Bq/m3"))  return 1.    * Bq_per_m3;
      if ((a_word == "mBq/m3")) return 1.e-3 * Bq_per_m3;
      if ((a_word == "uBq/m3")) return 1.e-6 * Bq_per_m3;
      if ((a_word == "kBq/m3")) return 1.e+3 * Bq_per_m3;
      if ((a_word == "MBq/m3")) return 1.e+6 * Bq_per_m3;
      if ((a_word == "GBq/m3")) return 1.e+9 * Bq_per_m3;
      throw_bad_unit_ ("volume_activity", a_word);
      return numeric_limits<double>::quiet_NaN ();
    }

    double units::get_surface_activity_unit_from (const string & a_word)
    {
      double Bq_per_m2 = 1. / CLHEP::second / CLHEP::m2;
      if ((a_word == "Bq/m2"))  return 1.    * Bq_per_m2;
      if ((a_word == "mBq/m2")) return 1.e-3 * Bq_per_m2;
      if ((a_word == "uBq/m2")) return 1.e-6 * Bq_per_m2; 
      if ((a_word == "kBq/m2")) return 1.e+3 * Bq_per_m2;
      if ((a_word == "MBq/m2")) return 1.e+6 * Bq_per_m2;
      if ((a_word == "GBq/m2")) return 1.e+9 * Bq_per_m2;
      throw_bad_unit_ ("surface_activity", a_word);
      return numeric_limits<double>::quiet_NaN ();
    }

    double units::get_mass_activity_unit_from (const string & a_word)
    {
      double Bq_per_kg = 1. / CLHEP::second / CLHEP::kg;
      if ((a_word == "Bq/kg"))  return 1.    * Bq_per_kg;
      else if ((a_word == "mBq/kg")) return 1.e-3 * Bq_per_kg;
      else if ((a_word == "uBq/kg")) return 1.e-6 * Bq_per_kg; 
      else if ((a_word == "kBq/kg")) return 1.e+3 * Bq_per_kg;
      else if ((a_word == "MBq/kg")) return 1.e+6 * Bq_per_kg;
      else if ((a_word == "GBq/kg")) return 1.e+9 * Bq_per_kg;
      else throw_bad_unit_ ("mass_activity", a_word);
      return numeric_limits<double>::quiet_NaN ();
    }

    double units::get_unit_from (const string & a_unit_type, 
				 const string & a_unit_str)
    {
      if (a_unit_type == "length")
	{
	  return get_length_unit_from (a_unit_str);
	}
      else if (a_unit_type == "surface")
	{
	  return get_surface_unit_from (a_unit_str);
	}
      else if (a_unit_type == "volume")
	{
	  return get_volume_unit_from (a_unit_str);
	}
      else if (a_unit_type == "time")
	{
	  return get_time_unit_from (a_unit_str);
	}
      else if (a_unit_type == "angle")
	{
	  return get_angle_unit_from (a_unit_str);
	}
      else if (a_unit_type == "solid_angle")
	{
	  return get_solid_angle_unit_from (a_unit_str);
	}
      else if (a_unit_type == "energy")
	{
	  return get_energy_unit_from (a_unit_str);
	}
      else if (a_unit_type == "mass")
	{
	  return get_mass_unit_from (a_unit_str);
	}
      else if (a_unit_type == "pressure")
	{
	  return get_pressure_unit_from (a_unit_str);
	}
      else if (a_unit_type == "magnetic_field")
	{
	  return get_magnetic_field_unit_from (a_unit_str);
	}
      else if (a_unit_type == "temperature")
	{
	  return get_temperature_unit_from (a_unit_str);
	}
      else if (a_unit_type == "density")
	{
	  return get_density_unit_from (a_unit_str);
	}
      else if (a_unit_type == "activity")
	{
	  return get_activity_unit_from (a_unit_str);
	}
      else if (a_unit_type == "surface_activity")
	{
	  return get_surface_activity_unit_from (a_unit_str);
	}
      else if (a_unit_type == "volume_activity")
	{
	  return get_volume_activity_unit_from (a_unit_str);
	}
      else if (a_unit_type == "mass_activity")
	{
	  return get_mass_activity_unit_from (a_unit_str);
	}
      ostringstream message;
      message << "Invalid " << a_unit_type << " of unit :'" << a_unit_type << "' !";
      throw runtime_error (message.str ());
      return numeric_limits<double>::quiet_NaN ();
    }
 
    bool units::find_unit (const string & a_unit_str, 
			   double & a_unit_value, 
			   string & a_unit_label)
    {
      //clog << endl << "DEVEL: units::find_unit: Entering for '" << a_unit_str << "'" <<  endl;
      a_unit_label = "";
      a_unit_value = numeric_limits<double>::quiet_NaN ();
     
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
	  //clog << "DEVEL: units::find_unit: Checking " << *i << " for '" << a_unit_str << "'" <<  endl;
	  try
	    {
	      count++;
	      a_unit_label = *i;
	      val = get_unit_from (*i, a_unit_str);
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
	  a_unit_label = "";
	  a_unit_value = val;
	  return false;
	}
      a_unit_label = ulabels[count];
      a_unit_value = val;
      return true;
    }

    double units::get_unit (const string & a_unit_str)
    {
      double unit_val;
      string unit_label;
      bool res = find_unit (a_unit_str, unit_val, unit_label);
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
	      l = get_unit_from (*i, a_unit_str);
	      break;
	    }
	  catch (exception & x)
	    {
	    }
	}
      if (! isnormal (l))
	{
	  throw_bad_unit_ ("type of", a_unit_str);
	}
      
      return l;
      */
      return unit_val;
    }

    bool units::find_value_with_unit (const string & a_word, 
				      double & a_value, 
				      string & a_unit_label)
    {
      a_value = numeric_limits<double>::quiet_NaN ();
      a_unit_label = "";

      double val;
      istringstream iss (a_word);
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
	  a_unit_label = any_unit_label;
	}
      a_value = val;
      return true;
    }

    double units::get_value_with_unit (const string & a_word)
    {
      double unit_value = numeric_limits<double>::quiet_NaN ();
      string unit_label;
      if (! find_value_with_unit (a_word, unit_value, unit_label))
	{
	  ostringstream message;
	  message << "get_value_with_unit: Cannot parse a value with its units !";
	  throw logic_error (message.str ());
	}
      return unit_value;
      /*
     double val = numeric_limits<double>::quiet_NaN ();

      istringstream iss (a_word);
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
