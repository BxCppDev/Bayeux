#ifndef __A__base_hpp__
#define __A__base_hpp__ 1

#include <boost/serialization/access.hpp>

namespace A {

  class base
  {
  public:

    virtual void print () const = 0;

    virtual const char * get_key () const = 0;

  private:
     
    friend class boost::serialization::access;
      
    template<class Archive>
    void serialize (Archive & ar, const unsigned int file_version);
     
  };

} // end of namespace A


#endif // __A__base_hpp__
