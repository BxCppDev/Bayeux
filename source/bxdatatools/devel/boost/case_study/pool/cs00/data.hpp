// data.hpp

#ifndef __data_h__
#define __data_h__ 1

using namespace std;

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/cstdint.hpp>

class data
{
  int16_t __value;
public:
  data () : __value (0) {}
  void set_value (int value_)
  {
    __value = value_;
  }
  data (int value_) : __value (value_) {}
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

#endif // __data_h__

// end of data.hpp
