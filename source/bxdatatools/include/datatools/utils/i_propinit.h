/* i_propinit.h */

#ifndef __datatools__utils__i_propinit_h
#define __datatools__utils__i_propinit_h 1

#include <datatools/utils/properties.h>

namespace datatools {

  namespace utils {

    class i_propinit
    {
    public:
      virtual void init_from(const datatools::utils::properties &)
	{
	  std::cerr << "datatools/utils/properties.h: i_propinit::init_from: "
		    << "default method!" << std::endl; 
	}
    };

  } // end of namespace utils 

} // end of namespace datatools 

#endif // __datatools__utils__i_propinit_h

/* end of i_propinit.h */
