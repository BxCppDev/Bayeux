// -*- mode: c++ ; -*- 
/* not_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2010-09-18
 * 
 * License: 
 * 
 * Description: 
 *   NOT logic cut.
 * 
 * History: 
 * 
 */

#ifndef _CUTS_NOT_CUT_H
#define _CUTS_NOT_CUT_H 1

#include <cuts/i_cut.h>
#include <cuts/cut_macros.h>

namespace cuts {


  //CUT_CLASS_DECLARE (not_cut)
  /// \brief The NOT cut  
  class not_cut : public i_cut
  {
     
  public: 
    
    void set_cut (cut_handle_type &);
    
    virtual void set_user_data (void *);
    
  public: 

    // ctor:
    CUT_INTERFACE_CTOR_DTOR (not_cut);

  private:

    // Macro to automate the registration of the cut :
    CUT_REGISTRATION_INTERFACE(not_cut);
 
  protected: 

    cut_handle_type _handle; //!< The cut to be negated by the "NOT" cut class

  };

} // end of namespace cuts

#endif // _CUTS_NOT_CUT_H

// end of not_cut.h
