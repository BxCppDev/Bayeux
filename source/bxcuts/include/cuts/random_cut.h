/// \file cuts/random_cut.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef CUTS_RANDOM_CUT_H
#define CUTS_RANDOM_CUT_H 1

// Third party:
// - Boost:
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

// This project:
#include <cuts/i_cut.h>

namespace cuts {

  /// \brief A cut that randomly accepts
  class random_cut : public i_cut
  {

  public:
    typedef boost::mt19937 base_generator_type;

    /// Constructor
    random_cut(datatools::logger::priority a_logging_priority =
               datatools::logger::PRIO_FATAL);

    /// Destructor
    ~random_cut() override;

    /// Initialization
    void initialize(const datatools::properties &,
                            datatools::service_manager &,
                            cuts::cut_handle_dict_type &) override;

    /// Reset
    void reset() override;

  protected :

    /// Selection
    int _accept() override;

  private:

    int    _seed_;
    double _accept_probability_;
    boost::scoped_ptr<base_generator_type>    _bg_;
    boost::scoped_ptr<boost::uniform_real<> > _uni_dist_;
    boost::scoped_ptr<boost::variate_generator<base_generator_type&, boost::uniform_real<> > > _uni_;

    // Macro to automate the registration of the cut :
    CUT_REGISTRATION_INTERFACE(random_cut)

  };

} // end of namespace cuts

#endif // CUTS_RANDOM_CUT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
