// -*- mode: c++ ; -*-
/* i_model.cc
 */

#include <geomtools/i_model.h>

#include <stdexcept>
#include <iostream>

#include <datatools/exception.h>

#include <geomtools/sensitive.h>
#include <geomtools/material.h>
#include <geomtools/visibility.h>
#include <geomtools/mapping_utils.h>

namespace geomtools {

  using namespace std;

  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION (i_model, "geomtools::i_model/__system__");


  // static
  const i_model::constants &
  i_model::constants::instance ()
  {
    static boost::scoped_ptr<i_model::constants> g_global_constants (0);
    if ( g_global_constants.get () == 0) {
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

  datatools::logger::priority i_model::get_logging_priority() const
  {
    return _logging_priority;
  }

  void i_model::set_logging_priority(datatools::logger::priority p_)
  {
    _logging_priority = p_;
  }


  void i_model::assert_constructed (const string & where_,
                                    const string & what_) const
  {
    if (is_constructed ()) return;
    ostringstream message;
    if (! where_.empty ()) {
      message << where_ << ": ";
    }
    if (! what_.empty ()) {
      message << what_;
    } else {
      message << "Operation not allowed ! Model has not been constructed yet";
    }
    message << " !";
    DT_THROW_IF(true, logic_error, message.str ());
  }

  void i_model::assert_unconstructed (const string & where_,
                                      const string & what_) const
  {
    if (! is_constructed ()) return;
    ostringstream message;
    if (! where_.empty ()) {
      message << where_ << ": ";
    }
    if (! what_.empty ()) {
      message << what_;
    } else {
      message << "Operation not allowed ! Model has already been constructed";
    }
    message << " !";
    DT_THROW_IF(true, logic_error, message.str ());
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
    DT_THROW_IF(pos == physical_volume_name_.npos,
                logic_error,
                "Do not recognize a physical volume name from '"
                << physical_volume_name_ << "' !");
    return physical_volume_name_.substr (0, pos);
  }

  bool i_model::is_constructed () const
  {
    return _constructed_;
  }

  bool i_model::is_debug () const
  {
    return _logging_priority >= datatools::logger::PRIO_DEBUG;
  }

  void i_model::set_debug (bool new_value_)
  {
    if (new_value_) {
      _logging_priority = datatools::logger::PRIO_DEBUG;
    } else {
      _logging_priority = datatools::logger::PRIO_WARNING;
    }
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
    _logging_priority = datatools::logger::PRIO_WARNING;
    set_name (name_);
    _phantom_solid = false;
    return;
  }

  // dtor:
  i_model::~i_model ()
  {
    _parameters_.clear ();
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
    DT_THROW_IF(true,runtime_error, "This method MUST be overloaded !");
    return;
  }

  void i_model::_pre_construct (datatools::properties & setup_)
  {
    if (setup_.has_key("logging.priority")) {
      std::string lp_label = setup_.fetch_string("logging.priority");
      datatools::logger::priority lp = datatools::logger::get_priority(lp_label);
      DT_THROW_IF(lp ==  datatools::logger::PRIO_UNDEFINED,
                  std::logic_error,
                  "Invalid logging priority '" << lp_label << "' !");
      set_logging_priority(lp);
    }

    if (setup_.has_flag (i_model::constants::instance().PHANTOM_SOLID_FLAG)) {
      _set_phantom_solid (true);
    }

    return;
  }

  void i_model::_post_construct (datatools::properties & setup_)
  {
    /*
    visibility::extract (setup_, get_logical ().parameters ());
    material::extract (setup_, get_logical ().parameters ());
    sensitive::extract (setup_, get_logical ().parameters ());
    mapping_utils::extract (setup_, get_logical ().parameters ());
    */
    return;
  }

  void i_model::construct (const string & name_,
                           const datatools::properties & setup_,
                           models_col_type * models_)
  {
    bool devel_track_name = false;

    DT_THROW_IF (_constructed_,
                 logic_error,
                 "Model '" << name_ << "' has been already constructed !");
    datatools::properties & setup = const_cast<datatools::properties &> (setup_);
    _pre_construct (setup);
    _at_construct (name_, setup_, models_);
    _post_construct (setup);
    _constructed_ = true;
    return;
  }

  void i_model::tree_dump (ostream & out_,
                           const string & title_,
                           const string & indent_,
                           bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    if (! title_.empty ()) {
      out_ << indent << title_ << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Name        : \"" << _name_ << "\"" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Model ID    : \"" << get_model_id () << "\"" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Logging priority threshold       : \""
         << datatools::logger::get_priority_label(_logging_priority) << "\"" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Constructed : " << _constructed_ << std::endl;

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Parameters  : ";
      out_ << std::endl;
      {
        ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _parameters_.tree_dump (out_,"",indent_oss.str ());
      }
    }

    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
           << "Logical  : " << std::endl;
      ostringstream indent_oss;
      indent_oss << indent;
      indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (inherit_);
      _logical.tree_dump (out_, "", indent_oss.str ());
    }

    return;
  }

} // end of namespace geomtools

// end of i_model.cc
