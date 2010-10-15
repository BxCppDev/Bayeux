// -*- mode: c++; -*- 
/* i_stackable.cc
 */
 
#include <geomtools/i_stackable.h>

namespace geomtools {

  using namespace std;

  const string stackable::STACKABLE_PREFIX = "stackable.";
  const string stackable::STACKABLE_LENGTH_UNIT_PROPERTY = "length_unit";
  const string stackable::STACKABLE_XMIN_PROPERTY = "xmin";
  const string stackable::STACKABLE_XMAX_PROPERTY = "xmax";
  const string stackable::STACKABLE_YMIN_PROPERTY = "ymin";
  const string stackable::STACKABLE_YMAX_PROPERTY = "ymax";
  const string stackable::STACKABLE_ZMIN_PROPERTY = "zmin";
  const string stackable::STACKABLE_ZMAX_PROPERTY = "zmax";
  const string stackable::STACKABLE_PLAY_PROPERTY = "play";
  const string stackable::STACKABLE_LIMITS_PROPERTY = "limits";

  string stackable::make_key (const string & key_)
  {
    ostringstream key_oss;
    key_oss << stackable::STACKABLE_PREFIX << key_;
    return key_oss.str ();
  }
  
  void stackable::extract (const datatools::utils::properties & source_,
			    datatools::utils::properties & target_)
  {
    source_.export_starting_with (target_, stackable::STACKABLE_PREFIX);
    return;
  }
 
  /***********************************************/

  bool stackable_data::initialize (const datatools::utils::properties & config_)
  {
    // datatools::utils::properties stackable_setup;
    //stackable::extract (config_, stackable_setup);

    double lunit = CLHEP::mm;
    
    string length_unit_key = stackable::make_key (stackable::STACKABLE_LENGTH_UNIT_PROPERTY);
    if (config_.has_key (length_unit_key))
      {
	string length_unit_str = config_.fetch_string (length_unit_key);
	lunit = datatools::utils::units::get_length_unit_from (length_unit_str);
      }

    string stackable_limits_key = stackable::make_key (stackable::STACKABLE_LIMITS_PROPERTY);
    if (config_.has_key (stackable_limits_key))
      {
	vector<double> limits;
	config_.fetch (stackable_limits_key, limits);
	if (limits.size () != 6)
	  {
	    ostringstream message;
	    message << "stackable_data::initialize: "
		    << "Stacking limits vector should provide 6 dimensions !";
	    throw runtime_error (message.str ());
	  }
	xmin = limits[0] *= lunit;
	ymin = limits[2] *= lunit;
	zmin = limits[4] *= lunit;
	xmax = limits[1] *= lunit;
	ymax = limits[3] *= lunit;
	zmax = limits[5] *= lunit;
      }
    else 
      {
	string xmin_key = stackable::make_key (stackable::STACKABLE_XMIN_PROPERTY);
	if (config_.has_key (xmin_key))
	  {
	    xmin = config_.fetch_real (xmin_key);
	    xmin *= lunit;
	  }
	
	string xmax_key = stackable::make_key (stackable::STACKABLE_XMAX_PROPERTY);
	if (config_.has_key (xmax_key))
	  {
	    xmax = config_.fetch_real (xmax_key);
	    xmax *= lunit;
	  }
	
	string ymin_key = stackable::make_key (stackable::STACKABLE_YMIN_PROPERTY);
	if (config_.has_key (ymin_key))
	  {
	    ymin = config_.fetch_real (ymin_key);
	    ymin *= lunit;
	  }
	
	string ymax_key = stackable::make_key (stackable::STACKABLE_YMAX_PROPERTY);
	if (config_.has_key (ymax_key))
	  {
	    ymax = config_.fetch_real (ymax_key);
	    ymax *= lunit;
	  }
	
	string zmin_key = stackable::make_key (stackable::STACKABLE_ZMIN_PROPERTY);
	if (config_.has_key (zmin_key))
	  {
	    zmin = config_.fetch_real (zmin_key);
	    zmin *= lunit;
	  }
	
	string zmax_key = stackable::make_key (stackable::STACKABLE_ZMAX_PROPERTY);
	if (config_.has_key (zmax_key))
	  {
	    zmax = config_.fetch_real (zmax_key);
	    zmax *= lunit;
	  }
      }

    return is_valid ();
  }

  bool stackable_data::is_valid () const
  {
    return 
      datatools::utils::is_valid (xmin)
      && datatools::utils::is_valid (xmax)
      && datatools::utils::is_valid (ymin)
      && datatools::utils::is_valid (ymax)
      && datatools::utils::is_valid (zmin)
      && datatools::utils::is_valid (zmax);
  }
  
  void stackable_data::invalidate ()
  {
    datatools::utils::invalidate (xmin);
    datatools::utils::invalidate (xmax);
    datatools::utils::invalidate (ymin);
    datatools::utils::invalidate (ymax);
    datatools::utils::invalidate (zmin);
    datatools::utils::invalidate (zmax);
    return;
  }

  stackable_data::stackable_data ()
  {
    invalidate ();
  }
   
  void stackable_data::dump (ostream & out_, const string & title_) const
  {
    if (! title_.empty ())
      {
	out_ << title_ << endl;
      }
    out_ << "|-- " << "xmin = " << xmin << endl;
    out_ << "|-- " << "xmax = " << xmax << endl;
    out_ << "|-- " << "ymin = " << ymin << endl;
    out_ << "|-- " << "ymax = " << ymax << endl;
    out_ << "|-- " << "zmin = " << zmin << endl;
    out_ << "`-- " << "zmax = " << zmax << endl;
    return;
  }

} // end of namespace geomtools

// end of i_stackable.cc
