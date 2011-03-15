#ifndef __A__c2_hpp__
#define __A__c2_hpp__ 1

#include <iostream>

#include <boost/cstdint.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/string.hpp>

#include "Abase.hpp"

namespace A {

  class c2 : public base
  {
  private:

    std::string m_name;

  public:

    c2 ();

    c2 (const std::string & name);

    void set_name (const std::string & name);

    const std::string & get_name () const;

    virtual void print () const;

    virtual const char * get_key () const;

  private:
     
    friend class boost::serialization::access;
      
    template<class Archive>
    void serialize (Archive & ar, const unsigned int file_version)
    {
      ar & boost::serialization::make_nvp("A__base", boost::serialization::base_object<base>(*this));
      ar & boost::serialization::make_nvp("name", m_name);
      return;
    }
     
  };

} // end of namespace A

std::ostream & operator<< (std::ostream &, const A::c2 &);

BOOST_CLASS_EXPORT_KEY2 (A::c2, "A::c2")

BOOST_CLASS_TYPE_INFO(
    A::c2,
    extended_type_info_no_rtti<A::c2>
)

#endif // __A__c2_hpp__
