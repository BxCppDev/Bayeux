// -*- mode: c++ ; -*-
/* multi_and_cut.cc
 */

#include <cuts/multi_and_cut.h>

#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <vector>

#include <datatools/properties.h>

namespace cuts {

  using namespace std;

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(multi_and_cut, "cuts::multi_and_cut");

  void multi_and_cut::add_cut (cut_handle_type & a_cut_handle)
  {
    if (&a_cut_handle.grab () == this) {
      ostringstream message;
      message << "cuts::multi_and_cut::add_cut: "
              << "Self-referenced multi_and_cut is not allowed !";
      throw logic_error (message.str ( ));
    }
    for (cuts_col_type::iterator i = _cuts_.begin ();
         i != _cuts_.end ();
         i++) {
      cut_handle_type & ch = *i;
      if (&ch.grab () == &a_cut_handle.grab ())
        {
          ostringstream message;
          message << "cuts::multi_and_cut::add_cut: "
                  << "Cut is already registered !";
          cerr << "WARNING: " << message.str () << endl;
          return;
        }
    }
    _cuts_.push_back (a_cut_handle);
    return;
  }

  void multi_and_cut::set_user_data (void * user_data_)
  {
    if (_cuts_.size () == 0) {
      throw logic_error ("cuts::multi_and_cut::set_user_data: Missing cuts !");
    }
    for (cuts_col_type::iterator i = _cuts_.begin ();
         i !=  _cuts_.end ();
         i++) {
      cut_handle_type & ch = *i;
      ch.grab ().set_user_data (user_data_);
    }
    return;
  }

  // ctor:
  CUT_CONSTRUCTOR_IMPLEMENT_HEAD (multi_and_cut,
                                  a_debug_devel,
                                  "cuts::multi_and_cut",
                                  "Multiple cut",
                                  "1.0")
  {
    return;
  }


  // dtor:
  CUT_DEFAULT_DESTRUCTOR_IMPLEMENT (multi_and_cut)

  CUT_RESET_IMPLEMENT_HEAD (multi_and_cut)
  {
    _cuts_.clear ();
    this->i_cut::reset ();
    _set_initialized (false);
    return;
  }

  CUT_ACCEPT_IMPLEMENT_HEAD(multi_and_cut)
  {
    if (_cuts_.size () == 0) {
      throw logic_error ("cuts::multi_and_cut::_accept: Missing cuts !");
    }

    for (cuts_col_type::iterator i = _cuts_.begin ();
         i !=  _cuts_.end ();
         i++) {
      cut_handle_type & ch = *i;
      int status = ch.grab ().process ();
      if (status != ACCEPTED)
        {
          return status;
        }
    }
    return (ACCEPTED);
  }

  CUT_INITIALIZE_IMPLEMENT_HEAD(multi_and_cut,
                                a_configuration,
                                a_service_manager,
                                a_cut_dict)
  {
    using namespace std;
    if (is_initialized ()) {
      ostringstream message;
      message << "cuts::multi_and_cut::initialize: "
              << "Cut '" << get_name () << "' is already initialized ! ";
      throw logic_error (message.str ());
    }

    if (_cuts_.size () == 0) {
      vector<string> cut_names;
      if (a_configuration.has_key ("cuts")) {
        a_configuration.fetch ("cuts", cut_names);
        if (cut_names.size() == 0) {
          ostringstream message;
          message << "cuts::multi_and_cut::initialize: "
                  << "The 'cuts' property setups an empty list of cuts !";
          throw logic_error (message.str ());
        }
      } else {
        throw logic_error ("cuts::multi_and_cut::initialize: Missing 'cuts' name property !");
      }

      for (int i = 0; i < cut_names.size (); i++) {
        const string & cut_name = cut_names[i];
        cut_handle_dict_type::iterator found = a_cut_dict.find (cut_name);
        if (found == a_cut_dict.end ()) {
          ostringstream message;
          message << "cuts::multi_and_cut::initialize: "
                  << "Can't find any cut named '" << cut_name
                  << "' from the external dictionnary ! ";
          throw logic_error (message.str ());
        }
        this->add_cut (found->second.grab_initialized_cut_handle ());
      }

    }

    _set_initialized (true);
    return;
  }

} // end of namespace cuts

// end of multi_and_cut.cc
