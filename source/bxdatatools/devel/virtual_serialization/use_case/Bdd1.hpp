#ifndef __B__dd1_hpp__
#define __B__dd1_hpp__ 1

#include "Ac1.hpp"

namespace B {

  class dd1 : public A::c1
  {
  private:
    
    int32_t m_j;
    
  public:

    dd1 ();

    dd1 (int32_t i, int32_t j);

    void set_j (int32_t j);

    uint32_t get_j () const;

    virtual void print () const;

    virtual const char * get_key () const;

  private:
     
    friend class boost::serialization::access;
      
    template<class Archive>
    void serialize (Archive & ar, const unsigned int file_version);
    
  };

}

std::ostream & operator<< (std::ostream &, const B::dd1 &);

#endif // __B__dd1_hpp__
