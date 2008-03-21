/* i_clear.h */

#ifndef __datatools__utils__i_clear_h
#define __datatools__utils__i_clear_h 1

namespace datatools {

  namespace utils {

    class i_clear
    {
    public:
      virtual void clear() = 0;
    };

  } // end of namespace utils 

} // end of namespace datatools 
   
//BOOST_IS_ABSTRACT(datatools::utils::i_clear)

#endif // __datatools__utils__i_clear_h

/* end of i_clear.h */
