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

    template<class Mother, class Daughter>
    class mother_to_daughter_predicate : public i_predicate<Daughter>
    {
      i_predicate<Mother> * mother_predicate_;
    public:
      mother_to_daughter_predicate (i_predicate<Mother> & a_mother_predicate)
	{
	  mother_predicate_ = &a_mother_predicate;
	  return;
	}
      virtual bool operator () (const Daughter & a_obj) const
      {
	return (*mother_predicate_) (a_obj);
      }
    };

  } // end of namespace utils 

} // end of namespace datatools 

#endif // __datatools__utils__i_predicate_h

/* end of i_predicate.h */
