// -*- mode: c++ ; -*- 
/* accept_cut.cc
 */

#include <cuts/accept_cut.h>

#include <stdexcept>
#include <sstream>

namespace cuts {
    
  using namespace std;

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(accept_cut, "cuts::accept_cut");

  // ctor:
  CUT_CONSTRUCTOR_IMPLEMENT_HEAD (accept_cut,
				  a_debug_devel,
				  "cuts::accept_cut",
				  "Always accept cut",
				  "1.0")
  {
    return;
  }
  
  // dtor:
  CUT_DEFAULT_DESTRUCTOR_IMPLEMENT (accept_cut)

  CUT_ACCEPT_IMPLEMENT_HEAD(accept_cut)
  {
    return i_cut::ACCEPTED;
  } 

  CUT_INITIALIZE_IMPLEMENT_HEAD(accept_cut,
				a_configuration,
				a_service_manager,
				a_dict)
  {
    using namespace std;
    if (is_initialized ())
      {
	ostringstream message;
	message << "cuts::accept_cut::initialize: "
		<< "Cut '" << get_name () << "' is already initialized ! ";
	throw logic_error (message.str ());
      }
    _set_initialized (true);
    return;	
  }

  CUT_RESET_IMPLEMENT_HEAD (accept_cut) 
  {
    this->i_cut::reset ();
    _set_initialized (false);
    return;
  }
  
} // end of namespace cuts

// end of accept_cut.cc
