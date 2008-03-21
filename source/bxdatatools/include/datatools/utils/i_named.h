/* i_named.h */

#ifndef __datatools__utils__i_named_h
#define __datatools__utils__i_named_h 1

namespace datatools {

  namespace utils {

    class i_named
    {
    public:
      virtual std::string get_name() = 0;
    };

  } // end of namespace utils 

} // end of namespace datatools 

#endif // __datatools__utils__i_named_h

/* end of i_named.h */
