#ifndef __A__c1_hpp__
#define __A__c1_hpp__ 1

#include <iostream>

#include <boost/cstdint.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>

#include "Abase.hpp"

namespace A {

  class c1 : public base
  {
  private:

    int32_t m_i;

  public:

    c1 ();

    c1 (int32_t i);

    void set_i (int32_t i);

    uint32_t get_i () const;

    virtual void print () const;

    virtual const char * get_key() const;

  private:
     
    friend class boost::serialization::access;
      
    template<class Archive>
    void serialize (Archive & ar, const unsigned int file_version)
    {
      ar & boost::serialization::make_nvp("A__base", boost::serialization::base_object<base>(*this));
      ar & boost::serialization::make_nvp("i", m_i);
      return;
    }
     
  };

} // end of namespace A

std::ostream & operator<< (std::ostream &, const A::c1 &);

BOOST_CLASS_EXPORT_KEY2 (A::c1, "A::c1")

BOOST_CLASS_TYPE_INFO(
    A::c1,
    extended_type_info_no_rtti<A::c1>
)

#endif // __A__c1_hpp__
