/* i_predicate.h */

#ifndef __datatools__utils__i_predicate_h
#define __datatools__utils__i_predicate_h 1

namespace datatools {

  namespace utils {

    //! \file datatools/utils/i_predicate.h

    template <class T>
    class i_predicate : public std::unary_function<const T &, bool>
    {
    public:
      virtual bool operator () (const T & a_obj) const = 0;
    };

  } // end of namespace utils 

} // end of namespace datatools 

#endif // __datatools__utils__i_predicate_h

/* end of i_predicate.h */
