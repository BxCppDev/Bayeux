// -*- mode: c++ ; -*- 
/* multi_or_cut.cc
 */

#include <cuts/multi_or_cut.h>

#include <stdexcept>
//#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>

#include <datatools/utils/properties.h>

namespace cuts {

  using namespace std;
  
  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(multi_or_cut, "cuts::multi_or_cut");

  void multi_or_cut::add_cut (cut_handle_type & a_cut_handle)
  {
    if (&a_cut_handle.get () == this)
      {
        ostringstream message;
        message << "cuts::multi_or_cut::add_cut: "
                << "Self-referenced multi_or_cut is not allowed !";
        throw logic_error (message.str ( ));
      }
    for (cuts_col_t::iterator i = _cuts_.begin ();
         i != _cuts_.end ();
         i++)
      {
        cut_handle_type & ch = *i;
        if (&ch.get () == &a_cut_handle.get ())
          {
            ostringstream message;
            message << "cuts::multi_or_cut::add_cut: "
                    << "Cut is already registered !";
            cerr << "WARNING: " << message.str () << endl;
            return;
          }
      }
    _cuts_.push_back (a_cut_handle);
    return;
  }
    
  void multi_or_cut::set_user_data (void * user_data_)
  {
    if (_cuts_.size () == 0) 
      {
        throw logic_error ("cuts::multi_or_cut::set_user_data: Missing cuts !");
      }
    for (cuts_col_t::iterator i = _cuts_.begin ();
         i !=  _cuts_.end ();
         i++)
      {
        cut_handle_type & ch = *i;
        ch.get ().set_user_data (user_data_);
      }
    return;
  }
  
  // ctor:
  CUT_CONSTRUCTOR_IMPLEMENT_HEAD (multi_or_cut,
                                  a_debug_devel,
                                  "cuts::multi_or_cut",
                                  "Multiple cut",
                                  "1.0")
  {
    return;
  }

  
  // dtor:
  CUT_DEFAULT_DESTRUCTOR_IMPLEMENT (multi_or_cut)

  CUT_RESET_IMPLEMENT_HEAD (multi_or_cut)
  {
    _cuts_.clear ();
    this->i_cut::reset ();
    _set_initialized (false);
    return;
  }

  CUT_ACCEPT_IMPLEMENT_HEAD(multi_or_cut)
  {
    if (_cuts_.size () == 0) 
      {
        throw logic_error ("cuts::multi_or_cut::_accept: Missing cuts !");
      }
    for (cuts_col_t::iterator i = _cuts_.begin ();
         i != _cuts_.end ();
         i++)
      {
        cut_handle_type & ch = *i;
        int status = ch.get ().process ();
        if (is_debug ())
          {
            clog << "DEBUG: " << "cuts::multi_or_cut::process: " 
                 << "Cut status = " << status 
                 << endl;
          }
        if (status == ACCEPTED)
          {
            return status;
          }
        else if (status == INAPPLICABLE)
          {
            return status;
          } 
      }
    return (REJECTED);
  }

  CUT_INITIALIZE_IMPLEMENT_HEAD(multi_or_cut,
                                a_configuration,
                                a_service_manager,
                                a_cut_dict)
  {
    using namespace std;
    if (is_initialized ())
      {
        ostringstream message;
        message << "cuts::multi_or_cut::initialize: "
                << "Cut '" << get_name () << "' is already initialized ! ";
        throw logic_error (message.str ());
      }

    if (a_configuration.has_flag ("debug"))
      {
        set_debug (true); 
      }

    if (_cuts_.size () == 0) 
      { 
        vector<string> cut_names;
        if (a_configuration.has_key ("cuts"))
          {
            a_configuration.fetch ("cuts", cut_names);
          }
        else
          {
            throw logic_error ("cuts::multi_or_cut::initialize: Missing 'cuts' name property !");
          }
        
        for (int i = 0; i < cut_names.size (); i++)
          {
            const string & cut_name = cut_names[i];
            cut_handle_dict_type::iterator found = a_cut_dict.find (cut_name);
            if (found == a_cut_dict.end ())
              {
                ostringstream message;
                message << "cuts::multi_or_cut::initialize: "
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

// end of multi_or_cut.cc
