// -*- mode: c++ ; -*- 
/* i_binary_cut.cc
 */

#include <cuts/i_binary_cut.h>

#include <stdexcept>
#include <sstream>

#include <datatools/properties.h>

namespace cuts {

  using namespace std;

  void i_binary_cut::set_cut_1 (cut_handle_type & a_cut_handle)
  {
    if (&a_cut_handle.get () == this)
      {
        ostringstream message;
        message << "cuts::i_binary_cut::set_cut_1: "
                << "Self-referenced not_cut is not allowed !";
        throw logic_error (message.str ( ));
      }
    _handle_1 = a_cut_handle;
    return;
  }

  void i_binary_cut::set_cut_2 (cut_handle_type & a_cut_handle)
  {
    if (&a_cut_handle.get () == this)
      {
        ostringstream message;
        message << "cuts::i_binary_cut::set_cut_2: "
                << "Self-referenced not_cut is not allowed !";
        throw logic_error (message.str ( ));
      }
    _handle_2 = a_cut_handle;
    return;
  }
   
  void i_binary_cut::set_cuts (cut_handle_type & a_cut_handle_1, 
                               cut_handle_type & a_cut_handle_2)
  {
    set_cut_1 (a_cut_handle_1);
    set_cut_2 (a_cut_handle_2);
    return;
  }
    
  void i_binary_cut::set_user_data (void * a_user_data)
  {
    _handle_1.get ().set_user_data (a_user_data);
    _handle_2.get ().set_user_data (a_user_data);
    return;
  }
  
  // ctor:
  i_binary_cut::i_binary_cut (const string & a_cut_name, 
                              const string & a_cut_description, 
                              const string & a_cut_version, 
                              int a_debug_level) :
    cuts::i_cut (a_cut_name, 
                 a_cut_description, 
                 a_cut_version,
                 a_debug_level)
  {
    return;
  }
 
  // dtor:
  CUT_DEFAULT_DESTRUCTOR_IMPLEMENT(i_binary_cut)
 
  CUT_RESET_IMPLEMENT_HEAD (i_binary_cut) 
  {
    _handle_1.reset ();
    _handle_2.reset ();
    this->i_cut::reset ();
    _set_initialized (false);
    return;
  }

  void i_binary_cut::_install_cuts (const datatools::properties & a_configuration,
                                    cuts::cut_handle_dict_type & a_cut_dict)
  {
    if (! _handle_1.has_data ())
      {
        string cut_name;
        if (a_configuration.has_key ("cut_1"))
          {
            cut_name = a_configuration.fetch_string ("cut_1");
          }
        else
          {
            throw logic_error ("cuts::i_binary_cut::_install_cuts: Missing 'cut_1' name property !");
          }
        
        cut_handle_dict_type::iterator found = a_cut_dict.find (cut_name);
        if (found == a_cut_dict.end ())
          {
            ostringstream message;
            message << "cuts::i_binary_cut::_install_cuts: "
                    << "Can't find any cut named '" << cut_name 
                    << "' from the external dictionnary ! ";
            throw logic_error (message.str ());
          }
        this->set_cut_1 (found->second.grab_initialized_cut_handle ());
      }

    if (! _handle_2.has_data ())
      {
        string cut_name;
        if (a_configuration.has_key ("cut_2"))
          {
            cut_name = a_configuration.fetch_string ("cut_2");
          }
        else
          {
            throw logic_error ("cuts::i_binary_cut::_install_cuts: Missing 'cut_2' name property !");
          }
        
        cut_handle_dict_type::iterator found = a_cut_dict.find (cut_name);
        if (found == a_cut_dict.end ())
          {
            ostringstream message;
            message << "cuts::i_binary_cut::_install_cuts: "
                    << "Can't find any cut named '" << cut_name 
                    << "' from the external dictionnary ! ";
            throw logic_error (message.str ());
          }
        this->set_cut_2 (found->second.grab_initialized_cut_handle ());
      }

    if (! _handle_1.has_data ())
      {
        throw logic_error ("cuts::i_binary_cut::_install_cuts: Null 'cut 1' !");
      }

    if (! _handle_2.has_data ())
      {
        throw logic_error ("cuts::i_binary_cut::_install_cuts: Null 'cut 2' !");
      }
      
    return;
  }
  
} // end of namespace cuts

// end of i_binary_cut.cc
