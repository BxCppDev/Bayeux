#ifndef __B__c3_hpp__
#define __B__c3_hpp__ 1

#include <iostream>

#include <boost/cstdint.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>

#include "Abase.hpp"

namespace B {

  class c3 : public A::base
  {
  private:

    int8_t m_c;

  public:

    c3 ();

    c3 (int8_t c);

    void set_c (int8_t c);

    uint8_t get_c () const;

    //virtual A::base * clone () const;

    virtual void print () const;

    virtual const char * get_key () const;

  private:
     
    friend class boost::serialization::access;
      
    template<class Archive>
    void serialize (Archive & ar, const unsigned int file_version)
    {
      ar & boost::serialization::make_nvp("A__base", boost::serialization::base_object<A::base>(*this));
      ar & boost::serialization::make_nvp("c", m_c);
      return;
    }
     
  };

} // end of namespace B

std::ostream & operator<< (std::ostream &, const B::c3 &);

BOOST_CLASS_EXPORT_KEY2 (B::c3, "B::c3")

BOOST_CLASS_TYPE_INFO(
    B::c3,
    extended_type_info_no_rtti<B::c3>
)

#endif // __B__c3_hpp__
