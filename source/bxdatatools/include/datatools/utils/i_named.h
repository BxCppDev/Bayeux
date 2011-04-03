/* datatools/utils/i_named.h */

#ifndef __datatools__utils__i_named_h
#define __datatools__utils__i_named_h 1

#include <datatools/serialization/serialization.h>

//! \file datatools/utils/i_named.h

namespace datatools {

  namespace utils {

    //! \brief A pure abstract class (interface) for inherited named objects.
    class i_named
    {
    public:

     /**
      * A pure virtual member.
      * @return the name associated to the object.
      */
      virtual std::string get_name() = 0;

      static bool is_anonymous(const i_named & a_named)
      {
	return a_named.get_name ().empty ();
      }

    };

  } // end of namespace utils 

} // end of namespace datatools 

#endif // __datatools__utils__i_named_h

/* end of datatools/utils/i_named.h */
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
