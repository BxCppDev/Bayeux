/* i_named.h */

#ifndef __datatools__utils__i_named_h
#define __datatools__utils__i_named_h 1

#include <datatools/serialization/serialization.h>

namespace datatools {

  namespace utils {

    class i_named
    {
    public:
      virtual std::string get_name() = 0;
    };

  } // end of namespace utils 

} // end of namespace datatools 

//BOOST_IS_ABSTRACT(datatools::utils::i_named)

#endif // __datatools__utils__i_named_h

/* end of i_named.h */
