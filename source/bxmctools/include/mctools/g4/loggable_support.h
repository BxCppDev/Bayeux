/* loggable_support.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-06-03
 * Last modified : 2013-06-03
 */

#ifndef MCTOOLS_G4_LOGGABLE_SUPPORT_H_
#define MCTOOLS_G4_LOGGABLE_SUPPORT_H_ 1

#include <datatools/logger.h>

namespace datatools {
  class properties;
}

namespace mctools {

  namespace g4 {

    class loggable_support {

    public:

      loggable_support();

      explicit loggable_support(datatools::logger::priority);

      datatools::logger::priority get_logging_priority() const;

      void set_logging_priority(datatools::logger::priority);

      bool is_debug () const;

      void set_debug (bool);

      bool is_verbose () const;

      void set_verbose (bool);

    protected:

      void _initialize_logging_support(const datatools::properties &);

      datatools::logger::priority _logprio() const;

    protected:

      datatools::logger::priority _logging_priority; //!< Logging priority threshold

    };

  } // end of namespace g4

} // end of namespace mctools

#endif //MCTOOLS_G4_LOGGABLE_SUPPORT_H_

// end of track_history.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/

