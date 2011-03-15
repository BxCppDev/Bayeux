#ifndef __A__base_hpp__
#define __A__base_hpp__ 1

#include <boost/serialization/access.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/type_info_implementation.hpp>
#include <boost/serialization/extended_type_info_no_rtti.hpp>
#include <boost/serialization/nvp.hpp>

#ifdef HERE
#include "archives.hpp"
#endif

namespace A {

  class base
  {
  public:

    //virtual base * clone () const = 0;
    virtual void print () const = 0;

    virtual const char * get_key () const = 0;

  private:
     
    friend class boost::serialization::access;
      
    template<class Archive>
    void serialize (Archive & ar, const unsigned int file_version)
    {
      return;
    }
     
  };

} // end of namespace A

BOOST_SERIALIZATION_ASSUME_ABSTRACT(A::base)

// the no_rtti system requires this !!!
BOOST_CLASS_EXPORT_KEY(A::base)

BOOST_CLASS_TYPE_INFO(
    A::base,
    boost::serialization::extended_type_info_no_rtti<A::base>
)

#endif // __A__base_hpp__
