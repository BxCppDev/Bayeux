// bag.hpp

#ifndef __bag_h__
#define __bag_h__ 1

using namespace std;

#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/list.hpp>
#include <boost/cstdint.hpp>

#include "data.hpp"

class bag
{
  int16_t      __n;
  list<data *> __values;
public:
  int get_n () const 
  {
    return __n;
  }
  bag () : __n (0) 
  {
  }
  void clear ()
  {
    for (list<data *>::const_iterator i = __values.begin ();
	 i != __values.end ();
	 i++) 
      {
	if (*i) delete *i;
      }
    __values.clear ();
  }
  ~bag () 
  {
    clear ();
  }
  void set (int n_) 
  {
    clear ();
    __n = n_;
    for (int i = 1; i <= __n; i++)
      {
	data * p = new data (i);
	__values.push_back (p);
      }
  }
  bag (int n_) : __n (0)
  {
    set (n_);
  }
  int sum () const
  {
    int s = 0;
    for (list<data *>::const_iterator i = __values.begin ();
	 i != __values.end ();
	 i++) s += (*i)->get_value ();
    return s;
  }
  bool check_sum () const
  {
    int inner_sum = sum ();
    int expected_sum = (__n * (__n + 1)) / 2;
    return inner_sum == expected_sum;
  }
private:
  friend class boost::serialization::access; 
  template<class Archive>
  void serialize (Archive & ar_, const unsigned int version_)
  {
    ar_ & boost::serialization::make_nvp ("n", __n);
    ar_ & boost::serialization::make_nvp ("values", __values);
  }
};

#endif // __bag_h__

// end of bag.hpp
