// datum.hpp

#ifndef __datum_h__
#define __datum_h__ 1

using namespace std;

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/cstdint.hpp>

class datum
{
  int16_t __value;
public:
  datum () : __value (0) {}
  void set_value (int value_)
  {
    __value = value_;
  }
  datum (int value_) : __value (value_) {}
  int get_value () const 
  {
    return __value;
  }
private:
  friend class boost::serialization::access; 
  template<class Archive>
  void serialize (Archive & ar_, const unsigned int version_)
  {
    ar_ & boost::serialization::make_nvp ("value", __value);
  }
};

#endif // __datum_h__

// end of datum.hpp
