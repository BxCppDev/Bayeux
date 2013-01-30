// -*- mode: c++ ; -*- 
/* not_cut.cc
 */

#include <cuts/not_cut.h>

#include <stdexcept>
#include <sstream>

#include <datatools/properties.h>

namespace cuts {

  using namespace std;
  
  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(not_cut, "cuts::not_cut");

  void not_cut::set_cut (cut_handle_type & a_cut_handle)
  {
    if (&a_cut_handle.get () == this)
      {
        ostringstream message;
        message << "cuts::not_cut::set_cut: "
                << "Self-referenced not_cut is not allowed !";
        throw logic_error (message.str ( ));
      }
    _handle = a_cut_handle;
    return;
  }
    
  void not_cut::set_user_data (void * a_user_data)
  {
    _handle.get ().set_user_data (a_user_data);
    return;
  }
  
  // ctor:
  CUT_CONSTRUCTOR_IMPLEMENT_HEAD (not_cut,
                                  a_debug_devel,
                                  "cuts::not_cut",
                                  "Not cut",
                                  "1.0")
  {
    return;
  }

  
  // dtor:
  CUT_DEFAULT_DESTRUCTOR_IMPLEMENT (not_cut)


  CUT_ACCEPT_IMPLEMENT_HEAD(not_cut)
  {
    if (! _handle.has_data ()) 
      {
        throw std::runtime_error ("not_cut::_accept: Handle has no 'cut' !");
      }
    int status = _handle.get ().process ();
    if (status == REJECTED) 
      {
        return ACCEPTED;
      }
    else  if (status == ACCEPTED) 
      {
        return REJECTED;
      }
    return status;
  }


  CUT_RESET_IMPLEMENT_HEAD (not_cut) 
  {
    
    _handle.reset ();
    this->i_cut::reset ();
    _set_initialized (false);
    return;
  }


  CUT_INITIALIZE_IMPLEMENT_HEAD(not_cut,
                                a_configuration,
                                a_service_manager,
                                a_cut_dict)
  {
    using namespace std;
    if (is_initialized ())
      {
        ostringstream message;
        message << "cuts::not_cut::initialize: "
                << "Cut '" << get_name () << "' is already initialized ! ";
        throw logic_error (message.str ());
      }

    if (! _handle.has_data ())
      {
        string cut_name;
        if (a_configuration.has_key ("cut"))
          {
            cut_name = a_configuration.fetch_string ("cut");
          }
        else
          {
            throw logic_error ("cuts::not_cut::initialize: Missing 'cut' name property !");
          }
        
        cut_handle_dict_type::iterator found = a_cut_dict.find (cut_name);
        if (found == a_cut_dict.end ())
          {
            ostringstream message;
            message << "cuts::not_cut::initialize: "
                    << "Can't find any cut named '" << cut_name 
                    << "' from the external dictionnary ! ";
            throw logic_error (message.str ());
          }
        this->set_cut (found->second.grab_initialized_cut_handle ());
      }

    _set_initialized (true);
    return;     
  }

} // end of namespace cuts

// end of not_cut.cc
