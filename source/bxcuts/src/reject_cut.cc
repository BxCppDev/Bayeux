// -*- mode: c++ ; -*- 
/* reject_cut.cc
 */

#include <cuts/reject_cut.h>

#include <stdexcept>
#include <sstream>

namespace cuts {
  
  using namespace std;

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(reject_cut, "cuts::reject_cut");

  // ctor:
  CUT_CONSTRUCTOR_IMPLEMENT_HEAD (reject_cut,
				  a_debug_devel,
				  "cuts::reject_cut",
				  "Always reject cut",
				  "1.0")
  {
    return;
  }
  
  // dtor:
  CUT_DEFAULT_DESTRUCTOR_IMPLEMENT (reject_cut)

  CUT_ACCEPT_IMPLEMENT_HEAD(reject_cut)
  {
    return i_cut::REJECTED;
  } 

  CUT_INITIALIZE_IMPLEMENT_HEAD(reject_cut,
				a_configuration,
				a_service_manager,
				a_dict)
  {
    using namespace std;
    if (is_initialized ())
      {
	ostringstream message;
	message << "cuts::reject_cut::initialize: "
		<< "Cut '" << get_name () << "' is already initialized ! ";
	throw logic_error (message.str ());
      }
    set_initialized_ (true);
    return;	
  }

  CUT_RESET_IMPLEMENT_HEAD (reject_cut) 
  {
    this->i_cut::reset ();
    set_initialized_ (false);
    return;
  }
  
} // end of namespace cuts

// end of reject_cut.cc
