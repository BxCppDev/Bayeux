// -*- mode: c++ ; -*-
/* random_cut.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-02-18
 * Last modified: 2013-04-22
 *
 * License:
 *
 * Description:
 *   Cut that accepts randomly
 *
 * History:
 *
 */

#ifndef CUTS_RANDOM_CUT_H_
#define CUTS_RANDOM_CUT_H_ 1

#include <cuts/i_cut.h>
#include <cuts/cut_macros.h>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

namespace cuts {

  //CUT_CLASS_DECLARE (accept_cut)
  /// \brief A cut that randomly accepts
  class random_cut : public i_cut
  {

  public:
    typedef boost::mt19937 base_generator_type;

    // ctor:
    CUT_INTERFACE_CTOR_DTOR(random_cut);

  private:

    int    _seed_;
    double _accept_probability_;
    boost::scoped_ptr<base_generator_type>    _bg_;
    boost::scoped_ptr<boost::uniform_real<> > _uni_dist_;
    boost::scoped_ptr<boost::variate_generator<base_generator_type&, boost::uniform_real<> > > _uni_;

    // Macro to automate the registration of the cut :
    CUT_REGISTRATION_INTERFACE(random_cut);

  };

} // end of namespace cuts

#endif // CUTS_RANDOM_CUT_H_

// end of random_cut.h
