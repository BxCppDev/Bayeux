// -*- mode: c++ ; -*-
/* base_cut.cc
 */

#include <dpp/base_cut.h>

#include <stdexcept>
#include <sstream>

#include <datatools/properties.h>
#include <datatools/things.h>

namespace dpp {

  // ctor:
  base_cut::base_cut (const std::string & a_cut_name,
                      const std::string & a_cut_description,
                      const std::string & a_cut_version,
                      int a_debug_level) :
    cuts::i_cut (a_cut_name,
                 a_cut_description,
                 a_cut_version,
                 a_debug_level)
  {
    return;
  }

  // dtor:
  CUT_DEFAULT_DESTRUCTOR_IMPLEMENT(base_cut)

  CUT_RESET_IMPLEMENT_HEAD (base_cut)
  {
    this->i_cut::reset ();
    _set_initialized (false);
    return;
  }

  const datatools::things &
  base_cut::get_event_record () const
  {
    if (! has_user_data ())
      {
        std::ostringstream message;
        message << "dpp::base_cut::get_event_record: "
                << "No event record as an embedded user data ! ";
        throw std::logic_error (message.str ());
      }
    //      base_cut & const_cut = this
    const datatools::things * er_ptr = static_cast<const datatools::things *> (this->i_cut::_get_user_data ());
    return *er_ptr;
  }

  /*
    datatools::thing & base_cut::grab_event_record
    () { if (! has_user_data ()) { std::ostringstream message;
    message << "dpp::base_cut::grab_event_record: " << "No event
    record as an embedded user data ! "; throw std::logic_error
    (message.str ()); }
    //      base_cut & const_cut = this
    datatools::things * er_ptr = static_cast<datatools::things *> (this->i_cut::_get_user_data ());
    return *er_ptr;
    }
  */

}  // end of namespace dpp

// end of base_cut.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
