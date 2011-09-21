// -*- mode: c++ ; -*- 
/* multi_xor_cut.cc
 */

#include <cuts/multi_xor_cut.h>

#include <stdexcept>
//#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>

#include <datatools/utils/properties.h>

namespace cuts {

  using namespace std;
  
  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(multi_xor_cut, "cuts::multi_xor_cut");

  void multi_xor_cut::add_cut (cut_handle_type & a_cut_handle)
  {
    if (&a_cut_handle.get () == this)
      {
        ostringstream message;
        message << "cuts::multi_xor_cut::add_cut: "
                << "Self-referenced multi_xor_cut is not allowed !";
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
            message << "cuts::multi_xor_cut::add_cut: "
                    << "Cut is already registered !";
            cerr << "WARNING: " << message.str () << endl;
            return;
          }
      }
    _cuts_.push_back (a_cut_handle);
    return;
  }
    
  void multi_xor_cut::set_user_data (void * user_data_)
  {
    if (_cuts_.size () == 0) 
      {
        throw logic_error ("cuts::multi_xor_cut::set_user_data: Missing cuts !");
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
  CUT_CONSTRUCTOR_IMPLEMENT_HEAD (multi_xor_cut,
                                  a_debug_devel,
                                  "cuts::multi_xor_cut",
                                  "Multiple cut",
                                  "1.0")
  {
    return;
  }

  
  // dtor:
  CUT_DEFAULT_DESTRUCTOR_IMPLEMENT (multi_xor_cut)

  CUT_RESET_IMPLEMENT_HEAD (multi_xor_cut)
  {
    _cuts_.clear ();
    this->i_cut::reset ();
    set_initialized_ (false);
    return;
  }

  CUT_ACCEPT_IMPLEMENT_HEAD(multi_xor_cut)
  {
    if (_cuts_.size () == 0) 
      {
        throw logic_error ("cuts::multi_xor_cut::_accept: Missing cuts !");
      }
    /**
     * See : http://en.wikipedia.org/wiki/NOR_logic#XOR
     * Input A  Input B Output Q
     * 0        0       0
     * 0        1       1
     * 1        0       1
     * 1        1       0
     * See also : http://en.wikipedia.org/wiki/One-hot
     */
    int status_sum = 0;
    for (cuts_col_t::iterator i = _cuts_.begin ();
         i != _cuts_.end ();
         i++)
      {
        cut_handle_type & ch = *i;
        int status = ch.get ().process ();
        if (status == INAPPLICABLE)
          {
            return status;
          }
        if (status == ACCEPTED)
          { 
            status_sum++;
          }
      }
    if (status_sum == 1) return (ACCEPTED);
    return (REJECTED);
  }

  CUT_INITIALIZE_IMPLEMENT_HEAD(multi_xor_cut,
                                a_configuration,
                                a_service_manager,
                                a_cut_dict)
  {
    using namespace std;
    if (is_initialized ())
      {
        ostringstream message;
        message << "cuts::multi_xor_cut::initialize: "
                << "Cut '" << get_name () << "' is already initialized ! ";
        throw logic_error (message.str ());
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
            throw logic_error ("cuts::multi_xor_cut::initialize: Missing 'cuts' name property !");
          }
        
        for (int i = 0; i < cut_names.size (); i++)
          {
            const string & cut_name = cut_names[i];
            cut_handle_dict_type::iterator found = a_cut_dict.find (cut_name);
            if (found == a_cut_dict.end ())
              {
                ostringstream message;
                message << "cuts::multi_xor_cut::initialize: "
                        << "Can't find any cut named '" << cut_name 
                        << "' from the external dictionnary ! ";
                throw logic_error (message.str ());
              }
            this->add_cut (found->second);
          }

      }

    set_initialized_ (true);
    return;      
  }

} // end of namespace cuts

// end of multi_xor_cut.cc
