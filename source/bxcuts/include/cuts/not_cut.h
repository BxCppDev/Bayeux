// -*- mode: c++ ; -*-
/* not_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-18
 * Last modified: 2013-04-22
 *
 * License:
 *
 * Description:
 *   NOT logic unary cut.
 *
 * History:
 *
 */

#ifndef CUTS_NOT_CUT_H_
#define CUTS_NOT_CUT_H_ 1

#include <cuts/i_cut.h>
#include <cuts/cut_macros.h>

namespace cuts {

  /// \brief The NOT cut
  CUT_CLASS_DECLARE(not_cut)
  {
  public:

    void set_cut(cut_handle_type &);

    // Cut interface macro
    CUT_INTERFACE_CTOR_DTOR (not_cut);

  protected:

    virtual void _at_set_user_data();

    virtual void _at_reset_user_data();

  protected:

    cut_handle_type _handle; //!< The cut to be negated by the "NOT" cut class

  private:

    /// Macro to automate the registration of the cut
    CUT_REGISTRATION_INTERFACE(not_cut);

  };

} // end of namespace cuts

#endif // _CUTS_NOT_CUT_H_

// end of not_cut.h
