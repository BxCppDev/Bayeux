#ifndef __A__base_hpp__
#define __A__base_hpp__ 1

#include <boost/serialization/access.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/nvp.hpp>

#ifdef INCLUDE_ARCHIVES_BASE
#include "archives.hpp"
#endif

#include "archives.hpp"

namespace A {

  class base
  {
  public:

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

//BOOST_CLASS_EXPORT_KEY(A::base)

#endif // __A__base_hpp__
