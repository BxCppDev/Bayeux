// -*- mode: c++; -*- 
/* i_stackable.cc
 */
 
#include <geomtools/i_stackable.h>
#include <sstream>
#include <stdexcept>

#include <datatools/clhep_units.h>
#include <datatools/units.h>

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
 
  bool i_stackable::has_xmin() const
  {
    return datatools::is_valid(get_xmin ());
  }

  bool i_stackable::has_xmax() const
  {
    return datatools::is_valid(get_xmax ());
  }

  bool i_stackable::has_ymin() const
  {
    return datatools::is_valid(get_ymin ());
  }

  bool i_stackable::has_ymax() const
  {
    return datatools::is_valid(get_ymax ());
  }

  bool i_stackable::has_zmin() const
  {
    return datatools::is_valid(get_zmin ());
  }

  bool i_stackable::has_zmax() const
  {
    return datatools::is_valid(get_zmax ());
  }
  
  string stackable::make_key (const string & key_)
  {
    ostringstream key_oss;
    key_oss << stackable::STACKABLE_PREFIX << key_;
    return key_oss.str ();
  }
  
  void stackable::extract (const datatools::properties & source_,
                           datatools::properties & target_)
  {
    source_.export_starting_with (target_, stackable::STACKABLE_PREFIX);
    return;
  }

  bool stackable::has_xmin (const datatools::properties & source_)
  {
    return source_.has_key (stackable::make_key (stackable::STACKABLE_XMIN_PROPERTY));
  }
  
  bool stackable::has_xmax (const datatools::properties & source_)
  {
    return source_.has_key (stackable::make_key (stackable::STACKABLE_XMAX_PROPERTY));
  }
  
  bool stackable::has_ymin (const datatools::properties & source_)
  {
    return source_.has_key (stackable::make_key (stackable::STACKABLE_YMIN_PROPERTY));
  }
  
  bool stackable::has_ymax (const datatools::properties & source_)
  {
    return source_.has_key (stackable::make_key (stackable::STACKABLE_YMAX_PROPERTY));
  }
  
  bool stackable::has_zmin (const datatools::properties & source_)
  {
    return source_.has_key (stackable::make_key (stackable::STACKABLE_ZMIN_PROPERTY));
  }
  
  bool stackable::has_zmax (const datatools::properties & source_)
  {
    return source_.has_key (stackable::make_key (stackable::STACKABLE_ZMAX_PROPERTY));
  }

  /****************************/

  double stackable::get_xmin (const datatools::properties & source_)
  {
    return source_.fetch_real (stackable::make_key (stackable::STACKABLE_XMIN_PROPERTY));
  }
  
  double stackable::get_xmax (const datatools::properties & source_)
  {
    return source_.fetch_real (stackable::make_key (stackable::STACKABLE_XMAX_PROPERTY));
  }
  
  double stackable::get_ymin (const datatools::properties & source_)
  {
    return source_.fetch_real (stackable::make_key (stackable::STACKABLE_YMIN_PROPERTY));
  }
  
  double stackable::get_ymax (const datatools::properties & source_)
  {
    return source_.fetch_real (stackable::make_key (stackable::STACKABLE_YMAX_PROPERTY));
  }
  
  double stackable::get_zmin (const datatools::properties & source_)
  {
    return source_.fetch_real (stackable::make_key (stackable::STACKABLE_ZMIN_PROPERTY));
  }
  
  double stackable::get_zmax (const datatools::properties & source_)
  {
    return source_.fetch_real (stackable::make_key (stackable::STACKABLE_ZMAX_PROPERTY));
  }

  /****************************/

  void stackable::set_xmin (datatools::properties & target_,
                            double xmin_)
  {
    target_.store (stackable::make_key (stackable::STACKABLE_XMIN_PROPERTY),
                   xmin_);
    return;
  }

  void stackable::set_xmax (datatools::properties & target_,
                            double xmax_)
  {
    target_.store (stackable::make_key (stackable::STACKABLE_XMAX_PROPERTY),
                   xmax_);
    return;
  }

  void stackable::set_ymin (datatools::properties & target_,
                            double ymin_)
  {
    target_.store (stackable::make_key (stackable::STACKABLE_YMIN_PROPERTY),
                   ymin_);
    return;
  }

  void stackable::set_ymax (datatools::properties & target_,
                            double ymax_)
  {
    target_.store (stackable::make_key (stackable::STACKABLE_YMAX_PROPERTY),
                   ymax_);
    return;
  }

  void stackable::set_zmin (datatools::properties & target_,
                            double zmin_)
  {
    target_.store (stackable::make_key (stackable::STACKABLE_ZMIN_PROPERTY),
                   zmin_);
    return;
  }

  void stackable::set_zmax (datatools::properties & target_,
                            double zmax_)
  {
    target_.store (stackable::make_key (stackable::STACKABLE_ZMAX_PROPERTY),
                   zmax_);
    return;
  }

  /****************************/

  void stackable::unset_xmin (datatools::properties & target_)
  {
    target_.clean (stackable::make_key (stackable::STACKABLE_XMIN_PROPERTY));
    return;
  }

  void stackable::unset_xmax (datatools::properties & target_)
  {
    target_.clean (stackable::make_key (stackable::STACKABLE_XMAX_PROPERTY));
    return;
  }

  void stackable::unset_ymin (datatools::properties & target_)
  {
    target_.clean (stackable::make_key (stackable::STACKABLE_YMIN_PROPERTY));
    return;
  }

  void stackable::unset_ymax (datatools::properties & target_)
  {
    target_.clean (stackable::make_key (stackable::STACKABLE_YMAX_PROPERTY));
    return;
  }

  void stackable::unset_zmin (datatools::properties & target_)
  {
    target_.clean (stackable::make_key (stackable::STACKABLE_ZMIN_PROPERTY));
    return;
  }

  void stackable::unset_zmax (datatools::properties & target_)
  {
    target_.clean (stackable::make_key (stackable::STACKABLE_ZMAX_PROPERTY));
    return;
  }

  void stackable::unset (datatools::properties & target_)
  {
    unset_xmin (target_);
    unset_xmax (target_);
    unset_ymin (target_);
    unset_xmax (target_);
    unset_zmin (target_);
    unset_zmax (target_);
    return;
  }
  
  void stackable::set (datatools::properties & target_, 
                       const i_stackable & sd_)
  {
    set_xmin (target_, sd_.get_xmin ());
    set_xmax (target_, sd_.get_xmax ());
    set_ymin (target_, sd_.get_ymin ());
    set_xmax (target_, sd_.get_ymax ());
    set_zmin (target_, sd_.get_zmin ());
    set_zmax (target_, sd_.get_zmax ()); 
    return;
  }
 
  /***********************************************/

  bool stackable_data::initialize (const datatools::properties & config_)
  {
    double lunit = CLHEP::mm;
    
    string length_unit_key = stackable::make_key (stackable::STACKABLE_LENGTH_UNIT_PROPERTY);
    if (config_.has_key (length_unit_key))
      {
        string length_unit_str = config_.fetch_string (length_unit_key);
        lunit = datatools::units::get_length_unit_from (length_unit_str);
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
            throw logic_error (message.str ());
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

  bool stackable_data::is_valid_x () const
  {
    return 
      datatools::is_valid (xmin)
      && datatools::is_valid (xmax);
  }
  
  bool stackable_data::is_valid_y () const
  {
    return 
      datatools::is_valid (ymin)
      && datatools::is_valid (ymax);
  }
  
  bool stackable_data::is_valid_z () const
  {
    return 
      datatools::is_valid (zmin)
      && datatools::is_valid (zmax);
  }

  bool stackable_data::is_valid () const
  {
    return is_valid_x () && is_valid_y () && is_valid_z ();
  }

  bool stackable_data::is_valid_weak () const
  {
    return is_valid_x () || is_valid_y () || is_valid_z ();
  }
  
  void stackable_data::invalidate ()
  {
    datatools::invalidate (xmin);
    datatools::invalidate (xmax);
    datatools::invalidate (ymin);
    datatools::invalidate (ymax);
    datatools::invalidate (zmin);
    datatools::invalidate (zmax);
    return;
  }

  stackable_data::stackable_data ()
  {
    invalidate ();
  }
 
  void stackable_data::tree_dump (ostream & out_, 
                                  const string & title_, 
                                  const string & indent_, 
                                  bool inherit_) const
  { 
    string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ()) 
      {
        out_ << indent << title_ << endl;
      }
    out_ << indent << datatools::i_tree_dumpable::tag << "xmin = " << xmin << endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "xmax = " << xmax << endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "ymin = " << ymin << endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "ymax = " << ymax << endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "zmin = " << zmin << endl;
    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "zmax = " << zmax << endl;
    return;
  }
  
  void stackable_data::dump (ostream & out_) const
  {
    tree_dump (out_, "geomtools::stackable_data: ");

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
