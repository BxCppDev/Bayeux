// -*- mode: c++ ; -*- 
/* i_binary_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2010-09-18
 * 
 * License: 
 * 
 * Description: 
 *   A abstract cut interface that combines two other cuts
 * 
 * History: 
 * 
 */

#ifndef __cuts__i_binary_cut_h
#define __cuts__i_binary_cut_h 1

#include <string>
#include <cuts/i_cut.h>
#include <cuts/cut_macros.h>
#include <datatools/utils/properties.h>

namespace cuts {
  
  CUT_CLASS_DECLARE (i_binary_cut)
  {
    
  public: 
    
    void set_cut_1 (cut_handle_type &);
    
    void set_cut_2 (cut_handle_type &);
    
    void set_cuts (cut_handle_type &, cut_handle_type &);
    
    virtual void set_user_data (void *);
    
  public: 

    /// Constructor
    i_binary_cut (const std::string & a_cut_name, 
                  const std::string & a_cut_description = "", 
                  const std::string & a_cut_version = "", 
                  int a_debug_level = 0);
   
    /// Destructor
    CUT_DESTRUCTOR_DECLARE (i_binary_cut);

    /// Reset
    CUT_RESET_DECLARE ();

  protected:

    void _install_cuts (const datatools::utils::properties & a_configuration,
                        cuts::cut_handle_dict_type & a_cut_dict);
  
  protected: 
    
    cut_handle_type _handle_1;
    cut_handle_type _handle_2;

  };

} // end of namespace cuts

#endif // __cuts__i_binary_cut_h

// end of i_binary_cut.h
