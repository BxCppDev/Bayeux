#ifndef __B__c3_hpp__
#define __B__c3_hpp__ 1

#include <iostream>

#include <boost/cstdint.hpp>

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

    virtual void print () const;

    virtual const char * get_key () const;

  private:
     
    friend class boost::serialization::access;
      
    template<class Archive>
    void serialize (Archive & ar, const unsigned int file_version);
     
  };

} // end of namespace B

std::ostream & operator<< (std::ostream &, const B::c3 &);

#endif // __B__c3_hpp__
