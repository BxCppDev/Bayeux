// -*- mode: c++ ; -*-
/* multi_xor_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2013-04-22
 *
 * License:
 *
 * Description:
 *   A abstract cut interface that combines two other cuts
 *
 * History:
 *
 */

#ifndef CUTS_MULTI_XOR_CUT_H_
#define CUTS_MULTI_XOR_CUT_H_ 1

#include <string>
#include <list>

#include <cuts/i_cut.h>
#include <cuts/cut_macros.h>

namespace cuts {

  //CUT_CLASS_DECLARE(multi_xor_cut)
  /// \brief A multi XOR cut
  class multi_xor_cut : public i_cut
  {
  public:

    typedef std::list<cut_handle_type> cuts_col_type;

  public:

    void add_cut (cut_handle_type &);

    virtual void set_user_data (void *);

  public:

    CUT_INTERFACE_CTOR_DTOR (multi_xor_cut);

  protected:

    cuts_col_type _cuts_;

    // Macro to automate the registration of the cut :
    CUT_REGISTRATION_INTERFACE(multi_xor_cut);

  };

} // end of namespace cuts

#endif // CUTS_MULTI_XOR_CUT_H_

// end of multi_xor_cut.h
