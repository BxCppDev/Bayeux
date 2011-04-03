/* datatools/utils/i_invokable.h */

#ifndef __datatools__utils__i_invokable_h
#define __datatools__utils__i_invokable_h 1

#include <datatools/utils/command.h>

//! \file datatools/utils/i_invokable.h

namespace datatools {

  namespace utils {


    //! \brief An abstract class for inherited invokable classes.
    class i_invokable
    {
    public:

      static int invoke (i_invokable &, datatools::utils::command &);

      virtual void do_invoke (datatools::utils::command &);

      int invoke (datatools::utils::command &);

    };

  } // end of namespace utils 

} // end of namespace datatools 

#endif // __datatools__utils__i_invokable_h

/* end of i_invokable.h */
