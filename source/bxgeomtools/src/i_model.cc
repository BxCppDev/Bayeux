// -*- mode: c++ ; -*- 
/* i_model.cc
 */ 

#include <geomtools/i_model.h>

#include <stdexcept>
#include <iostream> 

#include <geomtools/sensitive.h>
#include <geomtools/material.h>
#include <geomtools/visibility.h>
#include <geomtools/mapping_utils.h>

namespace geomtools {
  
  using namespace std;  

  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION (i_model, "geomtools::i_model/__system__");
  
  bool i_model::g_devel = false;

  // static 
  const i_model::constants & 
  i_model::constants::instance ()
  {
    static boost::scoped_ptr<i_model::constants> g_global_constants (0);
    if ( g_global_constants.get () == 0)
      {
        g_global_constants.reset (new i_model::constants);
      }
    return *g_global_constants.get ();
  }

  i_model::constants::constants ()
  {
    SOLID_SUFFIX = ".solid";
    LOGICAL_SUFFIX = ".log";
    PHYSICAL_SUFFIX = ".phys";
    PHANTOM_SOLID_FLAG = "phantom_solid";
    return;
  }

  void i_model::assert_constructed (const string & where_, 
                                    const string & what_) const
  {
    if (is_constructed ()) return;
    ostringstream message;
    if (! where_.empty ())
      {
        message << where_ << ": ";
      }
    if (! what_.empty ())
      {
        message << what_;
      }
    else
      {
        message << "Operation not allowed ! Model has not been constructed yet";
      }
    message << " !";
    throw logic_error (message.str ());
  }

  void i_model::assert_unconstructed (const string & where_, 
                                      const string & what_) const
  {
    if (! is_constructed ()) return;
    ostringstream message;
    if (! where_.empty ())
      {
        message << where_ << ": ";
      }
    if (! what_.empty ())
      {
        message << what_;
      }
    else
      {
        message << "Operation not allowed ! Model has already been constructed";
      }
    message << " !";
    throw logic_error (message.str ());
  }

  string i_model::make_solid_name (const string & basename_)
  {
    ostringstream oss;
    oss << basename_ << i_model::constants::instance().SOLID_SUFFIX;
  }

  string i_model::make_logical_volume_name (const string & basename_)
  {
    ostringstream oss;
    oss << basename_ << i_model::constants::instance().LOGICAL_SUFFIX;
    return oss.str ();
  }
  
  string i_model::make_physical_volume_name (const string & basename_)
  {
    ostringstream oss;
    oss << basename_ << i_model::constants::instance().PHYSICAL_SUFFIX;
    return oss.str ();
  }

  string i_model::make_physical_volume_name (const string & basename_,
                                             int nitems_)
  {
    ostringstream oss;
    oss << basename_ << i_model::constants::instance().PHYSICAL_SUFFIX
        << "." << "__" << 0 << ".." << (nitems_ - 1) << "__";
    return oss.str ();
  }

  string i_model::make_physical_volume_name_per_item (const string & basename_,
                                                      int item_)
  {
    ostringstream oss;
    oss << basename_ << i_model::constants::instance().PHYSICAL_SUFFIX
        << "." << "__" << item_ << "__";
    return oss.str ();
  }

  string i_model::make_physical_volume_name_per_item (const string & basename_,
                                                      int i_, int j_)
  {
    ostringstream oss;
    oss << basename_ << i_model::constants::instance().PHYSICAL_SUFFIX
        << "." << "__" << i_ << "__"
        << "__" << j_ << "__";
    return oss.str ();
  }

  string i_model::make_physical_volume_name (const string & basename_,
                                             int ncols_,
                                             int nrows_)
  {
    ostringstream oss;
    oss << basename_ << i_model::constants::instance().PHYSICAL_SUFFIX
        << "." << "__" << 0 << ".." << (ncols_ - 1) << "__"
        << "__" << 0 << ".." << (nrows_ - 1) << "__";
    return oss.str ();
  }

  string i_model::extract_label_from_physical_volume_name (const string & physical_volume_name_)
  {
    size_t pos = physical_volume_name_.rfind (i_model::constants::instance().PHYSICAL_SUFFIX);
    if (pos == physical_volume_name_.npos)
      {
        ostringstream message;
        message << "geomtools::i_model::extract_label_from_physical_volume_name: "
                << "Do not recognize a physical volume name !";
        throw logic_error (message.str ());
      }
    return physical_volume_name_.substr (0, pos);
  }

  bool i_model::is_constructed () const
  {
    return _constructed_;
  }
 
  bool i_model::is_debug () const
  {
    return _debug_;
  }
  
  void i_model::set_debug (bool new_value_)
  {
    _debug_ = new_value_;
    return;
  }
   
  const string & i_model::get_name () const
  {
    return _name_;
  }
 
  void i_model::set_name (const string & name_)
  {
    _name_ = name_;
    return;
  }

  bool i_model::is_phantom_solid () const
  {
    return _phantom_solid;
  }

  void i_model::_set_phantom_solid (bool ps_)
  {
    _phantom_solid = ps_;
    return;
  }
  
  const datatools::properties & 
  i_model::parameters () const
  {
    return _parameters_;
  }

  datatools::properties & 
  i_model::parameters ()
  {
    return _parameters_;
  }

  // ctor:
  i_model::i_model (const string & name_)
  {
    _constructed_ = false;
    _debug_ = false;
    set_name (name_);
    _phantom_solid = false;
    return;
  }

  // dtor:
  i_model::~i_model ()
  {
    if (g_devel) clog << "DEVEL: i_model::~i_model: Entering..." << endl;
    _parameters_.clear ();
    if (g_devel) clog << "DEVEL: i_model::~i_model: Exiting." << endl;
    return;
  }
 
  const geomtools::logical_volume & i_model::get_logical () const
  {
    return _logical;
  }
 
  geomtools::logical_volume & i_model::grab_logical ()
  {
    return _logical;
  }
 
  geomtools::logical_volume & i_model::get_logical ()
  {
    return this->i_model::grab_logical ();
  }
 
  void i_model::_at_construct (const string & name_,
                               const datatools::properties & setup_,
                               models_col_type * models_)
  {
    if (g_devel) clog << "DEVEL: i_model::_at_construct: Entering..." << endl;

    throw logic_error ("geomtools::i_model::_at_construct: This method MUST be overloaded !");

    if (g_devel) clog << "DEVEL: i_model::_at_construct: Exiting." << endl;
    return;      
  }

  void i_model::_pre_construct (datatools::properties & setup_)
  {
    if (setup_.has_flag (i_model::constants::instance().PHANTOM_SOLID_FLAG))
      {
        _set_phantom_solid (true);
      }
    return;
  }

  void i_model::_post_construct (datatools::properties & setup_)
  {
    visibility::extract (setup_, get_logical ().parameters ());
    material::extract (setup_, get_logical ().parameters ());
    sensitive::extract (setup_, get_logical ().parameters ());
    mapping_utils::extract (setup_, get_logical ().parameters ());
    return;
  }

  void i_model::construct (const string & name_,
                           const datatools::properties & setup_,
                           models_col_type * models_)
  {
    if (g_devel) clog << "DEVEL: i_model::construct: Entering..." << endl;
    bool devel_track_name = false;

    if (setup_.has_flag ("geomtools::i_model::devel_track_name"))
      {
        devel_track_name = true;
      }
    if (devel_track_name) clog << "DEVEL: i_model::construct: Constructing name='" << name_ << "'..." << endl;
    if (_constructed_)
      {
        throw logic_error ("geomtools::i_model::construct: Already constructed !");
      }
    datatools::properties & setup = const_cast<datatools::properties &> (setup_);
    _pre_construct (setup);
    _at_construct (name_, setup_, models_);
    _post_construct (setup);
    _constructed_ = true;
    if (g_devel) clog << "DEVEL: geomtools::i_model::construct: Exiting." << endl;
    if (devel_track_name) clog << "DEVEL: geomtools::i_model::construct: Constructed name='" << name_ << "'." << endl;
    return;      
  }

  void i_model::tree_dump (ostream & out_, 
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

    out_ << indent << datatools::i_tree_dumpable::tag 
         << "Name        : \"" << _name_ << "\"" << endl;

    out_ << indent << datatools::i_tree_dumpable::tag 
         << "Model ID    : \"" << get_model_id () << "\"" << endl;
 
    /*
      out_ << indent << datatools::i_tree_dumpable::tag 
      << "Debug       : " << _debug_ << endl;
    */

    out_ << indent << datatools::i_tree_dumpable::tag 
         << "Constructed : " << _constructed_ << endl;
      
    {
      out_ << indent << datatools::i_tree_dumpable::tag 
           << "Parameters  : ";
      /*
        if ( _parameters_.size () == 0) 
        {
        out_ << "<empty>"; 
        }
      */
      out_ << endl; 
      {
        ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _parameters_.tree_dump (out_,"",indent_oss.str ());
      }
    }
      
    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_) 
           << "Logical  : " << endl;
      ostringstream indent_oss;
      indent_oss << indent;
      indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
      _logical.tree_dump (out_, "", indent_oss.str ());
    }

    return;
  }

} // end of namespace geomtools

// end of i_model.cc
