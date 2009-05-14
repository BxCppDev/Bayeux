// foo.hpp

#ifndef __foo_h__
#define __foo_h__ 1

#include <string>

#include <boost/cstdint.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>

using namespace std;

class foo
{
  int32_t __i;
  double  __x;
  string  __name;

public:
  foo ()
  {
    __i = 666;
    __x = 3.14159;
  }
  int get_i () const
  {
    return __i;
  }
  double get_x () const
  {
    return __x;
  }
  const string & get_name () const
  {
    return __name;
  }
  void set_i (int i_)
  {
    __i = i_;
  }
  void set_x (double x_)
  {
    __x = x_;
  }
  void set_name (const string & name_)
  {
    __name = name_;
  }
private:
  friend class boost::serialization::access; 

  template<class Archive>
  void serialize (Archive & ar_, const unsigned int version_)
  {
    ar_ & boost::serialization::make_nvp ("i", __i);
    ar_ & boost::serialization::make_nvp ("x", __x);
    ar_ & boost::serialization::make_nvp ("name", __name);
  }
};

ostream & operator<< (ostream & out_, const foo & f_)
{
  out_ << '(' << f_.get_i () 
       << ',' << f_.get_x () 
       << ',' << f_.get_name () 
       << ')';
  return out_;
}

#endif // __foo_h__

// end of foo.hpp
